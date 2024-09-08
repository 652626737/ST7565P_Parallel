/*!
    @file ST7565_Parallel.h
    @brief ST7565 LCD driven by UC1609C controller, parallel interface, for STM32Cube.
    @author [Your Name]
    @details Adapted for parallel interface and STM32Cube framework.
*/

#ifndef ST7565_PARALLEL_H
#define ST7565_PARALLEL_H

#include "main.h"
#include "ST7565_graphics.h"
#include "stm32f1xx_hal.h" // 根据你的 STM32 系列调整
#include "us_delay.h"
#include <cstring> // 或者 #include <string.h

// 显示器像素颜色定义
#define FOREGROUND 1
#define BACKGROUND 0
#define COLORINVERSE 2

// UC1609 命令寄存器定义
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

// Rotate
#define UC1609_ROTATION_FLIP_TWO 0x06   /**< mirror image Rotation LCD command*/
#define UC1609_ROTATION_NORMAL 0x04     /**< Normal Rotation LCD command*/
#define UC1609_ROTATION_FLIP_ONE 0x02   /**< 180 degree  Rotation LCD command*/
#define UC1609_ROTATION_FLIP_THREE 0x00 /**< mirror image Rotation LCD command*/

// 延迟定义
#define UC1609_RESET_DELAY 2    /**<  ms Rest Delay ,datasheet >3uS */
#define UC1609_RESET_DELAY2 0   /**< mS reset delay datasheet says > 5mS, does not work */
#define UC1609_INIT_DELAY 100   /**<  mS init delay ,after init  */
#define UC1609_INIT_DELAY2 5    /**<  mS init delay,  before reset called datasheet <3mS */
#define UC1609_HIGHFREQ_DELAY 0 /**< uS  delay, Can be used in software SPI for high freq MCU*/
// ... 其他延迟定义 ...

// 并口 LCD 控制引脚抽象

#define LCD_CS_HIGH() HAL_GPIO_WritePin(_LCD_CS.port, _LCD_CS.pin, GPIO_PIN_SET)
#define LCD_CS_LOW() HAL_GPIO_WritePin(_LCD_CS.port, _LCD_CS.pin, GPIO_PIN_RESET)

#define LCD_RST_HIGH() HAL_GPIO_WritePin(_LCD_RST.port, _LCD_RST.pin, GPIO_PIN_SET)
#define LCD_RST_LOW() HAL_GPIO_WritePin(_LCD_RST.port, _LCD_RST.pin, GPIO_PIN_RESET)

#define LCD_DC_HIGH() HAL_GPIO_WritePin(_LCD_DC.port, _LCD_DC.pin, GPIO_PIN_SET)
#define LCD_DC_LOW() HAL_GPIO_WritePin(_LCD_DC.port, _LCD_DC.pin, GPIO_PIN_RESET)

#define LCD_WR_HIGH() HAL_GPIO_WritePin(_LCD_WR.port, _LCD_WR.pin, GPIO_PIN_SET)
#define LCD_WR_LOW() HAL_GPIO_WritePin(_LCD_WR.port, _LCD_WR.pin, GPIO_PIN_RESET)

#define LCD_RD_HIGH() HAL_GPIO_WritePin(_LCD_RD.port, _LCD_RD.pin, GPIO_PIN_SET)
#define LCD_RD_LOW() HAL_GPIO_WritePin(_LCD_RD.port, _LCD_RD.pin, GPIO_PIN_RESET)

// 数据引脚定义（假设使用整个 GPIO 端口作为数据总线）
// #define LCD_DATA_PORT GPIOB
// #define LCD_DATA_PINS 0xFF // 假设使用整个 8 位端口

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
} LcdDataPin;

/*! @brief class to hold screen data , multiple screens can be made for the shared buffer. Buffer must be same size and offsets to if saving Data memory is goal
 */
class ST7565_Parallel_Screen
{
public:
    ST7565_Parallel_Screen(uint8_t *mybuffer, uint8_t w, uint8_t h, int16_t x, int16_t y);
    ~ST7565_Parallel_Screen() {};

    uint8_t *screenBuffer; /**<  pointer to shared buffer screen data */
    uint8_t width;         /**< buffer x size in pixels  */
    uint8_t height;        /**< buffer y size in pixels */
    int16_t xoffset;       /**< x offset in pixels */
    int16_t yoffset;       /**< y offset in pixels */
private:
};

class ST7565_Parallel : public ST7565_graphics
{
public:
    ST7565_Parallel(int16_t width, int16_t height, LcdDataPin cs, LcdDataPin rst, LcdDataPin dc, LcdDataPin wr, LcdDataPin rd, LcdDataPin *dataPins);
    ~ST7565_Parallel() = default;

