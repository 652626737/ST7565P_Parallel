#include "main.h"
#include "time.h"
#include "stm32f1xx_it.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_rtc.h"
#include "stm32f1xx_hal_uart.h"
#include "string.h"
#include "ST7565_Parallel.h"
#include <stdarg.h>
#include <stdio.h> 

// #include "fonts/Font_3_Tiny.h"
// #include "fonts/Font_3_PicoPixel.h"
// #include "fonts/Font_3_TomThumb_Extended.h"
// #include "fonts/Font_5_Org.h"
// #include "fonts/Font_8_Sinclair.h"
// #include "fonts/Font_8_Sinclair_Inverted.h"
// #include "fonts/Font_8_Tiny.h"
// #include "fonts/Font_8_Myke.h"
// #include "fonts/Font_8_Default.h"
// #include "fonts/Font_8_Retro.h"
// #include "fonts/Font_9_Mono.h"
// #include "fonts/Font_9_Mono_Bold.h"
// #include "fonts/Font_9_Mono_Bold_Oblique.h"
// #include "fonts/Font_9_Mono_Oblique.h"
// #include "fonts/Font_9_Sans.h"
// #include "fonts/Font_9_Sans_Bold.h"
// #include "fonts/Font_9_Sans_Bold_Oblique.h"
// #include "fonts/Font_9_Sans_Oblique.h"
// #include "fonts/Font_9_Serif.h"
// #include "fonts/Font_9_Serif_Bold.h"
// #include "fonts/Font_9_Serif_Bold_Italic.h"
// #include "fonts/Font_9_Serif_Italic.h"
// #include "fonts/Font_12_Mono.h"
// #include "fonts/Font_12_Mono_Bold.h"
// #include "fonts/Font_12_Mono_Bold_Oblique.h"
// #include "fonts/Font_12_Mono_Oblique.h"
// #include "fonts/Font_12_Sans.h"
// #include "fonts/Font_12_Sans_Bold.h"
// #include "fonts/Font_12_Sans_Bold_Oblique.h"
// #include "fonts/Font_12_Sans_Oblique.h"
// #include "fonts/Font_12_Serif.h"
// #include "fonts/Font_12_Serif_Bold.h"
// #include "fonts/Font_12_Serif_Bold_Italic.h"
// #include "fonts/Font_12_Serif_Italic.h"
// #include "fonts/Font_16_Arial.h"
// #include "fonts/Font_16_Arial_Bold.h"
// #include "fonts/Font_16_Arial_Italic.h"
// #include "fonts/Font_16_Default.h"
// #include "fonts/Font_16_FranklinGothic.h"
// #include "fonts/Font_16_Hallfetica.h"
// #include "fonts/Font_16_Nadianne.h"
// #include "fonts/Font_16_Sinclair.h"
// #include "fonts/Font_16_Sinclair_Inverted.h"
// #include "fonts/Font_16_Swiss_Outline.h"
// #include "fonts/Font_16_Matrix_Full.h"
// #include "fonts/Font_16_Matrix_Full_Slash.h"
// #include "fonts/Font_16_Matrix_Num.h"
// #include "fonts/Font_16_Arial_Round.h"
// #include "fonts/Font_16_OCR.h"
// #include "fonts/Font_16_Segment_16_Full.h"
// #include "fonts/Font_16_Grotesk.h"
// #include "fonts/Font_16_Grotesk_Bold.h"
// #include "fonts/Font_16_Retro.h"
// #include "fonts/Font_18_Mono.h"
// #include "fonts/Font_18_Mono_Bold.h"
// #include "fonts/Font_18_Mono_Bold_Oblique.h"
// #include "fonts/Font_18_Mono_Oblique.h"
// #include "fonts/Font_18_Sans.h"
// #include "fonts/Font_18_Sans_Bold.h"
// #include "fonts/Font_18_Sans_Bold_Oblique.h"
// #include "fonts/Font_18_Sans_Oblique.h"
// #include "fonts/Font_18_Serif.h"
// #include "fonts/Font_18_Serif_Bold.h"
// #include "fonts/Font_18_Serif_Bold_Italic.h"
// #include "fonts/Font_18_Serif_Italic.h"
// #include "fonts/Font_24_Matrix_Num.h"
// #include "fonts/Font_24_Inconsola.h"
// #include "fonts/Font_24_Ubuntu.h"
// #include "fonts/Font_24_Ubuntu_Bold.h"
// #include "fonts/Font_24_Segment_16_Full.h"
// #include "fonts/Font_24_Mono.h"
// #include "fonts/Font_24_Mono_Bold.h"
// #include "fonts/Font_24_Mono_Bold_Oblique.h"
// #include "fonts/Font_24_Mono_Oblique.h"
// #include "fonts/Font_24_Grotesk.h"
// #include "fonts/Font_24_Grotesk_Bold.h"
// #include "fonts/Font_24_Sans.h"
// #include "fonts/Font_24_Sans_Bold.h"
// #include "fonts/Font_24_Sans_Bold_Oblique.h"
// #include "fonts/Font_24_Sans_Oblique.h"
// #include "fonts/Font_24_Serif.h"
// #include "fonts/Font_24_Serif_Bold.h"
// #include "fonts/Font_24_Serif_Bold_Italic.h"
// #include "fonts/Font_24_SerifItalic.h"
// #include "fonts/Font_32_Dingbats.h"
// #include "fonts/Font_32_Special.h"
// #include "fonts/Font_32_Calibri_Bold.h"
// #include "fonts/Font_32_Arial_Num_Plus.h"
// #include "fonts/Font_32_Matrix_Num.h"
// #include "fonts/Font_32_Segment_7_Full.h"
// #include "fonts/Font_32_Segment_7_Num_Plus.h"
// #include "fonts/Font_32_Segment_16_Full.h"
// #include "fonts/Font_32_Segment_18_Full.h"
// #include "fonts/Font_32_Grotesk.h"
// #include "fonts/Font_32_Grotesk_Bold.h"
// #include "fonts/Font_40_Segment_16_Full.h"
// #include "fonts/Font_48_Battery.h"
// #include "fonts/Font_48_Segment_16_Num.h"
// #include "fonts/Font_64_Segment_16_Num.h"
// #include "fonts/Font_64_Segment_7_Num.h"

