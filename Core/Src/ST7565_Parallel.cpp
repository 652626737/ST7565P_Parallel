/*!
    @file ST7565_Parallel.cpp
    @brief ST7565 LCD driven by UC1609C controller, Source file.
    @details Adapted for parallel interface and STM32Cube framework.
*/

#include "ST7565_Parallel.h"

/*!
    @brief init the screen/sharedBuffer object
    @param mybuffer Pointer to buffer array data(arrays decay to pointers)
    @param w width of buffer
    @param h height of buffer
    @param x offset of buffer
    @param y offset of buffer
 */
ST7565_Parallel_Screen::ST7565_Parallel_Screen(uint8_t *mybuffer, uint8_t w, uint8_t h, int16_t x, int16_t y)
{
    screenBuffer = mybuffer; // pointer to buffer
    width = w;               // bitmap x size
    height = h;              // bitmap y size
    xoffset = x;             // x offset
    yoffset = y;             // y offset
}

/*!
    @brief init the LCD class object
    @param width width of LCD in pixels
    @param height height of LCD in pixels
    @param cs GPIO Chip select
    @param rst GPIO reset
    @param dc GPIO data or command
    @param wr GPIO write
    @param rd GPIO read
 */
ST7565_Parallel::ST7565_Parallel(int16_t width, int16_t height, LcdDataPin cs, LcdDataPin rst, LcdDataPin dc, LcdDataPin wr, LcdDataPin rd, LcdDataPin *dataPins) : ST7565_graphics(width, height)
{
    // Initialize other members if necessary
    _LCD_CS = cs;
    _LCD_RST = rst;
    _LCD_DC = dc;
    _LCD_WR = wr;
    _LCD_RD = rd;
    _dataPins = dataPins;

    _widthScreen = width;
    _heightScreen = height;
    _InactiveBuffer = new uint8_t[_bufferSize];
}

/*!
    @brief begin Method initialise LCD Sets pinmodes and SPI setup
    @param VbiasPOT contrast default = 0x49 , range 0x00 to 0xFE
    @param AddressSet AC [2:0] registers for RAM addr ctrl. default=2 range 0-7
 */
void ST7565_Parallel::LCDbegin(uint8_t _VbiasPot, uint8_t _AddressSet)
{
    // 启用 GPIO 时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    LcdDataPin gpio_pins[13] = {
        _LCD_CS,
        _LCD_RST,
        _LCD_DC,
        _LCD_WR,
        _LCD_RD,

    };
    // 将 _dataPins 数组的内容复制到 gpio_pins 数组中
    for (size_t i = 0; i < 8; i++)
    {
        gpio_pins[i + 5] = _dataPins[i]; // 从第 6 个位置开始填充数据引脚
    }

    // 使用 gpio_pins 数组的大小进行循环
    for (size_t i = 0; i < sizeof(gpio_pins) / sizeof(gpio_pins[0]); i++)
    {
        // 初始化 GPIO 引脚
        GPIO_InitStruct.Pin = gpio_pins[i].pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(gpio_pins[i].port, &GPIO_InitStruct);
    }

    // _VbiasPOT = VbiasPOT;

    // if (AddressSet > 7) // Check User input. Address set cannot exceed 7.
    // {
    //     AddressSet = UC1609_ADDRESS_SET;
    // }

    // _AddressCtrl = AddressSet;

    LCDinit();
}

/*!
    @brief Called from LCDbegin carries out Power on sequence and register init
    Can be used to reset LCD to default values.
 */
