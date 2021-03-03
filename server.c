#include "server.h"

CONNECTION mqs[MAX_CONNECTIONS];
char *current_password, *key, *current_password_encrypted;
int pass_length, key_length;

int main(int argc, char *argv[]) {
	
	if (argc != 3){
	    msg_stamp(SERVER_NAME, ERROR);
	    printf("too few arguments -l [# password]\n");
	    exit(-1);
	}

          
       pass_length = atoi(argv[2]);
       key_length = pass_length / 8;
        

     for(int i=0; i< MAX_CONNECTIONS; i++)
           mqs[i].mq = -1;
     
    current_password = malloc(pass_length + 1);
    current_password_encrypted = malloc(pass_length + 1);
    key = malloc(key_length + 1);
    
    if (!current_password || !current_password_encrypted || !key) {
    	exit(EXIT_FAILURE);
    }
    
	
	struct mq_attr attr = {0};
	attr.mq_maxmsg = MQ_MAX_MESSAGE;
	attr.mq_msgsize = MQ_MAX_MESSAGE_SIZE;


	mq_unlink(SERVER_MQ_NAME);
	mqs[SERVER_MQ_IND].mq = mq_open(SERVER_MQ_NAME, O_CREAT | O_RDONLY, S_IRWXU | S_IRWXG, &attr);
	
	
	if (mqs[SERVER_MQ_IND].mq == -1) {
	        msg_stamp(SERVER_NAME, ERROR);
		printf("at mq_open errno:%d\n", errno);
		exit(errno);
	}
	
	
	GEN_MSG_T *message = malloc(MQ_MAX_MESSAGE_SIZE);
	if (!message) {
		exit(EXIT_FAILURE);
	}


	generate_password(current_password, pass_length);
	generate_key(key, key_length);
	encrypt_password(current_password, pass_length, key, key_length);
	
	for (;;) {
	    
		int msg_len = mq_receive(mqs[SERVER_MQ_IND].mq, (char*)message, MQ_MAX_MESSAGE_SIZE, 0);
		
		if (msg_len == -1) {
			msg_stamp(SERVER_NAME, ERROR);
		        printf("at recieve message from %s errno: %d\n", SERVER_MQ_NAME, errno);
			exit(errno);
		}

		switch (message->type) {
			case NEW_CONNECTION:
				create_connection((NEW_CONNECTION_MSG_T *) message);
				break;
			
			case CLOSE_CONNECTION: 
				close_connection((CLOSE_CONNECTION_MSG_T*)message);
				break;
			
			case NEW_PASSWORD:
				password_guess_handler((PASSWORD_MSG_T *) message);
				break;
			
			default:
				fprintf(stderr, "Unknown message type!\n");
				exit(-1);
				
		}

	
   }

  return 1;
}



int create_connection(NEW_CONNECTION_MSG_T *message) {
	
       int i = find_free_mq(MAX_CONNECTIONS);
       	if (i < 0) {
		return MAX_CONNECTIONS_EC;
	        } 
        
	mqs[i].mq = mq_open(message->mq_name, O_WRONLY);
	if (mqs[i].mq == -1) {
		msg_stamp(SERVER_NAME, ERROR);
		printf("at mq_open");
		exit(-1);
	}
	
	mqs[i].id = message->id;
	
	msg_stamp(SERVER_NAME, INFO);
	printf("Recieved connection request from decryptor id: %d on queue: %s\n", message->id, message->mq_name);

	PASSWORD_MSG_T *password_message = malloc(sizeof(PASSWORD_MSG_T) + pass_length + 1);
	password_message->type = NEW_PASSWORD;
	password_message->pass_len = pass_length;
	memcpy(password_message->pass, current_password_encrypted, pass_length+1);
	
	mq_send(mqs[i].mq, (char*)password_message, MQ_MAX_MESSAGE_SIZE, 0);
	
	free(password_message);
	
	return 0;
}

