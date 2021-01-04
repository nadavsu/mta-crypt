#ifndef MTA_CRYPT_H
#define MTA_CRYPT_H

#include <stdlib.h>
#include <limits.h>

#include "utils.h"
#include "encrypter.h"
#include "decrypter.h"

#define NUM_OF_ENCRYPTERS	1

typedef struct password_ {
	char *decrypted_password;
	char *encrypted_password;
	char *key;
	int key_len;
	int password_len;

	char *found_password;
} password_t;

extern pthread_mutex_t 	mutex;
extern volatile password_t password;

#endif	//MTA_CRYPT_H