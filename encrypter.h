#ifndef ENCRYPTER_H
#define ENCRYPTER_H

#include <errno.h>

#include "main.h"
#include "password_queue.h"

void *encrypt(void *arg);
void update_current_password(char *encrypted_password, int encrypted_password_length);
char *generate_password();
void password_consumer();
int check_password(char *decrypted_password);

#endif	//ENCRYPTER_H