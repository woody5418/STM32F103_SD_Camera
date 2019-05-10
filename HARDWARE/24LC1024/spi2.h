#ifndef __SPI2_H
#define __SPI2_H

#include "stm32f10x.h"


#define SPI_CS_1   GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define SPI_CS_0   GPIO_ResetBits(GPIOB, GPIO_Pin_12)


void SPI2_Init(void);

#endif





