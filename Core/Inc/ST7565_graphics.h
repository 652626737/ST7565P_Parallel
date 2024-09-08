/*!
    @file ST7565_graphics.h
    @brief ERM19264 LCD driven by UC1609 controller, header file
        for the graphics based functions.
    @details Project Name: ERM19264_UC1609
        URL: <https://github.com/gavinlyonsrepo/ERM19264_UC1609>
    @author  Gavin Lyons
*/

#ifndef _ST7565_GRAPHICS_H
#define _ST7565_GRAPHICS_H

#include "stm32f1xx_hal.h" // 根据你的 STM32 系列选择合适的 HAL 头文件
#include <cstdlib>

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

#define ST7565_swap(a, b) \
    {                     \
        int16_t t = a;    \
        a = b;            \
        b = t;            \
    }

/*! LCD旋转模式，以度为单位。注意这个与LCD旋转命令的方法是分开的。 */
enum LCD_rotate_e : uint8_t
{
    LCD_Degrees_0 = 0, /**< 无旋转，0度 */
    LCD_Degrees_90 ,    /**< 旋转90度 */
    LCD_Degrees_180,   /**< 旋转180度 */
    LCD_Degrees_270  /**< 旋转270度 */
};

/*! LCD Enum to define return codes from some text and bitmap functions  */
enum LCD_Return_Codes_e : uint8_t
{
    LCD_Success = 0,                /**< Success!  */
    LCD_WrongFont = 2,              /**< Wrong Font selected for this method , There are two families of font included with different overloaded functions*/
    LCD_CharScreenBounds = 3,       /**< Text Character is out of Screen bounds, Check x and y*/
    LCD_CharFontASCIIRange = 4,     /**< Text Character is outside of chosen Fonts ASCII range, Check the selected Fonts ASCII range.*/
    LCD_CharArrayNullptr = 5,       /**< Text Character Array is an invalid pointer object */
    LCD_BitmapNullptr = 7,          /**< The Bitmap data array is an invalid pointer object */
    LCD_BitmapScreenBounds = 8,     /**< The bitmap starting point is outside screen bounds check x and y */
    LCD_BitmapLargerThanScreen = 9, /**< The Bitmap is larger than screen , check  w and h*/
    LCD_BitmapVerticalSize = 10,    /**< A vertical  Bitmap's height must be divisible by 8. */
    LCD_BitmapHorizontalSize = 11,  /**< A horizontal Bitmap's width  must be divisible by 8  */
};

/*! LCD Enum to define current font type selected  */
enum LCD_Font_Type_e : uint8_t
{
    UC1609Font_Default = 1,    /**< Default Font, Full extended ASCII */
    UC1609Font_Thick = 2,      /**< Thick font , no lower case letters*/
    UC1609Font_Seven_Seg = 3,  /**< Seven Segment Font */
    UC1609Font_Wide = 4,       /**< Wide font no lowercase letters*/
    UC1609Font_Tiny = 5,       /**< Tiny font */
    UC1609Font_Homespun = 6,   /**< Homespun Font */
    UC1609Font_Bignum = 7,     /**< Bignum numbers only */
    UC1609Font_Mednum = 8,     /**< Mednum number only */
    UC1609Font_ArialRound = 9, /**< Arial round font */
    UC1609Font_ArialBold = 10, /**< Arial bold font */
    UC1609Font_Mia = 11,       /**< Mia font */
    UC1609Font_Dedica = 12     /**< Dedica font */
};

/*! @brief Graphics class to hold graphic related functions */
class ST7565_graphics
{

public:
    ST7565_graphics(int16_t w, int16_t h);

    // 纯虚函数，要求子类实现，以在指定坐标绘制像素
    // 参数x和y指定像素坐标，color指定像素颜色
    virtual void drawPixel(int16_t x, int16_t y, uint8_t color) = 0;
    // 绘制直线
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);
    // 绘制一个填充矩形
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
    // 绘制一条垂直线的函数声明
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint8_t color);
    // 绘制水平线
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint8_t color);
    // 绘制矩形
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
    // 填充整个屏幕
    void fillScreen(uint8_t color);
    // 绘制圆形
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color);
    // 绘制圆形辅助函数
    void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint8_t color);
    // 填充圆形
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color);
    // 填充圆形辅助函数
    void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint8_t color);
    // 绘制三角形
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
    // 填充三角形
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
    // 绘制圆角矩形
    void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint8_t color);
    // 填充圆角矩形
    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint8_t color);
    // Bitmap相关函数
    LCD_Return_Codes_e drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t color, uint8_t bg);
    void setDrawBitmapAddr(bool mode);


    // 文本相关函数
    void setCursor(int16_t x, int16_t y);
    void setTextColor(uint8_t c);
    void setTextColor(uint8_t c, uint8_t bg);
    void setTextSize(uint8_t s);
    void setTextWrap(bool w);
    void setFontNum(LCD_Font_Type_e FontNumber);
    LCD_Return_Codes_e drawChar(uint8_t x, uint8_t y, uint8_t c, uint8_t color, uint8_t bg);
    LCD_Return_Codes_e drawText(uint8_t x, uint8_t y, char *pText, uint8_t color, uint8_t bg);
    LCD_Return_Codes_e drawChar(int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t s);
    LCD_Return_Codes_e drawText(uint8_t x, uint8_t y, char *pTxt, uint8_t c, uint8_t bg, uint8_t s);

    virtual uint32_t write(uint8_t);

    // Screen related
    int16_t height(void) const;
    int16_t width(void) const;
    void setRotation(LCD_rotate_e);
    LCD_rotate_e getRotation(void);

