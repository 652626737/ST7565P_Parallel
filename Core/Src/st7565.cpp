#include "st7565.h"
#include "us_delay.h"

uint8_t ST7565_Buffer[(LCD_WIDTH + 1) * LCD_HEIGHT / 8];

uint8_t ST7565_Buffer_logo[(LCD_WIDTH_logo + 1) * LCD_HEIGHT_logo];

typedef struct
{
  uint16_t CurrentX;
  uint16_t CurrentY;
  uint8_t Inverted;
  uint8_t Initialized;
} ST7565_t;

static ST7565_t ST7565;

/**
 * @brief 向ST7565写入数据
 *
 * 将给定的数据写入到ST7565显示屏中。
 *
 * @param data 要写入的数据
 */
void ST7565_write(uint8_t data)
{
  // 激活片选引脚（CS1）
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // CS1
  delay_us(DELAY_US);

  // 置写使能引脚（WR）为低电平
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // WR
  delay_us(DELAY_US);

  // 设置数据引脚（D0-D7）
  for (uint8_t i = 0; i < 8; i++)
  {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 << i, (data >> i) & 0x01 ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
  delay_us(DELAY_US);

  // 置写使能引脚（WR）为高电平
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); // WR
  delay_us(DELAY_US);

  // 取消片选（CS1）
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); // CS1
  delay_us(DELAY_US);
}

/**
 * @brief 发送命令到ST7565显示器
 *
 * 将给定的命令发送到ST7565显示器。在发送命令之前，会先设置命令/数据引脚（A0）为低电平。
 *
 * @param command 要发送的命令
 */
static void ST7565_send_command(uint8_t command)
{
  // 设置命令/数据引脚（A0）为低电平
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); // A0
  delay_us(DELAY_US);

  // 调用写入数据的函数，将命令发送到显示器
  ST7565_write(command);
};

/**
 * @brief 发送数据到ST7565显示器
 *
 * 将给定的数据发送到ST7565显示器。
 *
 * @param data 要发送的数据
 */
static void ST7565_send_data(uint8_t data)
{
  // 设置命令/数据引脚（A0）为高电平
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); // A0
  delay_us(DELAY_US);

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
void LCD_SetPage(uint8_t page)
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
static void LCD_SetColumn(uint8_t column)
{
  // 设置列地址低4位
  ST7565_send_command(CMD_SET_COLUMN_LOWER | (column & 0x00));

  // 设置列地址高4位
  ST7565_send_command(CMD_SET_COLUMN_UPPER | ((column >> 4) & 0x00));
}

/**
 * @brief 初始化 ST7565 GPIO 引脚
 *
 * 启用 GPIO 时钟，并初始化指定的 GPIO 引脚为输出模式。
 *
 * @param pins GPIO 引脚数组指针
 * @param num_pins GPIO 引脚数量
 */
static void ST7565_gpio_init(gpio_pin_t *pins, size_t num_pins)
{
  // 启用 GPIO 时钟
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  for (size_t i = 0; i < num_pins; i++)
  {
    // 初始化 GPIO 引脚
    GPIO_InitStruct.Pin = pins[i].pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(pins[i].port, &GPIO_InitStruct);
  }
}
/**
 * @brief 初始化 ST7565 液晶显示屏
 *
 * 该函数用于初始化 ST7565 液晶显示屏，包括 GPIO 引脚的初始化、延迟函数的初始化以及液晶显示屏的初始配置。
 *
 * @param lcd ST7565 液晶显示屏设备结构体指针
 */
