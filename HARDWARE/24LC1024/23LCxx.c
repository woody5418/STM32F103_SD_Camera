#include "stm32f10x.h"
#include "23LCxx.h"
#include "spi.h"

GPIO_InitTypeDef G;
SPI_InitTypeDef S;

uint8_t WriteMode = 0;

//Unions for saving floating points in "int" space
typedef union{
	uint32_t Int;
	float Flt;
} Type;

typedef union{
	uint32_t *IntP;
	float *FltP;
} TypeP;

//Static function to wait for SPI transfer to be complete. The value sent
//to this function states whether the function should wait for data to be
//received before returning.
static void SPI_WaitFlags(uint8_t TxRx){
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	if(!TxRx){
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	}
	return;
}

//Static function to send one byte of SPI data
static void SPI_SB(uint8_t Dat){
	uint8_t DatB;
	SPI_I2S_SendData(SPI1, Dat);
	SPI_WaitFlags(1);
	DatB = SPI_I2S_ReceiveData(SPI1);
}

//Static function to receive one byte of SPI data
static  uint8_t SPI_RB(void){
	uint8_t Dat;
	SPI_I2S_SendData(SPI1, 0xFF);
	SPI_WaitFlags(0);
	Dat = SPI_I2S_ReceiveData(SPI1);
    return Dat;
}

//Initialize the SPI RAM!
void S23LC_Init(void){    

	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );	
 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
	GPIO_SetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
	S23_IOAccess(Single);
	S23_WriteModeReg(0x00);  
	/*
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE );
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,  ENABLE );
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	
  SPI_InitStructure.SPI_CRCPolynomial = 7;	
  SPI_Init(SPI2, &SPI_InitStructure);  
 
  SPI_Cmd(SPI2, ENABLE); 
        
  //Enable the SPI peripheral
  GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15); 
	//Set the IO access mode to single
	S23_IOAccess(Single);

	//Set the write mode to byte
	S23_WriteModeReg(0x00);   
*/	
}

//The simplest of functions, merely to change the IO access to the SPI RAM IC.
//This obviously will never really be called other than initially.
void S23_IOAccess(S23_Access S){
	GPIO_ResetBits(S_GPIO, S_NS);

	switch(S){
	case Single:
		SPI_SB(S_RSTIO);
		break;
	case Dual:
		SPI_SB(S_EDIO);
		break;
	case Quad:
		SPI_SB(S_EQIO);
		break;
	}

	GPIO_SetBits(S_GPIO, S_NS);
}

//Read a single byte from the address sent to the function
uint8_t S23_RByte(uint32_t Addr){
	uint8_t Dat;

	GPIO_ResetBits(S_GPIO, S_NS);
        
        /*
	//This is another annoying feature of the STM32F0. You need to make sure the
	//Rx FIFO is empty before you receive data otherwise you will mess up the
	//ordering at which data is received!
	while(SPI_GetReceptionFIFOStatus(SPI2) != SPI_ReceptionFIFOStatus_Empty){
		SPI_I2S_ReceiveData(SPI2);
	}
        */
        
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != RESET){
          SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_RXNE);
		SPI_I2S_ReceiveData(SPI1);
	}
	//Send the READ command to the SPI RAM IC
	SPI_SB(S_READ);

	//Send the address to be read
	SPI_SB(Addr>>16);
	SPI_SB((Addr>>8)&255);
	SPI_SB(Addr&255);

	//Receive a byte from the RAM IC
	Dat = SPI_RB();

	GPIO_SetBits(S_GPIO, S_NS);
	return Dat;
}

//Read a word from the IC
uint16_t S23_RWord(uint32_t Addr){
	uint16_t Dat;

	//In this case, the SPI RAM IC is changed from byte read to sequential read
	//allowing faster access for multiple bytes. If the system is already in
	//the sequential read mode, the command won't be set, saving time even
	//even more so.
	if(WriteMode != 0x40){
		S23_WriteModeReg(0x40);
	}

	GPIO_ResetBits(S_GPIO, S_NS);

        /*
	//Clear FIFO
	while(SPI_GetReceptionFIFOStatus(SPI2) != SPI_ReceptionFIFOStatus_Empty){
		SPI_I2S_ReceiveData(SPI2);
	}
        */
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != RESET){
          SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_RXNE);
		  SPI_I2S_ReceiveData(SPI1);
	}
	SPI_SB(S_READ);

	SPI_SB(Addr>>16);
	SPI_SB((Addr>>8)&255);
	SPI_SB(Addr&255);

	Dat = SPI_RB()<<8;
	Dat |= SPI_RB();

	GPIO_SetBits(S_GPIO, S_NS);
	return Dat;
}

