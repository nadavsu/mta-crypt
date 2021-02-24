#ifndef DECRYPTER_H
#define DECRYPTER_H

#include "utils.h"
#include "include.h"

#define MAX_PASS_LEN 64 

int decrypt_pass(mqd_t mq_decrypter, PASSWORD_MSG_T *encrypt_pass,int* rounds, int iterations );
void get_msg(mqd_t mq_decrypt,PASSWORD_MSG_T *pass_msg);
void send_decrypt_pass(mqd_t mq_server, char pass[]);
void send_conn_req(char mq_name[], mqd_t mq_server);
void close_conn(mqd_t mq_server, char mq_name[]);


#endif
