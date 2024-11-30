#include "stm32f10x.h"
#include "font.h"
// C?u hình GPIO
void GPIO_Config(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

    // CS, DC, RST (PA1, PA2, PA3) -> Output
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // SCL (PA5), SDA (PA7) -> Alternate function
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void SPI_Config(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    SPI_InitTypeDef SPI_InitStruct;

    SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStruct);
		SPI1->CR1 &= ~SPI_CR1_DFF;
    SPI_Cmd(SPI1, ENABLE);
}
void SPI_SendData(uint8_t data) {
    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); // Wait TXE (Transmit buffer empty)
    SPI_I2S_SendData(SPI1, data);
    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY)); // Wait SPI not busy
}
#define CS_LOW()    GPIO_ResetBits(GPIOA, GPIO_Pin_1)
#define CS_HIGH()   GPIO_SetBits(GPIOA, GPIO_Pin_1)
#define DC_COMMAND() GPIO_ResetBits(GPIOA, GPIO_Pin_2)
#define DC_DATA()    GPIO_SetBits(GPIOA, GPIO_Pin_2)
#define RST_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_3)
#define RST_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_3)

void ST7735_SendCommand(uint8_t cmd) {
    CS_LOW();
    DC_COMMAND();
    SPI_SendData(cmd);
    CS_HIGH();
}

void ST7735_SendData(uint8_t data) {
    CS_LOW();
    DC_DATA();
    SPI_SendData(data);
    CS_HIGH();
}
void ST7735_Init(void) {
    // Reset màn hình
    RST_LOW();
    for (volatile int i = 0; i < 100000; i++); // Delay
    RST_HIGH();
    for (volatile int i = 0; i < 100000; i++); // Delay

    // L?nh kh?i t?o (tham kh?o datasheet)
    ST7735_SendCommand(0x01); // Software reset
    for (volatile int i = 0; i < 150000; i++); // Delay 150ms

    ST7735_SendCommand(0x11); // Exit sleep mode
    for (volatile int i = 0; i < 150000; i++); // Delay 150ms

    ST7735_SendCommand(0x3A); // Interface Pixel Format
    ST7735_SendData(0x05);    // 16-bit color (RGB565)

    ST7735_SendCommand(0x36); // Memory Data Access Control
    ST7735_SendData(0xC8);    // MADCTL: row/column exchange, RGB

    ST7735_SendCommand(0x29); // Display ON
    for (volatile int i = 0; i < 150000; i++); // Delay
}
void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    ST7735_SendCommand(0x2A); // Column address set
    ST7735_SendData(0x00);
    ST7735_SendData(x0);
    ST7735_SendData(0x00);
    ST7735_SendData(x1);

    ST7735_SendCommand(0x2B); // Row address set
    ST7735_SendData(0x00);
    ST7735_SendData(y0);
    ST7735_SendData(0x00);
    ST7735_SendData(y1);

    ST7735_SendCommand(0x2C); // Memory write
}

void ST7735_DrawPixel(uint8_t x, uint8_t y, uint16_t color) {
    ST7735_SetAddressWindow(x, y, x, y); // Set window to 1 pixel
    ST7735_SendData(color >> 8);        // Send high byte
    ST7735_SendData(color & 0xFF);      // Send low byte
}

#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_WHITE   0xFFFF
#define COLOR_BLACK   0x0000
void ST7735_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
    ST7735_SetAddressWindow(x, y, x + w - 1, y + h - 1); // Set window

    for (uint16_t i = 0; i < w * h; i++) {
        ST7735_SendData(color >> 8);   // High byte
        ST7735_SendData(color & 0xFF); // Low byte
    }
}
void ST7735_DrawChar(uint8_t x, uint8_t y, char ch, uint16_t color, uint16_t bgcolor) {
    if (ch < 32 || ch > 126) return; // Ký t? không h?p l?
    const uint8_t *char_data = Font5x7[ch-0]; // L?y d? li?u font c?a ký t?

    for (uint8_t col = 0; col < 5; col++) { // 5 c?t c?a ký t?
        uint8_t line = char_data[col];
        for (uint8_t row = 0; row < 8; row++) { // 7 hàng + 1 hàng tr?ng
            if (line & 0x01) { // Bit 1 => pixel sáng
                ST7735_DrawPixel(x + col, y + row, color);
            } else { // Bit 0 => pixel t?t
                ST7735_DrawPixel(x + col, y + row, bgcolor);
            }
            line >>= 1;
        }
    }
}
void ST7735_DrawString(uint8_t x, uint8_t y, const char *str, uint16_t color, uint16_t bgcolor) {
    while (*str) {
        ST7735_DrawChar(x, y, *str, color, bgcolor);
        x += 6; // M?i ký t? chi?m 6 pixel (5 pixel + 1 pixel kho?ng cách)
        str++;
    }
}