#include "stm32f10x.h"
#include "I2C.h"          // ���������� ��� ������ �� I2C

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ������� ��� ���������� ���������� 

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// �������� � ���
void I2C_delay(uint32_t tick)
{ 
	uint32_t wait = 0;
	
	while(tick) 
 {
  __ASM("NOP"); 
	wait +=1; 
  tick--;
 } 
}
//

// ���������� ������������� ������� SDA � SCL ��� I2C (�������� Errata ���.26)
void I2C_GPIO_conf_with_Errata(void)
{
	// PB6 - I2C SCL
	// PB7 - I2C SDA
	GPIOB->CRL &= ~( GPIO_CRL_MODE6 | GPIO_CRL_MODE7 );
	GPIOB->CRL &= ~( GPIO_CRL_CNF6 | GPIO_CRL_CNF7 );
	
	// General Output Open-Drain High Level
	// High Level
	GPIOB->CRL |= ( GPIO_CRL_MODE6 | GPIO_CRL_CNF6_0 | GPIO_CRL_MODE7 | GPIO_CRL_CNF7_0 );
	GPIOB->ODR |= ( GPIO_ODR_ODR6 | GPIO_ODR_ODR7 );
	
	// Control SDA SCL High Level
	while(!(GPIOB->IDR & (GPIO_IDR_IDR7 | GPIO_IDR_IDR6))) { GPIOC->BSRR = GPIO_BSRR_BS4; }
	GPIOC->BSRR = GPIO_BSRR_BR4;
	
	// SDA Low Level
	GPIOB->ODR &= ~( GPIO_ODR_ODR7 );
	
	// Control SDA Low Level
	while(GPIOB->IDR & GPIO_IDR_IDR7) { GPIOC->BSRR = GPIO_BSRR_BS4; }
	GPIOC->BSRR = GPIO_BSRR_BR4;	
	
	// SCL Low Level
	GPIOB->ODR &= ~( GPIO_ODR_ODR6 );
	
	// Control SCL Low Level
	while(GPIOB->IDR & GPIO_IDR_IDR6) { GPIOC->BSRR = GPIO_BSRR_BS4; }
	GPIOC->BSRR = GPIO_BSRR_BR4;
	
		// SCL High Level
	GPIOB->ODR |= GPIO_ODR_ODR6;
	
	// Control SCL Low Level
	while(!(GPIOB->IDR & GPIO_IDR_IDR6)) { GPIOC->BSRR = GPIO_BSRR_BS4; }
	GPIOC->BSRR = GPIO_BSRR_BR4;
	
	// SDA High Level
	GPIOB->ODR |= GPIO_ODR_ODR7;
	
	// Control SDA High Level
	while(!(GPIOB->IDR & GPIO_IDR_IDR7)) { GPIOC->BSRR = GPIO_BSRR_BS4; }
	GPIOC->BSRR = GPIO_BSRR_BR4;
	
	// PB6 - I2C SCL - �������������� �������, "open drain" , 50MHz
	// PB7 - I2C SDA - �������������� �������, "open drain" , 50MHz
	GPIOB->CRL &= ~( GPIO_CRL_MODE6 | GPIO_CRL_MODE7 );
	GPIOB->CRL &= ~( GPIO_CRL_CNF6 | GPIO_CRL_CNF7 );
	GPIOB->CRL |= ( GPIO_CRL_MODE6 | GPIO_CRL_CNF6 | GPIO_CRL_MODE7 | GPIO_CRL_CNF7 );	
}
//

