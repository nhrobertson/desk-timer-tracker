/*
 * uprint.c
 *
 *  Created on: Jan 29, 2026
 *      Author: nhrob
 */
#include "uprint.h"


void uputc(char c) {
	//HAL_UART_Transmit(&huart2, (uint8_t*)&c, 1, HAL_MAX_DELAY);
}

void uputs(char* str) {
	//HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

static void uprint_int(int val) {
    char buf[12]; // enough for int32
    int i = 0;
    int neg = 0;

    if (val == 0) {
        uputc('0');
        return;
    }

    if (val < 0) {
        neg = 1;
        val = -val;
    }

    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }

    if (neg)
        buf[i++] = '-';

    while (i--)
        uputc(buf[i]);
}

static void uprint_hex(uint32_t val) {
	char buf[8]; //Supports a 32 bit value in hex
	int i = 0;

	if (val == 0) {
		uputs("0x0");
		return;
	}

	while (val > 0) {
		uint8_t nib = val & NIBBLE_MASK;
		char nibChar = nibbleToChar(nib);
		buf[i++] = nibChar;
		val = val >> 4;
	}

	uputs("0x");
	while (i--) {
		uputc(buf[i]);
	}
}

static void uprint_double(double val, uint8_t precision) {
    // Handle sign
    if (val < 0.0) {
        uputc('-');
        val = -val;
    }

    // Integer part
    uint32_t int_part = (uint32_t)val;
    uprint_int(int_part);

    uputc('.');

    // Fractional part
    double frac = val - (double)int_part;

    for (uint8_t i = 0; i < precision; i++) {
        frac *= 10.0;
        uint8_t digit = (uint8_t)frac;
        uputc('0' + digit);
        frac -= digit;
    }
}

//Inspired by the minprintf function from the
//The C ANSI Programming Language
//
//Supports Integer, Hex, and Floating Point
void uprintf(char* unformatted_str, ...) {
	va_list ap;
	char *str;
	int ival;
	double dval;

	va_start(ap, unformatted_str);
	for (str = unformatted_str; *str; str++) {
		if (*str != '%') {
			uputc(*str);
			continue;
		}
		switch (*++str) {
		case 'd':
			//Integer
			ival = va_arg(ap, int);
			uprint_int(ival);
			break;
		case 'x':
			ival = va_arg(ap, int);
			uprint_hex((uint32_t)ival);
			break;
		case 'f':
			dval = va_arg(ap, double);
			uprint_double(dval, 5);
			break;
		}
	}

	va_end(ap);
}
