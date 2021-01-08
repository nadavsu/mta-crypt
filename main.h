#ifndef MTA_CRYPT_H
#define MTA_CRYPT_H

#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <ctype.h>

#include "mta_rand.h"
#include "mta_crypt.h"

#include "utils.h"
#include "encrypter.h"
#include "decrypter.h"

#define NUM_OF_ENCRYPTERS	1

/*
 * Will be used as a global variable for communication between consumer and producer.
 */
typedef struct password_ {
	char *password;
	int length;
} PASSWORD_T;

/*
 * Arguments for the encrypter thread.
 */
typedef struct encrypter_args {
	unsigned int timeout;
	unsigned int password_length;
} ENCRYPTER_ARGS_T;


extern pthread_cond_t encrypter_cond;
extern pthread_cond_t decrypter_cond;
extern pthread_mutex_t mutex;
extern pthread_mutex_t print_mutex;
extern volatile PASSWORD_T encrypted_password;
extern PASSWORD_QUEUE_T password_queue;
extern volatile int created_passwords_counter;	//Encrypter records the number of passwords created.

#endif	//MTA_CRYPT_H