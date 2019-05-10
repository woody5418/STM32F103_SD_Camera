#ifndef __USART3_H
#define __USART3_H

#include "stm32f10x.h"
#include <stdio.h>

#define  USART3_RX_LEN 2048  //支持最大2K的串口写入

extern uint16_t USART3_RX_STA;  //数据接收标志位
extern uint8_t USART3_RX_BUF[USART3_RX_LEN];  //串口数据缓存数组

void uart3_init(u32 baud);
uint16_t crc16(char* puchMsg,uint8_t usDataLen);

#endif


