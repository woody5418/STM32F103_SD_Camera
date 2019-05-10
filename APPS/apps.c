#include "string.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "USART3.h"
#include "apps.h"
#include "MMC_SD.h"
#include "ff.h"
#include "TEST_FATFS.h"
#include "ov2640.h"
#include "led.h"
#include "ram.h"

uint8_t times=0; // 拍摄计时器
uint8_t ov2640_framebuf[60000];//帧缓存
//uint8_t jpg_frame_data_byte_new = 0, jpg_frame_data_byte_old = 0;
//uint8_t jpg_frame_data_start = 0;
//uint32_t jpg_frame_data_length = 0;

char ovname[40]; //Photo_Times 配置文件路径
char ovtemp[40]; //照片

uint8_t Read_times1[5];//第一段配置变量
uint8_t Read_times2[5];//第二段配置变量
uint8_t Read_times3[5];//第三段配置变量
uint8_t Read_times4[5];//第四段配置变量
uint8_t Read_times5[5];//第五段配置变量
uint8_t Read_times6[5];//第六段配置变量

//uint8_t *jpeg_buf[59000];  //图片缓存数组

//OV2640速度控制
//根据LCD分辨率的不同，设置不同的参数
void ov2640_speed_ctrl(void)
{
    u8 clkdiv,pclkdiv;			//时钟分频系数和PCLK分频系数
    clkdiv=15;  //15  20
    pclkdiv=10;  //4   25
    SCCB_WR_Reg(0XFF,0X00);
    SCCB_WR_Reg(0XD3,pclkdiv);	//设置PCLK分频
    SCCB_WR_Reg(0XFF,0X01);
    SCCB_WR_Reg(0X11,clkdiv);	//设置CLK分频
}

//OV2640拍照jpg图片
//pname:要保存的jpg照片路径+名字
//返回值:0,成功
//    其他,错误代码
void ov2640_jpg_photo(void)
{
    u32 i=0;
    //u8 jpg_frame_data_temp;
    u32 jpeglen=0;
    u8* pbuf;
    uint8_t j=0;
    //jpg_frame_data_length = 0;
    OV2640_JPEG_Mode();//切换为JPEG模式
    OV2640_OutSize_Set(OV2640_JPEG_WIDTH,OV2640_JPEG_HEIGHT); //设置图片尺寸
    //ov2640_speed_ctrl();
    SCCB_WR_Reg(0XFF,0X00);
    SCCB_WR_Reg(0XD3,30);
    SCCB_WR_Reg(0XFF,0X01);
    SCCB_WR_Reg(0X11,0X1);
    LED_Green=1;  //亮
    for(i=0; i<10; i++) {	//丢弃10帧，等待OV2640自动调节好（曝光白平衡之类的）
        while(OV2640_VSYNC==1);
        while(OV2640_VSYNC==0);
    }
    while(OV2640_VSYNC==1) {	//开始采集jpeg数据
        while(OV2640_HREF) {
            while(OV2640_PCLK==0);
            ov2640_framebuf[jpeglen]=OV2640_DATA;
            while(OV2640_PCLK==1);
            jpeglen++;
        }
    }
    LED_Green=0;  //灭
    printf("jpeg data size: %d \r\n",jpeglen);
    pbuf = (uint8_t *)ov2640_framebuf;
    for(i=0; i<jpeglen; i++) { //查找0XFF,0XD8
        if((pbuf[i]==0XFF)&&(pbuf[i+1]==0XD8))
            break;
    }
    if(i==jpeglen) {
        for(j=0; j<5; j++) {
            LED_Red=1;
            delay_ms(50);
            LED_Green=0;
            delay_ms(50);
        }
        printf("没找到0XFF,0XD8......\r\n");//没找到0XFF,0XD8
    } else {	//找到了
        LED_Green=0;  //灭
        pbuf+=i;//偏移到0XFF,0XD8处
        sprintf(ovname,"Photo_Times:%4d\r\n",fphotos); // 配置文件的数据记录情况6
        Test_f_writefile("config.ini",ovname,rmove_data_conf,strlen(ovname)); //配置文件写入 覆盖Read_times
        printf("Write Photos:%4d OK!\r\n",fphotos);
        sprintf(ovtemp,"picture/IMG_%d.jpg",fphotos);
        Test_f_writefile(ovtemp,(char *)pbuf,0,jpeglen-i); //配置文件写入
        printf("photo data is: %d \r\n",jpeglen-i);
    }
    /*
    while(OV2640_VSYNC == 1) {
        while(OV2640_HREF) {
            while(OV2640_PCLK == 0);
            jpg_frame_data_temp = (u8)OV2640_DATA;
            jpg_frame_data_byte_new = jpg_frame_data_temp;
            if( (jpg_frame_data_start == 0) && (jpg_frame_data_byte_old == 0xFF) && (jpg_frame_data_byte_new == 0xD8) ) {
                jpeg_buf[0] = 0xFF;
                jpeg_buf[1] = 0xD8;
                jpg_frame_data_start = 1;
                jpg_frame_data_length = 2;
            } else if(jpg_frame_data_start == 1) {
                if(jpg_frame_data_length < 0XE678) {
                    jpeg_buf[jpg_frame_data_length] = jpg_frame_data_byte_new;
                    jpg_frame_data_length ++;
                    if( (jpg_frame_data_byte_old == 0xFF) && (jpg_frame_data_byte_new == 0xD9) ) {
                        jpg_frame_data_start = 0;
                    }
                }
            }
            jpg_frame_data_byte_old = jpg_frame_data_byte_new;
            while(OV2640_PCLK == 1);
        }
    }

    LED_Green=0;  //灭
    sprintf(ovname,"Photo_Times:%4d\r\n",fphotos); // 配置文件的数据记录情况
    Test_f_writefile("config.ini",ovname,rmove_data_conf,strlen(ovname)); //配置文件写入 覆盖Read_times
    printf("Write Photos:%4d OK!\r\n",fphotos);
    sprintf(ovtemp,"picture/IMG_%d.jpg",fphotos);
    Test_f_writefile(ovtemp,(char *)jpeg_buf,0,jpg_frame_data_length); //配置文件写入
    printf("photo data is: %d \r\n",jpg_frame_data_length);
    jpg_frame_data_length=0;
    */
}