#define MAX_INIT_ATTEMPTS 3
static uint8_t init_attempts = 0;

// 定义显示器的宽度和高度
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

#define FULLSCREEN (DISPLAY_WIDTH * (DISPLAY_HEIGHT / 8)) // 1536 bytes

// 定义缓冲区以覆盖整个屏幕
uint8_t screenBuffer[FULLSCREEN];

// 定义 LCD 引脚结构体
LcdDataPin cs = {GPIOA, GPIO_PIN_8};   // Chip Select 引脚
LcdDataPin rst = {GPIOA, GPIO_PIN_11}; // Reset 引脚
LcdDataPin dc = {GPIOB, GPIO_PIN_1};   // Data/Command 引脚
LcdDataPin wr = {GPIOB, GPIO_PIN_0};   // Write 引脚
LcdDataPin rd = {GPIOB, GPIO_PIN_3};   // Read 引脚
LcdDataPin dataPins[] = {
    {GPIOB, GPIO_PIN_8},  // D0
    {GPIOB, GPIO_PIN_9},  // D1
    {GPIOB, GPIO_PIN_10}, // D2
    {GPIOB, GPIO_PIN_11}, // D3
    {GPIOB, GPIO_PIN_12}, // D4
    {GPIOB, GPIO_PIN_13}, // D5
    {GPIOB, GPIO_PIN_14}, // D6
    {GPIOB, GPIO_PIN_15}  // D7
};

// 定义 LCD 引脚结构体
ST7565_Parallel mylcd(DISPLAY_WIDTH, DISPLAY_HEIGHT, cs, rst, dc, wr, rd, dataPins);
ST7565_Parallel_Screen fullScreen(screenBuffer, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, 0);
RTC_HandleTypeDef hrtc;
UART_HandleTypeDef huart1;

