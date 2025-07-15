#ifndef __WAVE_DISPLAY_H
#define __WAVE_DISPLAY_H

typedef struct 
{
    uint16_t min;
    uint16_t max;
    float vpp;
    float freq;
    float duty;
} WaveData;

WaveData WD_Process_Wave_Data(volatile uint16_t *data, uint16_t length);
void WD_Draw_Waveform(volatile uint16_t *data, uint16_t length, uint16_t min_val, uint16_t max_val);
void WD_Display_Results(WaveData *wave);

#endif