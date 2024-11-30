#ifndef ADC_H
#define ADC_H
#include <stdint.h>
void TIM1_Config(void);
void ADC_Config(void);
uint16_t Read_ADC(void);
#endif