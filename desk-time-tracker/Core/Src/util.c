/*
 * util.c
 *
 *  Created on: Jan 29, 2026
 *      Author: nhrob
 */

#include "util.h"

char nibbleToChar(uint8_t nib) {
    switch (nib & 0x0F) {
        case 0x0: return '0';
        case 0x1: return '1';
        case 0x2: return '2';
        case 0x3: return '3';
        case 0x4: return '4';
        case 0x5: return '5';
        case 0x6: return '6';
        case 0x7: return '7';
        case 0x8: return '8';
        case 0x9: return '9';
        case 0xA: return 'A';
        case 0xB: return 'B';
        case 0xC: return 'C';
        case 0xD: return 'D';
        case 0xE: return 'E';
        case 0xF: return 'F';
        default:  return '?';  // should never happen
    }
}