void close_connection(CLOSE_CONNECTION_MSG_T *message) {
             
       for (int i=1; i< MAX_CONNECTIONS; i++)
            if(mqs[i].id == message->id){
                 mq_close(mqs[i].mq);
                 mq_unlink(message->mq_name);
                 mqs[i].mq = -1;
                 mqs[i].id = -1;
                 
                 }
       
       msg_stamp(SERVER_NAME, INFO);
       
       printf("Connection closed on: %s\n",message->mq_name); 
       
}



void send_confirmation(PASSWORD_MSG_T * msg, int is_correct){
       int i;
       
       for (i=1; i< MAX_CONNECTIONS; i++)
            if(mqs[i].id == msg->id)
                 break;
       
       PASSWORD_STATE_M *pass_state = malloc(sizeof(PASSWORD_STATE_M));
       pass_state->type = PASSWORD_STATE;
       pass_state->is_decrypted = is_correct;
       pass_state->ack = msg->pass_num;
       
       mq_send(mqs[i].mq, (char*) pass_state, MQ_MAX_MESSAGE_SIZE, 0);
       
       free(pass_state);
   
 }

int password_guess_handler(PASSWORD_MSG_T * message) {

	if (strcmp(current_password, message->pass) == 0) {
	        msg_stamp(SERVER_NAME, OK);
		printf("Password decrypted successfully by: %d, received: (%s) is: (%s)\n",message->id, message->pass, current_password);
		
		send_confirmation(message, 1);
		
		generate_password(current_password, pass_length);
		generate_key(key, key_length);
		encrypt_password(current_password, pass_length, key, key_length);

		PASSWORD_MSG_T  *password_message = malloc(sizeof(PASSWORD_MSG_T) + pass_length + 1);
		password_message->type = NEW_PASSWORD;
		password_message->pass_len = pass_length;
		memcpy(password_message->pass, current_password_encrypted,pass_length+1);
		
		update_decrypters(password_message);
		free(password_message);

		return 1;
	} else {
	        msg_stamp(SERVER_NAME, ERROR);
		printf("Wrong password by: %d, received: (%s) is - (%s)\n",message->id, message->pass, current_password);
		send_confirmation(message, 0);
		
		return 0;
	}
}

int encrypt_password(char *password, int password_length, char *key, int key_length) { 
    srand(time(NULL));
    MTA_CRYPT_RET_STATUS status = MTA_encrypt(key, key_length, current_password, password_length,
                                              current_password_encrypted,
                                              &password_length);
                                              
    
    if (status != MTA_CRYPT_RET_OK) {
        msg_stamp(SERVER_NAME, ERROR);
        printf("at MTA_CRYPT");
        return -1;
    }
    msg_stamp(SERVER_NAME, INFO);
    printf("New password generated: (%s)  key: (%s) - encrypted password: (%s)\n", current_password, key,
           current_password_encrypted);
    return 0;
}

int update_decrypters(PASSWORD_MSG_T* message) {
	for (int i = 1; i < MAX_CONNECTIONS; i++) {
		if (mqs[i].mq != -1) {
			mq_send(mqs[i].mq, (char*) message, MQ_MAX_MESSAGE_SIZE, 0);
		}
	}
}

void generate_key(char *key, int key_length) {
	 MTA_get_rand_data(key, key_length);
}

int generate_password(char *password, int length){
    int i = 0;
    while (i < length) {
    	char ch;
    	while(!isprint(ch = MTA_get_rand_char())){}
    	password[i++] = ch;
    	
    }
    return 0;
 }

int find_free_mq(int length) {
 	for (int i = 1; i < length; i++) {
 		if (mqs[i].mq == -1) {
 			return i;
 		}
 	}
 	return -1;
 }
 
 
 void msg_stamp(char* author, char *message_type) {
	printf("%-10u %-14s %-9s ", getpid(), author, message_type);
}

 
 
 
 
