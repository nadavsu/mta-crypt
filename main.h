#ifndef MTA_CRYPT_H
#define MTA_CRYPT_H

#include <stdlib.h>
#include <limits.h>

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
	char *encrypted_password;
	int password_len;
} password_t;

/*
 * Arguments for the encrypter thread.
 */
typedef struct encrypter_args {
	int timeout;
	int password_length;
} encrypter_args_t;


/*
 * Queue for password guesses. Each decrypter thread will produce a guess, the encrypter
 * thread will consume the guess.
 */
typedef struct password_queue {
	int sent_passwords;		 //decrypter increments - will tell the encrypter when a new password is sent.
	int checked_passwords;	 //encrypter increments - will be incremented every time a password is checked.
	char **password_guesses; //The password queue
} password_queue_t;

extern pthread_cond_t encrypter_cond;
extern pthread_cond_t decrypter_cond;
extern pthread_mutex_t 	mutex;
extern volatile password_t password;
extern volatile password_queue_t password_queue;

#endif	//MTA_CRYPT_H