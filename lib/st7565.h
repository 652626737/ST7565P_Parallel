// st7565.h

#ifndef ST7565_H
#define ST7565_H

#include "st7565_graphics.h"
#include "stm32f1xx_hal.h"

// LCD dimensions
#define ST7565_WIDTH  128
#define ST7565_HEIGHT 64

// Command definitions
#define ST7565_CMD_DISPLAY_OFF           0xAE
#define ST7565_CMD_DISPLAY_ON            0xAF
#define ST7565_CMD_SET_DISP_START_LINE   0x40
#define ST7565_CMD_SET_PAGE              0xB0
#define ST7565_CMD_SET_COLUMN_UPPER      0x10
#define ST7565_CMD_SET_COLUMN_LOWER      0x00
#define ST7565_CMD_SET_ADC_NORMAL        0xA0
#define ST7565_CMD_SET_ADC_REVERSE       0xA1
#define ST7565_CMD_SET_DISP_NORMAL       0xA6
#define ST7565_CMD_SET_DISP_REVERSE      0xA7
#define ST7565_CMD_SET_ALLPTS_NORMAL     0xA4
#define ST7565_CMD_SET_ALLPTS_ON         0xA5
#define ST7565_CMD_SET_BIAS_9            0xA2
#define ST7565_CMD_SET_BIAS_7            0xA3
#define ST7565_CMD_RMW                   0xE0
#define ST7565_CMD_RMW_CLEAR             0xEE
#define ST7565_CMD_INTERNAL_RESET        0xE2
#define ST7565_CMD_SET_COM_NORMAL        0xC0
#define ST7565_CMD_SET_COM_REVERSE       0xC8
#define ST7565_CMD_SET_POWER_CONTROL     0x28
#define ST7565_CMD_SET_RESISTOR_RATIO    0x20
#define ST7565_CMD_SET_VOLUME_FIRST      0x81
#define ST7565_CMD_SET_VOLUME_SECOND     0
#define ST7565_CMD_SET_STATIC_OFF        0xAC
#define ST7565_CMD_SET_STATIC_ON         0xAD
#define ST7565_CMD_SET_STATIC_REG        0x0
#define ST7565_CMD_SET_BOOSTER_FIRST     0xF8
#define ST7565_CMD_SET_BOOSTER_234       0
#define ST7565_CMD_SET_BOOSTER_5         1
#define ST7565_CMD_SET_BOOSTER_6         3
#define ST7565_CMD_NOP                   0xE3
#define ST7565_CMD_TEST                  0xF0

// GPIO pin structure
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} ST7565_GPIO_t;

// ST7565 LCD structure
typedef struct {
    ST7565_Graphics graphics;
    ST7565_GPIO_t cs;
    ST7565_GPIO_t rst;
    ST7565_GPIO_t a0;
    ST7565_GPIO_t wr;
    ST7565_GPIO_t rd;
    ST7565_GPIO_t data[8];
} ST7565_LCD;

// Function prototypes

// Initialize the ST7565 LCD
void ST7565_Init(ST7565_LCD* lcd);

// Write a command to the LCD
void ST7565_WriteCommand(ST7565_LCD* lcd, uint8_t command);

// Write data to the LCD
void ST7565_WriteData(ST7565_LCD* lcd, uint8_t data);

// Set the contrast of the LCD
void ST7565_SetContrast(ST7565_LCD* lcd, uint8_t contrast);

// Turn the display on
void ST7565_DisplayOn(ST7565_LCD* lcd);

// Turn the display off
void ST7565_DisplayOff(ST7565_LCD* lcd);

// Update the display with the contents of the graphics buffer
void ST7565_UpdateDisplay(ST7565_LCD* lcd);

// Clear the display
void ST7565_Clear(ST7565_LCD* lcd);

// Draw a single pixel on the display
void ST7565_DrawPixel(ST7565_LCD* lcd, int16_t x, int16_t y, uint8_t color);

// Draw a line on the display
void ST7565_DrawLine(ST7565_LCD* lcd, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);

// Draw a rectangle on the display
void ST7565_DrawRect(ST7565_LCD* lcd, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);

// Fill a rectangle on the display
void ST7565_FillRect(ST7565_LCD* lcd, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);

#endif // ST7565_H
