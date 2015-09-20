//GatorEncryptor.c
//Author: Rahul Bobhate

#include <stdio.h>
#include <gcrypt.h>
#include <GatorCryptInitializer.h>

#define BLOCK_SIZE 1024 
#define KEY_SIZE 32 

/*
 * This method reads the inputFile,
 * and using the cipher handle, encrypts
 * the data read from the input file 
 * and writes it to the output file.
 */
void decrypt(FILE *inputFile, FILE *outputFile, char *password)
{
    char key[KEY_SIZE];
    gcry_cipher_hd_t handle;
    gpg_error_t errorCode;
    char buffer[BLOCK_SIZE] = {0x0};     
    int sizeRead = 0;

    generateKey(password, key);

    handle = createHandle(key);

    while(fread(buffer, sizeof(char), BLOCK_SIZE, inputFile))
    {
        errorCode = gcry_cipher_decrypt(handle, buffer, sizeof(buffer), NULL, 0);

        if(errorCode != 0)
        {
            printf("An error occurred while decrypting the cipherText.\n");
            fprintf (stderr, "Failure: %s/%s\n",
               gcry_strsource (errorCode),
               gcry_strerror (errorCode));
        }

        sizeRead = strlen(buffer);

        if(sizeRead < BLOCK_SIZE)
        {
            fwrite(buffer, sizeof(char), sizeRead, outputFile);
        }
        else
        {
            fwrite(buffer, sizeof(char), BLOCK_SIZE,  outputFile);
        }

        memset(buffer, 0x0, sizeof(buffer));
    }    
}