protected:
    const int16_t WIDTH;                     /**< This is the 'raw' display w - never changes */
    const int16_t HEIGHT;                    /**< This is the 'raw' display h - never changes*/
    int16_t _width;                          /**< Display w as modified by current rotation*/
    int16_t _height;                         /**< Display h as modified by current rotation*/
    int16_t _cursorX;                        /**< Current X co-ord cursor position */
    int16_t _cursorY;                        /**< Current Y co-ord cursor position */
    LCD_rotate_e LCD_rotate = LCD_Degrees_0; /**< Enum to hold rotation */

    uint8_t _textColor = 0x01;   /**< Text foreground color */
    uint8_t _textBgColor = 0x00; /**< Text background color */
    uint8_t _textSize = 1;       /**< Size of text ,fonts 1-6 */
    bool _textWrap;              /**< If set, 'Wrap' text at right edge of display*/

    bool drawBitmapAddr; /**< data addressing mode for method drawBitmap, True-vertical, false-horizontal */

private:
    /*! Width of the font in bits each representing a bytes sized column*/
    enum LCD_Font_width_e : uint8_t
    {
        UC1609Font_width_3 = 3,  /**< 3 tiny font */
        UC1609Font_width_4 = 4,  /**< 4 seven segment font */
        UC1609Font_width_5 = 5,  /**< 5 default font */
        UC1609Font_width_6 = 6,  /**< 12 dedica font  */
        UC1609Font_width_7 = 7,  /**< 6 homespun & 2 thick font*/
        UC1609Font_width_8 = 8,  /**< 8 wide & mia font*/
        UC1609Font_width_16 = 16 /**< 16 font 7-10*/
    };

    /*! font offset in the ASCII table*/
    enum LCD_Font_offset_e : uint8_t
    {
        UC1609Font_offset_none = 0x00,  /**< Full extended ASCII */
        UC1609Font_offset_space = 0x20, /**< Starts at Space, alphanumeric */
        UC1609Font_offset_minus = 0x2D, /**< Starts at Minus, extended numeric */
        UC1609Font_offset_zero = 0x30   /**< Starts at zero, numeric */
    };

    /*! Height of the font in bits*/
    enum LCD_Font_height_e : uint8_t
    {
        UC1609Font_height_8 = 8,   /**< 8 bit font 1-6 at size 1*/
        UC1609Font_height_12 = 12, /**< 12 bit font 12 */
        UC1609Font_height_16 = 16, /**< 16 bit font 8, 10 11*/
        UC1609Font_height_24 = 24, /**< 24 bit font 9 */
        UC1609Font_height_32 = 32  /**< 32 bit font 7 */
    };

    /*! Number of ASCII characters in Font */
    enum LCD_Font_Length_e : uint8_t
    {
        UC1609FontLenNumeric = 14,         /**< extended Numeric 0x2D - 0x3A */
        UC1609FontLenAlphaNumNoLCase = 59, /**< reduced Alphanumeric 0x20-0x5A*/
        UC1609FontLenAlphaNum = 95,        /**< Full Alphanumeric 0x20-0x7E */
        UC1609FontLenHalf = 128,           /**< Full Range  0-127*/
        UC1609FontLenAll = 255             /**< Full Range  0-0xFF */
    };

    uint8_t _FontNumber = 1;          /**< Store current font */
    uint8_t _CurrentFontWidth = 5;    /**< Store current font width */
    uint8_t _CurrentFontoffset = 0;   /**< Store current offset width */
    uint8_t _CurrentFontheight = 8;   /**< Store current offset height */
    uint8_t _CurrentFontLength = 128; /**< Store current font number of characters */
};

#endif
