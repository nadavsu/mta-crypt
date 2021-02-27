#ifndef DECRYPTER_H
#define DECRYPTER_H


#include "include.h"

#define MAX_PASS_LEN 64 

int decrypt_pass(PASSWORD_MSG_T *encrypt_pass,int* rounds, int iterations );
void get_msg(GEN_MSG_T *msg);
void send_decrypt_pass();
void send_conn_req(char mq_name[]);
void close_connection(char mq_name[]);
int printable_pass(char* decrypt_pass, int len);
void msg_stamp(char* author, char *message_type);

#endif
