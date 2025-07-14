#include <stm32f10x.h>
#include "IC.h"

uint16_t IC_Prescaler = 72 - 1;

// 输入捕获单元初始化
void IC_INIT(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_InternalClockConfig(TIM2);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = IC_Prescaler;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    // 配置输入捕获单元
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_PWMIConfig(TIM2, &TIM_ICInitStructure);

    TIM_SelectInputTrigger(TIM2, TIM_TS_TI2FP2);

    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);

    TIM_Cmd(TIM2, ENABLE);
}

// 重新配置预分频器，用于实时调整测频模式
void IC_Reconfig_Prescaler(uint16_t prescaler)
{
    TIM_Cmd(TIM2, DISABLE);
    
    // 重新配置时基结构
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = prescaler;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
    
    TIM_Cmd(TIM2, ENABLE);

    IC_Prescaler = prescaler;
}

uint32_t IC_GetFreq_High(void)
{
    return 1000000 / TIM_GetCapture2(TIM2);  // 高频模式，预分频值为72
}

uint32_t IC_GetFreq_Low(void)
{
    return 10000 / TIM_GetCapture2(TIM2);  // 低频模式，预分频值为720
}

// 获取占空比
uint32_t IC_GetDuty(void)
{
    return 100 * TIM_GetCapture1(TIM2) / TIM_GetCapture2(TIM2) + 1;
}