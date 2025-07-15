#ifndef __OLED_H
#define __OLED_H

void OLED_WriteCmd(uint8_t cmd);
void OLED_Init(void);
void OLED_WriteData(uint8_t *data, uint8_t count);
void OLED_SetCursor(uint8_t x, uint8_t page);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t x, uint8_t page, char ch);
void OLED_ShowString(uint8_t x, uint8_t page, const char *str);
void OLED_ShowNum(uint8_t x, uint8_t page, uint32_t num, uint8_t len);
void OLED_Update(void);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t state);
void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

#endif