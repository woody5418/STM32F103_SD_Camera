#ifndef __MALLOC_H_
#define __MALLOC_H_

#include "stm32f10x.h"

//定义三个内存池
#define SRAMIN	 0	//外部内存池

#define SRAMBANK 	1	//定义支持的SRAM块数.

//mem1内存参数设定.mem1完全处于内部SRAM里面.
#define MEM1_BLOCK_SIZE			32  	  													//内存块大小为32字节
#define MEM1_MAX_SIZE			35*1024  													//最大管理内存 5K
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	//内存表大小

//内存管理控制器
struct _m_malloc_dev {
    void (*init)(u8);																						//初始化
    u8 (*perused)(u8);		  	    															//内存使用率
    u8 	*membase[SRAMBANK];																			//内存池 管理SRAMBANK个区域的内存
    u16 *memmap[SRAMBANK]; 																			//内存管理状态表
    u8  memrdy[SRAMBANK]; 																			//内存管理是否就绪
};
extern struct _m_malloc_dev malloc_dev;	 //在mallco.c里面定义

void mymemcpy(void *des,void *src,u32 n);
void mymemset(void *s,u8 c,u32 count);
void init_my_mem(u8 memx);
u8 my_mem_perused(u8 memx);
u32 my_mem_malloc(u8 memx,u32 size);
u8 my_mem_free(u8 memx,u32 offset);

void myfree(u8 memx,void *ptr);
void *mymalloc(u8 memx,u32 size);
void *myrealloc(u8 memx,void *ptr,u32 size);

#endif
