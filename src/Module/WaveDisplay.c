#include <stm32f10x.h>
#include "OLED.h"
#include "math.h"
#include "WaveDisplay.h"
#include <stdio.h>

WaveData WD_Process_Wave_Data(volatile uint16_t *data, uint16_t length)
{
    WaveData result = {4095, 0, 0, 0, 0};
    
    // 寻找最大值和最小值
    for(uint16_t i = 0; i < length; i++) 
    {
        if(data[i] < result.min) result.min = data[i];
        if(data[i] > result.max) result.max = data[i];
    }
    return result;
}

// 绘制波形
void WD_Draw_Waveform(volatile uint16_t *data, uint16_t length, uint16_t min_val, uint16_t max_val) 
{
    // 计算垂直缩放比例
    float scale = (max_val == min_val) ? 1.0f : 63.0f / (max_val - min_val);
    
    // 绘制波形点
    for(uint16_t i = 0; i < length; i++) 
    {
        uint8_t x = i * 128 / length; // 水平缩放
        uint8_t y = 63 - (uint8_t)((data[i] - min_val) * scale);
        
        // 确保点在显示范围内
        if(x < 128 && y < 64) 
        {
            OLED_DrawPoint(x, y, 1);
            
            // 连接相邻点
            if(i > 0) 
            {
                uint8_t prev_x = (i-1) * 128 / length;
                uint8_t prev_y = 63 - (uint8_t)((data[i-1] - min_val) * scale);
                OLED_DrawLine(prev_x, prev_y, x, y);
            }
        }
    }
}

// 显示测量结果
void WD_Display_Results(WaveData *wave)
{
    char buffer[20];
    // 显示电压范围
    uint16_t min_mv = (uint32_t)wave->min * 3300 / 4096;
    sprintf(buffer, "Min:%4dmV", min_mv);
    OLED_ShowString(0, 0, buffer);
    uint16_t max_mv = (uint32_t)wave->max * 3300 / 4096;
    sprintf(buffer, "Max:%4dmV", max_mv);
    OLED_ShowString(0, 1, buffer);
}