void ST7565_init(st7565_lcd_dev_t *lcd)
{
  // 启用 GPIOA 和 GPIOB 的时钟
  // 初始化 GPIO 引脚
  gpio_pin_t gpio_pins[] = {
      lcd->cs1,
      lcd->reset,
      lcd->a0,
      lcd->wr,
      lcd->rd,
      lcd->data_pins[0],
      lcd->data_pins[1],
      lcd->data_pins[2],
      lcd->data_pins[3],
      lcd->data_pins[4],
      lcd->data_pins[5],
      lcd->data_pins[6],
      lcd->data_pins[7],
  };

  // 初始化GPIO引脚
  ST7565_gpio_init(gpio_pins, sizeof(gpio_pins) / sizeof(gpio_pins[0]));

  // 初始化微秒延时函数
  delay_us_init();

  // 设置初始状态
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);    // 使能片选CS1

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); // 复位RESET
  HAL_Delay(100);                                        // 等待复位稳定
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);   // 复位完成

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // 写使能WR设为低电平
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);   // 读使能RD设为高电平

  // 初始化LCD，发送一系列命令
  ST7565_send_command(CMD_SET_BIAS_9);          // 设置偏置比为1/9
  ST7565_send_command(CMD_SET_ADC_NORMAL);      // 设置ADC为正常
  ST7565_send_command(CMD_SET_COM_REVERSE);     // 设置COM输出方向
  ST7565_send_command(CMD_SET_DISP_START_LINE); // 设置显示起始行

  ST7565_send_command(CMD_SET_BOOSTER_FIRST | CMD_SET_BOOSTER_234);

  ST7565_send_command(CMD_SET_POWER_CONTROL | 0x4);
  // HAL_Delay(5);
  ST7565_send_command(CMD_SET_POWER_CONTROL | 0x6);
  // HAL_Delay(5);
  ST7565_send_command(CMD_SET_POWER_CONTROL | 0x7);
  // HAL_Delay(5);
  ST7565_send_command(CMD_SET_RESISTOR_RATIO | 0x4);

  HAL_Delay(5);
  // 设置对比度
  LCD_Contrast(0x23); // 设置对比度值

  // 开启显示
  ST7565_send_command(CMD_DISPLAY_ON);
  LCD_Mode(0);

  // 清空显示缓冲区
  memset(ST7565_Buffer, 0, sizeof(ST7565_Buffer));
  memset(ST7565_Buffer_logo, 0, sizeof(ST7565_Buffer_logo));

  // 更新屏幕显示
  LCD_UpdateScreen();

  // 重置当前光标位置
  ST7565.CurrentX = 0;
  ST7565.CurrentY = 0;

  // 标记初始化完成
  ST7565.Initialized = 1;
}

/**
 * @brief 打开LCD屏幕
 *
 * 通过发送一系列命令来打开LCD屏幕。
 *
 * 示例命令：
 * - CMD_SET_DISP_START_LINE：设置显示起始行
 * - CMD_SET_DISP_NORMAL：设置正常显示模式
 * - CMD_DISPLAY_ON：打开显示
 */
void LCD_ON()
{
  ST7565_send_command(CMD_SET_DISP_START_LINE); // Example command
  ST7565_send_command(CMD_SET_DISP_NORMAL);     // Example command
  ST7565_send_command(CMD_DISPLAY_ON);          // Turn on the display
  ST7565_send_command(CMD_SET_VOLUME_FIRST);
  // 发送命令 0x10
  ST7565_send_command(0x20);
}

/**
 * @brief 关闭LCD屏幕
 *
 * 该函数用于关闭LCD屏幕，通过向ST7565发送命令实现。
 *
 * 首先，发送CMD_SET_VOLUME_FIRST命令来设置音量级别（这里的具体作用需要根据硬件文档来确定）。
 * 然后，发送CMD_DISPLAY_OFF命令来关闭LCD屏幕显示。
 */
void LCD_OFF()
{
  // 发送命令 CMD_SET_VOLUME_FIRST
  ST7565_send_command(CMD_SET_VOLUME_FIRST);
  // 发送命令 0x10
  ST7565_send_command(0x10);
  // 发送命令 CMD_DISPLAY_OFF，关闭显示屏
  ST7565_send_command(CMD_DISPLAY_OFF);
}

