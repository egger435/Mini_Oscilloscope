#ifndef __ENCODER_H
#define __ENCODER_H

extern int32_t Delay_value;  // 延时值，单位微秒，调整采样间隔来控制屏幕波形缩放

void Encoder_Init(void);
void Process_Encoder();

#endif