void SystemClock_Config(void);
void MX_RTC_Init(void);
void MX_GPIO_Init(void);
void UART_Print(const char *str);
void MX_USART1_UART_Init(void);
void Set_RTC_Alarm(void);
void MX_NVIC_Init(void);
void HandleClockInitFailure(void);

uint32_t start_time, end_time;
uint32_t elapsed_times[10]; // Array to store elapsed times
int main(void)
{

  start_time = HAL_GetTick();
  // Initialize the HAL Library
  HAL_Init();
  end_time = HAL_GetTick();
  elapsed_times[0] = end_time - start_time; // Time for HAL_Init()

  // System Clock Configuration
  start_time = HAL_GetTick();
  do
  {
    SystemClock_Config();
  } while (init_attempts > 0 && init_attempts < MAX_INIT_ATTEMPTS);
  end_time = HAL_GetTick();
  elapsed_times[1] = end_time - start_time; // Time for HAL_Init()
  // GPIO Initialization
  start_time = HAL_GetTick();
  MX_GPIO_Init();
  end_time = HAL_GetTick();
  elapsed_times[2] = end_time - start_time; // Time for MX_GPIO_Init()

  // RTC Initialization
  start_time = HAL_GetTick();
  MX_RTC_Init();
  end_time = HAL_GetTick();
  elapsed_times[3] = end_time - start_time; // Time for MX_RTC_Init()

  // NVIC Initialization
  start_time = HAL_GetTick();
  MX_NVIC_Init();
  end_time = HAL_GetTick();
  elapsed_times[4] = end_time - start_time; // Time for MX_NVIC_Init()

  // USART1 UART Initialization
  start_time = HAL_GetTick();
  MX_USART1_UART_Init();
  end_time = HAL_GetTick();
  elapsed_times[6] = end_time - start_time; // Time for MX_USART1_UART_Init()

  // Now you can use UART to print the elapsed times
  for (int i = 0; i < 10; i++)
  {
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "Step %d elapsed time: %lu ms\n", i + 1, elapsed_times[i]);
    UART_Print(buffer);
  }

  // Indicate that initialization is complete
  UART_Print("init_ok\n");

  // 刷新显示器，将缓冲区内容更新到显示器
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // 打开LED
  // MX_IWDG_Init();

  mylcd.LCDbegin();                      // initialize the OLED                // 设置显示方向
  mylcd.ActiveBuffer = &fullScreen;      // Assign address of screen object to be the "active buffer" pointer
  mylcd.LCDclearBuffer();                // Clear a
  mylcd.setFontNum(UC1609Font_Default); // set font type
  mylcd.setTextColor(0x00, 0x01);        // set text color
  // mylcd.setTextSize(3);
  // mylcd.drawChar(10, 10, 'A', 0x00, 0x01, 1); // 绘制字符 'A'
  mylcd.drawText(0, 0,"Hello World", 0x01, 0x00, 1);
  mylcd.drawRect(0, 15, 64, 30, 0x01);
  mylcd.drawRoundRect(64, 15, 20, 30, 5, 0x01);

  mylcd.fillRect(10, 10, 20, 20, 0x01);
  // set text size
  // for (int i = 200; i < 1000; i++)

  for (size_t i = 0; i <= 7; i++)

  {
    switch (i)
    {
    case 0:
      mylcd.LCD_DrawIcon_phone(2);
      break;
    case 1:
      mylcd.LCD_DrawIcon_Signal(100);
      break;
    case 2:
      mylcd.LCD_DrawIcon_Printer(1);
      break;
    case 3:
      mylcd.LCD_DrawIcon_Card(1);
      break;
    case 4:
      mylcd.LCD_DrawIcon_Lock(1);

      break;
    case 5:
      mylcd.LCD_DrawIcon_Battery(100);
      break;
    case 6:
      mylcd.LCD_DrawIcon_Upload(1);
      break;
    case 7:
      mylcd.LCD_DrawIcon_Download(1);
      break;
    default:
      break;
    }
    mylcd.LCDupdate();
    HAL_Delay(50);
  }

  mylcd.LCDupdate(); // Update screen , write active buffer to screen

  while (1)
  {
  }
}
void MX_RTC_Init(void)
{
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  // hrtc.Init.SynchPrediv = 255;
  // hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  // hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  // hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
}

