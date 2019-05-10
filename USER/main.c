#include "string.h"
#include "stdlib.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "USART3.h"
#include "ram.h"
#include "MMC_SD.h"
#include "ff.h"
#include "TEST_FATFS.h"
#include "led.h"
#include "apps.h"
#include "sccb.h"
#include "ov2640.h"

//EXCEL  表格头
char senddata[256]="Beep,Light,Servo,Harmfulgas,Ultraviolet,Temperature,Humidity,	Photoconductor,Pressure,Gyro_x,Gyro_y,Gyro_z,Accel_x,Accel_y,Accel_z,Mag_x,mag_y,Mag_z,Long,Lati\r\n";
char receive_SD_data[128]; //串口读取配置文件的数据缓存数组
unsigned long int rmove_data_num=0; //SD卡数据偏移地址计算
unsigned long int rmove_data_conf=0; //SD卡配置文件偏移地址计算
unsigned fdirectory=0; //读取的SD卡创建excel类型文件的计数变量
unsigned fphotos=0; //读取的SD卡创建photo类型文件的计数变量
char fname[40]; //Read_Times 配置文件路径
char ftmp[40];  //excel文件名字
char pname[40]; //Photo_Times 配置文件路径
char ptmp[40];  //photo文件名字
char errcode = 0;
char start_ov2640_flag=0;  //检测OV2640是否初始化成功 1失败 0成功
uint16_t secs=0;

int main(void)
{
    SystemInit();
    delay_init(72);	//延时初始化
    NVIC_Configuration();
    uart_init(115200);
    uart3_init(115200);  //数据接收频率设置为115200
    LED_Init();

    while(SD_Initialize() != 0) {
        //检测不到SD卡
        printf("SD Card Failed!\r\n");
        LED_Red=1;
        delay_ms(500);
        printf("Please Check!\r\n");
        LED_Red=0;
        delay_ms(500);
    }
    printf("Check SD OK!\r\n");

    readfirenum("config.ini",receive_SD_data); // 读取里面的数据查看是第几次上电
    // 获取里面的配置信息
    json_data_config((char *)receive_SD_data,(char *)Read_times1,(char *)Read_times2,(char *)Read_times3,(char *)Read_times4,(char *)Read_times5,(char *)Read_times6);
    fdirectory = atoi((char *)Read_times1); //读取配置文件里的excel表格的存储次数
    fphotos = atoi((char *)Read_times2);    //读取配置文件里的photo照片的存储次数

    if(fdirectory == 0) {
        Test_f_mkdir("userdata");// 如果内存卡已经被格式化，则创建文件夹 数据存储
        printf("FIRST Create userdata success!\r\n");  //创建excel表格成功
        fdirectory++;
        sprintf(fname,"Read_Times:%4d\r\n",fdirectory); // 配置文件的数据记录情况
        rmove_data_conf = strlen(fname);  // 获取配置文件第一段的长度
        Test_f_writefile("config.ini",fname,0,strlen(fname)); //配置文件写入
        printf("FIRST Write Read_Times:%4d ok!\r\n",fdirectory);

        sprintf(ftmp,"userdata/%d_datas.csv",fdirectory);  //创建userdata下的 1_datas.csv 文件
        rmove_data_num = strlen(senddata);// 表头
        Test_f_writefile(ftmp,senddata,0,strlen(senddata)); //写入excel格式文件的表头
        printf("FIRST Write userdata/%d_datas.csv\r\n",fdirectory);
    } else {
        if(fdirectory == 0XFFFF) fdirectory=0;  //如果存储到最大限度，则从0开始重新存储
        fdirectory++;
        sprintf(fname,"Read_times:%4d\r\n",fdirectory); // 配置文件的数据记录情况
        rmove_data_conf = strlen(fname);  // 获取配置文件第一段的长度
        Test_f_writefile("config.ini",fname,0,strlen(fname)); //配置文件写入 覆盖Read_times
        printf("Create config.ini success!\r\n");
        printf("Create Read_times:%4d OK !\r\n",fdirectory);

        sprintf(ftmp,"userdata/%d_datas.csv",fdirectory);  //创建userdata下的 x_datas.csv 文件
        rmove_data_num = strlen(senddata);// 表头
        Test_f_writefile(ftmp,senddata,0,strlen(senddata)); //写入excel格式文件的表头
        printf("Create %d_datas.csv OK !\r\n",fdirectory);
    }
    //  照片
    if(fphotos == 0) {
        Test_f_mkdir("picture");// 如果内存卡已经被格式化，则创建文件夹  图片存储
        printf("FIRST Create picture success!\r\n"); //串口1打印信息
        fphotos++;
        sprintf(pname,"Photo_Times:%4d\r\n",fphotos); // 配置文件的数据记录情况
        Test_f_writefile("config.ini",pname,rmove_data_conf,strlen(pname)); //配置文件写入
        printf("FIRST Write Photo_Times:%4d OK!\r\n",fphotos);
    } else {
        if(fphotos == 0XFFFF) 	fphotos = 0;  //如果存储到最大限度，则从0开始重新存储
        fphotos++;
        sprintf(pname,"Photo_Times:%4d\r\n",fphotos); // 配置文件的数据记录情况
        Test_f_writefile("config.ini",pname,rmove_data_conf,strlen(pname)); //配置文件写入 覆盖Read_times
        printf("Create Photo_Times:%4d OK !\r\n",fphotos);
    }

    while(OV2640_Init()) {	//初始化OV2640 1s 检测
        if(errcode == 10) {
            errcode=0;
            printf("OV2640 错误!\r\n");
            start_ov2640_flag = 1;
            break;
        }
        errcode++;
        LED_Red=LED_Green=1;
        delay_ms(50);
        LED_Red=LED_Green=0;
        delay_ms(50);
    }
    OV2640_OutSize_Set(OV2640_JPEG_WIDTH,OV2640_JPEG_HEIGHT); //设置图片尺寸
    ov2640_speed_ctrl();

    LED_Red=LED_Green=0;
    printf("Start !\r\n");
    while(1) {
        usart3_data_analysis();
        if(secs == 6000) {   //1min拍摄一张照片
            secs=0;
            if( SD_Initialize() !=0 ) {
                LED_Red=1;
                delay_ms(500);
                LED_Red=0;
                printf("Check SD Card Failed!!\r\n");
                delay_ms(500);
            } else {
                if(start_ov2640_flag == 0)
                    ov2640_jpg_photo();
                printf("OK\r\n");
                fphotos++;
            }
        }
        secs++;
        delay_ms(10);
    }
}