//文件名自增（避免覆盖）
//jpg组合成:形如"0:PHOTO/PIC13141.jpg"的文件名
//pname = "picture/IMG_%d.jpeg"
//ovtemp 创建的jpeg文件路径
/*
void camera_new_pathname(void)
{
    sprintf(ovname,"Photo_Times:%4d\r\n",fphotos); // 配置文件的数据记录情况
    Test_f_writefile("config.ini",ovname,rmove_data_conf,strlen(ovname)); //配置文件写入 覆盖Read_times
    printf("Write Photos:%4d OK!\r\n",fphotos);
    sprintf(ovtemp,"picture/IMG_%d.jpg",fphotos);
    //Test_f_writefile(ovname,(char *)jpeg_buf,0,jpg_frame_data_length); //配置文件写入
    printf("photo data is: %d \r\n",jpg_frame_data_length);
}
*/
//处理配置文件获取数值
void json_data_config(char *str,char *R1,char *R2,char *R3,char *R4,char *R5,char *R6)
{
    uint8_t i;
    uint8_t comma_num = 0;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == ':') {
            switch (comma_num) {
            case 0: {
                comma_num = 1;
                R1[0] = str[i + 1];
                R1[1] = str[i + 2];
                R1[2] = str[i + 3];
                R1[3] = str[i + 4];
                break;
            }
            case 1: {
                comma_num =2;
                R2[0] = str[i + 1];
                R2[1] = str[i + 2];
                R2[2] = str[i + 3];
                R2[3] = str[i + 4];
                break;
            }

            case 2: {
                comma_num =3;
                R3[0] = str[i + 1];
                R3[1] = str[i + 2];
                R3[2] = str[i + 3];
                R3[3] = str[i + 4];
                break;
            }

            case 3: {
                comma_num += 1;
                R4[0] = str[i + 1];
                R4[1] = str[i + 2];
                R4[2] = str[i + 3];
                R4[3] = str[i + 4];
                break;
            }

            case 4: {
                comma_num =5;
                R5[0] = str[i + 1];
                R5[1] = str[i + 2];
                R5[2] = str[i + 3];
                R5[3] = str[i + 4];
                break;
            }

            case 5: {
                comma_num = 0;
                R6[0] = str[i + 1];
                R6[1] = str[i + 2];
                R6[2] = str[i + 3];
                R6[3] = str[i + 4];
                break;
            }

            }
        } else
            continue;
    }
}


// 串口3接收到的消息
void usart3_data_analysis(void)
{
    uint16_t cra16_check_Data=0;
    uint16_t cra16_rec_Data=0;
    uint16_t len=0;

    if(USART3_RX_STA&0x8000) {
        len=USART3_RX_STA&0x3FFF;   //得到此次接收到的数据长度
        printf("Receive Data OK\r\n");
        cra16_rec_Data = crc16((char *)USART3_RX_BUF,len-2);
        cra16_check_Data = USART3_RX_BUF[len-2]<<8|USART3_RX_BUF[len-1];
        if(cra16_rec_Data == cra16_check_Data) {
            USART3_RX_BUF[len-2] = 0x0d;
            USART3_RX_BUF[len-1] = 0x0a;
            USART3_RX_BUF[len] = '\0';
            if(SD_Initialize() == 0) {
                printf("Write data to Excel OK!!!\r\n");
                Test_f_writefile(ftmp,(char *)USART3_RX_BUF,rmove_data_num,len);
                rmove_data_num += strlen((char *)USART3_RX_BUF);
            } else {
                printf("SD Card Failed! Please Check!\r\n");
            }
        }
        USART3_RX_STA=0;
    } else {
        if(times == 50) {
            if(SD_Initialize() != 0) {
                LED_Red=!LED_Red;
                printf("Check SD Card Failed!!\r\n");
            } else
                LED_Red=0;
            times=0;
        }
        times++;
    }

}



















