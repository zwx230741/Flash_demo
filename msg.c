#include "msg.h"

uint16_t get_num_msg(struct msg_data *msg)
{
    uint16_t lastpage;
    uint16_t last;
    uint16_t first;

    if (find_last_page(msg) == 1) {
        return 0;
    }

    StartRead(msg, 1);

    if (msg->df_FileNumber == 0xFFFF) {
        return 0;
    }

    lastpage = find_last_page(msg);
    StartRead(msg, lastpage);
    last = msg->df_FileNumber;
    StartRead(msg, lastpage + 3);
    first = msg->df_FileNumber;
    if(first > last) {
        StartRead(msg, 1);
        first = msg->df_FileNumber;
    }
	
	printf("get_num_msg first %d last %d.\n", first, last);

    if (last == first) {
        return 1;
    }

    return (last - first + 1);
}

uint16_t start_new_msg(struct msg_data *msg)
{
    uint16_t last_page = find_last_page(msg);

    StartRead(msg, last_page);

    if(find_last_msg(msg) == 0 || msg->df_FileNumber == 0xFFFF) {
        msg->df_FileNumber = 1;
        msg->df_FilePage = 1;
        StartWrite(msg, 1);
        msg->msg_write_started = 1;
        return 1;
    }

    uint16_t new_msg_num;

    // Check for msg of length 1 page and suppress
    if(msg->df_FilePage <= 1) {
        new_msg_num = msg->df_FileNumber;
        // Last msg too short, reuse its number
        // and overwrite it
        msg->df_FilePage = 1;
        StartWrite(msg, last_page);
    } else {
        new_msg_num = msg->df_FileNumber+1;
        if (last_page == 0xFFFF) {
            last_page=0;
        }
		msg->df_FileNumber = new_msg_num;
        msg->df_FilePage = 1;
        StartWrite(msg, last_page + 1);
    }
    msg->msg_write_started = 1;
    return new_msg_num;
}

// This function finds the first and last pages of a msg
// The first page may be greater than the last page if the DataFlash has been filled and partially overwritten.
void get_msg_boundaries(struct msg_data *msg, uint16_t msg_num, uint16_t *start_page, uint16_t *end_page)
{
    uint16_t num = get_num_msg(msg);	
    uint16_t look;

    if (msg->df_BufferIdx != 0) {
        FinishWrite(msg);
		usleep(100);	//important!
    }

	msg_num = get_num_msg(msg);	//guess?

    if(num == 1)
    {
        StartRead(msg, msg->df_NumPages);
        if (msg->df_FileNumber == 0xFFFF)
        {
            *start_page = 1;
            *end_page = find_last_page_of_msg(msg, (uint16_t)msg_num);
        } else {
            *end_page = find_last_page_of_msg(msg, (uint16_t)msg_num);
            *start_page = *end_page + 1;
        }

    } else {
        if(msg_num==1) {
            StartRead(msg, msg->df_NumPages);
            if(msg->df_FileNumber == 0xFFFF) {
                *start_page = 1;
            } else {
                *start_page = find_last_page(msg) + 1;
            }
        } else {
            if(msg_num == find_last_msg(msg) - num + 1) {
                *start_page = find_last_page(msg) + 1;
            } else {
                look = msg_num-1;
                do {
                    *start_page = find_last_page_of_msg(msg, look) + 1;
                    look--;
                } while (*start_page <= 0 && look >=1);
            }
        }
    }
    if (*start_page == msg->df_NumPages+1 || *start_page == 0) {
        *start_page = 1;
    }
    *end_page = find_last_page_of_msg(msg, msg_num);
    if (*end_page == 0) {
        *end_page = *start_page;
    }
}

// This funciton finds the last msg number
uint16_t find_last_msg(struct msg_data *msg)
{	
	printf("find_last_msg here.\n");

    uint16_t last_page = find_last_page(msg);
    StartRead(msg, last_page);
    return msg->df_FileNumber;
}

// This function finds the last page of the last file
uint16_t find_last_page(struct msg_data *msg)
{	
	printf("find_last_page here.\n");

    uint16_t look;
    uint16_t bottom = 1;
    uint16_t top = msg->df_NumPages;
    uint32_t look_hash;
    uint32_t bottom_hash;
    uint32_t top_hash;

    StartRead(msg, bottom);
    bottom_hash = ((int32_t)msg->df_FileNumber<<16) | msg->df_FilePage;

    while(top-bottom > 1) {
        look = (top+bottom)/2;
        StartRead(msg, look);
        look_hash = (int32_t)msg->df_FileNumber<<16 | msg->df_FilePage;
        if (look_hash >= 0xFFFF0000) look_hash = 0;

        if(look_hash < bottom_hash) {
            // move down
            top = look;
        } else {
            // move up
            bottom = look;
            bottom_hash = look_hash;
        }
    }
	
    StartRead(msg, top);
    top_hash = ((int32_t)msg->df_FileNumber<<16) | msg->df_FilePage;
    if (top_hash >= 0xFFFF0000) {
        top_hash = 0;
    }
    if (top_hash > bottom_hash) {
        return top;
    }
	
    return bottom;
}

uint16_t find_last_page_of_msg(struct msg_data *msg, uint16_t msg_number)
{
    uint16_t look;
    uint16_t bottom;
    uint16_t top;
    uint32_t look_hash;
    uint32_t check_hash;

    if(check_wrapped(msg))
    {
        StartRead(msg, 1);
        bottom = msg->df_FileNumber;
        if (bottom > msg_number)
        {
            bottom = find_last_page(msg);
            top = msg->df_NumPages;
        } else {
            bottom = 1;
            top = find_last_page(msg);
        }
    } else {
        bottom = 1;
        top = find_last_page(msg);
    }

    check_hash = (int32_t)msg_number<<16 | 0xFFFF;

    while(top-bottom > 1)
    {
        look = (top+bottom)/2;
        StartRead(msg, look);
        look_hash = (int32_t)msg->df_FileNumber<<16 | msg->df_FilePage;
        if (look_hash >= 0xFFFF0000) look_hash = 0;

        if(look_hash > check_hash) {
            // move down
            top = look;
        } else {
            // move up
            bottom = look;
        }
    }

    StartRead(msg, top);
    if (msg->df_FileNumber == msg_number) return top;

    StartRead(msg, bottom);
    if (msg->df_FileNumber == msg_number) return bottom;

    return -1;
}

int check_wrapped(struct msg_data *msg)
{
    StartRead(msg, msg->df_NumPages);
    if(msg->df_FileNumber == 0xFFFF)
        return 0;
    else
        return 1;
}

