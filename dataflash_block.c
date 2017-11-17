#include "dataflash_block.h"

void StartWrite(struct msg_data *msg, uint16_t PageAdr)
{	
	printf("StartWrite here.\n");

    msg->df_BufferIdx  = 0;
    msg->df_PageAdr    = PageAdr;
    WaitReady();
}

void FinishWrite(struct msg_data *msg)
{
    // Write Buffer to flash, NO WAIT
    BufferToPage(msg->df_PageAdr, 0);
    msg->df_PageAdr++;
    // If we reach the end of the memory, start from the begining  
    //循环记录
    if (msg->df_PageAdr > msg->df_NumPages)
        msg->df_PageAdr = 1;
	
    msg->df_BufferIdx = 0;
}

int WritePrioritisedBlock(struct msg_data *msg, const void *pBuffer, uint16_t size)
{
    // is_critical is ignored - we're a ring buffer and never run out
    // of space.  possibly if we do more complicated bandwidth
    // limiting we can reservice bandwidth based on is_critical

    if (!msg->msg_write_started) {
        return -1;
    }
	
    while (size > 0) {//while 判断条件 	
        uint16_t n = msg->df_PageSize - msg->df_BufferIdx;
        if (n > size) {
            n = size;
        }
		
        if (msg->df_BufferIdx == 0) {
            // if we are at the start of a page we need to insert a
            // page header
            if (n > msg->df_PageSize - sizeof(struct PageHeader)) {
                n = msg->df_PageSize - sizeof(struct PageHeader);
            }
		
            struct PageHeader ph = { msg->df_FileNumber, msg->df_FilePage };// df_FileNumber, df_FilePage 初始化中从SD卡中读回  add 0xff

            BlockWrite(msg->df_BufferIdx, &ph, sizeof(ph), pBuffer, n);
            msg->df_BufferIdx += n + sizeof(ph);
        } else {

            BlockWrite(msg->df_BufferIdx, NULL, 0, pBuffer, n);
            msg->df_BufferIdx += n;
        }

        size -= n;
        pBuffer = (const void *)(n + (uintptr_t)pBuffer);

        if (msg->df_BufferIdx == msg->df_PageSize) {
		
            FinishWrite(msg);
            msg->df_FilePage++;
        }
    }

    return 0;
}

void StartRead(struct msg_data *msg, uint16_t PageAdr)
{
    msg->df_Read_PageAdr = PageAdr;

    // disable writing while reading
    msg->msg_write_started = 0;

    WaitReady();

    // copy flash page to buffer
    PageToBuffer(msg->df_Read_PageAdr);

    // We are starting a new page - read FileNumber and FilePage
    struct PageHeader ph;
    BlockRead(0, &ph, sizeof(ph));
    msg->df_FileNumber = ph.FileNumber;
    msg->df_FilePage   = ph.FilePage;
    msg->df_Read_BufferIdx = sizeof(ph);

//	printf("ph.FileNumber %d\n", ph.FileNumber);
//	printf("ph.FilePage %d\n", ph.FilePage);
}

void ReadBlock(struct msg_data *msg, void *pBuffer, uint16_t size)
{
    while (size > 0) {
        uint16_t n = msg->df_PageSize - msg->df_Read_BufferIdx;
        if (n > size) {
            n = size;
        }

        WaitReady();

        BlockRead(msg->df_Read_BufferIdx, pBuffer, n);
        size -= n;
        pBuffer = (void *)(n + (uintptr_t)pBuffer);
        
        msg->df_Read_BufferIdx += n;

        if (msg->df_Read_BufferIdx == msg->df_PageSize) {
			printf("msg->df_Read_PageAdr %d\n", msg->df_Read_PageAdr);
            msg->df_Read_PageAdr++;
            if (msg->df_Read_PageAdr > msg->df_NumPages) {
                msg->df_Read_PageAdr = 1;
            }
            PageToBuffer(msg->df_Read_PageAdr);

            // We are starting a new page - read FileNumber and FilePage
            struct PageHeader ph;
            BlockRead(0, &ph, sizeof(ph));

            msg->df_FileNumber = ph.FileNumber;
            msg->df_FilePage   = ph.FilePage;

            msg->df_Read_BufferIdx = sizeof(ph);
        }
    }
}

