// st7565.c

#include "st7565.h"
#include "stm32f1xx_hal.h"

// 延迟函数，用于某些操作之间的短暂延迟
static void ST7565_Delay(uint32_t ms) {
    HAL_Delay(ms);
}

// 初始化 GPIO 引脚
static void ST7565_InitGPIO(ST7565_LCD* lcd) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 初始化控制引脚
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Pin = lcd->cs.pin;
    HAL_GPIO_Init(lcd->cs.port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = lcd->rst.pin;
    HAL_GPIO_Init(lcd->rst.port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = lcd->a0.pin;
    HAL_GPIO_Init(lcd->a0.port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = lcd->wr.pin;
    HAL_GPIO_Init(lcd->wr.port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = lcd->rd.pin;
    HAL_GPIO_Init(lcd->rd.port, &GPIO_InitStruct);

    // 初始化数据引脚
    for (int i = 0; i < 8; i++) {
        GPIO_InitStruct.Pin = lcd->data[i].pin;
        HAL_GPIO_Init(lcd->data[i].port, &GPIO_InitStruct);
    }
}

// 向 LCD 写入一个字节
static void ST7565_WriteByte(ST7565_LCD* lcd, uint8_t byte) {
    // 设置数据引脚
    for (int i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(lcd->data[i].port, lcd->data[i].pin, (byte & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }

    // 写入时序
    HAL_GPIO_WritePin(lcd->wr.port, lcd->wr.pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(lcd->wr.port, lcd->wr.pin, GPIO_PIN_SET);
}

void ST7565_WriteCommand(ST7565_LCD* lcd, uint8_t command) {
    HAL_GPIO_WritePin(lcd->cs.port, lcd->cs.pin, GPIO_PIN_RESET);  // 选中芯片
    HAL_GPIO_WritePin(lcd->a0.port, lcd->a0.pin, GPIO_PIN_RESET);  // 命令模式
    ST7565_WriteByte(lcd, command);
    HAL_GPIO_WritePin(lcd->cs.port, lcd->cs.pin, GPIO_PIN_SET);    // 取消选中
}

void ST7565_WriteData(ST7565_LCD* lcd, uint8_t data) {
    HAL_GPIO_WritePin(lcd->cs.port, lcd->cs.pin, GPIO_PIN_RESET);  // 选中芯片
    HAL_GPIO_WritePin(lcd->a0.port, lcd->a0.pin, GPIO_PIN_SET);    // 数据模式
    ST7565_WriteByte(lcd, data);
    HAL_GPIO_WritePin(lcd->cs.port, lcd->cs.pin, GPIO_PIN_SET);    // 取消选中
}

void ST7565_Init(ST7565_LCD* lcd) {
    // 初始化 GPIO
    ST7565_InitGPIO(lcd);

    // 初始化图形库
    ST7565_Graphics_Init(&lcd->graphics, ST7565_WIDTH, ST7565_HEIGHT);

    // 硬件复位
    HAL_GPIO_WritePin(lcd->rst.port, lcd->rst.pin, GPIO_PIN_RESET);
    ST7565_Delay(50);
    HAL_GPIO_WritePin(lcd->rst.port, lcd->rst.pin, GPIO_PIN_SET);
    ST7565_Delay(50);

    // 发送初始化命令序列
    ST7565_WriteCommand(lcd, ST7565_CMD_DISPLAY_OFF);
    ST7565_WriteCommand(lcd, ST7565_CMD_SET_BIAS_7);
    ST7565_WriteCommand(lcd, ST7565_CMD_SET_ADC_NORMAL);
    ST7565_WriteCommand(lcd, ST7565_CMD_SET_COM_NORMAL);
    ST7565_WriteCommand(lcd, ST7565_CMD_SET_DISP_START_LINE);
    ST7565_WriteCommand(lcd, ST7565_CMD_SET_POWER_CONTROL | 0x4);
    ST7565_Delay(50);
    ST7565_WriteCommand(lcd, ST7565_CMD_SET_POWER_CONTROL | 0x6);
    ST7565_Delay(50);
    ST7565_WriteCommand(lcd, ST7565_CMD_SET_POWER_CONTROL | 0x7);
    ST7565_Delay(10);
    ST7565_WriteCommand(lcd, ST7565_CMD_SET_RESISTOR_RATIO | 0x6);

    // 设置初始对比度
    ST7565_SetContrast(lcd, 0x18);

    ST7565_WriteCommand(lcd, ST7565_CMD_DISPLAY_ON);
    ST7565_WriteCommand(lcd, ST7565_CMD_SET_ALLPTS_NORMAL);

    ST7565_Clear(lcd);
    ST7565_UpdateDisplay(lcd);
}

void ST7565_SetContrast(ST7565_LCD* lcd, uint8_t contrast) {
    ST7565_WriteCommand(lcd, ST7565_CMD_SET_VOLUME_FIRST);
    ST7565_WriteCommand(lcd, ST7565_CMD_SET_VOLUME_SECOND | (contrast & 0x3f));
}

void ST7565_DisplayOn(ST7565_LCD* lcd) {
    ST7565_WriteCommand(lcd, ST7565_CMD_DISPLAY_ON);
}

void ST7565_DisplayOff(ST7565_LCD* lcd) {
    ST7565_WriteCommand(lcd, ST7565_CMD_DISPLAY_OFF);
}

void ST7565_UpdateDisplay(ST7565_LCD* lcd) {
    for (int page = 0; page < 8; page++) {
        ST7565_WriteCommand(lcd, ST7565_CMD_SET_PAGE | page);
        ST7565_WriteCommand(lcd, ST7565_CMD_SET_COLUMN_LOWER);
        ST7565_WriteCommand(lcd, ST7565_CMD_SET_COLUMN_UPPER);
        
        for (int col = 0; col < ST7565_WIDTH; col++) {
            ST7565_WriteData(lcd, lcd->graphics.buffer[page * ST7565_WIDTH + col]);
        }
    }
}

void ST7565_Clear(ST7565_LCD* lcd) {
    ST7565_Graphics_Clear(&lcd->graphics, ST7565_COLOR_WHITE);
}

void ST7565_DrawPixel(ST7565_LCD* lcd, int16_t x, int16_t y, uint8_t color) {
    ST7565_Graphics_DrawPixel(&lcd->graphics, x, y, color);
}

void ST7565_DrawLine(ST7565_LCD* lcd, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color) {
    ST7565_Graphics_DrawLine(&lcd->graphics, x0, y0, x1, y1, color);
}

void ST7565_DrawRect(ST7565_LCD* lcd, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
    ST7565_Graphics_DrawRect(&lcd->graphics, x, y, w, h, color);
}

void ST7565_FillRect(ST7565_LCD* lcd, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
    ST7565_Graphics_FillRect(&lcd->graphics, x, y, w, h, color);
}