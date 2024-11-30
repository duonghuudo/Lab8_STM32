#include "stm32f10x.h"
#include <stdio.h>
#include "ADC.h"
#include "LCD.h"
#include <string.h>
void SystemClock_Config(void) {
    // B?t HSE
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)); // Ch? HSE s?n sàng

    // C?u hình PLL: HSE làm ngu?n d?u vào, nhân 9 (16 MHz * 9 = 72 MHz)
    RCC->CFGR |= RCC_CFGR_PLLSRC;        // HSE làm ngu?n PLL
    RCC->CFGR |= RCC_CFGR_PLLMULL9;      // Nhân 9

    // B?t PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)); // Ch? PLL s?n sàng

    // Ch?n PLL làm SYSCLK
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // Ch? PLL du?c ch?n làm SYSCLK

    // C?u hình prescaler cho AHB, APB1, APB2
    RCC->CFGR &= ~RCC_CFGR_HPRE;         // AHB prescaler = 1 (SYSCLK = HCLK)
    RCC->CFGR &= ~RCC_CFGR_PPRE1;        // APB1 prescaler = 2 (HCLK / 2 = 36 MHz)
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    RCC->CFGR &= ~RCC_CFGR_PPRE2;        // APB2 prescaler = 1 (HCLK / 1 = 72 MHz)
}
void uintToStr(unsigned int num, char *str) {
    int i = 4;
		str[0] = 'K';
		str[1] = '/';
		str[2] = 'C';
		str[3] = ':';
    // Chuy?n t?ng ch? s? thành ký t? (ngu?c)
    do {
        str[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0);

    // Thêm ký t? k?t thúc chu?i
    str[i] = '\0';

    // Ð?o chu?i (dang ngu?c)
    for (int j = 4, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}
void FixStr(char *dis, char *str) {
	int length = strlen(str);
	switch (length) {
		case 8:
				for(int i=0;i<5; i++) {
					dis[i] = str[i];
				}
				dis[5] = '.';
				for(int j=6;j<9;j++){
					dis[j] = str[j-1];
				}
				dis[9] = '\0';
				break;
		case 7:
			for(int i=0;i<4; i++) {
					dis[i] = str[i];
				}
				dis[4]='0';
				dis[5] = '.';
				for(int j=6;j<9;j++){
					dis[j] = str[j-1];
				}
				dis[9] = '\0';
				break;
		case 6:
			for(int i=0;i<4; i++) {
					dis[i] = str[i];
				}
			dis[4]= '0';
			dis[5] = '.';
			dis[6]= '0';
			dis[7]= str[length-2];
			dis[8]= str[length-1];
			dis[9] = '\0';
				break;
		case 5:
			for(int i=0;i<4; i++) {
					dis[i] = str[i];
				}
			dis[4]= '0';
			dis[5] = '.';
			dis[6]= '0';
			dis[7]= '0';
			dis[8]= str[length-1];
			dis[9] = '\0';
				break;
		default:
			dis[0] = 'E';
			dis[1] = 'R';
			dis[2] = 'R';
			dis[3] = '\0';
			break;
	}
	

}
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_WHITE   0xFFFF
#define COLOR_BLACK   0x0000
int main(void) {
		SystemInit();
		SystemClock_Config();
    GPIO_Config();
    SPI_Config();
		ADC_Config();
    ST7735_Init();
		uint16_t adc_value;
		unsigned int num = 12345;
    char str[20];
		char dis[20];

    //uintToStr(num, str);
    // Test màn hình
    ST7735_FillRect(0, 0, 128, 160, COLOR_BLACK); // Xóa màn hình (màu den)
    //ST7735_DrawPixel(64, 80, COLOR_RED);         // V? m?t pixel d? gi?a màn hình
    //ST7735_FillRect(10, 10, 50, 50, COLOR_GREEN); // V? kh?i màu xanh lá
    //ST7735_FillRect(70, 70, 30, 30, COLOR_BLUE);  // V? kh?i màu xanh duong
		//ST7735_DrawChar(10, 20, 'A', 0xF800, 0x0000); // Ch? d?, n?n den
		//ST7735_DrawString(10, 20, "HELLO THAO", 0x07E0, 0x0000); // Ch? xanh lá, n?n den

    while (1){
			adc_value = Read_ADC();
			num = adc_value/2;
			uintToStr(num, str);
			FixStr(dis, str);
			ST7735_DrawString(10, 20, dis, 0x001F, 0x0000);
			ST7735_DrawString(10, 120, "BaeSuzyLove", COLOR_WHITE, 0x0000);
			for (volatile int i = 0; i < 300000; i++);
		};
}