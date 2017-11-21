#ifndef _MSG_H_
#define _MSG_H_

#include "dataflash_block.h"

uint16_t start_new_msg(struct msg_data *msg);
uint16_t get_num_msg(struct msg_data *msg);
uint16_t get_num_msg(struct msg_data *msg);
void get_msg_boundaries(struct msg_data *msg, uint16_t msg_num, uint16_t *start_page, uint16_t *end_page);
uint16_t find_last_msg(struct msg_data *msg);
uint16_t find_last_page(struct msg_data *msg);
int find_last_page_of_msg(struct msg_data *msg, uint16_t msg_number);
int check_wrapped(struct msg_data *msg);

#endif

