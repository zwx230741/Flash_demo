#ifndef _DATAFLASH_BLOCK_H_
#define _DATAFLASH_BLOCK_H_

#include "dataflash_flash.h"

struct PageHeader {
    uint16_t FileNumber;
    uint16_t FilePage;
};

void StartRead(struct msg_data *msg, uint16_t PageAdr);
void ReadBlock(struct msg_data *msg, void *pBuffer, uint16_t size);
void StartWrite(struct msg_data *msg, uint16_t PageAdr);
void FinishWrite(struct msg_data *msg);
int WritePrioritisedBlock(struct msg_data *msg, const void *pBuffer, uint16_t size);

#endif

