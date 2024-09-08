/*
 * us_delay.h
 *
 *  Created on: [创建日期]
 *      Author: [你的名字]
 */

#ifndef US_DELAY_H
#define US_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

// #include <stdint.h>
#include "stm32f1xx_hal.h"

/**
 * @brief 初始化微秒延时功能
 * 
 * 在使用延时函数之前，需要先调用此函数进行初始化。
 */
void delay_us_init(void);

/**
 * @brief 微秒级延时函数
 * 
 * @param us 延时的微秒数
 */
void delay_us(uint32_t us);

/**
 * @brief 毫秒级延时函数
 * 
 * @param ms 延时的毫秒数
 */
void delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* US_DELAY_H */