// ������������� I2C (���������� I2C2)
void I2C_Init(void)
{
	
// �������� ������������ ��������� I2C
RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;	
	
// �������� I2C
I2C1->CR1 &= ~I2C_CR1_PE;
	
// ��� ���� ����������
while (I2C1->CR1 & I2C_CR1_PE) {};	

// ���������� ������������� ������� SDA � SCL ��� I2C (�������� Errata ���.26)	
I2C_GPIO_conf_with_Errata();

// ����� ������������ I2C
I2C1->CR1 |= I2C_CR1_SWRST;
	
// ������� ����� ������������ I2C
I2C1->CR1 &= ~I2C_CR1_SWRST;
	
// !!!!!!!!! �������� ��������� ������ I2C
	
// ������� ������������ ������ I2C = 8 ���, 1/Fc = 1/8000000 = 125ns
// Standart Mode, 100 kHz = 100 000, 1000ns
I2C1->CCR = 80; //(SystemCoreClock / 100000UL);	
I2C1->TRISE = 9; // ( 1000ns / 125ns ) + 1
	
// ������ ������� ������ ������ I2C => FREQ[5:0] = 8 (MHz)
// ��������� ������������ ���������� �� �������� �� ������ I2C
I2C1->CR2 = 8;
I2C1->CR2 |= I2C_CR2_ITBUFEN;	// I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | 
	
// �������� ������ ���������� I2C	
I2C1->CR1 |= I2C_CR1_PE;

// ��� ���� ���������
while (!(I2C1->CR1 & I2C_CR1_PE)) {};

// ��������� ��������� ���������� �� ������ I2C
// NVIC_EnableIRQ(I2C1_EV_IRQn);
// NVIC_EnableIRQ(I2C1_ER_IRQn);	

// NVIC_SetPriority(I2C1_EV_IRQn, 0);
// NVIC_SetPriority(I2C1_ER_IRQn, 1); // 7
}
//

// ������ ��������� ����� � �������� SR1
uint16_t I2C_read_SR1(void)
{
// 
return I2C1->SR1;
}
//

// ������ ��������� ����� � �������� SR1
uint16_t I2C_read_SR2(void)
{
// 
return I2C1->SR2;
}
//

// ��������� ������ ������ ����� (���� �������� ������, 16���, WORD) � EEPROM �� ���� I2C
char EEPROM_write_DWord(uint8_t device,  uint8_t cell, uint32_t raw)
{
	uint16_t MSB_h = 0, MSB_l = 0;
	uint16_t LSB_h = 0, LSB_l = 0;
	
  uint16_t MSB = ( (raw & 0xFFFF0000) >> 16);
  uint16_t LSB = (raw & 0x0000FFFF);
	
	MSB_h = ( (MSB & 0xFF00) >> 8 );
	MSB_l = ( MSB & 0x00FF );
	LSB_h = ( (LSB & 0xFF00) >> 8 );
	LSB_l = ( LSB & 0x00FF );

	// ����� ���� :)	
	I2C1->CR1 |= I2C_CR1_START;	

#define ERR_CNT_LIM 100000
	
	uint32_t err_cnt;

  // ������� EV5
  // ���� ����������� ����� START BYTE
	err_cnt = ERR_CNT_LIM;
	while(!( I2C1->SR1 & I2C_SR1_SB ) && --err_cnt) {};
	if(!err_cnt) return 1;
	
	// ������ ��������������� ������� ��������� SR1
	(void) I2C1->SR1;
	
	// ������ �� ���� ����� ���������� � 0 � ����� (����� ������)
	I2C1->DR = I2C_Set_Address(device, I2C_MODE_WRITE); 

	// ������� ����������� ���� ADDR (����� ���������� ������ � �����)
	err_cnt = ERR_CNT_LIM;
	while(!( I2C1->SR1 & I2C_SR1_ADDR ) && --err_cnt) {};
	if(!err_cnt) return 2;
		
	// ������ ��������������� �������� ��������� SR1 � SR2
	(void) I2C1->SR1;
	(void) I2C1->SR2;
		
	// ������� EV8
	// ������� ����������� ���� ��������� ��������
	err_cnt = ERR_CNT_LIM;
	while(!(I2C1->SR1 & I2C_SR1_TXE) && --err_cnt){};
	if(!err_cnt) return 3;
		
	// �������� ����� ��������� ������	
	I2C1->DR = cell; 
	
	// ������� EV8
	// ������� ����������� ���� ��������� ��������
	err_cnt = ERR_CNT_LIM;
	while(!(I2C1->SR1 & I2C_SR1_TXE) && --err_cnt){};
	if(!err_cnt) return 4;
		
	// ���������� ������� ����� ����� MSB
	I2C1->DR = MSB_h;	

	// ������� EV8
	// ������� ����������� ���� ��������� ��������
	err_cnt = ERR_CNT_LIM;
	while(!(I2C1->SR1 & I2C_SR1_TXE) && --err_cnt){};
	if(!err_cnt) return 5;
		
	// ���������� ������� ����� ����� MSB
	I2C1->DR = MSB_l;	
		
	// ������� EV8
	// ������� ����������� ���� ��������� ��������
	err_cnt = ERR_CNT_LIM;
	while(!(I2C1->SR1 & I2C_SR1_TXE) && --err_cnt){};
	if(!err_cnt) return 6;
		
	// ���������� ������� ����� ����� LSB
	I2C1->DR = LSB_h;	

	// ������� EV8
	// ������� ����������� ���� ��������� ��������
	err_cnt = ERR_CNT_LIM;
	while(!(I2C1->SR1 & I2C_SR1_TXE) && --err_cnt){};
	if(!err_cnt) return 7;
		
	// ���������� ������� ����� ����� LSB
	I2C1->DR = LSB_l;			

	// ������� ����������� ���� ��������� ��������
	err_cnt = ERR_CNT_LIM;
	while(!(I2C1->SR1 & I2C_SR1_TXE) && --err_cnt){};
	if(!err_cnt) return 8;
		
	// ��������� ��������	
	I2C1->CR1 |= I2C_CR1_STOP;	
		
	// ����� ����� ������	
	I2C_delay(25000);	

	return 0;
}
//

