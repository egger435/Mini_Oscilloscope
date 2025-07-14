#include <stm32f10x.h>
#include "Encoder.h"

#define ENCODER_TIMER TIM4  // 使用TIM4作为编码器接口
#define ENCODER_GPIO_PORT GPIOB  // 编码器连接到GPIOB
#define ENCODER_GPIO_CLK RCC_APB2Periph_GPIOB
#define ENCODER_PIN_A GPIO_Pin_6
#define ENCODER_PIN_B GPIO_Pin_7

int32_t Delay_value = 0;

// 用TIM4作为编码器输入
void Encoder_Init(void)
{
    RCC_APB2PeriphClockCmd(ENCODER_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = ENCODER_PIN_A | ENCODER_PIN_B;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_Init(ENCODER_GPIO_PORT, &GPIO_InitStructure);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(ENCODER_TIMER, &TIM_TimeBaseStructure);

    // 配置编码器接口
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1 | TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_ICInit(ENCODER_TIMER, &TIM_ICInitStructure);

    // 启用编码器模式
    TIM_EncoderInterfaceConfig(ENCODER_TIMER, TIM_EncoderMode_TI12, 
                               TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    // 清除计数器并使能定时器
    TIM_SetCounter(ENCODER_TIMER, 0);
    TIM_Cmd(ENCODER_TIMER, ENABLE);
}

// 处理编码器数据
void Process_Encoder()
{
    // 读取编码器计数值
    int32_t count = (int32_t)TIM_GetCounter(ENCODER_TIMER);
    static int32_t last_count = 0;
    int32_t delta = count - last_count;
    last_count = count;

    // 处理编码器增量
    if (delta != 0)
    {
        Delay_value += delta * 5;  // 根据增量调整延时值
        if (Delay_value < 0) Delay_value = 0;  // 确保延时值不为负
        if (Delay_value > 4000) Delay_value = 4000;  
    }
}