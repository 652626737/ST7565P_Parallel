/*!
* @file ST7565_graphics_font.h
* @brief  ST7565 LCD driven by ST7565 controller, font data file.
* @author Your Name.
* @details <https://example.com>
*/

#ifndef ST7565_GRAPHICS_FONT_H
#define ST7565_GRAPHICS_FONT_H

#include "stm32f1xx_hal.h" // 根据你的 STM32 系列选择合适的 HAL 头文件

/*!
	@brief USER FONT OPTION ONE , FONT DEFINE SECTION 
	@note Comment in the fonts YOU want, ST7565_Font_One is default. 
		-#  ST7565_Font_One  default  (Full ASCII with mods)
		-#  ST7565_Font_Two  thick (NO LOWERCASE)
		-#  ST7565_Font_Three seven segment 
		-#  ST7565_Font_Four wide (NO LOWERCASE)
		-#  ST7565_Font_Five  tiny
		-#  ST7565_Font_Six Home Spun
		-#  ST7565_Font_Seven big numbers (extended Numeric)
		-#  ST7565_Font_Eight Medium numbers (extended Numeric)
		-#  ST7565_Font_Nine Arial round
		-#  ST7565_Font_Ten  Arial bold
		-# ST7565_Font_Eleven  Mia
		-# ST7565_Font_Twelve  dedica
*/

#define UC1609_Font_One         /**< (1) default  (Full ASCII with mods) */ 
#define UC1609_Font_Two       /**< (2) thick (NO LOWERCASE) */
#define UC1609_Font_Three     /**< (3) seven segment  */
#define UC1609_Font_Four      /**< (4) wide (NO LOWERCASE) */
#define UC1609_Font_Five      /**< (5) tiny */
#define UC1609_Font_Six       /**< (6) Home Spun */
// #define UC1609_Font_Seven     /**< (7) big numbers (extended Numeric) */
// #define UC1609_Font_Eight     /**< (8) Medium numbers (extended Numeric) */
// #define UC1609_Font_Nine      /**< (9) Arial round */
// #define UC1609_Font_Ten       /**< (10) Arial bold */
// #define UC1609_Font_Eleven     /**< (11) Mia */
// #define UC1609_Font_Twelve     /**< (12) dedica */

/*! 
 * @brief USER FONT OPTION TWO
	@details This is defined to include full extended ASCII set 127-255, Will increase program size if enabled. 
	if commented in _CurrentFontLength in setFontNum must also be edited to 
	_CurrentFontLength = ST7565FontLenAll;
	@note  ST7565_Font_One only.
*/
//#define ST_FONT_MOD_TWO /**< USER OPTION 2 */

// Font data is in the cpp file accessed through extern pointers.

#ifdef UC1609_Font_One 
extern const unsigned char * pFontDefaultptr; /**< Pointer to Default font data */
#endif
#ifdef UC1609_Font_Two 
extern const unsigned char * pFontThickptr; /**< Pointer to thick font data */
#endif
#ifdef UC1609_Font_Three
extern const unsigned char * pFontSevenSegptr; /**< Pointer to Seven segment font data */
#endif
#ifdef UC1609_Font_Four
extern const unsigned char * pFontWideptr; /**< Pointer to Wide font data */
#endif
#ifdef UC1609_Font_Five
extern const unsigned char * pFontTinyptr; /**< Pointer to Tiny font data */
#endif
#ifdef UC1609_Font_Six
extern const unsigned char * pFontHomeSpunptr; /**< Pointer to Home Spun font data */
#endif
#ifdef UC1609_Font_Seven
extern const uint8_t (* pFontBigNum16x32ptr)[64]; /**< Pointer to Big Numbers font data */
#endif
#ifdef UC1609_Font_Eight
extern const uint8_t (* pFontMedNum16x16ptr)[32];  /**< Pointer to Medium Numbers font data */
#endif
#ifdef UC1609_Font_Nine
extern const uint8_t (* pFontArial16x24ptr)[48]; /**< Pointer to Arial Round font data */
#endif
#ifdef UC1609_Font_Ten
extern const uint8_t (* pFontArial16x16ptr)[32]; /**< Pointer to Arial bold font data */
#endif
#ifdef UC1609_Font_Eleven
extern const uint8_t (* pFontMia8x16ptr)[16]; /**< Pointer to Mia font data */
#endif
#ifdef UC1609_Font_Twelve
extern const uint8_t (* pFontDedica8x12ptr)[12]; /**< Pointer to Dedica font data */
#endif

#endif // ST7565_GRAPHICS_FONT_H