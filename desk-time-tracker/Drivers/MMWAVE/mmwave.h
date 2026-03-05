/*
 * mmwave.h
 *
 *  Created on: Jan 25, 2026
 *      Author: noahhr
 */

#ifndef MMWAVE_MMWAVE_H_
#define MMWAVE_MMWAVE_H_

#include <stdint.h>
#include "stm32c0xx_hal.h"

#define BAUD_RATE 256000 //1 stop bit, no parity

#define MMWAVE_UART_PORT huart1

#define BUFFER_SIZE 23 //The size of a data frame in default mode

#define MASK(pos) (1UL << pos)


//Configuration

//Send command protocol
//Frame Header | 2 byte intra-frame data length | Intra-frame data | End of frame

#define MMWAVE_CONFIG_HEADER 0xFDFCFBFA
#define MMWAVE_CONFIG_FOOTER 0x04030201

#define MMWAVE_ENABLE_CONFIG_CW 0x00FF
#define MMWAVE_ENABLE_CONFIG_CV 0x0001

#define MMWAVE_DISABLE_CONFIG_CW 0x00FE

#define MMWAVE_MAX_DIST_GATE_CW 0x0060
//Command Value -> 2 byte max motion distance gate word + 4 byte max motion distance gate param + 2 byte max standstill distance gate word + 4 byte max standstill distance gate param + 2 byte unoccupied duration word + 4 byte unoccupied duration param

#define MMWAVE_DISTANCE_RESOLUTION_CW 0x00AA
//Command Value -> 2 byte distance resolution selection index (0x0000 -> 0.75m, 0x0001 -> 0.2m (but fd is 0x0001 which is 0.75m))

#define PADDING 10

#define HEADER_SEQUENCE 0xF4F3F2F1
//Header
//0xF4F3F2F1

//Final Sequence
//0xF8F7F6F5

typedef struct {
	uint32_t header;
	uint16_t data_len;
	uint16_t cmd_word;
	uint8_t cmd_value[32];
	uint32_t footer;
} mmwave_Packet;

typedef struct {
    uint8_t state; //0 - None, 1 - Campaign, 2 - Stationary, 3 - Both, any other is Invalid
    uint8_t buf[BUFFER_SIZE];
} mmwave_Data_t;

extern mmwave_Data_t mmwave_Data;

void mmwave_Write_Command(mmwave_Packet* packet, UART_HandleTypeDef* huart1);
int mmwave_Init(UART_HandleTypeDef* huart1);
int mmwave_Recieve(UART_HandleTypeDef* huart1);
int mmwave_Parse();


#endif /* MMWAVE_MMWAVE_H_ */