/**
 * @brief 更新 LCD 屏幕显示
 *
 * 此函数负责更新 LCD 屏幕的显示内容。通过遍历所有页面和列，从缓冲区中获取数据并发送到 LCD 控制器进行显示。
 */
void LCD_UpdateScreen(void)
{
  uint8_t page, col;
  for (page = 0; page < 8; page++)
  {
    LCD_SetPage(page); // 设置当前页地址（页从0到7）
    LCD_SetColumn(0);  // 从列0开始

    for (col = 0; col < 128; col++)
    {
      // 从缓冲区中获取数据
      ST7565_send_data(ST7565_Buffer[(129 * page) + col]);
      // ST7565_send_data(0xFF);
    }
  }
  LCD_SetPage(8);
  LCD_SetColumn(0); // 从列0开始

  for (col = 0; col < 128; col++)
  {
    // 从缓冲区中获取数据
    ST7565_send_data(ST7565_Buffer_logo[col]);
    // ST7565_send_data(0xFF);
  }
}
/**
 * @brief 设置 LCD 显示模式
 *
 * 根据给定的模式设置 LCD 显示模式。
 *
 * @param set_mode 设置模式，0 表示正常显示，1 表示反色显示
 */
void LCD_Mode(uint8_t set_mode)
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

/**
 * @brief 设置LCD的休眠状态
 *
 * 根据给定的参数设置LCD的休眠状态。如果参数为0，则进入休眠模式；如果参数为1，则从休眠模式唤醒。
 *
 * @param set 休眠状态设置参数，0表示进入休眠模式，1表示从休眠模式唤醒
 */
void LCD_Sleep_Enable(void)
{
    // Turn off display
    ST7565_send_command(CMD_DISPLAY_OFF);
    
    // Set all pixels on (this reduces power consumption in some LCDs)
    ST7565_send_command(CMD_SET_ALLPTS_ON);
    
    // Disable booster circuit
    ST7565_send_command(CMD_SET_POWER_CONTROL | 0x00);
    
    // Set lower contrast to reduce power consumption
    ST7565_send_command(CMD_SET_VOLUME_FIRST);
    ST7565_send_command(0);  // Lowest contrast value
    
    // Optional: Turn off voltage regulator
    ST7565_send_command(CMD_SET_STATIC_OFF);
}

void LCD_Sleep_Disable(void)
{
    // Re-enable booster circuit
    ST7565_send_command(CMD_SET_POWER_CONTROL | 0x07);  // Adjust value as needed
    
    // Restore contrast (you may want to store the previous contrast value)
    ST7565_send_command(CMD_SET_VOLUME_FIRST);
    ST7565_send_command(0x20);  // Adjust to your default contrast value
    
    // Set display to normal mode
    ST7565_send_command(CMD_SET_ALLPTS_NORMAL);
    
    // Turn on display
    ST7565_send_command(CMD_DISPLAY_ON);
    
    // Optional: Re-initialize display if needed
    // ST7565_init();  // Call your initialization function if necessary
}

/**
 * @brief 切换LCD显示反转状态
 *
 * 切换LCD显示的反转状态，即将当前的反转状态取反。
 * 如果当前是反转显示，则切换为正常显示；如果当前是正常显示，则切换为反转显示。
 * 同时对缓冲区中的像素数据进行反转处理，以适应新的显示状态。
 */
void LCD_ToggleInvert(void)
{
  uint16_t i;

  ST7565.Inverted = !ST7565.Inverted;
  for (i = 0; i < sizeof(ST7565_Buffer); i++)
    ST7565_Buffer[i] = ~ST7565_Buffer[i];
}

