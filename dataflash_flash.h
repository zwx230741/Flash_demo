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

#define PAGESIZE      512		//ҳ�Ĵ�С 512
#define NUMPAGES 	   (STORAGE_SIZE/PAGESIZE)   	//	ҳ������

struct msg_data
{
	uint16_t df_BufferIdx; //д�뻺������������
	uint16_t df_Read_BufferIdx;		//��ȡ��������������
	uint16_t df_PageAdr;		//д��ҳ�ı��
	uint16_t df_Read_PageAdr;		//��ȡҳ�ı��

    uint16_t df_FileNumber;		//��־�ļ��ı�ţ��½���־��д��ҳͷ
    uint16_t df_FilePage;		//��־�ļ���ҳ�ţ�x����־��һҳΪ1,�Դ�����

	uint16_t df_PageSize;		//ҳ�Ĵ�С 512
    uint16_t df_NumPages;		//ҳ������

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

