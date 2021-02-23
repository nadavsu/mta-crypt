#include "server.h"

mqd_t mqs[MAX_CONNECTIONS + 1] = {-1};
char *current_password, *key;
int pass_length, key_length;

int main(int argc, char *argv[]) {
	int timeout = INT_MAX;
	int opt;

	while ((opt = getopt(argc, argv, "l:t:")) != -1) {
        switch (opt) {
        case 'l':
        	pass_length = atoi(optarg);
        	key_length = pass_length / 8;
         	break;
        case 't':
        	timeout = atoi(optarg);
        	break;
        default:
            fprintf(stderr, "Usage: %s [-lt] [number]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    current_password = (char *) malloc(sizeof(char) * pass_length);
    if (!current_password) {
    	exit(EXIT_FAILURE);
    }
	
	struct mq_attr server_attr = {0};
	server_attr.mq_maxmsg = MQ_MAX_MESSAGE;
	server_attr.mq_msgsize = (sizeof(char) * (pass_length + 1)) + sizeof(TYPE_E);

	struct mq_attr decrypter_attr = {0};
	decrypter_attr.mq_maxmsg = MQ_MAX_MESSAGE;
	decrypter_attr.mq_msgsize = (sizeof(NEW_CONNECTION_T));

	mq_unlink(SERVER_MQ_NAME);
	mqs[SERVER_MQ_IND] = mq_open(SERVER_MQ_NAME, O_CREAT | O_RDONLY, S_IRWXU | S_IRWXG, &server_attr);
	if (mqs[SERVER_MQ_IND] == -1) {
		exit(errno);
	}

	GEN_MSG_T *message = (MESSAGE_T *) malloc(MQ_MAX_MSG_SIZE);
	if (!message) {
		exit(EXIT_FAILURE);
	}

	generate_password(current_password, password_length);
	generate_key(key, key_length);
	encrypt_password(current_password, password_length, key, key_length);

	for (;;) {
		int msg_len = mq_receive(mqs[SERVER_MQ_IND], (char*)message, MQ_MAX_MSG_SIZE, NULL);
		if (msg_len == -1) {
			exit(errno);
		}

		switch (message->type) {
			case NEW_CONNECTION:
				create_connection((NEW_CONNECTION_MSG_T *) message, &decrypter_attr);
				break;
			case PASSWORD:
				password_guess_handler((PASSWORD_MSG_T *) message);
				break;
			case CLOSE_CONNECTION:
				close_connection((CLOSE_CONNECTION_MSG_T *) message);
				break;
		}

	}
}

int create_connection(NEW_CONNECTION_MSG_T *message, mq_attr *attr) {
	int i = get_free_mq(mqs, MAX_CONNECTIONS);
	if (i < 0) {
		return MAX_CONNECTIONS_EC;
	}

	mqs[i] = mq_open(message->mq_name, O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG, attr);
	if (mqs[i] == -1) {
		return errno;
	}


	PASSWORD_MSG_T password_message;
	password_message.type 	= NEW_PASSWORD;
	password_message.length = pass_length;
	strdup(password_message.pass, current_password); 

	mq_send(mqs[i], (char*)password_message, MQ_MAX_MSG_SIZE, 0);
	return 0;
}

int close_connection(CLOSE_CONNECTION_MSG_T *message) {
	return mq_unlink(message->mq_name);
}

int password_guess_handler(PASSWORD_MSG_T * message) {
	if (strcmp(current_password, message->pass) == 0) {
		printf("Password decrypted successfully, received - %s original - %s\n", msg_password, original_password);
		generate_password(current_password, password_length);
		generate_key(key, key_length);
		encrypt_password(current_password, password_length, key, key_length);

		PASSWORD_MSG_T message;
		message.type = NEW_PASSWORD;
		message.length = password_length;
		strdup(message.pass, current_password);
		int message_size = 1 * sizeof(int) + 1 * sizeof(TYPE_E) + password_length * sizeof(char);

		update_decrypters(message, message_size);

		return 1;
	} else {
		printf("Wrong password, received - %s original - %s\n", msg_password, original_password);
		return 0;
	}
}

int encrypt_password(char *password, int password_length, char *key, int key_length) { 
    srand(ENCRYPTER_RAND_SEED);
    MTA_CRYPT_RET_STATUS status = MTA_encrypt(key, key_length, original_password, password_length,
                                              encrypted_password.password,
                                              &password_length);
    if (status != MTA_CRYPT_RET_OK) {
        printf("ERROR at encrypter -- at MTA_CRYPT");
        return NULL;
    }
    printf("New password generated: %s - key: %s - encrypted password: %s\n", original_password, key,
           encrypted_password.password);
    return 0;
}

int update_decrypters(PASSWORD_MSG_T message, int message_size) {
	for (int i = 1; i < MAX_CONNECTIONS + 1; i++) {
		if (mqs[i] != -1) {
			mq_send(mqs[i], (char*) message, MQ_MAX_MESSAGE_SIZE, NEW_PASSWORD_PRIORITY);
		}
	}
}

int generate_key(char *key, int key_length) {
	return MTA_get_rand_data(key, key_length);
}

int generate_password(char *password, int length){
    int i = 0;
    while (i < length) {
    	char ch;
    	while(!isprint(ch = MTA_get_rand_char())){}
    	password[i] = ch;
    	i++; 
    }
    return 0;
 }

 int find_free_mq(mqd_t *mqs, int length) {
 	for (int i = 0; i < length; i++) {
 		if (mqs[i] == -1) {
 			return i;
 		}
 	}
 	return -1
 }
 