    virtual void drawPixel(int16_t x, int16_t y, uint8_t colour) override;
    void LCDupdate(void);
    void LCDclearBuffer(void);
    void LCDBuffer(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t *data);
    void LCDBuffer_Icon(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t *data);
    void LCDbegin(uint8_t _VbiasPot = _VbiasPOT, uint8_t _AddressSet = _AddressCtrl);
    void LCDinit(void);
    void LCD_Mode(uint8_t set_mode);
    void LCD_Sleep_Enable(void);
    void LCD_Sleep_Disable(void);
    // void LCDEnable(uint8_t on);
    void LCDFillScreen(uint8_t pixel);
    void LCDFillPage(uint8_t pixels);
    void LCD_Contrast(uint8_t val);
    void LCDGotoXY(uint8_t column, uint8_t page);
    // void LCDrotate(uint8_t rotatevalue);
    void LCDInvertDisplay(uint8_t on);
    void LCDallpixelsOn(uint8_t bits);
    // void LCDscroll(uint8_t bits);
    void LCDReset(void);
    LCD_Return_Codes_e LCDBitmap(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t *data);
    // void LCDPowerDown(void);

    uint16_t LCDLibVerNumGet(void);
    uint16_t LCDHighFreqDelayGet(void);
    void LCDHighFreqDelaySet(uint16_t);
    uint8_t LCDGetConstrast(void);
    uint8_t LCDGetAddressCtrl(void);

    void LCD_DrawIcon_Battery(uint8_t level);
    void LCD_DrawIcon_Lock(uint8_t status);
    void LCD_DrawIcon_Upload(uint8_t status);
    void LCD_DrawIcon_Download(uint8_t status);
    void LCD_DrawIcon_Card(uint8_t status);

    void LCD_DrawIcon_phone(uint8_t icon);
    void LCD_DrawIcon_Signal(uint8_t level);
    void LCD_DrawIcon_Printer(uint8_t status);

    ST7565_Parallel_Screen *ActiveBuffer = nullptr;   /**< Active buffer pointer , a pointer to which screen object shared buffer will be written to */

    // void ST7565_init(st7565_lcd_dev_t *lcd);
    // void LCD_Contrast(uint8_t set_contrast);
    // void LCD_ON(void);
    // void LCD_OFF(void);
    // void LCD_UpdateScreen(void);

    // void LCD_ToggleInvert(void);
    // void LCD_Fill(ST7565_COLOR_t color);
    // void LCD_Pixel(int16_t x, int16_t y, uint8_t colour);
    // void LCD_Triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ST7565_COLOR_t color);
    // void LCD_Triangle_Fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ST7565_COLOR_t color);
    // void LCD_Ellipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint8_t fill, ST7565_COLOR_t color);
    // void LCD_Circle(uint16_t x, uint16_t y, uint8_t radius, uint8_t fill, uint8_t size, ST7565_COLOR_t color);
    // void LCD_Rect_Round(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint16_t r, uint8_t size, uint32_t color24);
    // void LCD_Rect_Round_Fill(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint16_t r, uint32_t color);
    // void LCD_Char(int16_t x, int16_t y, const GFXglyph *glyph, const GFXfont *font, uint8_t size, ST7565_COLOR_t color);
    // void LCD_Font(uint16_t x, uint16_t y, const char *text, const GFXfont *p_font, uint8_t size, ST7565_COLOR_t color);

    // void LCD_DrawIcon_Printer(u_int8_t status);
    // void LCD_DrawIcon_Battery(uint8_t level);
    // void LCD_DrawIcon_Lock(uint8_t status);
    // void LCD_DrawIcon_Upload(uint8_t status);
    // void LCD_DrawIcon_Download(uint8_t status);
    // void LCD_DrawIcon_Card(uint8_t status);

    // void ST7565_write(uint8_t data);
    // void send_command(uint8_t command, uint8_t value);
    // bool isHardwareSPI(void);
    // void CustomshiftOut(uint8_t bitOrder, uint8_t val);
private:
    void ST7565_write(uint8_t data);
    void ST7565_send_command(uint8_t command);
    void ST7565_send_data(uint8_t data);
    void LCD_SetPage(uint8_t page);
    void LCD_SetColumn(uint8_t column);
    // void ST7565_gpio_init(gpio_pin_t *pins, size_t num_pins);

    LcdDataPin _LCD_CS;
    LcdDataPin _LCD_RST;
    LcdDataPin _LCD_DC;
    LcdDataPin _LCD_WR;
    LcdDataPin _LCD_RD;
    LcdDataPin *_dataPins;

    static const uint8_t _VbiasPOT = 0x20;                        /**< Contrast default 0x49 datasheet 00-FE */
    static const uint8_t _AddressCtrl = 0x04;                     /**< Set AC [2:0] Program registers  for RAM address control. 0x00 to 0x07*/
    uint16_t _HighFreqDelay = UC1609_HIGHFREQ_DELAY; /**< uS GPIO Communications delay, SW SPI ONLY */
    uint16_t _LibVersionNum = 182;             /**< Library version number 180 = 1.8.0*/

    uint8_t _widthScreen = 128; /**< Width of screen in pixels */
    uint8_t _heightScreen = 64; /**< Height of screen in pixels */
    uint16_t _bufferSize = _widthScreen * (_heightScreen / 8);
    uint8_t *_InactiveBuffer;
    static const uint8_t _iconwidthScreen = 128; /**< Width of screen in pixels */
    static const uint8_t _iconheightScreen = 8;  /**< Height of screen in pixels */
};

#endif // ST7565_PARALLEL_H
