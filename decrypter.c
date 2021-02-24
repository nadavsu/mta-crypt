#include "decrypter.h"


int ID;
char decrypter_name[20];
    
int main(int argc, char *argv[]){
   if(argc < 2)
      printf("ERROR: expecting for id number");
      
   ID = atoi(argv[1]);
   
   int rounds = UINT_MAX;
   if(argc > 3)
       rounds = atoi(argv[3]);
   
   mqd_t mq_server = mq_open(SERVER_MQ_NAME, O_WRONLY);
   
   char mq_name[MQ_NAME_LEN];
   sprintf(mq_name, "mq_%d",ID);
   sprintf(decrypter_name, "[CLIENT: %d]",ID);
   
   struct mq_attr attr = {0};
   attr.mq_maxmsg = MQ_MAX_MESSAGE;
   attr.mq_msgsize = sizeof(PASSWORD_MSG_T) + MAX_PASS_LEN;
   
   mq_unlink(mq_name);
   mqd_t mq_dec = mq_open(mq_name, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG, &attr); 
   
   
   send_conn_req(mq_name, mq_server);
   
   PASSWORD_MSG_T encrypt_pass;
   
   get_msg(mq_dec, &encrypt_pass);
   printf("%s recieved first encrypted password (%s)", decrypter_name, encrypt_pass.pass);  
  
   struct mq_attr mqAttr = {0};
   
   int iter = 0;
   
   while(rounds > 0){
      mq_getattr(mq_dec, &mqAttr);
      
      if(mqAttr.mq_curmsgs){
          get_msg(mq_dec, &encrypt_pass);
          printf("%s recieved encrypted password (%s)", decrypter_name, encrypt_pass.pass);  
          iter = 0;
          
      }
      
      iter++;
      decrypt_pass(mq_dec, &encrypt_pass, &rounds, iter);
    
        }
    
   
   close_conn(mq_server, mq_name);         

    
}


void close_conn(mqd_t mq_server, char mq_name[]){
   CLOSE_CONNECTION_MSG_T close_conn;
   close_conn.type = CLOSE_CONNECTION;
   memcpy(close_conn.mq_name, mq_name, sizeof(char) * strlen(mq_name));
   
   printf("%s sending close connection", decrypter_name);
   
   if(mq_send(mq_server, (char*)&close_conn, sizeof(close_conn), 0) < 0){
      //message_stamp(decrypter_name, MESSAGE_TYPE_ERR);
      printf("ERROR [CLIENT: %d] -- at sending close connection", ID);
      exit(-1);
   }
   
   mq_close(mq_server);
}

void send_conn_req(char mq_name[], mqd_t mq_server){
  
  NEW_CONNECTION_MSG_T new_conn;
  new_conn.type = NEW_CONNECTION;
  memcpy(new_conn.mq_name, mq_name, sizeof(char) * strlen(mq_name));
  
  printf("%s sending connection request on %s" , decrypter_name, mq_name);
  
  int is_send = mq_send(mq_server, (char*)&new_conn, sizeof(new_conn), 0);
  
  if(is_send < 0){
     printf("ERROR [CLIENT: %d] -- at sending new connection request", ID);
      exit(-1);
    }
    
  printf("%s sent connection request on %s" , decrypter_name, mq_name);  
   
    
}


void send_decrypt_pass(mqd_t mq_server, char pass[]){

  PASSWORD_MSG_T *decrypt_pass = malloc(sizeof(PASSWORD_MSG_T) + sizeof(char) * strlen(pass));
  decrypt_pass->pass_len = strlen(pass);
  memcpy(decrypt_pass->pass, pass, sizeof(char) * strlen(pass));
  decrypt_pass->type = NEW_PASSWORD;
   
  int is_send = mq_send(mq_server, (char*)decrypt_pass, sizeof(decrypt_pass), 0);
  
  if(is_send < 0){
     printf("ERROR [CLIENT: %d] -- at sending decrypted password", ID);
     exit(-1);
     }
  
}


void get_msg(mqd_t mq_decrypt,PASSWORD_MSG_T *pass_msg){
          
    if(mq_receive(mq_decrypt, (char*)pass_msg, MQ_MAX_MESSAGE_SIZE, NULL) < 0){
       printf("ERROR [CLIENT: %d] -- at sending decrypted password", ID);
            exit(-1);
    }
}

    

int decrypt_pass(mqd_t mq_decrypter, PASSWORD_MSG_T *encrypt_pass,int* rounds, int iterations ){

   int key_len = encrypt_pass->pass_len/8;
   
   char key[MAX_PASS_LEN+1] = {0};
   char decrypt_pass[MAX_PASS_LEN+1] = {0};
    
   srand(DECRYPTER_RAND_SEED);
   MTA_get_rand_data(key,key_len);
   MTA_CRYPT_RET_STATUS status = MTA_decrypt(key, key_len, encrypt_pass->pass, encrypt_pass->pass_len,   decrypt_pass, &(encrypt_pass->pass_len));

     if(status != MTA_CRYPT_RET_OK){
            printf("ERROR [CLIENT: %d] -- at MTA_CRYPT", ID);
            exit(-1);
        }

        if(printable_pass(decrypt_pass, encrypt_pass->pass_len)) {
            send_decrypt_pass(mq_decrypter, decrypt_pass);
            *rounds--;
                 
            printf("%s After decryption password - %s key guessed - %s Sending to server after %d iterations\n", decrypter_name, decrypt_pass, key, iterations);
                }
   

  }





