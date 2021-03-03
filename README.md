# mta-crypt IPC
To start the program write `make` and then sudo ./launcher.out [# clients] -l [# password] [-r optional] [# rounds]
for example: sudo ./launcher.out 4 -l 8 -r 3

To run server : sudo ./server.out -l [# password] 
For example: sudo ./server.out -l 8

To run decrypter: sudo ./decrypter [id]
For example: sudo ./decrypter.out 1
 
* We Assume mta_crypt_lib installed 
* We support password length
* After Dercyptor comes to printable password he continues to decrypt until gets acknowledge 
  from the server that the password decrypted , then waits until recieving new password.
 
 
Nadav Suliman 200041739
Shimon Arshavsky 312119126
