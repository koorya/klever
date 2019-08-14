#include "stm32f10x.h"
#include "GPIO.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ������� ��� ���������� ���������� 

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// �������� "���-�������"
void GPIO_delay(uint32_t tick)
{ 
//	uint32_t wait = 0;
// 	tick = tick * 8000;
//	
//	while(tick) 
// {
//  __ASM("NOP"); 
//	wait +=1; 
//  tick--;
// } 
__ASM("NOP"); __ASM("NOP"); __ASM("NOP"); __ASM("NOP"); __ASM("NOP"); 	
}
//

// ��������� ������ �����/������
void GPIO_Init(void)
{
	// ���������� ������������ ������� GPIO � ���� ������������
	// ���������� ������� �������������� ������� ��� GPIO � ���� ������������
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;
	
	// ��������� �������� ������� JTAG �� PB3 � PB4 (������ ����� ��� ������� ������ PB3 � PB4)
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
	
	// �������� �������������� ������� �������-3 ��:
	// PC6 - ������� 3 ����� 1 
	// PC7 - ������� 3 ����� 2 
	// PC8 - ������� 3 ����� 3 
	// PC9 - ������� 3 ����� 4 
	AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP;
	
	// LED ��������� GLCD - ����� PA10 - ����� �������� ��� �����
	GPIOA->CRH &= ~( GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
	GPIOA->CRH |= ( GPIO_CRH_MODE10_0 );
	
	// GLCD ��������� � ���������� SPI1
	// MOSI (RW) - ����� PA7 - �������������� �������
	// Reset     - ����� PA0 - ����� �������� ��� �����
	// CLK (E)   - ����� PA5 - �������������� �������
	// CS (RS)   - ����� PA4 - ����� �������� ��� �����
	GPIOA->CRL &= ~( GPIO_CRL_MODE0 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5 | GPIO_CRL_MODE7 | GPIO_CRL_CNF0 | GPIO_CRL_CNF4 | GPIO_CRL_CNF5 | GPIO_CRL_CNF7 );
	GPIOA->CRL |= ( GPIO_CRL_MODE0_0 | GPIO_CRL_MODE4_0 | GPIO_CRL_MODE5_1 | GPIO_CRL_CNF5_1 | GPIO_CRL_MODE7_1 | GPIO_CRL_CNF7_1 );
	
	// �������� ����� 0..5 (PB0 - PB5) � ��������� � Vcc = 6 ���� 
	// PB6 - I2C SCL - �������������� �������, "open drain" , 50MHz
	// PB7 - I2C SDA - �������������� �������, "open drain" , 50MHz
	GPIOB->CRL &= ~( GPIO_CRL_MODE0 | GPIO_CRL_MODE1 | GPIO_CRL_MODE2 | GPIO_CRL_MODE3 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5 | GPIO_CRL_MODE6 | GPIO_CRL_MODE7 );
	GPIOB->CRL &= ~( GPIO_CRL_CNF0 | GPIO_CRL_CNF1 | GPIO_CRL_CNF2 | GPIO_CRL_CNF3 | GPIO_CRL_CNF4 | GPIO_CRL_CNF5 | GPIO_CRL_CNF6 | GPIO_CRL_CNF7 );
	GPIOB->CRL |= ( GPIO_CRL_CNF0_1 | GPIO_CRL_CNF1_1 | GPIO_CRL_CNF2_1 | GPIO_CRL_CNF3_1 | GPIO_CRL_CNF4_1 | GPIO_CRL_CNF5_1 | GPIO_CRL_MODE6 | GPIO_CRL_CNF6 | GPIO_CRL_MODE7 | GPIO_CRL_CNF7 );
	GPIOB->ODR |= ( GPIO_ODR_ODR0 | GPIO_ODR_ODR1 | GPIO_ODR_ODR2 | GPIO_ODR_ODR3 | GPIO_ODR_ODR4 | GPIO_ODR_ODR5 );
	
	// �������� ������ (PB8, PB9, PB12, PB13) = 4 �����
	GPIOB->CRH &= ~( GPIO_CRH_MODE8 | GPIO_CRH_MODE9 | GPIO_CRH_MODE12 | GPIO_CRH_MODE13 );
	GPIOB->CRH &= ~( GPIO_CRH_CNF8 | GPIO_CRH_CNF9 | GPIO_CRH_CNF12 | GPIO_CRH_CNF13 );
	GPIOB->CRH |= ( GPIO_CRH_MODE8_0 | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE12_0 | GPIO_CRH_MODE13_0 );
	
	// ������� ���������� ������ �� �����
	// KI0 = PB14 - ����� �������� ��� ���� c ��������� � Gnd
	// KI1 = PB15 - ����� �������� ��� ���� c ��������� � Gnd
	// KB0 = PC11 - ����� �������� ��� �����
	// KB1 = PC12 - ����� �������� ��� �����
	GPIOB->CRH &= ~( GPIO_CRH_MODE14 | GPIO_CRH_MODE15 | GPIO_CRH_CNF14 | GPIO_CRH_CNF15 );
	GPIOB->CRH |= ( GPIO_CRH_CNF14_1 | GPIO_CRH_CNF15_1 );
	GPIOB->ODR &= ~( GPIO_ODR_ODR14 | GPIO_ODR_ODR15 );
	GPIOC->CRH &= ~( GPIO_CRH_MODE11 | GPIO_CRH_MODE12 | GPIO_CRH_CNF11 | GPIO_CRH_CNF12 );
	GPIOC->CRH |= ( GPIO_CRH_MODE11_0 | GPIO_CRH_MODE12_0 );
	
	// ������������ ������� ��� ���������� ������� ������:
	// PC0 - 1-� ���������� ����, ��� ��� ��� AIN_10
	// PC1 - 2-� ���������� ����, ��� ��� ��� AIN_11
	// PC2 - 3-� ���������� ����, ��� ��� ��� AIN_12
	// PC3 - 4-� ���������� ����, ��� ��� ��� AIN_13
	GPIOC->CRL &= ~( GPIO_CRL_MODE0 | GPIO_CRL_MODE1 | GPIO_CRL_MODE2 | GPIO_CRL_MODE3 | GPIO_CRL_CNF0 | GPIO_CRL_CNF1 | GPIO_CRL_CNF2 | GPIO_CRL_CNF3 );
		
	// PC4 - ������� ��������� �� �����
	// PC5 - ������� ��������� �� �����
	GPIOC->CRL &= ~( GPIO_CRL_MODE4 | GPIO_CRL_MODE5 | GPIO_CRL_CNF4 | GPIO_CRL_CNF5 );
	GPIOC->CRL |= ( GPIO_CRL_MODE4_0 | GPIO_CRL_MODE5_0 );
	
	// ������������ ������� ��� ��������� ��� �������� (���������� �������� �������):
	// PC6 - ��� PWM ����� (��������� ���������), �������������� �������
	// PC7 - ��� PWM ����� (��������� ���������), �������������� �������
	// PC8 - ��� PWM ����� (��������� ���������), �������������� �������
	// PC9 - ��� PWM ����� (��������� ���������), �������������� �������
	GPIOC->CRL &= ~( GPIO_CRL_CNF6 | GPIO_CRL_MODE6 | GPIO_CRL_CNF7 | GPIO_CRL_MODE7 );
	GPIOC->CRL |= ( GPIO_CRL_CNF6_1 | GPIO_CRL_MODE6 | GPIO_CRL_CNF7_1  | GPIO_CRL_MODE7 );
	GPIOC->CRH &= ~( GPIO_CRH_CNF8 | GPIO_CRH_MODE8 | GPIO_CRH_CNF9 | GPIO_CRH_MODE9 );
	GPIOC->CRH |= ( GPIO_CRH_CNF8_1 | GPIO_CRH_MODE8 | GPIO_CRH_CNF9_1  | GPIO_CRH_MODE9 );
		
}
//

// �������� ������
void FC_rotate_Fwd(void)
{
D0_out_OFF;
GPIO_delay(5);	
D1_out_ON;
}

// �������� �����
void FC_rotate_Rev(void)
{
D1_out_OFF;
GPIO_delay(5);	
D0_out_ON;	
}

// ���������� ��������
void FC_rotate_Stop(void)
{
D0_out_OFF;
D1_out_OFF;
GPIO_delay(5);
}	

// ���������� ���������
void FC_tension_Up(void)
{
D2_out_OFF;
GPIO_delay(5);	
D3_out_ON;
}

// ���������� ���������
void FC_tension_Down(void)
{
D3_out_OFF;
GPIO_delay(5);	
D2_out_ON;
}

// ���������� ������
void FC_tension_Hold(void)
{
D2_out_OFF;
D3_out_OFF;
GPIO_delay(5);
}


//

