# mta-crypt
A bruteforce password decryptor for the RC2_ECB encryption algorithm.

## Usage
To start the program write `make` and then `sudo ./launcher.out [# clients] -l [# password] [-r optional] [# rounds]`
for example: `sudo ./launcher.out 4 -l 8 -r 3`

To run server : `sudo ./server.out -l [# password]`
For example: `sudo ./server.out -l 8`

To run decrypter: `sudo ./decrypter [id]`
For example: `sudo ./decrypter.out 1`
 
 
## Dependencies
* [mta_crypt_lib](https://github.com/gavrielk/LinuxCourseCodePub/tree/master/mta_crypt_lib) should be installed

## More info
* We support password length
* After decryptor finds a printable password it continues to decrypt until gets an acknowledgement
  from the server that the password decrypted , then waits until recieving new password.
 
 
## Contributors
- Nadav Suliman
- Shimon Arshavsky
