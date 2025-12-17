#ifndef OLED_H
#define OLED_H

#include "stm32c0xx_hal.h"
#include "models.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_I2C_ADDR 0x32 << 1

void oled_Init(I2C_HandleTypeDef *hi2c);
void oled_Clear();
void oled_Update();
void oled_DrawPixel(vec2_uint8_t pos, uint8_t color);


#endif
