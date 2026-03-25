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

void timestampToChar(char* str, int32_t timestamp) {
  
  uint8_t hours = 0;
  uint8_t minutes = 0;

  getHoursMins(&hours, &minutes, (uint32_t)timestamp);

  sprintf(str, "%02d:%02d", hours, minutes);
}

void getHoursMins(uint8_t* hours8b, uint8_t* mins8b, uint32_t timestamp) {
  int32_t minutes = timestamp / 60;
  int32_t hours = minutes / 60;
  
  minutes = minutes % 60;

  if (hours > 99) {
    hours = 0;
  }
  
  *hours8b = ((uint8_t)hours);
  *mins8b  = ((uint8_t)minutes);
}

uint32_t hoursMinsToTimestamp(uint8_t hours, uint8_t minutes) {
  return (hours*60 + minutes) * 60;
}

int32_t getActualAverage(uint32_t* avg_arr, uint16_t* avg_count) {
    int32_t avg = 0;
    uint16_t count = 0;  // local count, don't touch the struct's counter

    for (int i = 0; i < 64; ++i) {
        if (avg_arr[i] != 0xFFFFFFFF && avg_arr[i] != 0x0) {
            avg += avg_arr[i];
            count++;
        }
    }

    *avg_count = count;  // set once, don't increment
    if (count == 0) return 0;
    return avg / count;
}
