/*
 * mmwave.c
 *
 *  Created on: Jan 26, 2026
 *      Author: noahhr
 */

#include "mmwave.h"

mmwave_Data_t mmwave_Data;

void mmwave_Init() {

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

