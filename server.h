#ifndef SERVER_H
#define SERVER_H

#include "include.h"

int create_connection(NEW_CONNECTION_MSG_T *message, mq_attr *attr);
int close_connection(CLOSE_CONNECTION_MSG_T *message);
int password_guess_handler(PASSWORD_MSG_T * message);

int encrypt_password(char *password, int password_length, char *key, int key_length);
int update_decrypters(PASSWORD_MSG_T message, int message_size);
int generate_key(char *key, int key_length);
int generate_password(char *password, int length);

#endif