#ifndef __ST7565P_H_
#define __ST7565P_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f1xx_hal.h"

#include "stdlib.h"
#include "string.h"

#include "stdio.h"
#include "stm32f1xx_ll_gpio.h"

#define ST7565_PARALLEL

#define LCD_WIDTH 128
#define LCD_HEIGHT 64

#define LCD_WIDTH_logo 128
#define LCD_HEIGHT_logo 8

#define CMD_DISPLAY_OFF 0xAE         // 关闭显示
#define CMD_DISPLAY_ON 0xAF          // 开启显示
#define CMD_SET_DISP_START_LINE 0x40 // 设置显示起始行
#define CMD_SET_PAGE 0xB0            // 设置页地址
#define CMD_SET_COLUMN_UPPER 0x10    // 设置列地址高位
#define CMD_SET_COLUMN_LOWER 0x00    // 设置列地址低位
#define CMD_SET_ADC_NORMAL 0xA0      // 设置ADC正常模式
#define CMD_SET_ADC_REVERSE 0xA1     // 设置ADC反转模式

#define CMD_SET_DISP_NORMAL 0xA6  // 设置显示正常模式
#define CMD_SET_DISP_REVERSE 0xA7 // 设置显示反转模式

#define CMD_SET_ALLPTS_NORMAL 0xA4  // 设置所有点正常模式
#define CMD_SET_ALLPTS_ON 0xA5      // 设置所有点亮模式
#define CMD_SET_BIAS_9 0xA2         // 设置9偏置模式
#define CMD_SET_BIAS_7 0xA3         // 设置7偏置模式
#define CMD_RMW 0xE0                // 读-修改-写模式
#define CMD_RMW_CLEAR 0xEE          // 清除读-修改-写模式
#define CMD_INTERNAL_RESET 0xE2     // 内部复位
#define CMD_SET_COM_NORMAL 0xC0     // 设置COM扫描方向正常
#define CMD_SET_COM_REVERSE 0xC8    // 设置COM扫描方向反转
#define CMD_SET_POWER_CONTROL 0x28  // 设置电源控制
#define CMD_SET_RESISTOR_RATIO 0x20 // 设置电阻比率
#define CMD_SET_VOLUME_FIRST 0x81   // 设置音量第一部分
#define CMD_SET_VOLUME_SECOND 0     // 设置音量第二部分
#define CMD_SET_STATIC_OFF 0xAC     // 关闭静态指示器
#define CMD_SET_STATIC_ON 0xAD      // 开启静态指示器
#define CMD_SET_STATIC_REG 0x0      // 设置静态寄存器
#define CMD_SET_BOOSTER_FIRST 0xF8  // 设置升压第一部分
#define CMD_SET_BOOSTER_234 0       // 设置升压234部分
#define CMD_SET_BOOSTER_5 1         // 设置升压5部分
#define CMD_SET_BOOSTER_6 3         // 设置升压6部分
#define CMD_NOP 0xE3                // 无操作
#define CMD_TEST 0xF0               // 测试命令

#define ST7565_STARTBYTES 0
#define DELAY_US 2

    typedef enum
    {
        ST7565_COLOR_BLACK = 0x00,
        ST7565_COLOR_WHITE = 0x01
    } ST7565_COLOR_t;

    typedef struct
    {                            // Data stored PER GLYPH
        uint16_t bitmapOffset;   // Pointer into GFXfont->bitmap
        uint8_t width, height;   // Bitmap dimensions in pixels
        uint8_t xAdvance;        // Distance to advance cursor (x axis)
        int8_t xOffset, yOffset; // Dist from cursor position to UL corner
    } GFXglyph;

    typedef struct
    {                        // Data stored for FONT AS A WHOLE:
        uint8_t *bitmap;     // Glyph bitmaps, concatenated
        GFXglyph *glyph;     // Glyph array
        uint8_t first, last; // ASCII extents
        uint8_t yAdvance;    // Newline distance (y axis)
    } GFXfont;

    typedef struct
    {
        GPIO_TypeDef *port; // GPIO 端口
        uint16_t pin;       // GPIO 引脚
    } gpio_pin_t;

    typedef struct
    {
        gpio_pin_t cs1;          // 片选引脚
        gpio_pin_t reset;        // 复位引脚
        gpio_pin_t a0;           // 命令/数据引脚
        gpio_pin_t wr;           // 写使能引脚
        gpio_pin_t rd;           // 读使能引脚
        gpio_pin_t data_pins[8]; // 数据引脚
    } st7565_lcd_dev_t;

    void ST7565_init(st7565_lcd_dev_t * lcd);
    static void ST7565_write(uint8_t data);
    static void ST7565_send_command(uint8_t command);
    static void ST7565_send_data(uint8_t data);
    static void LCD_SetPage(uint8_t page);
    static void LCD_SetColumn(uint8_t column);
    static void ST7565_gpio_init(gpio_pin_t * pins, size_t num_pins);
    void LCD_Contrast(uint8_t set_contrast);
    void LCD_ON(void);
    void LCD_OFF(void);
    void LCD_UpdateScreen(void);
    void LCD_Mode(uint8_t set_mode);
    void LCD_Sleep_Enable(void);
    void LCD_Sleep_Disable(void);
    void LCD_ToggleInvert(void);
    void LCD_Fill(ST7565_COLOR_t color);
    void LCD_Pixel(uint16_t x, uint16_t y, ST7565_COLOR_t color);
    void LCD_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);
    void LCD_Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, ST7565_COLOR_t color);
    void LCD_Rect_Fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, ST7565_COLOR_t color);
    void LCD_Triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ST7565_COLOR_t color);
    void LCD_Circle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint8_t size, ST7565_COLOR_t color);
    void LCD_Triangle_Fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ST7565_COLOR_t color);
    void LCD_Ellipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint8_t fill, ST7565_COLOR_t color);
    void LCD_Circle(uint16_t x, uint16_t y, uint8_t radius, uint8_t fill, uint8_t size, ST7565_COLOR_t color);
    void LCD_Rect_Round(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint16_t r, uint8_t size, ST7565_COLOR_t color24);
    void LCD_Rect_Round_Fill(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint16_t r, ST7565_COLOR_t color);
    void LCD_Char(int16_t x, int16_t y, const GFXglyph *glyph, const GFXfont *font, uint8_t size, ST7565_COLOR_t color);
    void LCD_Font(uint16_t x, uint16_t y, const char *text, const GFXfont *p_font, uint8_t size, ST7565_COLOR_t color);
    void LCD_DrawIcon_phone(u_int8_t level);
    void LCD_DrawIcon_Signal(u_int8_t level);
    void LCD_DrawIcon_Printer(u_int8_t status);
    void LCD_DrawIcon_Printer(u_int8_t status);
    void LCD_DrawIcon_Battery(uint8_t level);
    void LCD_DrawIcon_Lock(uint8_t status);
    void LCD_DrawIcon_Upload(uint8_t status);
    void LCD_DrawIcon_Download(uint8_t status);
    void LCD_DrawIcon_Card(uint8_t status);



#ifdef __cplusplus
}
#endif

#endif