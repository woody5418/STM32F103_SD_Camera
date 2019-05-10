#ifndef __SCCB_H
#define __SCCB_H
#include "sys.h"


#define SCCB_SDA_IN()  {GPIOA->CRH&=0XFFF0FFFF;GPIOA->CRH|=0X00080000;}
#define SCCB_SDA_OUT() {GPIOA->CRH&=0XFFF0FFFF;GPIOA->CRH|=0X00030000;}
//PG13 替换为PA12  PD3替换为PA11
//IO操作函数
#define SCCB_SCL    		PAout(11)	 	//SCL
#define SCCB_SDA    		PAout(12) 		//SDA	 

#define SCCB_READ_SDA    	PAin(12)  		//输入SDA    
#define SCCB_ID   			0X60  			//OV2640的ID

///////////////////////////////////////////
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
u8 SCCB_WR_Byte(u8 dat);
u8 SCCB_RD_Byte(void);
u8 SCCB_WR_Reg(u8 reg,u8 data);
u8 SCCB_RD_Reg(u8 reg);
#endif