void UART_Print(const char *str)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}

void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

void MX_NVIC_Init(void)
{
  /* RTC_IRQn interrupt configuration */
  // HAL_NVIC_SetPriority(RTC_IRQn, 0, 0);
  // HAL_NVIC_EnableIRQ(RTC_IRQn);
  // HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 3, 0);
  // HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

// static void MX_IWDG_Init(void)
// {
//   hiwdg.Instance = IWDG;
//   hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
//   hiwdg.Init.Reload = 4095;
//   HAL_IWDG_Init(&hiwdg);
// }

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  // 初始化 RCC 振荡器
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    HandleClockInitFailure();
  }

  // 初始化 CPU、AHB 和 APB 总线时钟
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  // 初始化外设时钟
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    HandleClockInitFailure();
  }

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    HandleClockInitFailure();
  }

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    HandleClockInitFailure();
  }

  // 启用或禁用时钟安全系统
  HAL_RCC_DisableCSS(); // 如果你希望禁用 CSS
                        // 重置初始化尝试次数
  init_attempts = 0;
}

void HandleClockInitFailure(void)
{
  init_attempts++;

  if (init_attempts >= MAX_INIT_ATTEMPTS)
  {
    // 如果多次尝试失败，执行硬件复位
    HAL_NVIC_SystemReset();
  }
  else
  {
    // 配置独立看门狗
    IWDG_HandleTypeDef hiwdg;
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
    hiwdg.Init.Reload = 4095; // 约1秒的超时时间

    if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
      Error_Handler();
    }

    // 等待看门狗触发重启
    while (1)
    {
      // 可以在这里添加一些指示灯闪烁等来表示重启状态
    }
  }
}

void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void Set_RTC_Alarm(void)
{
  RTC_AlarmTypeDef sAlarm = {0};
  RTC_TimeTypeDef sTime = {0};

  // 获取当前时间
  if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  // 计算闹钟时间（当前时间 + 2秒）
  sAlarm.AlarmTime.Hours = sTime.Hours;
  sAlarm.AlarmTime.Minutes = sTime.Minutes;
  sAlarm.AlarmTime.Seconds = sTime.Seconds + 2;

  // 处理秒数溢出
  if (sAlarm.AlarmTime.Seconds >= 60)
  {
    sAlarm.AlarmTime.Seconds -= 60;
    sAlarm.AlarmTime.Minutes += 1;

    // 处理分钟数溢出
    if (sAlarm.AlarmTime.Minutes >= 60)
    {
      sAlarm.AlarmTime.Minutes -= 60;
      sAlarm.AlarmTime.Hours += 1;

      // 处理小时数溢出
      if (sAlarm.AlarmTime.Hours >= 24)
      {
        sAlarm.AlarmTime.Hours -= 24;
      }
    }
  }

  sAlarm.Alarm = RTC_ALARM_A;
  // sAlarm.AlarmTime.SubSeconds = 0; // STM32F103 不需要配置
  // sAlarm.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY; // 用于选择日期，配置为 Monday 或其他日期
  // sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  // sAlarm.AlarmMask = RTC_ALARMMASK_NONE; // 不掩盖日期时间
  // sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE; // 不掩盖子秒

  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  // 使能 RTC 闹钟中断
  __HAL_RTC_ALARM_ENABLE_IT(&hrtc, RTC_IT_ALRA);
  // __HAL_RTC_WRITEPROTECTION_ENABLE();
}

void Error_Handler(void)
{

  // Example error message
  UART_Print("Error: Initialization failed!\n");

  while (1)
  {
    // 错误处理
  }
}
