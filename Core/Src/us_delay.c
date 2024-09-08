/*
 * us_delay.c
 *
 *  Created on: 2024/04/27
 *      Author: DubininRE
 */

#include "us_delay.h"

static TIM_HandleTypeDef htim2;
static uint32_t arr;

/**
 * @brief ?????????
 * 
 * ?? TIM2 ????????????
 */
void delay_us_init(void) {
    RCC_ClkInitTypeDef clkinitstruct = {0};
    uint32_t pFLatency;
    
    /* ?? TIM2 ?? */
    __HAL_RCC_TIM2_CLK_ENABLE();
    
    /* ???????? */
    HAL_RCC_GetClockConfig(&clkinitstruct, &pFLatency);
    arr = HAL_RCC_GetPCLK1Freq() / 1000000;  // ?????????
    
    /* ?? TIM2 ????? */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = arr - 1;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        /* ??????? */
        while(1);
    }
    
    /* ??????? */
    if (HAL_TIM_OnePulse_Init(&htim2, TIM_OPMODE_SINGLE) != HAL_OK) {
        /* ??????? */
        while(1);
    }
    
    /* ?????? */
    __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
}

/**
 * @brief ???????
 * 
 * @param us ??????
 */
void delay_us(uint32_t us) {
    __HAL_TIM_SET_AUTORELOAD(&htim2, arr * us);
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
    HAL_TIM_Base_Start(&htim2);
    while(__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_UPDATE) == RESET);
    HAL_TIM_Base_Stop(&htim2);
    __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
}

/**
 * @brief ???????
 * 
 * @param ms ??????
 */
void delay_ms(uint32_t ms) {
    HAL_Delay(ms);  // ?? HAL ????????
}
