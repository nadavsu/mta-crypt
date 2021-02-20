#ifndef INCLUDE_H
#define INCLUDE_H

#define MQ_NAME_LEN 				32
#define NEW_CONNECTION_PRIORITY		2
#define NEW_PASSWORD_PRIORITY		1

enum TYPE {
	NEW_CONNECTION,
	NEW_PASSWORD,
};


typedef struct MESSAGE {
	enum TYPE message_type;
	char data[];
} MESSAGE_T;


typedef struct MESSAGE_PASSWORD {
	int pass_num;
	int pass_len;
	char pass[];
} MESSAGE_PASSWORD_T;


//Should be higher priority than normal message.
typedef struct NEW_CONNECTION {
	int pid;
	char[MQ_NAME_LEN] mq_name;
} NEW_CONNECTION_T;

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