void LCD_Fill(ST7565_COLOR_t color)
{
  memset(ST7565_Buffer, (color == ST7565_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(ST7565_Buffer));
  memset(ST7565_Buffer_logo, (color == ST7565_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(ST7565_Buffer_logo));
}

/**
 * @brief 绘制像素点
 *
 * 在LCD屏幕上的指定位置绘制一个像素点，并设置其颜色。
 *
 * @param x 像素点的横坐标
 * @param y 像素点的纵坐标
 * @param color 像素点的颜色，取值为ST7565_COLOR_t枚举类型
 */
void LCD_Pixel(uint16_t x, uint16_t y, ST7565_COLOR_t color)
{
  if (x >= (LCD_WIDTH + 1) || y >= LCD_HEIGHT)
    return;
  if (ST7565.Inverted)
    color = (ST7565_COLOR_t)!color;
  if (color == ST7565_COLOR_WHITE)
    // uint8_t ST7565_Buffer[(LCD_WIDTH+1)  * LCD_HEIGHT/8];
    ST7565_Buffer[x + (y / 8) * (LCD_WIDTH + 1)] |= 1 << (y % 8);
  else
    ST7565_Buffer[x + (y / 8) * (LCD_WIDTH + 1)] &= ~(1 << (y % 8));
}

/**
 * @brief 在LCD上绘制一条直线
 *
 * 根据给定的起点和终点坐标，在LCD上使用指定的颜色绘制一条直线。
 *
 * @param x0 起点x坐标
 * @param y0 起点y坐标
 * @param x1 终点x坐标
 * @param y1 终点y坐标
 * @param color 直线颜色
 */
void LCD_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, ST7565_COLOR_t color)
{
  uint16_t dx, dy, sx, sy, err, e2, i, tmp;

  if (x0 >= (LCD_WIDTH + 1))
    x0 = (LCD_WIDTH + 1) - 1;
  if (x1 >= (LCD_WIDTH + 1))
    x1 = (LCD_WIDTH + 1) - 1;
  if (y0 >= LCD_HEIGHT)
    y0 = LCD_HEIGHT - 1;
  if (y1 >= LCD_HEIGHT)
    y1 = LCD_HEIGHT - 1;

  dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
  dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
  sx = (x0 < x1) ? 1 : -1;
  sy = (y0 < y1) ? 1 : -1;
  err = ((dx > dy) ? dx : -dy) / 2;

  if (dx == 0)
  {
    if (y1 < y0)
    {
      tmp = y1;
      y1 = y0;
      y0 = tmp;
    }

    if (x1 < x0)
    {
      tmp = x1;
      x1 = x0;
      x0 = tmp;
    }

    for (i = y0; i <= y1; i++)
      LCD_Pixel(x0, i, color);
    return;
  }

  if (dy == 0)
  {
    if (y1 < y0)
    {
      tmp = y1;
      y1 = y0;
      y0 = tmp;
    }

    if (x1 < x0)
    {
      tmp = x1;
      x1 = x0;
      x0 = tmp;
    }

    for (i = x0; i <= x1; i++)
      LCD_Pixel(i, y0, color);
    return;
  }

  while (1)
  {
    LCD_Pixel(x0, y0, color);
    if (x0 == x1 && y0 == y1)
      break;
    e2 = err;
    if (e2 > -dx)
    {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dy)
    {
      err += dx;
      y0 += sy;
    }
  }
}

/**
 * @brief 在LCD上绘制矩形
 *
 * 在指定的LCD坐标位置绘制一个矩形，矩形的大小由宽度和高度决定，颜色由参数指定。
 *
 * @param x 矩形左上角的x坐标
 * @param y 矩形左上角的y坐标
 * @param w 矩形的宽度
 * @param h 矩形的高度
 * @param color 矩形的颜色
 */
void LCD_Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, ST7565_COLOR_t color)
{
  if (x >= (LCD_WIDTH - 1) || y >= LCD_HEIGHT)
    return;
  if ((x + w) >= (LCD_WIDTH - 1))
    w = (LCD_WIDTH - 1) - x;
  if ((y + h) >= LCD_HEIGHT)
    h = LCD_HEIGHT - y;

  LCD_Line(x, y, x + w, y, color);         /* Top line */
  LCD_Line(x, y + h, x + w, y + h, color); /* Bottom line */
  LCD_Line(x, y, x, y + h, color);         /* Left line */
  LCD_Line(x + w, y, x + w, y + h, color); /* Right line */
}

