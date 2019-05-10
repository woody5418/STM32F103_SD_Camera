#ifndef __23LCXX_H
#define __23LCXX_H

#include "stm32f10x.h"

//23LC1024 Pin and peripheral definitions
#define S_NS		GPIO_Pin_2
#define S_CLK		GPIO_Pin_5
#define S_MISO		GPIO_Pin_6
#define S_MOSI		GPIO_Pin_7
#define S_GPIO         GPIOA


#define	CS_23lc1024   PAout(2) //SD¿¨Æ¬Ñ¡Òý½Å		

//23LC1024 SPI Commands
#define S_READ		0x03
#define S_WRITE		0x02
#define S_EDIO		0x3B
#define S_EQIO		0x38
#define S_RSTIO		0xFF
#define S_RDMR		0x05
#define S_WRMR		0x01

//SPI RAM Size
#define S_SIZE		0x1FFFF

//A simple typedef enumeration of the access modes
typedef enum{
	Single,
	Dual,
	Quad
} S23_Access;

//SPI RAM Initialization
void S23LC_Init(void);

//Data writing functions
void S23_WByte(uint32_t, uint8_t);
void S23_WWord(uint32_t, uint16_t);
void S23_WDWord(uint32_t, uint32_t);
void S23_WFloat(uint32_t, float);

//Speed testing function (using data writes)
void S23_SpeedTest(void);

//Data reading functions
uint8_t S23_RByte(uint32_t);
uint16_t S23_RWord(uint32_t);
uint32_t S23_RDWord(uint32_t);
float S23_RFloat(uint32_t);

//SPI RAM Setup functions
void S23_IOAccess(S23_Access);
void S23_WriteModeReg(uint8_t);
uint8_t S23_ReadModeReg(void);

#endif
