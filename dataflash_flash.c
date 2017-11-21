#include "dataflash_flash.h"

#define FLASH_DEV "test"

static uint8_t msg_buffer[2][PAGESIZE];	//0:write 1:read
static int flash_fd;

struct msg_data* Init()
{
	if (flash_fd == 0) {
		flash_fd = open(FLASH_DEV, O_RDWR | O_CREAT | O_TRUNC, 0777);
		if (flash_fd == -1) {
			printf("DataFlash_Flash open failed.\n");
			return NULL;
		}
	}

#if 1
	uint8_t buff[STORAGE_SIZE];
	memset(buff, 0xff, STORAGE_SIZE);
	write(flash_fd, buff, STORAGE_SIZE);
#endif

	struct msg_data *msg = (struct msg_data *)malloc(sizeof(struct msg_data));
	if(msg == NULL)
	{
		printf("DataFlash_Flash open failed.\n");
		return NULL;
	}

	msg->df_PageSize = PAGESIZE;
    msg->df_NumPages = NUMPAGES - 1;

	return msg;
}

void BlockRead(uint16_t IntPageAdr, void *pBuffer, uint16_t size)
{
//	printf("BlockRead IntPageAdr %d size %d\n", IntPageAdr, size);

	assert(IntPageAdr <= PAGESIZE);

	memset(pBuffer, 0, size);

	memcpy(pBuffer, &msg_buffer[1][IntPageAdr], size);
	
    return;
}

void BlockWrite(uint16_t IntPageAdr, const void *pHeader, uint8_t hdr_size,
                    const void *pBuffer, uint16_t size)
{
   printf("BlockWrite IntPageAdr %d hdr_size %d size %d\n", IntPageAdr, hdr_size, size);

   assert(IntPageAdr <= PAGESIZE);
   
    if (hdr_size) {
		memset(msg_buffer[0], 0, PAGESIZE);
        memcpy(msg_buffer[0], pHeader, hdr_size);
    }

    memcpy(&msg_buffer[0][IntPageAdr+hdr_size], pBuffer, size);

#if 1
	int i = 0;
	printf("BlockWrite pBuffer IntPageAdr %d size+hdr_size %d:", IntPageAdr, size+hdr_size);
	for(i = IntPageAdr; i < (IntPageAdr + (size + hdr_size)); i++)
	{
		printf("%d ", msg_buffer[0][i]);
	}
	printf("\n");
#endif

}

void BufferToPage (uint16_t PageAdr, unsigned char wait)
{
	printf("BufferToPage here PageAdr %d.\n", PageAdr);

	assert(PageAdr <= NUMPAGES);
	PageAdr -= 1;

	off_t ofs = PageAdr * PAGESIZE;

	if (flash_fd == -1) {		
		return;
	}

	if (lseek(flash_fd, ofs, SEEK_SET) != ofs) {
		printf("BufferToPage lseek err.\n");
		return;
	}

#if 1
	int i = 0;
	printf("DataFlash_Flash BufferToPage:");
	for(i = 0; i < PAGESIZE; i++)
	{
		printf("%d ", msg_buffer[0][i]);
	}
	printf("\n");
#endif

	if (write(flash_fd, msg_buffer[0], PAGESIZE) != PAGESIZE)
	{
		printf("BufferToPage write err.\n");
		return;
	}

	return;
}

void PageToBuffer(uint16_t PageAdr)
{
//	printf("PageToBuffer PageAdr %d.\n", PageAdr);
	
	assert(PageAdr <= NUMPAGES);
	PageAdr -= 1;
		
	off_t ofs = PageAdr * PAGESIZE;

	memset(msg_buffer[1], 0, PAGESIZE);

	if (lseek(flash_fd, ofs, SEEK_SET) != ofs) {
		printf("PageToBuffer lseek err.\n");
        return;
    }
	if (read(flash_fd, msg_buffer[1], PAGESIZE) != PAGESIZE)
	{
		printf("PageToBuffer read err.\n");
		return;
	}

#if 0
	int i = 0;
	printf("DataFlash_Flash PageToBuffer:");
	for(i = 0; i < PAGESIZE; i++)
	{
		printf("%d ", msg_buffer[1][i]);
	}
	printf("\n");
#endif
	
}

void WaitReady()
{
	return;
}

