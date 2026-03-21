/*
 * mmwave.c
 *
 *  Created on: Jan 26, 2026
 *      Author: noahhr
 */

#include "mmwave.h"

mmwave_Data_t mmwave_Data;
uint8_t tx_buff[32];

uint8_t dma_rx_buf[DMA_BUF_SIZE];
uint16_t last_pos;

#define MMWAVE_MAX_RETRIES 3

static int mmwave_Send_With_Retry(mmwave_Packet* packet, UART_HandleTypeDef* huart) {
    uint8_t flush;
    for (int attempt = 0; attempt < MMWAVE_MAX_RETRIES; attempt++) {
        // Flush any stale data before each attempt
        while(HAL_UART_Receive(huart, &flush, 1, 10) == HAL_OK);

        mmwave_Write_Command(packet, huart);
        int ack = mmwave_Check_For_Ack(huart);
        if (ack == 0) return 0;

        HAL_Delay(50); // Wait before retrying
    }
    return 1; // All retries failed
}

void mmwave_Write_Command(mmwave_Packet* packet, UART_HandleTypeDef* huart1) {

	//Setup Header
	tx_buff[0] = (packet->header >> 0) & 0xFF;
	tx_buff[1] = (packet->header >> 8) & 0xFF;
	tx_buff[2] = (packet->header >> 16) & 0xFF;
	tx_buff[3] = (packet->header >> 24) & 0xFF;

	//Send Intradata length
	tx_buff[4] = (packet->data_len) & 0xFF;
	tx_buff[5] = (packet->data_len >> 8) & 0xFF;

	//Cmd word (Start data_len)
	tx_buff[6] = (packet->cmd_word) & 0xFF;
	tx_buff[7] = (packet->cmd_word >> 8) & 0xFF;

	for (int i = 0; i < packet->data_len - 2; ++i) {
		tx_buff[8 + i] = packet->cmd_value[i];
	}

	tx_buff[6 + packet->data_len + 0] = (packet->footer >>  0) & 0xFF;
	tx_buff[6 + packet->data_len + 1] = (packet->footer >>  8) & 0xFF;
	tx_buff[6 + packet->data_len + 2] = (packet->footer >> 16) & 0xFF;
	tx_buff[6 + packet->data_len + 3] = (packet->footer >> 24) & 0xFF;

	HAL_UART_Transmit(huart1, tx_buff, packet->data_len + PADDING, 1000);
}

int mmwave_Check_For_Ack(UART_HandleTypeDef* huart1) {
	uint16_t data_len = 0;
	if(HAL_UART_Receive(huart1, mmwave_Data.buf, 6, 1000)==HAL_OK) {
		//Check for success.
		uint32_t header = (mmwave_Data.buf[3] << 24)
		                + (mmwave_Data.buf[2] << 16)
		                + (mmwave_Data.buf[1] << 8)
		                + (mmwave_Data.buf[0]);

		if (header != MMWAVE_CONFIG_HEADER) {
			return 1;
		}
		data_len = (mmwave_Data.buf[5] << 8) + (mmwave_Data.buf[4]);
	} else {
		return 2;
	}

	if(HAL_UART_Receive(huart1, mmwave_Data.buf, data_len + 4, 1000)==HAL_OK) {
		uint16_t sent_cmd = (mmwave_Data.buf[1] << 8) + (mmwave_Data.buf[0]);
		uint16_t ack      = (mmwave_Data.buf[3] << 8) + (mmwave_Data.buf[2]);
		if (ack == 0) {
			return 0;
		} else {
			return 1;
		}
	} else {
		return 2;
	}
}

