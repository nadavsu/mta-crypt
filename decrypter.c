#include "decrypter.h"


int ID, key_len, pass_len, pass_num;
char decrypter_name[20], *dec_pass, *key;
mqd_t mq_server, mq_dec;
    
int main(int argc, char *argv[]){
   if(argc < 2){
      printf("[ERROR]: expecting for id number\n");
      exit(-1);
      }
      
   ID = atoi(argv[1]);
   
   int rounds = INT_MAX;
   if(argc > 3)
       rounds = atoi(argv[3]);
       
   
   char mq_name[MQ_NAME_LEN];
   sprintf(mq_name, "/mq_%d",ID);
   sprintf(decrypter_name, "[CLIENT #%d]",ID);
   
   mq_server = mq_open(SERVER_MQ_NAME, O_WRONLY);
   
    if(mq_server == -1){
        msg_stamp(decrypter_name, ERROR);
        printf("at opening %s errno:%d\n", SERVER_MQ_NAME, errno);
        exit(-1);
   }
   
   struct mq_attr attr = {0};
   attr.mq_maxmsg = MQ_MAX_MESSAGE;
   attr.mq_msgsize = MQ_MAX_MESSAGE_SIZE;
   
   mq_unlink(mq_name);
   mq_dec = mq_open(mq_name, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG, &attr); 
   
   
    if(mq_dec == -1){
        msg_stamp(decrypter_name, ERROR);
        printf("at creating: %s\n", mq_name);
        exit(-1);
   }
   
   
   send_conn_req(mq_name);
   
   
   PASSWORD_MSG_T *encrypt_pass = malloc(MQ_MAX_MESSAGE_SIZE);
   GEN_MSG_T *msg = malloc(MQ_MAX_MESSAGE_SIZE);
   
   get_msg((GEN_MSG_T *)encrypt_pass);
   msg_stamp(decrypter_name, INFO);
   printf("recieved first encrypted password (%s)\n", encrypt_pass->pass); 
   
   int iter = 0;
   pass_len = encrypt_pass->pass_len;
   key_len = pass_len / 8;
   
   dec_pass = malloc(pass_len+1);
   key = malloc(key_len + 1);
  
   struct mq_attr mqAttr = {0};
   int is_decrypted = 0;
   pass_num = 0;
   
   while(rounds > 0){  
      mq_getattr(mq_dec, &mqAttr);
      
      if(mqAttr.mq_curmsgs > 0){
          get_msg(msg);
          
          switch(msg->type){
          
		  case NEW_PASSWORD: memcpy((char*)encrypt_pass, (char*)msg, MQ_MAX_MESSAGE_SIZE);  
		  			msg_stamp(decrypter_name, INFO);
		  			printf("recieved encrypted password (%s)\n",encrypt_pass->pass);  
          				iter = 0;
          				pass_num++;
          				is_decrypted = 0;
		  			break; 
		 
		  case PASSWORD_STATE: if (pass_num == ((PASSWORD_STATE_M*)msg)->ack) 
		    				is_decrypted = ((PASSWORD_STATE_M*)msg)->is_decrypted;
		  			break;       
          }
          
                    
      }
      

      
      
      if(!is_decrypted){
	      iter++;
	      decrypt_pass(encrypt_pass, &rounds, iter);
          }
    }
    
   
   close_connection(mq_name);
   
   free(encrypt_pass);
   free(msg);
   free(key);
   free(dec_pass);
   
   return 1;         

    
}


void close_connection(char mq_name[]){
   CLOSE_CONNECTION_MSG_T* close_conn = malloc(sizeof(CLOSE_CONNECTION_MSG_T));
   close_conn->type = CLOSE_CONNECTION;
   strcpy(close_conn->mq_name, mq_name);
   close_conn->id = ID;
   msg_stamp(decrypter_name, INFO);
   printf("sending close connection\n");
   
   int is_send = mq_send(mq_server, (char*)close_conn,MQ_MAX_MESSAGE_SIZE, 0);
   
   if(is_send < 0){
      msg_stamp(decrypter_name, ERROR);
      printf("at sending close connection %d\n",is_send);
      exit(-1);
   }
   
   
   free(close_conn);
   mq_close(mq_server);
   mq_close(mq_dec);
}

void send_conn_req(char mq_name[]){
  
  NEW_CONNECTION_MSG_T* new_conn = malloc(MQ_MAX_MESSAGE_SIZE);
  new_conn->type = NEW_CONNECTION;
  new_conn->id = ID;
  strcpy(new_conn->mq_name, mq_name);
  
  msg_stamp(decrypter_name, INFO);
  printf("sending connection request on %s\n", mq_name);
  
  int is_send = mq_send(mq_server, (char*)new_conn, MQ_MAX_MESSAGE_SIZE, 0);
  
  if(is_send < 0){
      msg_stamp(decrypter_name, ERROR);
     printf("at sending new connection request errno: %d\n", errno);
      exit(-1);
    }
    
  msg_stamp(decrypter_name, INFO);  
  printf("sent connection request on %s\n", mq_name);
    
  free(new_conn);
}


void send_decrypt_pass(){

  PASSWORD_MSG_T *decrypt_pass = malloc(MQ_MAX_MESSAGE_SIZE);
  decrypt_pass->pass_len = pass_len;
  decrypt_pass->id = ID;
  decrypt_pass->pass_num = pass_num;
  memcpy(decrypt_pass->pass, dec_pass, pass_len+1);
  decrypt_pass->type = NEW_PASSWORD;
   
  int is_send = mq_send(mq_server, (char*)decrypt_pass, MQ_MAX_MESSAGE_SIZE, 0);
  
  free(decrypt_pass);
  
  if(is_send < 0){
     printf("ERROR [CLIENT: %d] -- at sending decrypted password\n", ID);
     exit(-1);
     }
  
  
}


void get_msg(GEN_MSG_T *msg){
          
    if(mq_receive(mq_dec, (char*)msg, MQ_MAX_MESSAGE_SIZE, NULL) < 0){
       msg_stamp(decrypter_name, ERROR);
       printf("at sending decrypted password\n");
            exit(-1);
    }
    
   }

    

int decrypt_pass(PASSWORD_MSG_T *encrypt_pass,int* rounds, int iterations ){

    
   srand(time(NULL));
   MTA_get_rand_data(key,key_len);
   MTA_CRYPT_RET_STATUS status = MTA_decrypt(key, key_len, encrypt_pass->pass, pass_len, dec_pass, &pass_len);
   
     if(status != MTA_CRYPT_RET_OK){
     	    msg_stamp(decrypter_name, ERROR);
            printf("at MTA_CRYPT\n");
            exit(-1);
        }

        if(printable_pass(dec_pass, pass_len)) {
            send_decrypt_pass(mq_server);
            *rounds = *rounds-1;
                 
            msg_stamp(decrypter_name, INFO);     
            printf("After decryption password: (%s) key guessed: (%s) Sending to server after: %d iterations\n", dec_pass, key, iterations);
                }
   

  }

int printable_pass(char* decrypt_pass, int len){
    for(int i = 0;i < len; i++){
        if (isprint(decrypt_pass[i]) == 0)
            return 0;
    }
    return 1;
}

void msg_stamp(char* author, char *message_type) {
	printf("%-10u %-14s %-9s ", getpid(), author, message_type);
}




