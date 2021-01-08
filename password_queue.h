#ifndef PASSWORD_QUEUE_H
#define PASSWORD_QUEUE_H

#include <stdlib.h>
#include <string.h>

#define QUEUE_SIZE 128

typedef struct decrypted_password {
	char *password;
	int thread_id;
} DECRYPTED_PASSWORD_T;

/*
 * Queue for password guesses. Each decrypter thread will produce a guess, the encrypter
 * thread will consume the guess.
 */
typedef struct password_queue {
	int sent_passwords;		 						//decrypter increments - will tell the encrypter when a new password is sent.
	int checked_passwords;	 						//encrypter increments - will be incremented every time a password is checked.
	DECRYPTED_PASSWORD_T passwords[QUEUE_SIZE];        	//The password queue
} PASSWORD_QUEUE_T;


void init_password_queue(PASSWORD_QUEUE_T *queue);
DECRYPTED_PASSWORD_T get_password(PASSWORD_QUEUE_T *queue);
void put_password(PASSWORD_QUEUE_T * queue, char *password, int thread_id);

#endif //PASSWORD_QUEUE_H