/**
 * @brief 填充LCD矩形区域
 *
 * 在LCD的指定位置填充一个矩形区域，并设置指定颜色。
 *
 * @param x 矩形左上角的x坐标
 * @param y 矩形左上角的y坐标
 * @param w 矩形的宽度
 * @param h 矩形的高度
 * @param color 填充颜色
 */
void LCD_Rect_Fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, ST7565_COLOR_t color)
{
  uint8_t i;
  if (x >= (LCD_WIDTH + 1) || y >= LCD_HEIGHT)
    return;
  if ((x + w) >= (LCD_WIDTH + 1))
    w = (LCD_WIDTH + 1) - x;
  if ((y + h) >= LCD_HEIGHT)
    h = LCD_HEIGHT - y;
  for (i = 0; i <= h; i++)
    LCD_Line(x, y + i, x + w, y + i, color);
}

/**
 * @brief 在LCD上绘制三角形
 *
 * 使用给定的三个坐标点绘制一个三角形，并将其显示在LCD上。
 *
 * @param x1 第一个坐标点的x坐标
 * @param y1 第一个坐标点的y坐标
 * @param x2 第二个坐标点的x坐标
 * @param y2 第二个坐标点的y坐标
 * @param x3 第三个坐标点的x坐标
 * @param y3 第三个坐标点的y坐标
 * @param color 三角形的颜色
 */
void LCD_Triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ST7565_COLOR_t color)
{
  LCD_Line(x1, y1, x2, y2, color);
  LCD_Line(x2, y2, x3, y3, color);
  LCD_Line(x3, y3, x1, y1, color);
}

#define ABS(x) ((x) > 0 ? (x) : -(x))

/**
 * @brief 填充LCD屏幕上的三角形
 *
 * 在LCD屏幕上使用指定的颜色填充一个三角形。三角形的三个顶点由给定的坐标确定。
 *
 * @param x1 第一个顶点的x坐标
 * @param y1 第一个顶点的y坐标
 * @param x2 第二个顶点的x坐标
 * @param y2 第二个顶点的y坐标
 * @param x3 第三个顶点的x坐标
 * @param y3 第三个顶点的y坐标
 * @param color 填充颜色
 */
void LCD_Triangle_Fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ST7565_COLOR_t color)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
          yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
          curpixel = 0;

  deltax = ABS(x2 - x1);
  deltay = ABS(y2 - y1);
  x = x1;
  y = y1;

  if (x2 >= x1)
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else
  {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (y2 >= y1)
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay)
  {
    xinc1 = 0;
    yinc2 = 0;
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;
  }
  else
  {
    xinc2 = 0;
    yinc1 = 0;
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;
  }

  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    LCD_Line(x, y, x3, y3, color);

    num += numadd;
    if (num >= den)
    {
      num -= den;
      x += xinc1;
      y += yinc1;
    }
    x += xinc2;
    y += yinc2;
  }
}

