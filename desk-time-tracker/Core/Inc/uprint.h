/*
 * uprint.h
 *
 *  Created on: Jan 29, 2026
 *      Author: nhrob
 *
 *  Simple uart terminal printing function
 */

#ifndef INC_UPRINT_H_
#define INC_UPRINT_H_

#include "stm32c0xx_hal.h"
#include "util.h"
#include "main.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

void uprint(char* str);

void uprintf(char* str, ...);

#endif /* INC_UPRINT_H_ */
