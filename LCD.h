#ifndef LCD_H
#define LCD_H
#include <stdint.h>
void GPIO_Config(void);
void SPI_Config(void);
void SPI_SendData(uint8_t data);
void ST7735_SendCommand(uint8_t cmd);
void ST7735_SendData(uint8_t data);
void ST7735_Init(void);
void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void ST7735_DrawPixel(uint8_t x, uint8_t y, uint16_t color);
void ST7735_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
void ST7735_DrawChar(uint8_t x, uint8_t y, char ch, uint16_t color, uint16_t bgcolor);
void ST7735_DrawString(uint8_t x, uint8_t y, const char *str, uint16_t color, uint16_t bgcolor);

#endif