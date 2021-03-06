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
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "mta_rand.h"
#include "mta_crypt.h"

#define MAX_CONNECTIONS				32


#define SERVER_NAME                           "[SERVER]"

#define INFO                                  "[INFO]"
#define OK                                    "[OK]"
#define ERROR                                 "[ERROR]"

#define SERVER_MQ_NAME				"/server_mq"
#define SERVER_MQ_IND				0

#define MQ_MAX_MESSAGE				9
#define MQ_MAX_MESSAGE_SIZE			128




#define MQ_NAME_LEN 				32

#define BUFFER_SIZE                 64



#define MAX_CONNECTIONS_EC			-1

typedef enum TYPE {
	NEW_CONNECTION,
	CLOSE_CONNECTION,
	NEW_PASSWORD,
	PASSWORD_STATE
} TYPE_E;



typedef struct GENERIC_MSG_T {
	enum TYPE type;
	char data[];
} GEN_MSG_T;


typedef struct CONNECTION_T {
	int id;
	mqd_t mq;
} CONNECTION;


typedef struct MESSAGE_PASSWORD {
	enum TYPE type;
	int pass_len;
	int id;
	int pass_num;
	char pass[];
} PASSWORD_MSG_T;

typedef struct PASSWORD_STATE {
	enum TYPE type;
	int is_decrypted;
	int ack;
} PASSWORD_STATE_M;

typedef struct NEW_CONNECTION_MESSAGE {
	enum TYPE type;
	char mq_name[MQ_NAME_LEN];
	int id;
} NEW_CONNECTION_MSG_T;

typedef struct CLOSE_CONNECTION_MSG {
	enum TYPE type;
	char mq_name[MQ_NAME_LEN];
	int id;
} CLOSE_CONNECTION_MSG_T;



#endif

