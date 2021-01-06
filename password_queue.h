#ifndef PASSWORD_QUEUE_H
#define PASSWORD_QUEUE_H

#include <stdlib.h>
#include <string.h>

#define QUEUE_SIZE 128

/*
 * Queue for password guesses. Each decrypter thread will produce a guess, the encrypter
 * thread will consume the guess.
 */
typedef struct password_queue {
	int sent_passwords;		 			//decrypter increments - will tell the encrypter when a new password is sent.
	int checked_passwords;	 			//encrypter increments - will be incremented every time a password is checked.
	char *passwords[QUEUE_SIZE];        //The password queue
} password_queue_t;


void init_password_queue(password_queue_t *queue);
char *get_password(password_queue_t *queue);
void put_password(password_queue_t * queue, char *password);

#endif //PASSWORD_QUEUE_H