void ST7565_Parallel::LCDinit()
{
    LCD_CS_HIGH();
    HAL_Delay(UC1609_INIT_DELAY2);

    LCDReset();

    LCD_WR_LOW();
    LCD_RD_HIGH();
    delay_us_init();
    // 初始化LCD，发送一系列命令
    ST7565_send_command(CMD_SET_BIAS_9);          // 设置偏置比为1/9
    ST7565_send_command(CMD_SET_COM_NORMAL);      // 设置ADC为正常
    ST7565_send_command(CMD_SET_COM_REVERSE);     // 设置COM输出方向
    ST7565_send_command(CMD_SET_DISP_START_LINE); // 设置显示起始行

    ST7565_send_command(CMD_SET_BOOSTER_FIRST | CMD_SET_BOOSTER_234);

    ST7565_send_command(CMD_SET_POWER_CONTROL | 0x4);
    HAL_Delay(5);
    ST7565_send_command(CMD_SET_POWER_CONTROL | 0x6);
    HAL_Delay(5);
    ST7565_send_command(CMD_SET_POWER_CONTROL | 0x7);
    HAL_Delay(5);
    ST7565_send_command(CMD_SET_RESISTOR_RATIO | 0x4);

    HAL_Delay(5);

    // 设置对比度
    LCD_Contrast(_VbiasPOT); // 设置对比度值

    // 开启显示
    ST7565_send_command(CMD_DISPLAY_ON);
    LCD_Mode(0);

    // 清空显示缓冲区
    // memset(ST7565_Buffer, 0, sizeof(ST7565_Buffer));
    // memset(ST7565_Buffer_logo, 0, sizeof(ST7565_Buffer_logo));

    // 更新屏幕显示
    // LCD_UpdateScreen();
    LCDupdate();

    // // 重置当前光标位置
    // ST7565.CurrentX = 0;
    // ST7565.CurrentY = 0;

    // // 标记初始化完成
    // ST7565.Initialized = 1;

    // LCD_CS_HIGH();
}

/*!
    @brief Send data byte to the LCD
    @param byte the data byte to send
 */
