#ifndef INCLUDE_H
#define INCLUDE_H

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "mta_rand.h"
#include "mta_crypt.h"

#define MAX_CONNECTIONS				31

#define SERVER_MQ_NAME				"server_mq"
#define SERVER_MQ_IND				0

#define MQ_MAX_MESSAGE				1024
#define MQ_MAX_MESSAGE_SIZE			72

#define MQ_NAME_LEN 				32

#define CLOSE_CONNECTION_PRIORITY	2
#define NEW_CONNECTION_PRIORITY		2
#define NEW_PASSWORD_PRIORITY		1

#define MAX_CONNECTIONS_EC			-1

typedef enum TYPE {
	NEW_CONNECTION,
	CLOSE_CONNECTION,
	NEW_PASSWORD,
	UNDEFINED
} TYPE_E;


typedef struct GENERIC_MSG_T {
	enum TYPE type;
	char data[];
} GEN_MSG_T;


typedef struct MESSAGE_PASSWORD {
	enum TYPE type;
	int pass_len;
	char pass[];
} PASSWORD_MSG_T;


typedef struct NEW_CONNECTION_MESSAGE {
	enum TYPE type;
	char mq_name[MQ_NAME_LEN];
} NEW_CONNECTION_MSG_T;

typedef struct CLOSE_CONNECTION_MSG {
	enum TYPE type;
	char mq_name[MQ_NAME_LEN];
} CLOSE_CONNECTION_MSG_T;

#endif


//server
	//open mq server_mq
	//while (1) and listens for messages
		//if new request
			//create new mq with name mq_pid;
			//send password down queue.
		//if decrypted password





//decrypter
	//open sever_mq message queue
	//send message request on server_mq
	//wait for mq name
	//when it has mq name
	//listen for password on mq name
	//start decrypting.
	//when decrypted - send password to server_mq