void LCD_Ellipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint8_t fill, ST7565_COLOR_t color)
{
  int16_t x, y;
  int32_t rx2 = rx * rx;
  int32_t ry2 = ry * ry;
  int32_t fx2 = 4 * rx2;
  int32_t fy2 = 4 * ry2;
  int32_t s;
  if (fill)
  {
    for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++)
    {
      LCD_Line(x0 - x, y0 - y, x0 + x, y0 - y, color);
      LCD_Line(x0 - x, y0 + y, x0 + x, y0 + y, color);
      if (s >= 0)
      {
        s += fx2 * (1 - y);
        y--;
      }
      s += ry2 * ((4 * x) + 6);
    }
    for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++)
    {
      LCD_Line(x0 - x, y0 - y, x0 + x, y0 - y, color);
      LCD_Line(x0 - x, y0 + y, x0 + x, y0 + y, color);
      if (s >= 0)
      {
        s += fy2 * (1 - x);
        x--;
      }
      s += rx2 * ((4 * y) + 6);
    }
  }
  else
  {
    for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++)
    {
      LCD_Pixel(x0 + x, y0 + y, color);
      LCD_Pixel(x0 - x, y0 + y, color);
      LCD_Pixel(x0 + x, y0 - y, color);
      LCD_Pixel(x0 - x, y0 - y, color);
      if (s >= 0)
      {
        s += fx2 * (1 - y);
        y--;
      }
      s += ry2 * ((4 * x) + 6);
    }
    for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++)
    {
      LCD_Pixel(x0 + x, y0 + y, color);
      LCD_Pixel(x0 - x, y0 + y, color);
      LCD_Pixel(x0 + x, y0 - y, color);
      LCD_Pixel(x0 - x, y0 - y, color);
      if (s >= 0)
      {
        s += fy2 * (1 - x);
        x--;
      }
      s += rx2 * ((4 * y) + 6);
    }
  }
}

void LCD_Circle(uint16_t x, uint16_t y, uint8_t radius, uint8_t fill, uint8_t size, ST7565_COLOR_t color)
{
  int a_, b_, P;
  a_ = 0;
  b_ = radius;
  P = 1 - radius;
  while (a_ <= b_)
  {
    if (fill == 1)
    {
      LCD_Rect_Fill(x - a_, y - b_, 2 * a_ + 1, 2 * b_ + 1, color);
      LCD_Rect_Fill(x - b_, y - a_, 2 * b_ + 1, 2 * a_ + 1, color);
    }
    else
    {
      LCD_Rect_Fill(a_ + x, b_ + y, size, size, color);
      LCD_Rect_Fill(b_ + x, a_ + y, size, size, color);
      LCD_Rect_Fill(x - a_, b_ + y, size, size, color);
      LCD_Rect_Fill(x - b_, a_ + y, size, size, color);
      LCD_Rect_Fill(b_ + x, y - a_, size, size, color);
      LCD_Rect_Fill(a_ + x, y - b_, size, size, color);
      LCD_Rect_Fill(x - a_, y - b_, size, size, color);
      LCD_Rect_Fill(x - b_, y - a_, size, size, color);
    }
    if (P < 0)
    {
      P = (P + 3) + (2 * a_);
      a_++;
    }
    else
    {
      P = (P + 5) + (2 * (a_ - b_));
      a_++;
      b_--;
    }
  }
}

void LCD_Circle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint8_t size, ST7565_COLOR_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4)
    {
      LCD_Pixel(x0 + x, y0 + y, color);
      LCD_Pixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2)
    {
      LCD_Pixel(x0 + x, y0 - y, color);
      LCD_Pixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8)
    {
      LCD_Pixel(x0 - y, y0 + x, color);
      LCD_Pixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1)
    {
      LCD_Pixel(x0 - y, y0 - x, color);
      LCD_Pixel(x0 - x, y0 - y, color);
    }
  }
}

/**
 * @brief 绘制带圆角的矩形
 *
 * 在LCD屏幕上绘制一个带圆角的矩形。
 *
 * @param x 矩形左上角的x坐标
 * @param y 矩形左上角的y坐标
 * @param length 矩形的长度
 * @param width 矩形的宽度
 * @param r 圆角半径
 * @param size 线条粗细
 * @param color24 线条颜色（24位RGB）
 */