void ST7565_Parallel::ST7565_write(uint8_t byte)
{
    // 激活片选引脚（CS1）
    LCD_CS_LOW();
    delay_us(UC1609_RESET_DELAY);

    // 置写使能引脚（WR）为低电平
    LCD_WR_LOW();
    delay_us(UC1609_RESET_DELAY);

    // 设置数据引脚（D0-D7）
    for (uint8_t i = 0; i < 8; i++)
    {
        HAL_GPIO_WritePin(_dataPins[i].port, _dataPins[i].pin, (byte >> i) & 0x01 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
    delay_us(UC1609_RESET_DELAY);

    // 置写使能引脚（WR）为高电平
    LCD_WR_HIGH();
    delay_us(UC1609_RESET_DELAY);

    // 取消片选（CS1）
    LCD_CS_HIGH();
    delay_us(UC1609_RESET_DELAY);
}

/*!
    @brief Sends a command to the display
    @param command Command to send
    @param value the values to change
    @note command and value will be combined with OR
 */
void ST7565_Parallel::ST7565_send_command(uint8_t command)
{

    LCD_DC_LOW();
    delay_us(UC1609_RESET_DELAY);
    ST7565_write(command);
};

/**
 * @brief 发送数据到ST7565显示器
 *
 * 将给定的数据发送到ST7565显示器。
 *
 * @param data 要发送的数据
 */
void ST7565_Parallel::ST7565_send_data(uint8_t data)
{
    // 设置命令/数据引脚（A0）为高电平
    LCD_DC_HIGH();
    // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); // A0
    delay_us(UC1609_RESET_DELAY);
    // delay_us(DELAY_US);

    // 调用写入数据的函数，将数据发送到显示器
    ST7565_write(data);
}

/**
 * @brief 设置 LCD 的页面
 *
 * 将指定的页面号发送到 LCD 控制器，以设置当前显示的页面。
 *
 * @param page 页面号
 *
 * @note 页面号应该是一个 8 位无符号整数。
 */
void ST7565_Parallel::LCD_SetPage(uint8_t page)
{
    ST7565_send_command(CMD_SET_PAGE | page);
}

/**
 * @brief 设置LCD列地址
 *
 * 将给定的列地址发送到LCD控制器，以设置LCD的显示列地址。
 *
 * @param column 列地址值
 */
void ST7565_Parallel::LCD_SetColumn(uint8_t column)
{
    // 设置列地址低4位
    ST7565_send_command(CMD_SET_COLUMN_LOWER | (column & 0x0F));

    // 设置列地址高4位
    ST7565_send_command(CMD_SET_COLUMN_UPPER | ((column >> 4) & 0x0F));
}

/*!
    @brief Resets LCD in a four wire setup called at start
    and should also be called in a controlled power down setting
 */
void ST7565_Parallel::LCDReset()
{
    // 将LCD的复位引脚拉低
    LCD_RST_LOW();
    // 延时一段时间，等待LCD复位完成
    delay_ms(UC1609_INIT_DELAY);
    // 将LCD的复位引脚拉高
    LCD_RST_HIGH();
}

/**
 * @brief 设置LCD显示模式
 *
 * 根据传入的模式参数设置LCD的显示模式。
 *
 * @param set_mode 显示模式，0表示正常显示，1表示反转显示
 */
void ST7565_Parallel::LCD_Mode(uint8_t set_mode)
{
    if (set_mode == 0)
    {
        ST7565_send_command(CMD_SET_DISP_NORMAL);
    }
    if (set_mode == 1)
    {
        ST7565_send_command(CMD_SET_DISP_REVERSE);
    }
}

void ST7565_Parallel::LCD_Contrast(uint8_t val)
{
    // 关闭显示
    ST7565_send_command(CMD_DISPLAY_OFF);
    ST7565_send_command(CMD_SET_VOLUME_FIRST);
    ST7565_send_command(CMD_SET_VOLUME_SECOND | (val & 0x3f));
    ST7565_send_command(CMD_DISPLAY_ON);
}

// /*!
//     @brief Powerdown procedure for LCD see datasheet P40
//  */
// void ST7565_Parallel::LCDPowerDown()
// {
//     LCDReset();
//     LCDEnable(0);
// }

void ST7565_Parallel::LCD_Sleep_Disable(void)
{
    // Re-enable booster circuit
    ST7565_send_command(CMD_SET_POWER_CONTROL | 0x07); // Adjust value as needed

    // Restore contrast (you may want to store the previous contrast value)
    ST7565_send_command(CMD_SET_VOLUME_FIRST);
    ST7565_send_command(0x20); // Adjust to your default contrast value

    // Set display to normal mode
    ST7565_send_command(CMD_SET_ALLPTS_NORMAL);

    // Turn on display
    ST7565_send_command(CMD_DISPLAY_ON);

    // Optional: Re-initialize display if needed
    // ST7565_init();  // Call your initialization function if necessary
}

void ST7565_Parallel::LCD_Sleep_Enable(void)
{
    // Turn off display
    ST7565_send_command(CMD_DISPLAY_OFF);

    // Set all pixels on (this reduces power consumption in some LCDs)
    ST7565_send_command(CMD_SET_ALLPTS_ON);

    // Disable booster circuit
    ST7565_send_command(CMD_SET_POWER_CONTROL | 0x00);

    // Set lower contrast to reduce power consumption
    ST7565_send_command(CMD_SET_VOLUME_FIRST);
    ST7565_send_command(0); // Lowest contrast value

    // Optional: Turn off voltage regulator
    ST7565_send_command(CMD_SET_STATIC_OFF);
}

// /*!
//     @brief Turns On Display
//     @param bits 1 display on , 0 display off
//  */
// /**
//  * @brief 启用 LCD
//  *
//  * 将 LCD 设置为启用状态，并通过并行接口发送命令。
//  *
//  * @param bits 位数
//  *
//  * @note 在发送命令前，先拉低 LCD 的片选信号，发送命令后再拉高片选信号。
//  */
// void ST7565_Parallel::LCDEnable(uint8_t bits)
// {
//     LCD_CS_LOW();
//     send_command(UC1609_DISPLAY_ON, bits);
//     LCD_CS_HIGH();
// }

// /*!
//     @brief Scroll the displayed image up by SL rows.
//     @details The valid SL value is between 0 (for no
//     scrolling) and (64).
//     Setting SL outside of this range causes undefined effect on the displayed image.
//     @param bits 0-64 line number y-axis
//  */
// void ST7565_Parallel::LCDscroll(uint8_t bits)
// {
//     LCD_CS_LOW();
//     send_command(UC1609_SCROLL, bits);
//     LCD_CS_HIGH();
// }

// /*!
//     @brief Rotates the display
//     @details Set LC[2:1] for COM (row) mirror (MY), SEG (column) mirror (MX).
//     Param1: 4 possible values 000 010 100 110 (defined)
//     @note If Mx is changed the buffer must BE updated see examples.
//  */
// void ST7565_Parallel::LCDrotate(uint8_t rotatevalue)
// {
//     LCD_CS_LOW();
//     switch (rotatevalue)
//     {
//     case 0:
//         rotatevalue = 0;
//         break;
//     case 0x02:
//         rotatevalue = UC1609_ROTATION_FLIP_ONE;
//         break;
//     case 0x04:
//         rotatevalue = UC1609_ROTATION_NORMAL;
//         break;
//     case 0x06:
//         rotatevalue = UC1609_ROTATION_FLIP_TWO;
//         break;
//     default:
//         rotatevalue = UC1609_ROTATION_NORMAL;
//         break;
//     }
//     send_command(UC1609_LCD_CONTROL, rotatevalue);
//     LCD_CS_HIGH();
// }

/*!
    @brief invert the display
    @param bits 1 invert , 0 normal
 */
void ST7565_Parallel::LCDInvertDisplay(uint8_t bits)
{
    ST7565_send_command(CMD_SET_DISP_REVERSE);
}

/*!
    @brief turns on all Pixels
    @param bits Set DC[1] to force all SEG drivers to output ON signals
    1 all on , 0 all off
 */
void ST7565_Parallel::LCDallpixelsOn(uint8_t bits)
{
    ST7565_send_command(CMD_SET_DISP_REVERSE);
}

/*!
    @brief Fill the screen NOT the buffer with a datapattern
    @param dataPattern can be set to zero to clear screen (not buffer) range 0x00 to 0xFF
    @param delay optional delay in microseconds can be set to zero normally.
 */
void ST7565_Parallel::LCDFillScreen(uint8_t dataPattern)
{

    uint16_t numofbytes = _widthScreen * (_heightScreen / 8); // width * height
    for (uint16_t i = 0; i < numofbytes; i++)
    {
        ST7565_send_data(dataPattern);
    }
    uint16_t iconnumofbytes = _iconwidthScreen * (_iconheightScreen / 8); // width * height
    for (uint16_t i = 0; i < iconnumofbytes; i++)
    {
        ST7565_send_data(dataPattern);
    }
}

/*!
    @brief Fill the chosen page at cursor with a datapattern
    @param dataPattern can be set to 0 to FF (not buffer)
 */
void ST7565_Parallel::LCDFillPage(uint8_t dataPattern = 0)
{
    uint16_t numofbytes = ((_widthScreen * (_heightScreen / 8)) / 8); // (width * height/8)/8 = 192 bytes
    for (uint16_t i = 0; i < numofbytes; i++)
    {

        ST7565_send_data(dataPattern);
    }
}

/*!
    @brief Goes to X Y position
    @param column Column 0-192
    @param page 0-7
 */
void ST7565_Parallel::LCDGotoXY(uint8_t column, uint8_t page)
{

    // send_command(UC1609_SET_COLADD_LSB, (column & 0x0F));
    // send_command(UC1609_SET_COLADD_MSB, (column & 0xF0) >> 4);
    // send_command(UC1609_SET_PAGEADD, page++);

    LCD_SetPage(page++);
    LCD_SetColumn(column);
}

/*!
    @brief Draw a bitmap in PROGMEM to the screen
    @param x offset 0-192
    @param y offset 0-64
    @param w width 0-192
    @param h height 0-64
    @param data pointer to the bitmap must be in PROGMEM
    @return LCD_Return_Codes_e enum.
 */
LCD_Return_Codes_e ST7565_Parallel::LCDBitmap(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t *data)
{
    // User error checks
    if (data == nullptr)
    {
        return LCD_BitmapNullptr;
    }
    if (x > _width || y > _height)
    {
        return LCD_BitmapScreenBounds;
    }
    if (w > _width || h > _height)
    {
        return LCD_BitmapLargerThanScreen;
    }
    if ((h % 8 != 0))
    {
        return LCD_BitmapVerticalSize;
    }

    // LCD_CS_LOW();

    uint8_t tx, ty;
    uint16_t offset = 0;
    uint8_t column = (x < 0) ? 0 : x;
    uint8_t page = (y < 0) ? 0 : y >> 3;

    for (ty = 0; ty < h; ty = ty + 8)
    {
        if (y + ty < 0 || y + ty >= _heightScreen)
        {
            continue;
        }
        // send_command(UC1609_SET_COLADD_LSB, (column & 0x0F));
        // send_command(UC1609_SET_COLADD_MSB, (column & 0xF0) >> 4);
        // send_command(UC1609_SET_PAGEADD, page++);
        LCD_SetPage(page++);
        LCD_SetColumn(column);

        for (tx = 0; tx < w; tx++)
        {
            if (x + tx < 0 || x + tx >= _widthScreen)
            {
                continue;
            }
            offset = (w * (ty >> 3)) + tx;
            // send_data(pgm_read_byte(&data[offset]));
            ST7565_send_data(pgm_read_byte(&data[offset]));
        }
    }
    // LCD_CS_HIGH();
    return LCD_Success;
}

/*!
    @brief updates the LCD i.e. writes the shared buffer to the active screen
    pointed to by ActiveBuffer
 */
void ST7565_Parallel::LCDupdate()
{
    LCDBuffer(this->ActiveBuffer->xoffset, this->ActiveBuffer->yoffset, this->ActiveBuffer->width, this->ActiveBuffer->height, this->ActiveBuffer->screenBuffer);
    LCDBuffer_Icon(0, 0, _iconwidthScreen, _iconheightScreen, _InactiveBuffer);
}

/*!
    @brief clears the buffer of the active screen pointed to by ActiveBuffer
    @note Does NOT write to the screen
 */
void ST7565_Parallel::LCDclearBuffer()
{
    memset(this->ActiveBuffer->screenBuffer, 0x00, (this->ActiveBuffer->width * (this->ActiveBuffer->height / 8)));
    memset(_InactiveBuffer, 0x00, _bufferSize);
}

/*!
    @brief Draw a data array to the screen
    @param x offset 0-128
    @param y offset 0-64
    @param w width 0-28
    @param h height 0-64
    @param data pointer to the data array
    @note Called by LCDupdate internally to write buffer to screen, can be called standalone as well
 */
void ST7565_Parallel::LCDBuffer(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t *data)
{
    uint8_t tx, ty;
    uint16_t offset = 0;
    uint8_t column = (x < 0) ? 0 : x;
    uint8_t page = (y < 0) ? 0 : y / 8;

    for (ty = 0; ty < h; ty = ty + 8)
    {
        if (y + ty < 0 || y + ty >= _heightScreen)
        {
            continue;
        }

        LCD_SetPage(page++);
        LCD_SetColumn(column);

        for (tx = 0; tx < w; tx++)
        {
            if (x + tx < 0 || x + tx >= _widthScreen)
            {
                continue;
            }
            offset = (w * (ty / 8)) + tx;
            ST7565_send_data(data[offset++]);
        }
    }
}

void ST7565_Parallel::LCDBuffer_Icon(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t *data)
{
    uint8_t tx;
    uint16_t offset = 0;
    uint8_t column = (x < 0) ? 0 : x;
    uint8_t page = 8; // 将 page 设置为 8
    LCD_SetPage(page);

    // 处理整个高度 h
    for (tx = 0; tx < w; tx++)
    {
        LCD_SetColumn(tx);
        if (x + tx < 0 || x + tx >= _iconwidthScreen)
        {
            continue;
        }
        offset = tx; // 这里的 0 是因为我们不再使用 ty
        ST7565_send_data(data[offset]);
    }
}

/*!
    @brief Draws a Pixel to the screen, overrides the graphics library
    @param x x co-ord of pixel
    @param y y co-ord of pixel
    @param colour colour of pixel
 */
void ST7565_Parallel::drawPixel(int16_t x, int16_t y, uint8_t colour)
{
    if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
        return;

    int16_t temp;
    uint8_t RotateMode = getRotation();
    switch (RotateMode)
    {
    case 1:
        temp = x;
        x = WIDTH - 1 - y;
        y = temp;
        break;
    case 2:
        x = WIDTH - 1 - x;
        y = HEIGHT - 1 - y;
        break;
    case 3:
        temp = x;
        x = y;
        y = HEIGHT - 1 - temp;
        break;
    }

    uint16_t offset = (this->ActiveBuffer->width * (y / 8)) + x;
    switch (colour)
    {
    case FOREGROUND:
        this->ActiveBuffer->screenBuffer[offset] |= (1 << (y & 7));
        break;
    case BACKGROUND:
        this->ActiveBuffer->screenBuffer[offset] &= ~(1 << (y & 7));
        break;
    case COLORINVERSE:
        this->ActiveBuffer->screenBuffer[offset] ^= (1 << (y & 7));
        break;
    }
}

/*!
    @brief Getter for _VbiasPOT contrast member
    @return value of _VbiasPOT
 */
/**
 * @brief 获取 LCD 对比度
 *
 * 从 ST7565 并行接口 LCD 控制器中获取当前的对比度值。
 *
 * @return 对比度值
 */
uint8_t ST7565_Parallel::LCDGetConstrast()
{
    return _VbiasPOT;
}

/*!
    @brief Getter for _AddressCtrl Display RAM address control member
    @return value of _AddressCtrl
 */
/**
 * @brief 获取 LCD 地址控制寄存器值
 *
 * 获取 LCD 地址控制寄存器的值。
 *
 * @return LCD 地址控制寄存器的值
 */
uint8_t ST7565_Parallel::LCDGetAddressCtrl()
{
    return _AddressCtrl;
}

/*!
    @brief Library version number getter
    @return The lib version number eg 180 = 1.8.0
 */
uint16_t ST7565_Parallel::LCDLibVerNumGet()
{
    return _LibVersionNum;
}

/*!
    @brief Freq delay used in SW SPI getter, uS delay used in SW SPI method
    @return The GPIO communications delay in uS
 */
/**
 * @brief 获取LCD高频延迟值
 *
 * 获取LCD高频延迟的当前值。
 *
 * @return 返回LCD高频延迟值
 */
uint16_t ST7565_Parallel::LCDHighFreqDelayGet()
{
    return _HighFreqDelay;
}

/*!
    @brief Freq delay used in SW SPI setter, uS delay used in SW SPI method
    @param CommDelay The GPIO communications delay in uS
 */
void ST7565_Parallel::LCDHighFreqDelaySet(uint16_t CommDelay)
{
    _HighFreqDelay = CommDelay;
}

void ST7565_Parallel::LCD_DrawIcon_phone(uint8_t icon)
{

    // 使用 InactiveBuffer 的 screenBuffer 来更新图标
    uint8_t *ST7565_Buffer_logo = _InactiveBuffer;

    if (icon == 0)
    {
        ST7565_Buffer_logo[2] = 0x00;
        ST7565_Buffer_logo[3] = 0x00;
        ST7565_Buffer_logo[4] = 0x00;
    }
    else if (icon == 1)
    {
        ST7565_Buffer_logo[2] = 0xFF;
        ST7565_Buffer_logo[3] = 0x00; // 保持第3列不变
        ST7565_Buffer_logo[4] = 0xFF;
    }
    else if (icon == 2)
    {
        ST7565_Buffer_logo[2] = 0x00; // 保持第2列不变
        ST7565_Buffer_logo[3] = 0xFF;
        ST7565_Buffer_logo[4] = 0xFF;
    }

    // 最��，调用更新函数将缓冲区内容刷新到屏幕
    // LCD_UpdateScreen();
}

void ST7565_Parallel::LCD_DrawIcon_Signal(uint8_t level)
{
    // 使用 InactiveBuffer 的 screenBuffer 来更新图标
    uint8_t *ST7565_Buffer_logo = _InactiveBuffer;
    if (level == 0)
    {
        // 仅设置缓冲区中第20列的数据
        ST7565_Buffer_logo[20] = 0x00;
        ST7565_Buffer_logo[22] = 0x00;
        ST7565_Buffer_logo[24] = 0x00;
        ST7565_Buffer_logo[26] = 0x00;
        ST7565_Buffer_logo[28] = 0x00;
        ST7565_Buffer_logo[30] = 0x00;
    }
    else if (level > 0 && level <= 20)
    {
        // 设置缓冲区中第22列的数据
        ST7565_Buffer_logo[20] = 0xFF;
        ST7565_Buffer_logo[22] = 0xFF;
    }
    else if (level > 20 && level <= 50)
    {
        // 设置缓冲区中第22列和第24列的数据
        ST7565_Buffer_logo[20] = 0xFF;
        ST7565_Buffer_logo[22] = 0xFF;
        ST7565_Buffer_logo[24] = 0xFF;
    }
    else if (level > 50 && level <= 80)
    {
        // 设置缓冲区中第22列、第24列和第26列的数据
        ST7565_Buffer_logo[20] = 0xFF;
        ST7565_Buffer_logo[22] = 0xFF;
        ST7565_Buffer_logo[24] = 0xFF;
        ST7565_Buffer_logo[26] = 0xFF;
    }
    else if (level > 80 && level < 100)
    {
        // 设置缓冲区中第22列、第24列、第26列和第28列的数据
        ST7565_Buffer_logo[20] = 0xFF;
        ST7565_Buffer_logo[22] = 0xFF;
        ST7565_Buffer_logo[24] = 0xFF;
        ST7565_Buffer_logo[26] = 0xFF;
        ST7565_Buffer_logo[28] = 0xFF;
    }
    else if (level >= 100)
    {
        // 设置缓冲区中第22列、第24列、第26列、第28列和第30列的数据
        ST7565_Buffer_logo[20] = 0xFF;
        ST7565_Buffer_logo[22] = 0xFF;
        ST7565_Buffer_logo[24] = 0xFF;
        ST7565_Buffer_logo[26] = 0xFF;
        ST7565_Buffer_logo[28] = 0xFF;
        ST7565_Buffer_logo[30] = 0xFF;
    }
    else
    {
        // 处理其他情况
    }
}

void ST7565_Parallel::LCD_DrawIcon_Printer(uint8_t status)
{

    // 使用 InactiveBuffer 的 screenBuffer 来更新图标
    uint8_t *ST7565_Buffer_logo = _InactiveBuffer;

    if (status == 1)
    {
        // 将第42列的数据设置为0xFF
        ST7565_Buffer_logo[42] = 0xFF;
    }
    else if (status == 0)
    {
        // 将第42列的数据设置为0x00
        ST7565_Buffer_logo[42] = 0x00;
    }
    else
    {
        // 处理其他情况
    }
}

void ST7565_Parallel::LCD_DrawIcon_Card(uint8_t status)
{
    // 使用 InactiveBuffer 的 screenBuffer 来更新图标
    uint8_t *ST7565_Buffer_logo = _InactiveBuffer;
    if (status == 1)
    {
        // 将第61列的数据设置为0xFF
        ST7565_Buffer_logo[61] = 0xFF;
    }
    else if (status == 0)
    {
        // 将第61列的数据设置为0x00
        ST7565_Buffer_logo[61] = 0x00;
    }
    else
    {
        // 处理其他情况
    }
}

void ST7565_Parallel::LCD_DrawIcon_Battery(uint8_t level)
{
    // 使用 InactiveBuffer 的 screenBuffer 来更新图标
    uint8_t *ST7565_Buffer_logo = _InactiveBuffer;

    if (level == 0)
    {

        // 显示电池图标 0 格
        ST7565_Buffer_logo[93] = 0x00;
        ST7565_Buffer_logo[99] = 0x00;
        ST7565_Buffer_logo[101] = 0x00;
        ST7565_Buffer_logo[103] = 0x00;
        ST7565_Buffer_logo[105] = 0x00;
    }

    // 根据电量等级绘制电池电量
    else if (level > 0 && level <= 20)
    {
        // 显示电池图标 1 格
        ST7565_Buffer_logo[93] = 0xFF;
        ST7565_Buffer_logo[105] = 0xFF;
    }
    else if (level > 20 && level <= 50)
    {
        // 显示电池图标 2 格
        ST7565_Buffer_logo[93] = 0xFF;
        ST7565_Buffer_logo[103] = 0xFF;
        ST7565_Buffer_logo[105] = 0xFF;
    }
    else if (level > 50 && level <= 80)
    {
        // 显示电池图标 3 格
        ST7565_Buffer_logo[93] = 0xFF;
        ST7565_Buffer_logo[101] = 0xFF;
        ST7565_Buffer_logo[103] = 0xFF;
        ST7565_Buffer_logo[105] = 0xFF;
    }
    else if (level > 80 && level <= 100)
    {
        // 显示电池图标 4 格
        ST7565_Buffer_logo[93] = 0xFF;
        ST7565_Buffer_logo[99] = 0xFF;
        ST7565_Buffer_logo[101] = 0xFF;
        ST7565_Buffer_logo[103] = 0xFF;
        ST7565_Buffer_logo[105] = 0xFF;
    }
}

void ST7565_Parallel::LCD_DrawIcon_Lock(uint8_t status)
{
    // 使用 InactiveBuffer 的 screenBuffer 来更新图标
    uint8_t *ST7565_Buffer_logo = _InactiveBuffer;
    if (status == 1)
    {
        // 显示锁图标
        ST7565_Buffer_logo[77] = 0xFF;
    }
    else if (status == 0)
    {
        // 隐藏锁图标
        ST7565_Buffer_logo[77] = 0x00;
    }
    else
    {
        // 处理其他状态
        // 你可以在这里处理异常状态或不显示任何内容
    }
}

void ST7565_Parallel::LCD_DrawIcon_Upload(uint8_t status)
{
    // 使用 InactiveBuffer 的 screenBuffer 来更新图标
    uint8_t *ST7565_Buffer_logo = _InactiveBuffer;
    if (status == 0)
    {
        // 显示上传图标
        ST7565_Buffer_logo[108] = 0x00;
        // 根���上传图标的结构设置更多列，如果有的话
    }
    else if (status == 1)
    {
        // 隐藏上传图标
        ST7565_Buffer_logo[108] = 0xFF;
        // 清除上传图标的更多列
    }
}

void ST7565_Parallel::LCD_DrawIcon_Download(uint8_t status)
{
    // 使用 InactiveBuffer 的 screenBuffer 来更新图标
    uint8_t *ST7565_Buffer_logo = _InactiveBuffer;
    if (status == 1)
    {
        // 显示下载图标

        ST7565_Buffer_logo[122] = 0xFF;
    }
    else if (status == 0)
    {
        // 隐藏下载图标
        ST7565_Buffer_logo[122] = 0x00;
        // 清除下载图标的更多列
    }
    else
    {
        // 处理其他状态（可以选择不显示任何内容或显示默认状态）
    }
}