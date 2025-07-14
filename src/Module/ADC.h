#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

#define SAMPLE_COUNT 128  // ADC采样点数量
#define ADC1_DR_ADDRESS ((uint32_t)0x4001244C)  // ADC1数据寄存器地址

extern uint16_t adc_buffer[SAMPLE_COUNT];  // ADC采样数据缓冲区

void ADC1_Init(void);
void TIM3_Init(uint16_t sample_rate_hz);

#endif