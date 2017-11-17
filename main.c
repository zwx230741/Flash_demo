#include "msg.h"

int main()
{
	struct msg_data *msg = Init();	
	if(msg == NULL)
	{
		printf("msg_data init failed.\n");
		return -1;
	}
	
	char *str = "hello furong.";
	uint8_t buff[512];
	int i = 0;
	for(i = 0; i < sizeof(buff); i++)
	{
		buff[i] = i;
	}
	uint16_t start_page, end_page;
	
	start_new_msg(msg); //important!
	for(i = 0; i < 2; i++)
	{
//		WritePrioritisedBlock(msg, str, strlen(str));
		WritePrioritisedBlock(msg, buff, sizeof(buff));
//		FinishWrite(msg);
	}

	uint16_t num = get_num_msg(msg);
	printf("get_num_msg %d\n", num);
		
	get_msg_boundaries(msg, num, &start_page, &end_page);
	printf("get_msg_boundaries start_page %d end_page %d\n", start_page, end_page);
	
    StartRead(msg, start_page); //important!
	uint16_t read_len = 512;
	memset(buff, 0, sizeof(buff));
	ReadBlock(msg, buff, read_len);
	printf("ReadBlock:");
	for(i = 0; i < read_len; i++)
	{
		printf("%d ", buff[i]);
	}
	printf("\n");
	
	memset(buff, 0, sizeof(buff));
	ReadBlock(msg, buff, read_len);
	for(i = 0; i < read_len; i++)
	{
		printf("%d ", buff[i]);
	}
	printf("\n");
	
	return 0;
}

