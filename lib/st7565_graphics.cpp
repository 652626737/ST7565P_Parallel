/*!
    @file ST7565_graphics.cpp
    @brief ST7565 LCD graphics library source file for STM32Cube framework
    @details Project Name: ST7565_Graphics
    @author [Your Name]
*/

#include "ST7565_graphics.h"

/*!
    @brief Constructor for ST7565_Graphics class
    @param w width of the display
    @param h height of the display
*/
ST7565_Graphics::ST7565_Graphics(int16_t w, int16_t h) :
    WIDTH(w), HEIGHT(h)
{
    _width = WIDTH;
    _height = HEIGHT;
    cursor_y = cursor_x = 0;
    textsize = 1;
    textcolor = ST7565_BLACK;
    textbgcolor = ST7565_WHITE;
    textwrap = true;
    drawBitmapAddr = true;
    _rotation = ST7565_Rotation::Degrees_0;
}

/*!
    @brief Draws a circle
    @param x0 circle center x position
    @param y0 circle center y position
    @param r radius of circle
    @param color The color of the circle
*/
void ST7565_Graphics::drawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(x0, y0 + r, color);
    drawPixel(x0, y0 - r, color);
    drawPixel(x0 + r, y0, color);
    drawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}

// ... (implement other drawing functions like drawLine, fillRect, etc.)

/*!
    @brief Draws a character
    @param x X coordinate
    @param y Y coordinate
    @param c The ASCII character
    @param color text color
    @param bg background color
    @param size Text size multiplier
    @return ST7565_Return_Codes_e enum
*/
ST7565_Return_Codes_e ST7565_Graphics::drawChar(int16_t x, int16_t y, unsigned char c,
                                                uint8_t color, uint8_t bg, uint8_t size) {
    // Implementation similar to ERM19264_graphics::drawChar
    // You'll need to adapt this to your font data structure
    // ...
}

/*!
    @brief Writes text string on the LCD
    @param x X coordinate
    @param y Y coordinate
    @param text pointer to string
    @param color text color
    @param bg background color
    @param size Text size multiplier
    @return ST7565_Return_Codes_e enum
*/
ST7565_Return_Codes_e ST7565_Graphics::drawText(int16_t x, int16_t y, const char *text,
                                                uint8_t color, uint8_t bg, uint8_t size) {
    // Implementation similar to ERM19264_graphics::drawText
    // ...
}

/*!
    @brief Sets the rotation of the display
    @param rotation ST7565_Rotation enum value
*/
void ST7565_Graphics::setRotation(ST7565_Rotation rotation) {
    _rotation = rotation;
    switch (rotation) {
        case ST7565_Rotation::Degrees_0:
        case ST7565_Rotation::Degrees_180:
            _width = WIDTH;
            _height = HEIGHT;
            break;
        case ST7565_Rotation::Degrees_90:
        case ST7565_Rotation::Degrees_270:
            _width = HEIGHT;
            _height = WIDTH;
            break;
    }
}

/*!
    @brief Gets the rotation of the display
    @return ST7565_Rotation enum value
*/
ST7565_Rotation ST7565_Graphics::getRotation() const {
    return _rotation;
}

// ... (implement other methods like setCursor, setTextColor, etc.)
