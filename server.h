#ifndef SERVER_H
#define SERVER_H

#include "include.h"

int create_connection(NEW_CONNECTION_MSG_T *message);

void close_connection(CLOSE_CONNECTION_MSG_T *message);

int password_guess_handler(PASSWORD_MSG_T * message);
void send_confirmation(PASSWORD_MSG_T * msg, int is_correct);
int encrypt_password(char *password, int password_length, char *key, int key_length);
int update_decrypters(PASSWORD_MSG_T* message);
void generate_key(char *key, int key_length);
int generate_password(char *password, int length);
int find_free_mq(int length);
void msg_stamp(char* author, char *message_type);

#endif
