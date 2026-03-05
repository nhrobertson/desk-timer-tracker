/*
 * mmwave.c
 *
 *  Created on: Jan 26, 2026
 *      Author: noahhr
 */

#include "mmwave.h"

mmwave_Data_t mmwave_Data;
uint8_t tx_buff[32];

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
	if(HAL_UART_Receive(huart1, mmwave_Data.buf, 32, 1000)==HAL_OK) {
		//Check for success.
		uint32_t header = (mmwave_Data.buf[3] << 24)
		                + (mmwave_Data.buf[2] << 16)
		                + (mmwave_Data.buf[1] << 8)
		                + (mmwave_Data.buf[0]);

		if (header != MMWAVE_CONFIG_HEADER) {
			return 1;
		}

		uint16_t data_len = (mmwave_Data.buf[5] << 8) + (mmwave_Data.buf[4]);
		uint16_t sent_cmd = (mmwave_Data.buf[7] << 8) + (mmwave_Data.buf[6]);
		uint16_t ack      = (mmwave_Data.buf[9] << 8) + (mmwave_Data.buf[8]);
		if (ack == 0) {
			return 0;
		} else {
			return 1;
		}
	} else {
		return 1;
	}
}

int mmwave_Init(UART_HandleTypeDef* huart1) {
	mmwave_Packet cmd_packet;
	cmd_packet.header = MMWAVE_CONFIG_HEADER;
	cmd_packet.footer = MMWAVE_CONFIG_FOOTER;

	//Enable configuration commands
	cmd_packet.data_len = 0x0004;
	cmd_packet.cmd_word = 0x00FF;
	cmd_packet.cmd_value[0] = 0x01;
	cmd_packet.cmd_value[1] = 0x00;

	mmwave_Write_Command(&cmd_packet, huart1);
	if (mmwave_Check_For_Ack(huart1) > 0) {
		//Err
		return 1;
	}

	//Set distance resolution to 0.2m
	cmd_packet.data_len = 0x0004;
	cmd_packet.cmd_word = 0x00AA;
	cmd_packet.cmd_value[0] = 0x01; //maybe 0.2m? Isn't really clear....
	cmd_packet.cmd_value[1] = 0x00;

	mmwave_Write_Command(&cmd_packet, huart1);
	if (mmwave_Check_For_Ack(huart1) > 0) {
		//Err
		return 1;
	}

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

	mmwave_Write_Command(&cmd_packet, huart1);
	if (mmwave_Check_For_Ack(huart1) > 0) {
		//Err
		return 1;
	}



	//End configuration
	cmd_packet.data_len = 0x0002;
	cmd_packet.cmd_word = 0x00FE;
	mmwave_Write_Command(&cmd_packet, huart1);
	if (mmwave_Check_For_Ack(huart1) > 0) {
		//Err
		return 1;
	}
	return 0;
}

int mmwave_Recieve(UART_HandleTypeDef* huart1) {
	//Need to intakes data seeing that 0xF4F3F2F1 is the headers
	//Next 2 bytes is the length n
	//n bytes to parse (with the first two denoting the state)
	//Final Sequence of 0xF8F7F6F5

	if(HAL_UART_Receive(huart1, mmwave_Data.buf, 23, 1000)==HAL_OK) {
		//Recieved a buffer of 64 bytes
		uint32_t temp = 0;
		for (int i = 0; i < 4; ++i) {
			temp = (temp << 8) | mmwave_Data.buf[i];
		}
		if (temp == HEADER_SEQUENCE ) {
			//Recieved correct header sequence, parse data
			int ret = mmwave_Parse();
			return ret;
		} else {
			//Invalid Header Sequence, skip data frame

			//TODO: Error Handling
			return -1;
		}
	} else {
		return -1;
	}
}

int mmwave_Parse() {
	mmwave_Data.state = mmwave_Data.buf[6];
	return mmwave_Data.state;
}

