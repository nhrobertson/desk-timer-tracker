/*
 * util.h
 *
 *  Created on: Jan 29, 2026
 *      Author: nhrob
 *
 *  Utility functions
 */

#ifndef INC_UTIL_H_
#define INC_UTIL_H_

#include <stdint.h>
#include <stdio.h>

#define NIBBLE_MASK 0x0000000F //For an int32

char nibbleToChar(uint8_t nib);

void timestampToChar(char* str, int32_t timestamp);
void getHoursMins(uint8_t* hours, uint8_t* mins, uint32_t timestamp);

#endif /* INC_UTIL_H_ */