// ��������� ������ ������ ����� (�������s ������, 16���, WORD) �� EEPROM �� ���� I2C
uint16_t EEPROM_read_word(uint8_t device,  uint8_t cell)
{
	uint16_t data_raw = 0;
	uint8_t data_raw_low = 0;
	uint8_t data_raw_high = 0;
	
	// ����� ���� :)	
	I2C1->CR1 |= I2C_CR1_START;	
	
  // ������� EV5
  // ���� ����������� ����� START BYTE
	while(!( I2C1->SR1 & I2C_SR1_SB )) {}; 
	
	// ������ ��������������� ������� ��������� SR1
	(void) I2C1->SR1;
	
	// ������ �� ���� ����� ���������� � 0 � ����� (����� ������)
	I2C1->DR = I2C_Set_Address(device, I2C_MODE_WRITE);
		
	// ������� ����������� ���� ADDR (����� ���������� ������ � �����)
	while(!( I2C1->SR1 & I2C_SR1_ADDR )) {}; 
		
	// ������ ��������������� �������� ��������� SR1 � SR2
	(void) I2C1->SR1;
	(void) I2C1->SR2;	
		
	// ������� EV8
	// ������� ����������� ���� ��������� ��������
  while(!(I2C1->SR1 & I2C_SR1_TXE)){};	
	I2C1->DR = cell;

	// ������� EV8
	// ������� ����������� ���� ��������� ��������
  while(!(I2C1->SR1 & I2C_SR1_TXE)){};

	// ��������� ����� ���� :)	
	I2C1->CR1 |= I2C_CR1_START;	
	
  // ������� EV5
  // ���� ����������� ����� START BYTE
	while(!( I2C1->SR1 & I2C_SR1_SB )) {}; 
	
	// ������ ��������������� ������� ��������� SR1
	(void) I2C1->SR1;
	
	// ������ �� ���� ����� ���������� � 0 � ����� (����� ������)
	I2C1->DR = I2C_Set_Address(device, I2C_MODE_READ);
		
	// 	
  I2C1->CR1 |= I2C_CR1_ACK | I2C_CR1_POS;		// 1
		
	// ������� ����������� ���� ADDR (����� ���������� ������ � �����)
	while(!( I2C1->SR1 & I2C_SR1_ADDR )) {}; 
		
	// ������ ��������������� �������� ��������� SR1 � SR2
	(void) I2C1->SR1;
	(void) I2C1->SR2;		
		
	I2C1->CR1 &= ~I2C_CR1_ACK;	 // 2
		
	while(!(I2C1->SR1 & I2C_SR1_BTF)){};

  I2C1->CR1 |= I2C_CR1_STOP;

	data_raw_high = I2C1->DR;
	data_raw_low = I2C1->DR;	

	data_raw = ( (data_raw_high <<8 ) | data_raw_low );	
		
	I2C1->CR1 &= ~( I2C_CR1_ACK | I2C_CR1_POS );	

	return data_raw;
}
//

