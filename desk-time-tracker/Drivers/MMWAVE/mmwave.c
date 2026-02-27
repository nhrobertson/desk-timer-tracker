/*
 * mmwave.c
 *
 *  Created on: Jan 26, 2026
 *      Author: noahhr
 */

#include "mmwave.h"

mmwave_Data_t mmwave_Data;
uint8_t tx_buff[32];


void mmwave_Write_Command(mmwave_Packet packet, UART_HandleTypeDef huart1) {

	tx_buff[0] = packet.header;
	tx_buff[1] = packet.data_len;
	tx_buff[2] = packet.cmd_word;
	for (int i = 0; i < data_len; ++i) {
		tx_buff[3 + i] = packet.cmd_value[i];
	}
	tx_buff[3 + packet.data_len + 1] = packet.footer;
	HAL_UART_Transmit(&huart, tx_buff, packet.data_len, 1000);
}

void mmwave_Init(UART_HandleTypeDef huart1) {

}

int mmwave_Recieve(UART_HandleTypeDef huart1) {
	//Need to intakes data seeing that 0xF4F3F2F1 is the headers
	//Next 2 bytes is the length n
	//n bytes to parse (with the first two denoting the state)
	//Final Sequence of 0xF8F7F6F5

	if(HAL_UART_Receive(&huart1, mmwave_Data.buf, 23, 1000)==HAL_OK) {
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
	}
}

int mmwave_Parse() {
	mmwave_Data.state = mmwave_Data.buf[6];
	return mmwave_Data.state;
}