void LCD_Rect_Round(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint16_t r, uint8_t size, ST7565_COLOR_t color24)
{
  LCD_Line(x + (r + 2), y, x + length + size - (r + 2), y, color24);
  LCD_Line(x + (r + 2), y + width - 1, x + length + size - (r + 2), y + width - 1, color24);
  LCD_Line(x, y + (r + 2), x, y + width - size - (r + 2), color24);
  LCD_Line(x + length - 1, y + (r + 2), x + length - 1, y + width - size - (r + 2), color24);

  LCD_Circle_Helper(x + (r + 2), y + (r + 2), (r + 2), 1, size, color24);
  LCD_Circle_Helper(x + length - (r + 2) - 1, y + (r + 2), (r + 2), 2, size, color24);
  LCD_Circle_Helper(x + length - (r + 2) - 1, y + width - (r + 2) - 1, (r + 2), 4, size, color24);
  LCD_Circle_Helper(x + (r + 2), y + width - (r + 2) - 1, (r + 2), 8, size, color24);
}

void LCD_Circle_Fill_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, ST7565_COLOR_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if (cornername & 0x1)
    {
      LCD_Line(x0 + x, y0 - y, x0 + x, y0 - y + 2 * y + delta, color);
      LCD_Line(x0 + y, y0 - x, x0 + y, y0 - x + 2 * x + delta, color);
    }
    if (cornername & 0x2)
    {
      LCD_Line(x0 - x, y0 - y, x0 - x, y0 - y + 2 * y + delta, color);
      LCD_Line(x0 - y, y0 - x, x0 - y, y0 - x + 2 * x + delta, color);
    }
  }
}

/**
 * @brief 绘制带圆角的矩形并填充
 *
 * 在LCD上绘制一个带有圆角的矩形，并填充指定的颜色。
 *
 * @param x 矩形左上角的 x 坐标
 * @param y 矩形左上角的 y 坐标
 * @param length 矩形的长度
 * @param width 矩形的宽度
 * @param r 圆角半径
 * @param color 填充颜色
 */
void LCD_Rect_Round_Fill(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint16_t r, ST7565_COLOR_t color)
{
  LCD_Rect_Fill(x + r, y, length - 2 * r, width, color);
  LCD_Circle_Fill_Helper(x + length - r - 1, y + r, r, 1, width - 2 * r - 1, color);
  LCD_Circle_Fill_Helper(x + r, y + r, r, 2, width - 2 * r - 1, color);
}

void LCD_Char(int16_t x, int16_t y, const GFXglyph *glyph, const GFXfont *font, uint8_t size, ST7565_COLOR_t color)
{
  uint8_t *bitmap = font->bitmap;
  uint16_t bo = glyph->bitmapOffset;
  uint8_t bits = 0, bit = 0;
  uint16_t set_pixels = 0;
  uint8_t cur_x, cur_y;
  for (cur_y = 0; cur_y < glyph->height; cur_y++)
  {
    for (cur_x = 0; cur_x < glyph->width; cur_x++)
    {
      if (bit == 0)
      {
        bits = (*(const unsigned char *)(&bitmap[bo++]));
        bit = 0x80;
      }
      if (bits & bit)
        set_pixels++;
      else if (set_pixels > 0)
      {
        LCD_Rect_Fill(x + (glyph->xOffset + cur_x - set_pixels) * size, y + (glyph->yOffset + cur_y) * size, size * set_pixels, size, color);
        set_pixels = 0;
      }
      bit >>= 1;
    }
    if (set_pixels > 0)
    {
      LCD_Rect_Fill(x + (glyph->xOffset + cur_x - set_pixels) * size, y + (glyph->yOffset + cur_y) * size, size * set_pixels, size, color);
      set_pixels = 0;
    }
  }
}

// // 绘制单个字符的函数
// void drawChar(int x, int y, char c) {
//     // 确保字符在有效范围内
//     if (c < '0' || c > ':') return;

//     // 字符在数组中的索引
//     int index = c - '0';

//     // 取出该字符的字形数据
//     const uint8_t *charData = Font_1616[index];

//     // 绘制字符
//     for (int row = 0; row < 16; row++) {
//         for (int col = 0; col < 16; col++) {
//             // 计算字形数据中的字节位置和位位置
//             int byteIndex = row * 2 + col / 8;
//             int bitIndex = col % 8;

