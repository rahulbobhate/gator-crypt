//KeyGenerator.h
//Author: Rahul Bobhate

void initializeGcrypt(void);

gcry_cipher_hd_t createHandle(char *key);

gcry_mac_hd_t createMacHandle(char *key);

char* generateKey(char *password, char *key);
