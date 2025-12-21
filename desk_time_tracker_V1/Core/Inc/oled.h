#ifndef OLED_H
#define OLED_H

/**
 * Directly taken/modified from:
 * github.com/afiskon/stm32-ssd1306
 *
 * OLED screen is a ssd1306, renamed to OLED for simplification
 */
#include <string.h>
#include "stm32c0xx_hal.h"
#include "models.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_BUFFER_SIZE SCREEN_WIDTH * SCREEN_HEIGHT / 8

#define OLED_I2C_PORT hi2c1
#define OLED_I2C_ADDR 0x32 << 1

extern I2C_HandleTypeDef OLED_I2C_PORT;

typedef enum {
	black = 0x00,
	white = 0x01
} OLED_COLOR;

typedef struct {
	const uint8_t width;			//Font Width in Pixels
	const uint8_t height;			//Font Height in Pixels
	const uint16_t *const data;		//Pointer to font data array
	const uint8_t *const char_width;//Proportional character width in pixels
} OLED_Font_t;

typedef struct {
	uint16_t currentX;
	uint16_t currentY;
	uint8_t initialized;
	uint8_t displayOn;
} OLED_t;

//Main Accessor Functions for control
void oled_Init(void);
void oled_Fill(OLED_COLOR color);
void oled_Update(void);
void oled_DrawPixel(vec2_uint8_t pos, uint8_t color);
char oled_WriteChar(char ch, OLED_Font_t font, OLED_COLOR color);
char oled_WriteString(char* str, OLED_Font_t font, OLED_COLOR color);
//TODO: Custom implemenation of drawing "AVG section"
void oled_DrawAVGSection(OLED_COLOR color);


//Using I2C, not SPI - no reset needed
void oled_WriteCommand(uint8_t byte);
void oled_WriteData(uint8_t* buffer, size_t buffer_size);

void oled_SetDisplayOn(const uint8_t on);
uint8_t oled_GetDisplayOn();

uint8_t oled_FillBuffer(uint8_t* buffer,, uint32_t len);

#endif