// ������ 32������� (Double Word) ����� �� EEPROM �� ���� I2C
uint32_t EEPROM_read_DWord(uint8_t device, uint8_t cell)
{
uint16_t LW = 0, HW = 0;
uint32_t raw_out = 0;
	
HW = EEPROM_read_word(device, cell);
//I2C_delay(3000);
	
LW = EEPROM_read_word(device, cell + 2);
//I2C_delay(7500);
	
raw_out = (uint32_t)( (HW << 16) | LW );	
	
return raw_out;
}
//

/*

//
// ���������� �� �������� I2C
void I2C1_EV_IRQHandler(void)
{ 

	// ������� EV5
	// ( 0 - ��������� ������ � ����������, 1 - ������ ������ �� ���������� )
	if( I2C1->SR1 & I2C_SR1_SB ) 
	{ 
		(void) I2C1->SR1;
		I2C1->DR = I2C_Set_Address(EEPROM_I2C_24Cxx, I2C_action); 
		if(I2C_action) { I2C_read_step = 0; } else { I2C_write_step = 0; }
	}
	
	// ������� EV6
	if( I2C1->SR1 & I2C_SR1_ADDR ) 
	{ 
		(void) I2C1->SR1;
		(void) I2C1->SR2;
	}
	
	// ������� EV8
  if( I2C1->SR1 & I2C_SR1_TXE) 
	{
   if( I2C_write_step < 2 )
		{
			I2C1->DR = I2C_write_packet[I2C_write_step++]; 
		}
		else 
		{ 
			I2C1->CR1 |= I2C_CR1_STOP;
		}
	}
	
	// ������� EV8 - ���������� ���� ������ ��� ������ => 
	if( I2C1->SR1 & I2C_SR1_RXNE ) 
	{
		if( I2C_read_step == 2 ) 
		{
			//I2C1->CR1 &= ~I2C_CR1_ACK;
		}
		else if(I2C_read_step == 1) 
		{
			I2C1->CR1 &= ~I2C_CR1_ACK;
			I2C1->CR1 |= I2C_CR1_STOP;
		}
		
		if( I2C_read_step < 2 ) 
		{
			I2C_read_packet[I2C_read_step++] = I2C1->DR;
    }
	}	

}
//

// ���������� �� ������� I2C
void I2C1_ER_IRQHandler(void)
{
  // ������� STOP ���� - ���������� ��� ���������� ������ ����
	if (I2C1->SR1 & I2C_SR1_STOPF) 
	{ 
		I2C1->CR1 &= ~I2C_CR1_STOP; 
	}	
	
	// ������� NACK ���� - ���������� ��� ���������� ������ ����
	if (I2C1->SR1 & I2C_SR1_AF) 
	{ 
		I2C1->SR1 &= ~I2C_SR1_AF;
	}

	// ���� ���� ������ �� ���� - ������� ����� ������
	if (I2C1->SR1 & (I2C_SR1_ARLO | I2C_SR1_BERR))
	{
		I2C1->SR1 &= ~(I2C_SR1_ARLO | I2C_SR1_BERR);
	}
}
//

*/

////////


