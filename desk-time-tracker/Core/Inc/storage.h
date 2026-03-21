/*
 * storage.h
 *
 *  Created on: Mar 19, 2026
 *      Author: nhrob
 */

#ifndef INC_STORAGE_H_
#define INC_STORAGE_H_

#include <string.h>
#include "stm32c0xx_hal.h"


#define STORAGE_ADDR 0x8007800
#define STORAGE_OFFSET 256

#define STORAGE_START_ADDR (STORAGE_ADDR + STORAGE_OFFSET * sizeof(uint32_t)) //Add a 64 byte offset
#define AVERAGE_COUNT 64

#define BEEPER_STORAGE_ADDR  (STORAGE_START_ADDR + AVERAGE_COUNT * sizeof(uint32_t))

#define stored_avgs ((const uint32_t *)STORAGE_START_ADDR)

int __load_fromFlash(uint32_t averages[64], uint32_t* beeper);
HAL_StatusTypeDef __flash_store(uint32_t* averages, uint32_t count, uint32_t* beeper);


#endif /* INC_STORAGE_H_ */
