#include "stm32f10x.h"
#include "PWM.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ������� ��� ���������� ���������� 

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ������������� ���
void PWM_Init(void)
{
	// ���������� ������������ ������� GPIO � ���� ������������
	// ���������� ������� �������������� ������� ��� GPIO � ���� ������������
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;
	
	// ���������� ������-3 � ���� ������������
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;	

	// ������ 3 ��������� ���-������� �� ����� ������� Ch1, Ch2, Ch3, Ch4
	TIM3->CR1 = 0;
	TIM3->CR2 = 0;
	TIM3->SR = 0;
	TIM3->CCER = 0;
	TIM3->BDTR = 0;
	TIM3->EGR = 0;
	TIM3->CCMR1 = 0;
	TIM3->CCMR2 = 0;
	TIM3->DIER = 0;
	
	TIM3->PSC = (SystemCoreClock/1000000L) - 1;
	// ������ 3 ���������� ��� ����� ������� ����� ��� 1000 ��������, �� ���������� ������� ���� ���������� =10�
	TIM3->ARR = 1181;
	
	// CH1 - PWM Mode 1 
	// CH2 - PWM Mode 1 
	// CH3 - PWM Mode 1 
	// CH4 - PWM Mode 1 
	TIM3->CCMR1 |= ( TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 );
	TIM3->CCMR2 |= ( TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 );

//���������� ��� �� ������ ��� ������� ����������	(����� ������ �������������� ���� ��� ������������)
//	TIM3->CCER |= TIM_CCER_CC1P;
	
	TIM3->CCER |= ( TIM_CCER_CC4E | TIM_CCER_CC3E | TIM_CCER_CC2E | TIM_CCER_CC1E );
	TIM3->BDTR |= TIM_BDTR_MOE;
	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->EGR |= TIM_EGR_UG;
	
	TIM3->CCR1 = 0;
	TIM3->CCR2 = 0;
	TIM3->CCR3 = 0;
	TIM3->CCR4 = 0;
	
	// ��������� ������������� ���������� �� �������-3
	NVIC_EnableIRQ(TIM3_IRQn);	
}
//

// ��������� PWM
void PWM_Enable(void)
{
  TIM3->SR &= ~TIM_SR_UIF;
	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->CR1 |= TIM_CR1_CEN;
}
//

// ���������� PWM
void PWM_Disable(void)
{
  TIM3->SR &= ~TIM_SR_UIF;
	TIM3->DIER &= ~TIM_DIER_UIE;
	TIM3->CCR1 = 0;
	TIM3->CCR2 = 0;
	TIM3->CCR3 = 0;
	TIM3->CCR4 = 0;
	TIM3->CR1 &= ~TIM_CR1_CEN;
}
//

// ���������� ���-�� PWM
void PWM_Update(uint8_t Ch, uint16_t CCR)
{
	switch(Ch)
  {
	 case 1: { TIM3->CCR1 = CCR; break; }
	 case 2: { TIM3->CCR2 = CCR; break; }
	 case 3: { TIM3->CCR3 = CCR; break; }
	 case 4: { TIM3->CCR4 = CCR; break; }
	 default: break;
  }
	TIM3->SR &= ~TIM_SR_UIF;
}
//