//             // 取出当前像素的值
//             uint8_t pixel = (charData[byteIndex] >> (7 - bitIndex)) & 0x01;

//             // 绘制像素到显示屏
//             LCD_Pixel(x + col, y + row, pixel);
//         }
//     }
// }

// void LCD_drawText(int x, int y, const char *text)
// {
//   int cursorX = x;
//   int cursorY = y;

//   while (*text)
//   {
//     drawChar(cursorX, cursorY, *text);
//     cursorX += 16; // 每个字符的宽度
//     text++;
//   }
// }

void LCD_Font(uint16_t x, uint16_t y, const char *text, const GFXfont *p_font, uint8_t size, ST7565_COLOR_t color)
{
  int16_t cursor_x = x;
  int16_t cursor_y = y;
  GFXfont font;
  memcpy(&font, p_font, sizeof(GFXfont));

  for (uint16_t text_pos = 0; text_pos < strlen(text); text_pos++)
  {
    char c = text[text_pos];
    if (c == '\n')
    {
      cursor_x = x;
      cursor_y += font.yAdvance * size;
    }
    else if (c >= font.first && c <= font.last && c != '\r')
    {
      GFXglyph glyph;
      memcpy(&glyph, &font.glyph[c - font.first], sizeof(GFXglyph));

      // Draw the character
      LCD_Char(cursor_x, cursor_y, &glyph, &font, size, color);

      // Move cursor to the next character position
      cursor_x += (glyph.xAdvance + 1) * size; // Adjust for additional spacing if needed
    }
  }
}

unsigned char LCD_X, LCD_Y;

inline void LCD_Goto2X(uint16_t x, uint16_t y)
{
  // 设置页地址
  uint8_t page = y / 8; // 页地址是y坐标的整数除以8
  uint8_t col = x;      // 列地址是x坐标

  // 发送页地址命令
  ST7565_send_command(CMD_SET_PAGE | page);

  // 发送列地址命令
  ST7565_send_command(CMD_SET_COLUMN_UPPER | (col >> 4));   // 高4位
  ST7565_send_command(CMD_SET_COLUMN_LOWER | (col & 0x0F)); // 低4位
}

/**
 * @brief 设置LCD对比度
 *
 * 通过给定的值设置LCD的对比度。
 *
 * @param lcd LCD设备指针
 * @param val 对比度值（0-63）
 */
void LCD_Contrast(uint8_t val)
{
  // 关闭显示
  ST7565_send_command(CMD_DISPLAY_OFF);
  ST7565_send_command(CMD_SET_VOLUME_FIRST);
  ST7565_send_command(CMD_SET_VOLUME_SECOND | (val & 0x3f));
  ST7565_send_command(CMD_DISPLAY_ON);
}

void LCD_DrawIcon_phone(uint8_t icon)
{

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

void LCD_DrawIcon_Signal(uint8_t level)
{
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

void LCD_DrawIcon_Printer(uint8_t status)
{

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

void LCD_DrawIcon_Card(uint8_t status)
{

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

void LCD_DrawIcon_Battery(uint8_t level)
{

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
    ST7565_Buffer_logo[99] = 0xFF;
  }
  else if (level > 20 && level <= 50)
  {
    // 显示电池图标 2 格
    ST7565_Buffer_logo[93] = 0xFF;
    ST7565_Buffer_logo[99] = 0xFF;
    ST7565_Buffer_logo[101] = 0xFF;
  }
  else if (level > 50 && level <= 80)
  {
    // 显示电池图标 3 格
    ST7565_Buffer_logo[93] = 0xFF;
    ST7565_Buffer_logo[99] = 0xFF;
    ST7565_Buffer_logo[101] = 0xFF;
    ST7565_Buffer_logo[103] = 0xFF;
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

void LCD_DrawIcon_Lock(uint8_t status)
{
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

void LCD_DrawIcon_Upload(uint8_t status)
{
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

void LCD_DrawIcon_Download(uint8_t status)
{
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