int mmwave_Init(UART_HandleTypeDef* huart1) {
    // Flush any stale data frames sitting in RX buffer
    uint8_t flush;
    while(HAL_UART_Receive(huart1, &flush, 1, 10) == HAL_OK)
    	;

	mmwave_Packet cmd_packet;
	cmd_packet.header = MMWAVE_CONFIG_HEADER;
	cmd_packet.footer = MMWAVE_CONFIG_FOOTER;

	//Enable configuration commands
	cmd_packet.data_len = 0x0004;
	cmd_packet.cmd_word = 0x00FF;
	cmd_packet.cmd_value[0] = 0x01;
	cmd_packet.cmd_value[1] = 0x00;

	if (mmwave_Send_With_Retry(&cmd_packet, huart1) > 0) return 1;
	HAL_Delay(10);

	//Reset to Factory Defaults
	cmd_packet.cmd_word = 0x00A2;
	cmd_packet.data_len = 0x0002;
	if (mmwave_Send_With_Retry(&cmd_packet, huart1) > 0) return 1;
	HAL_Delay(50);

	// Restart to apply factory reset
	// Use plain transmit — NOT Send_With_Retry
	// Send_With_Retry flushes RX which will eat streaming frames
	cmd_packet.cmd_word = 0x00A3;
	cmd_packet.data_len = 0x0002;
	mmwave_Write_Command(&cmd_packet, huart1);
	HAL_Delay(1500); // sensor reboots, comes up at 256000 in working mode

	// Enable config again
	cmd_packet.cmd_word = 0x00FF;
	cmd_packet.data_len = 0x0004;
	cmd_packet.cmd_value[0] = 0x01;
	cmd_packet.cmd_value[1] = 0x00;
	mmwave_Send_With_Retry(&cmd_packet, huart1);
	HAL_Delay(50);

	//Set distance resolution to 0.2m
	cmd_packet.data_len = 0x0004;
	cmd_packet.cmd_word = 0x00AA;
	cmd_packet.cmd_value[0] = 0x01; //maybe 0.2m? Isn't really clear....
	cmd_packet.cmd_value[1] = 0x00;

	if (mmwave_Send_With_Retry(&cmd_packet, huart1) > 0) return 1;
	HAL_Delay(10);

	//Set maximum movement distance door, maximum resting, and no one duration
	cmd_packet.data_len = 0x0014;
	cmd_packet.cmd_word = 0x0060;
	cmd_packet.cmd_value[0] = 0x00;
	cmd_packet.cmd_value[1] = 0x00;
	cmd_packet.cmd_value[2] = 0x02; //2 * 0.2m
	cmd_packet.cmd_value[3] = 0x00;
	cmd_packet.cmd_value[4] = 0x00;
	cmd_packet.cmd_value[5] = 0x00;
	cmd_packet.cmd_value[6] = 0x01;
	cmd_packet.cmd_value[7] = 0x00;
	cmd_packet.cmd_value[8] = 0x02; //2 * 0.2m
	cmd_packet.cmd_value[9] = 0x00;
	cmd_packet.cmd_value[10] = 0x00;
	cmd_packet.cmd_value[11] = 0x00;
	cmd_packet.cmd_value[12] = 0x02;
	cmd_packet.cmd_value[13] = 0x00;
	cmd_packet.cmd_value[14] = 0x0A; //10 sec
	cmd_packet.cmd_value[15] = 0x00;
	cmd_packet.cmd_value[16] = 0x00;
	cmd_packet.cmd_value[17] = 0x00;

	if (mmwave_Send_With_Retry(&cmd_packet, huart1) > 0) return 1;
	HAL_Delay(10);


	// Restart module
	cmd_packet.data_len = 0x0002;
	cmd_packet.cmd_word = 0x00A3;
	mmwave_Write_Command(&cmd_packet, huart1);

	HAL_Delay(1000); // Give it time to reboot and start streaming


	// Enable config again
	cmd_packet.cmd_word = 0x00FF;
	cmd_packet.data_len = 0x0004;
	cmd_packet.cmd_value[0] = 0x01;
	cmd_packet.cmd_value[1] = 0x00;
	mmwave_Send_With_Retry(&cmd_packet, huart1);
	HAL_Delay(50);

	//End configuration
	cmd_packet.data_len = 0x0002;
	cmd_packet.cmd_word = 0x00FE;
	if (mmwave_Send_With_Retry(&cmd_packet, huart1) > 0) return 1;
	HAL_Delay(10);



	return 0;
}

int mmwave_Receive(UART_HandleTypeDef *huart) {
    uint16_t cur_pos = DMA_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);

    while (last_pos != cur_pos) {
        uint16_t i = last_pos;
        // Check for frame header F4 F3 F2 F1
        if (dma_rx_buf[i % DMA_BUF_SIZE]       == 0xF4 &&
            dma_rx_buf[(i+1) % DMA_BUF_SIZE]   == 0xF3 &&
            dma_rx_buf[(i+2) % DMA_BUF_SIZE]   == 0xF2 &&
            dma_rx_buf[(i+3) % DMA_BUF_SIZE]   == 0xF1) {

            // Make sure full 23-byte frame is available
            uint16_t available = (cur_pos - i + DMA_BUF_SIZE) % DMA_BUF_SIZE;
            if (available < 23) return -1; // wait for more data

            uint8_t frame[23];
            for (int j = 0; j < 23; j++)
                frame[j] = dma_rx_buf[(i + j) % DMA_BUF_SIZE];

            mmwave_Data.state    = frame[8];
            mmwave_Data.distance = frame[15] | (frame[16] << 8);
            last_pos = (i + 23) % DMA_BUF_SIZE;
            return mmwave_Data.state;
        }
        last_pos = (last_pos + 1) % DMA_BUF_SIZE;
    }
    return -1; // no complete frame yet
}
