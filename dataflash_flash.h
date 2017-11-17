#ifndef _DATAFLASH_FLASH_H_
#define _DATAFLASH_FLASH_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>

#define STORAGE_SIZE 16384
#define FLASH_SIZE   (128*1024*1024)

#define PAGESIZE      512		//页的大小 512
#define NUMPAGES 	   (STORAGE_SIZE/PAGESIZE)   	//	页的数量

struct msg_data
{
	uint16_t df_BufferIdx; //写入缓冲区的索引号
	uint16_t df_Read_BufferIdx;		//读取缓冲区的索引号
	uint16_t df_PageAdr;		//写入页的编号
	uint16_t df_Read_PageAdr;		//读取页的编号

    uint16_t df_FileNumber;		//日志文件的编号，新建日志是写入页头
    uint16_t df_FilePage;		//日志文件的页号，x号日志第一页为1,以此类推

	uint16_t df_PageSize;		//页的大小 512
    uint16_t df_NumPages;		//页的数量

	volatile int msg_write_started;
};

struct msg_data* Init();
void BufferToPage (uint16_t PageAdr, uint8_t wait);
void PageToBuffer(uint16_t PageAdr);
void BlockWrite(uint16_t IntPageAdr,
				const void *pHeader, uint8_t hdr_size,
				const void *pBuffer, uint16_t size);
void BlockRead(uint16_t IntPageAdr, void *pBuffer, uint16_t size);
void WaitReady();

#endif

