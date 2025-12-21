#import "oled.h"

/**
 * Taken and Modified directly from:
 * https://github.com/afiskon/stm32-ssd1306/blob/master/ssd1306/ssd1306.c
 *
 *
 */

//Write Command
void oled_WriteCommand(uint8_t byte) {
    HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY);
}

//Send Data
void oled_WriteData(uint8_t* buffer, size_t buff_size) {
    HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY);
}



//Fill Buffer
//Returns a uint8_t
//Anything besides 0 is an ERR
uint8_t oled_FillBuffer(uint8_t* buffer, uint32_t len) {
	uint8_t ret = 1;
	if (len <= OLED_BUFFER_SIZE) {
		memcpy(OLED_Buffer, buffer, len);
		ret = 0;
	}
	return ret;
}

static uint8_t OLED_Buffer[OLED_BUFFER_SIZE];

static OLED_t oled;

void oled_SetDisplayOn(const uint8_t on) {
	uint8_t val;
	if (on) {
		val = 0xAF;
		oled.DisplayOn = 1;
	} else {
		val = 0xAE;
		oled.DisplayOn = 0;
	}
	oled_WriteCommand(val);
}


