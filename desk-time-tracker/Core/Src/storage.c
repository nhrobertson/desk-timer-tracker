/*
 * storage.c
 *
 *  Created on: Mar 19, 2026
 *      Author: nhrob
 */

#include "storage.h"

int __load_fromFlash(uint32_t* averages, uint32_t* beeper) {
  if (stored_avgs[0] != 0xFFFFFFFF) {
	memcpy(averages, stored_avgs, AVERAGE_COUNT * sizeof(uint32_t));  // 256 bytes
	memcpy(beeper, (const uint32_t *)BEEPER_STORAGE_ADDR, sizeof(uint32_t));
    return 1;
  }
  return 0;
}

HAL_StatusTypeDef __flash_store(uint32_t* averages, uint32_t count, uint32_t* beeper) {
  HAL_StatusTypeDef status;

  HAL_FLASH_Unlock();

  FLASH_EraseInitTypeDef erase = {
    .TypeErase = FLASH_TYPEERASE_PAGES,
    .Page      = 15,
    .NbPages   = 1
  };

  uint32_t pageError = 0;
  status = HAL_FLASHEx_Erase(&erase, &pageError);
  if (status != HAL_OK) {
    HAL_FLASH_Lock();
    return status;
  }

  uint32_t addr = STORAGE_START_ADDR; //Set to starting address of the block
  for (int i = 0; i < AVERAGE_COUNT; i += 2) {
    uint64_t double_word = (uint64_t)averages[i] | ((uint64_t)averages[i + 1] << 32);
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, double_word);
    if (status != HAL_OK) break;
    addr += 8; //Increment address by 8
  }
  
  status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, (uint64_t)(*beeper));

  HAL_FLASH_Lock();
  return status;
}