//Read a double word or 32bit unsigned integer
uint32_t S23_RDWord(uint32_t Addr){
	uint32_t Dat;

	if(WriteMode != 0x40){
		S23_WriteModeReg(0x40);
	}

	GPIO_ResetBits(S_GPIO, S_NS);

        /*
	//Clear FIFO
	while(SPI_GetReceptionFIFOStatus(SPI2) != SPI_ReceptionFIFOStatus_Empty){
		SPI_I2S_ReceiveData(SPI2);
	}
        */
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != RESET){
          SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_RXNE);
		SPI_I2S_ReceiveData(SPI1);
	}
	SPI_SB(S_READ);

	SPI_SB(Addr>>16);
	SPI_SB((Addr>>8)&255);
	SPI_SB(Addr&255);

	Dat = SPI_RB()<<24;
	Dat |= (SPI_RB()<<16);
	Dat |= (SPI_RB()<<8);
	Dat |= SPI_RB();

	GPIO_SetBits(S_GPIO, S_NS);
	return Dat;
}

//Read a float at 'Addr' on the SPI RAM IC
float S23_RFloat(uint32_t Addr){
	Type N;

	//Here, a union is used to store one value and read the same
	//memory space as another value. In this case, a 32bit integer is
	//read from the IC using S23_RDWord, stored in the union then the
	//same raw bit data returned as a float format.
	N.Int = S23_RDWord(Addr);

	return N.Flt;
}
/**  写一个数据需要83.9us
4.80563940
4.80572330
4.80580710

0.0000839 S
0.0839ms
83.9us
83.8us
 */
//Write an unsigned single byte at 'Addr' of data 'Dat'
void S23_WByte(uint32_t Addr, uint8_t Dat){
	GPIO_ResetBits(S_GPIO, S_NS);

	SPI_SB(S_WRITE);

	SPI_SB(Addr>>16);
	SPI_SB((Addr>>8)&255);
	SPI_SB(Addr&255);

	SPI_SB(Dat);

	GPIO_SetBits(S_GPIO, S_NS);
}

//Write an unsigned word (16bit) to 'Addr' of data 'Dat'
void S23_WWord(uint32_t Addr, uint16_t Dat){

	//The same previously mentioned sequential write advantage is present
	//here, hence the check!
	if(WriteMode != 0x40){
		S23_WriteModeReg(0x40);
	}

	GPIO_ResetBits(S_GPIO, S_NS);

	SPI_SB(S_WRITE);

	SPI_SB(Addr>>16);
	SPI_SB((Addr>>8)&255);
	SPI_SB(Addr&255);

	SPI_SB(Dat>>8);
	SPI_SB(Dat);

	GPIO_SetBits(S_GPIO, S_NS);
}

//Write an unsigned double word (32bits) at 'Addr' of 'Dat'
void S23_WDWord(uint32_t Addr, uint32_t Dat){
	if(WriteMode != 0x40){
		S23_WriteModeReg(0x40);
	}

	GPIO_ResetBits(S_GPIO, S_NS);

	SPI_SB(S_WRITE);

	SPI_SB(Addr>>16);
	SPI_SB((Addr>>8)&255);
	SPI_SB(Addr&255);

	SPI_SB(Dat>>24);
	SPI_SB((Dat>>16)&255);
	SPI_SB((Dat>>8)&255);
	SPI_SB(Dat&255);

	GPIO_SetBits(S_GPIO, S_NS);
}

//Write a float at 'Addr' of 'Dat', once again, a
//union is used!
void S23_WFloat(uint32_t Addr, float Dat){
	Type N;

	N.Flt = Dat;

	S23_WDWord(Addr, N.Int);
}

//Write to mode register 'Dat' data.
void S23_WriteModeReg(uint8_t Dat){
	GPIO_ResetBits(S_GPIO, S_NS);

	WriteMode = Dat;

	SPI_SB(S_WRMR);
	SPI_SB(Dat);

	GPIO_SetBits(S_GPIO, S_NS);
}

//Read from mode register, returned as a byte
uint8_t S23_ReadModeReg(void){
	uint8_t Dat;

        /*
	//Clear FIFO
	while(SPI_GetReceptionFIFOStatus(SPI2) != SPI_ReceptionFIFOStatus_Empty){
		SPI_I2S_ReceiveData(SPI2);
	}
        */
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != RESET){
          SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_RXNE);
		SPI_I2S_ReceiveData(SPI1);
	}
	GPIO_ResetBits(S_GPIO, S_NS);

	SPI_SB(S_RDMR);
	Dat = SPI_RB();

	WriteMode = Dat;

	GPIO_SetBits(S_GPIO, S_NS);
	return Dat;
}

//The simplest of speed tests! The register mode is changed to sequential, the
//address is started at zero and bytes are continuously sent from 0 to the max
//possible address denoted by S_SIZE (0x1FFFF for the 23LC1024).
void S23_SpeedTest(void){
	uint32_t Cnt;
	S23_WriteModeReg(0x40);

	GPIO_ResetBits(S_GPIO, S_NS);

	SPI_SB(S_WRITE);

	SPI_SB(0);
	SPI_SB(0);
	SPI_SB(0);

	for(Cnt = 0; Cnt<S_SIZE; Cnt++){
		SPI_SB(Cnt);
	}

	GPIO_SetBits(S_GPIO, S_NS);
}

