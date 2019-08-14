//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//  ������������ ������������ ���� I2C ���������������� STM32F10xxxx
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#ifndef I2C_H	// �������� ��������� ��������� ����� ������
#define I2C_H

#define  EEPROM_I2C_24Cxx 0x50

#define I2C_MODE_READ	  1
#define I2C_MODE_WRITE	0
#define I2C_Set_Address(addr, mode)	( (addr << 1 ) | mode )

// ==================================================

// ���������� ������������� ������� SDA � SCL ��� I2C (�������� Errata ���.26)
void I2C_GPIO_conf_with_Errata(void);

// ������������� I2C (���������� I2C2)
void I2C_Init(void);

// ������ ��������� ����� � �������� SR1
uint16_t I2C_read_SR1(void);
	
// ������ ��������� ����� � �������� SR1
uint16_t I2C_read_SR2(void);

// ������ ������ ������� ����� (32���, DWORD) � EEPROM �� ���� I2C
char EEPROM_write_DWord(uint8_t device,  uint8_t cell, uint32_t raw);

// ��������� ������ ����� (�������s ������, 16���, WORD) �� EEPROM �� ���� I2C
uint16_t EEPROM_read_word(uint8_t device,  uint8_t cell);

// ������ ������ ������� ����� (32���, DWORD) �� EEPROM �� ���� I2C
uint32_t EEPROM_read_DWord(uint8_t device, uint8_t cell);


#endif
//
//
