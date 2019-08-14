#include "stm32f10x.h"
#include "usart.h"

volatile uint16_t usart_buff_index; 
volatile const char * usart_buff;

void keyscan_work(uint8_t code); //Keyboadr.c file contains definition of this function



void sendNextByte(void){ //��� �������
	
	if (!usart_buff)
			return;
		if(usart_buff_index<USART_BUFF_SIZE) //���� �� ��������� �������� ������������ ������ �������
			if(usart_buff[usart_buff_index]){  //����� ������������ ������ ��������� 0
					USART3->DR = usart_buff[usart_buff_index++]; //������ ���� � �������� � ��������� �� ���������
					return; //���
			}
		usart_buff_index = 0; //�������� ��������� �� ����� � ������ ������, �.�. �� ���� ���������
		usart_buff = 0;
}
	
void USART3_IRQHandler(void) //����������� �� �������� � �� ������ �����
{

	if (USART3->SR & USART_SR_RXNE ){ //���� ������� ������ �� ������
		keyscan_work(USART3->DR);
	}
	if (USART3->SR & USART_SR_TC){ //���� ������� �������� ������ (transmit complite)
		sendNextByte();
		USART3->SR &= ~USART_SR_TC;
	}
}

/*
void USART2_IRQHandler(void) //����������� �� �������� �����
{
	if (USART2->SR & USART_SR_TC){ //���� ������� �������� ������ (transmit complite)
		USART2->SR &= ~USART_SR_TC;
	}
}
*/

void DMA1_Channel7_IRQHandler(void){
	if(DMA1->ISR & DMA_ISR_TCIF7){ // A transfer complete (TC) event occurred on channel 7
		DMA1->IFCR |= DMA_IFCR_CTCIF7; //Clears the corresponding TCIF flag in the DMA_ISR register
//		GPIOA->BSRR = GPIO_BSRR_BS1; //�������� �� ����� ���������� max485
	//GPIOA->BSRR = GPIO_BSRR_BR1; //�������� �� �������� ���������� max485
	}
	DMA1->IFCR |= 0x0fffffff; //���������� ����� ���� ������� (�� ������ ������, ��)
	
}
volatile char i = 0;
void USART2_IRQHandler(void) //����������� �� �������� � �� ������ �����
{

	if (USART2->SR & USART_SR_TC){ //���� ������� �������� ������ (transmit complite)
		i++;
		USART2->SR &= ~USART_SR_TC;
		if(DMA1->ISR & DMA_ISR_TCIF7){
			DMA1->IFCR |= DMA_IFCR_CTCIF7;
			GPIOA->BSRR = GPIO_BSRR_BR1; //�������� �� ����� ���������� max485
		}
	}
}

void usart2_init(void) /*RS485*/
{


		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN; // PA2-Tx PA3-Rx (gpioa � ���� ������������, �������������� ������� � ���� ������������)
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN; //USART2 - PA2-Tx PA3-Rx , ������������ �� APB1 (36MHz ��� ����� �������)

	
		GPIOA->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1);
		GPIOA->CRL |= GPIO_CRL_MODE1; //output 50MHz, General purpose output push-pull
		GPIOA->BSRR = GPIO_BSRR_BR1; //�������� �� ����� ���������� max485
//		GPIOA->BSRR = GPIO_BSRR_BS1; //�������� �� �������� ���������� max485


	
//Tx PA2 (no remap, usart2)
		GPIOA->CRL	&= ~GPIO_CRL_CNF2 & ~GPIO_CRL_MODE2;
		GPIOA->CRL |= GPIO_CRL_CNF2_1 | GPIO_CRL_MODE2; //CNF2 0b10 PP alt, MODE2 0b11 Output mode max speed 50 MHz
