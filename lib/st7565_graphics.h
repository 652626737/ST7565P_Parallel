/*!
    @file ST7565_graphics.h
    @brief ST7565 LCD graphics library header file for STM32Cube framework
    @details Project Name: ST7565_Graphics
    @author [Your Name]
*/

#ifndef ST7565_GRAPHICS_H
#define ST7565_GRAPHICS_H

#include <cstdint>

#define ST7565_BLACK 0
#define ST7565_WHITE 1

#define ST7565_swap(a, b) { int16_t t = a; a = b; b = t; }

/*! LCD rotate modes in degrees */
enum class ST7565_Rotation : uint8_t
{
    Degrees_0 = 0,   /**< No rotation 0 degrees */
    Degrees_90,      /**< Rotation 90 degrees */
    Degrees_180,     /**< Rotation 180 degrees */
    Degrees_270      /**< Rotation 270 degrees */
};

/*! LCD Enum to define return codes from some text and bitmap functions */
enum class ST7565_Return_Codes : uint8_t
{
    Success = 0,                /**< Success! */
    WrongFont = 2,              /**< Wrong Font selected for this method */
    CharScreenBounds = 3,       /**< Text Character is out of Screen bounds */
    CharFontASCIIRange = 4,     /**< Text Character is outside of chosen Fonts ASCII range */
    CharArrayNullptr = 5,       /**< Text Character Array is an invalid pointer object */
    BitmapNullptr = 7,          /**< The Bitmap data array is an invalid pointer object */
    BitmapScreenBounds = 8,     /**< The bitmap starting point is outside screen bounds */
    BitmapLargerThanScreen = 9, /**< The Bitmap is larger than screen */
    BitmapVerticalSize = 10,    /**< A vertical Bitmap's height must be divisible by 8 */
    BitmapHorizontalSize = 11   /**< A horizontal Bitmap's width must be divisible by 8 */
};

/*! LCD Enum to define current font type selected */
enum class ST7565_Font_Type : uint8_t
{
    Default = 1,    /**< Default Font, Full extended ASCII */
    Thick = 2,      /**< Thick font, no lower case letters */
    Seven_Seg = 3,  /**< Seven Segment Font */
    Wide = 4,       /**< Wide font no lowercase letters */
    Tiny = 5,       /**< Tiny font */
    Homespun = 6,   /**< Homespun Font */
    Bignum = 7,     /**< Bignum numbers only */
    Mednum = 8,     /**< Mednum number only */
    ArialRound = 9, /**< Arial round font */
    ArialBold = 10, /**< Arial bold font */
    Mia = 11,       /**< Mia font */
    Dedica = 12     /**< dedica font */
};

/*! @brief Graphics class to hold graphic related functions */
class ST7565_Graphics {
public:
    ST7565_Graphics(int16_t w, int16_t h);
    virtual ~ST7565_Graphics() = default;

    // This should be defined by the subclass:
    virtual void drawPixel(int16_t x, int16_t y, uint8_t color) = 0;

    // Shape related
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint8_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint8_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
    void fillScreen(uint8_t color);

    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color);
    void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint8_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color);
    void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint8_t color);
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
    void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint8_t color);
    void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint8_t color);

    // Bitmap related
    ST7565_Return_Codes drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t color, uint8_t bg);
    void setDrawBitmapAddr(bool mode);

    // Text related
    void setCursor(int16_t x, int16_t y);
    void setTextColor(uint8_t c);
    void setTextColor(uint8_t c, uint8_t bg);
    void setTextSize(uint8_t s);
    void setTextWrap(bool w);
    void setFont(ST7565_Font_Type font);
    ST7565_Return_Codes drawChar(int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size);
    ST7565_Return_Codes drawText(int16_t x, int16_t y, const char *text, uint8_t color, uint8_t bg, uint8_t size);

    // Screen related
    int16_t height() const;
    int16_t width() const;
    void setRotation(ST7565_Rotation rotation);
    ST7565_Rotation getRotation() const;

protected:
    const int16_t WIDTH;   /**< This is the 'raw' display w - never changes */
    const int16_t HEIGHT;  /**< This is the 'raw' display h - never changes */
    int16_t _width;        /**< Display w as modified by current rotation */
    int16_t _height;       /**< Display h as modified by current rotation */
    int16_t cursor_x;      /**< Current X co-ord cursor position */
    int16_t cursor_y;      /**< Current Y co-ord cursor position */
    ST7565_Rotation _rotation = ST7565_Rotation::Degrees_0; /**< Enum to hold rotation */

    uint8_t textcolor = ST7565_BLACK;    /**< Text foreground color */
    uint8_t textbgcolor = ST7565_WHITE;  /**< Text background color */
    uint8_t textsize = 1;                /**< Size of text, fonts 1-6 */
    bool textwrap;                       /**< If set, 'wrap' text at right edge of display */

    bool drawBitmapAddr; /**< data addressing mode for method drawBitmap, True-vertical, false-horizontal */

private:
    /*! Width of the font in bits each representing a bytes sized column */
    enum class Font_width : uint8_t
    {
        Tiny = 3,      /**< 3 tiny font */
        Seven_Seg = 4, /**< 4 seven segment font */
        Default = 5,   /**< 5 default font */
        Dedica = 6,    /**< 12 dedica font */
        Thick = 7,     /**< 6 homespun & 2 thick font */
        Wide = 8,      /**< 8 wide & mia font */
        Big = 16       /**< 16 font 7-10 */
    };

    /*! font offset in the ASCII table */
    enum class Font_offset : uint8_t
    {
        None = 0x00,  /**< Full extended ASCII */
        Space = 0x20, /**< Starts at Space, alphanumeric */
        Minus = 0x2D, /**< Starts at Minus, extended numeric */
        Zero = 0x30   /**< Starts at zero, numeric */
    };

    /*! Height of the font in bits */
    enum class Font_height : uint8_t
    {
        H8 = 8,   /**< 8 bit font 1-6 at size 1 */
        H12 = 12, /**< 12 bit font 12 */
        H16 = 16, /**< 16 bit font 8, 10 11 */
        H24 = 24, /**< 24 bit font 9 */
        H32 = 32  /**< 32 bit font 7 */
    };

    /*! Number of ASCII characters in Font */
    enum class Font_Length : uint8_t
    {
        Numeric = 14,         /**< extended Numeric 0x2D - 0x3A */
        AlphaNumNoLCase = 59, /**< reduced Alphanumeric 0x20-0x5A */
        AlphaNum = 95,        /**< Full Alphanumeric 0x20-0x7E */
        Half = 128,           /**< Full Range  0-127 */
        All = 255             /**< Full Range  0-0xFF */
    };

    ST7565_Font_Type _currentFont = ST7565_Font_Type::Default; /**< Store current font */
    uint8_t _currentFontWidth = 5;     /**< Store current font width */
    uint8_t _currentFontOffset = 0;    /**< Store current offset width */
    uint8_t _currentFontHeight = 8;    /**< Store current offset height */
    uint8_t _currentFontLength = 128;  /**< Store current font number of characters */
};

#endif // ST7565_GRAPHICS_H