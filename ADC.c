#include "stm32f10x.h"
#include <stdio.h>

void TIM1_Config(void) {
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;  // B?t clock cho TIM1

    TIM1->PSC = 7200 - 1;                // Prescaler (72MHz / 7200 = 10kHz)
    TIM1->ARR = 10 - 1;                  // Auto-reload (10kHz / 10 = 1kHz = 1ms)
    TIM1->CR2 |= TIM_CR2_MMS_1;          // Update event là ngu?n kích ADC
    TIM1->CR1 |= TIM_CR1_CEN;            // B?t Timer 1
}
void ADC_Config(void) {
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;   // B?t clock cho ADC1
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;   // B?t clock cho GPIOA

    GPIOA->CRL &= ~GPIO_CRL_CNF0;         // PA0 ? ch? d? Analog
    GPIOA->CRL &= ~GPIO_CRL_MODE0;        // Input mode

    ADC1->SQR1 &= ~ADC_SQR1_L;            // Ch? m?t channel trong sequence
    ADC1->SQR3 |= 0;                      // Channel 0 (PA0)

    ADC1->CR2 |= ADC_CR2_EXTSEL;          // Trigger b?i TIM1 update event
    ADC1->CR2 |= ADC_CR2_EXTTRIG;         // Kích ho?t External Trigger
    ADC1->CR2 |= ADC_CR2_ADON;            // B?t ADC1

																				// Hi?u ch?nh ADC
    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL);      // Ch? hi?u ch?nh xong
}
uint16_t Read_ADC(void) {
    ADC1->CR2 |= ADC_CR2_SWSTART;        // B?t d?u chuy?n d?i
    while (!(ADC1->SR & ADC_SR_EOC));    // Ch? chuy?n d?i hoàn t?t
    return ADC1->DR;                     // Tr? v? giá tr? ADC
}