/**
//Rx PA3
		GPIOA->CRL &= ~GPIO_CRL_CNF3 & ~GPIO_CRL_MODE3;
		GPIOA->CRL |= GPIO_CRL_CNF3_0 ; //input
**/
		USART2->BRR =  0x753; //(36 000 000/19200)/16 = 117.18 ; //��� ���� ������������ �� �������, ����� � ������� ����� (�� ����� ����, �������� � �����)
	
		USART2->CR1 = 0;
		USART2->CR1 |= 	USART_CR1_TCIE	| //tr_compl interr
								USART_CR1_M 			| // 9 ��� 
								USART_CR1_PCE 		| // parity  
								//USART_CR1_PS			| // odd parity ��������
								USART_CR1_TE 			|	//Transmitter enable 
								//USART_CR1_RE 			; //Receiver enable
									0;
		
		USART2->CR2 = 0;

		USART2->CR3 |= USART_CR3_DMAT;

		USART2->CR1 |= USART_CR1_UE; //�������� UART2
		USART2->SR &= ~USART_SR_TC;
		
		RCC->AHBENR |= RCC_AHBENR_DMA1EN;
		DMA1_Channel7->CCR |= DMA_CCR1_MINC | DMA_CCR1_DIR ;//| DMA_CCR1_TCIE;//| DMA_CCR1_PL_1;// | DMA_CCR1_TCIE ;
		DMA1_Channel7->CPAR = (uint32_t)(&(USART2->DR));
//		DMA1_Channel7->CMAR = (uint32_t)(arr_to_transfer);
//		DMA1_Channel7->CNDTR = 4*4 ; // 4 times by 4 byte
//		DMA1_Channel7->CCR |= DMA_CCR1_EN ;
//		NVIC_EnableIRQ (DMA1_Channel7_IRQn); 
		NVIC_EnableIRQ (USART2_IRQn);
}

void sendStrToUart2(char *str, char size){

	while(DMA1_Channel7->CNDTR);
	//		GPIOA->BSRR = GPIO_BSRR_BR1; //�������� �� ����� ���������� max485
	GPIOA->BSRR = GPIO_BSRR_BS1; //�������� �� �������� ���������� max485
	DMA1_Channel7->CCR &= ~DMA_CCR1_EN ;
	DMA1_Channel7->CMAR = (uint32_t)(str);
	DMA1_Channel7->CNDTR = size ; // 
	DMA1_Channel7->CCR |= DMA_CCR1_EN ;
}

void usart3_init_keyboard(void)
{
		usart_buff_index = 0;
		usart_buff = 0;
	
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN; // PB10-Tx PB11-Rx (gpioa � ���� ������������, �������������� ������� � ���� ������������)
		RCC->APB1ENR |= RCC_APB1ENR_USART3EN; //USART3 - PB10-Tx PB11-Rx , ������������ �� APB1 (36MHz ��� ����� �������)

//Tx PB10 (no remap, usart2)
		GPIOB->CRH	&= ~GPIO_CRH_CNF10 & ~GPIO_CRH_MODE10;
		GPIOB->CRH |= GPIO_CRH_CNF10_1 | GPIO_CRH_MODE10; //CNF2 0b10 PP alt, MODE2 0b11 Output mode max speed 50 MHz

//Rx PB11
		GPIOB->CRH &= ~GPIO_CRH_CNF11 & ~GPIO_CRH_MODE11;
		GPIOB->CRH |= GPIO_CRH_CNF11_0 ; //input
		USART3->BRR =  0xA93; //13.300Hz //��� ���� ������������ �� �������, ����� � ������� ����� 
		USART3->CR1 = 0;
		USART3->CR1 |= 	USART_CR1_RXNEIE 	| //�������� ���������� �� ������,
										USART_CR1_TCIE		| //�������� ���������� �� ��������,
										USART_CR1_M 			| // 9 ��� 
										USART_CR1_PCE 		| // parity  
										USART_CR1_PS			| // odd parity ��������
										USART_CR1_TE 			|	//Transmitter enable 
										USART_CR1_RE 			; //Receiver enable
											
		USART3->CR2 = 0;
		USART3->CR3 = 0;
		USART3->CR1 |= USART_CR1_UE; //�������� UART3
		NVIC_EnableIRQ (USART3_IRQn); //�������� ���������� 
}



