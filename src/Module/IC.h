#ifndef __IC_H
#define __IC_H

extern uint16_t IC_Prescaler;

void IC_INIT(void);
void IC_Reconfig_Prescaler(uint16_t prescaler);
uint32_t IC_GetFreq_High(void);
uint32_t IC_GetFreq_Low(void);
uint32_t IC_GetDuty(void);

#endif