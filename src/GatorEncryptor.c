//GatorEncryptor.c
//Author: Rahul Bobhate

#include <stdio.h>
#include <gcrypt.h>
#include <GatorCryptInitializer.h>

#define BLOCK_SIZE 1024 
#define KEY_SIZE 32
#define MAC_SIZE 32

/*
 * This function reads the input file,
 * and using the generated key and the
 * cipher handle, encrypts the contents
 * of the file and stores it to an 
 * encrypted output file.
 */
void encrypt(FILE *inputFile, FILE *outputFile, char *password)
{
    char key[KEY_SIZE];
    gcry_cipher_hd_t handle;
    gcry_mac_hd_t plainTextMacHandle, cipherTextMacHandle; 
    gpg_error_t errorCode;
    char buffer[BLOCK_SIZE] = {0x0}, plainTextMac[MAC_SIZE] = {0x0}, cipherTextMac[MAC_SIZE];            
    size_t plainTextMacSize, cipherTextMacSize;

    initializeGcrypt();
    generateKey(password, key);

    handle = createHandle(key);

    plainTextMacHandle = createMacHandle(key);
    cipherTextMacHandle = createMacHandle(key);

    while(fread(buffer, sizeof(char), BLOCK_SIZE, inputFile))
    {
        errorCode = gcry_cipher_encrypt(handle, buffer, sizeof(buffer), NULL, 0);

        if(errorCode)
        {
            fprintf(stderr, "An error occurred while encrypting the plainText.\n");
            fprintf(stderr, "Error: %s %s\n", gcry_strsource(errorCode), gcry_strerror(errorCode));
        }

        fwrite(buffer, sizeof(char), sizeof(buffer), outputFile);
        memset(buffer, 0x0, sizeof(buffer));
    }
}

