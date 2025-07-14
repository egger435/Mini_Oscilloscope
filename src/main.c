#include "stm32f10x.h"
#include "./Module/OLED.h"
#include "./Module/ADC.h"
#include "./Module/WaveDisplay.h"
#include "./Module/TestPWM.h"
#include "./Module/Delay.h"
#include "./Module/IC.h"
#include "./Module/Encoder.h"
#include <stdio.h>

#define DISPLAY_HEIGHT 64

uint8_t data_ready = 0;
uint8_t i = 0;
uint8_t freq_state = 1;  // 0表示低频，1表示高频

// 按钮用于控制选择高频模式和低频模式
void Button_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t Button_IsPressed(void)
{
    return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == Bit_RESET;
}

void SetUp(void)
{
    SystemInit();

	PWM_INIT();  // PA10引脚输出PWM测试波形

	OLED_Init();
	Encoder_Init();
	Button_Init();
	
	ADC1_Init();
	TIM3_Init(1000);  // 1kHz采样率
	IC_INIT();

	OLED_Clear();
	OLED_Update();
}

void Update(void)
{
	if (Button_IsPressed())  // 测频模式选择
	{
		Delay_ms(30);
		if (Button_IsPressed())
		{
			freq_state = !freq_state;
			IC_Reconfig_Prescaler(freq_state ? 72 - 1 : 7200 - 1); // 高频或低频模式
			while (Button_IsPressed());
		}
	}

	if (data_ready)
	{
		data_ready = 0;
		// 获取采样数据
		WaveData wave = WD_Process_Wave_Data(adc_buffer, SAMPLE_COUNT);
		uint32_t freq = 0;
		if (freq_state == 1)
		{
			freq = IC_GetFreq_High();
		}
		else
		{
			freq = IC_GetFreq_Low();
		}
		uint32_t duty = IC_GetDuty();
		
		OLED_Clear();
		// 显示测量结果
		WD_Display_Results(&wave);
		char buffer[20];
		sprintf(buffer, "freq:%4luhz", freq);
		OLED_ShowString(0, 2, buffer);
		sprintf(buffer, "duty:%4lu%%", duty);
		OLED_ShowString(0, 3, buffer);
		OLED_ShowNum(0, 4, Delay_value, 4);
		OLED_ShowString(0, 5, freq_state ? "High" : "Low");
		WD_Draw_Waveform(adc_buffer, 128, 0, 4095);
		OLED_Update();

		ADC_Cmd(ADC1, ENABLE);
	}
	else
	{
		// 采样数据
		while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); 
		uint16_t value = ADC_GetConversionValue(ADC1);

		if (Delay_value > 0)
		{
			Delay_us(Delay_value);  // 调整采样间隔来控制屏幕波形缩放
		}

		adc_buffer[i++] = value;  // 将采样值存入缓冲区
		if (i >= SAMPLE_COUNT - 1)
		{
			i = 0;
			data_ready = 1;
			ADC_Cmd(ADC1, DISABLE);
		}
	}

	Process_Encoder();  // 编码器数据处理
}

int main(void)
{
	SetUp();

	while (1)
	{
		Update();
	}
}