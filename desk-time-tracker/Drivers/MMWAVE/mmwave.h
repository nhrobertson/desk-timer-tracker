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

#define HEADER_SEQUENCE 0xf4f3f2f1
//Header
//0xF4F3F2F1

//Final Sequence
//0xF8F7F6F5

typedef struct {
    uint8_t state; //0 - None, 1 - Campaign, 2 - Stationary, 3 - Both, any other is Invalid
    uint8_t buf[BUFFER_SIZE];
} mmwave_Data_t;

extern mmwave_Data_t mmwave_Data;


void mmwave_Init(void);
int mmwave_Recieve(UART_HandleTypeDef huart1);
int mmwave_Parse();


#endif /* MMWAVE_MMWAVE_H_ */
