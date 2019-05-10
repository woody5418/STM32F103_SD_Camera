#ifndef __APPS_H
#define __APPS_H
#include "stm32f10x.h"

#define OV2640_JPEG_WIDTH	800	//JPEG拍照的宽度	
#define OV2640_JPEG_HEIGHT	600		//JPEG拍照的高度

extern unsigned fdirectory; //读取的SD卡创建excel类型文件的计数变量
extern unsigned fphotos; //读取的SD卡创建photo类型文件的计数变量
extern unsigned long int rmove_data_conf; //SD卡配置文件偏移地址计算
// txt文件编写偏移地址
extern unsigned long int rmove_data_num;

extern uint8_t Read_times1[5];
extern uint8_t Read_times2[5];
extern uint8_t Read_times3[5];
extern uint8_t Read_times4[5];
extern uint8_t Read_times5[5];
extern uint8_t Read_times6[5];
extern char fname[40];
extern char ftmp[40];

void ov2640_speed_ctrl(void);
void camera_new_pathname(void);
void usart3_data_analysis(void);
void ov2640_jpg_photo(void);
void json_data_config(char *str,char *R1,char *R2,char *R3,char *R4,char *R5,char *R6);


#endif










