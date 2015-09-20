//GatorEncryptor.c
//Author: Rahul Bobhate

#include <stdio.h>
#include <gcrypt.h>

// Needed to compare the current version of the libgcrypt library.
#define GCRYPT_VERSION "1.6.3"

#define SALT_STRING 0x80
#define SALT_SIZE 64 
#define NUMBER_OF_KEYGEN_ITERATIONS 20000 
#define KEY_SIZE 32 

#define INITIALIZATION_VECTOR 0x0
#define CIPHER_IV_SIZE 16
#define MAC_IV_SIZE 32

/*
 * This method initializes the libcrypt library
 * and checks for the current version and its
 * compatibility.
 *
 */
void initializeGcrypt(void)
{
    if(!gcry_check_version (GCRYPT_VERSION))
    {
        fprintf(stderr, "libgcrypt version mismatch\n");
        fprintf(stderr, "Program will now exit.\n");
        exit (2);
    }
}

/*
 * This method generates the key which will be used
 * for the encryption and decryption of the files.
 * It uses the PBKDF2 algorithm for key generation
 * and uses salt, number of iterations and key length
 * to generate the key.
 */
void generateKey(char *password, char *key)
{
    gpg_error_t errorCode; 
    char salt[SALT_SIZE] = {SALT_STRING};

    errorCode = gcry_kdf_derive(password, sizeof(password), GCRY_KDF_PBKDF2, GCRY_MD_SHA256, salt, sizeof(salt), NUMBER_OF_KEYGEN_ITERATIONS, KEY_SIZE, key); 

    if(errorCode)
    {
        fprintf(stderr, "Error occurred while generating key:\n");
        fprintf(stderr, "Error is %s %s\n", gcry_strsource(errorCode), gcry_strerror(errorCode));
    }
}

/*
 * This method creates the cipher handle by providing
 * other information such as the IV, key
 * and length of the key.
 */
gcry_cipher_hd_t createHandle(char *key)
{
    gcry_cipher_hd_t handle;
    gpg_error_t errorCode;
    char iv[CIPHER_IV_SIZE] = {INITIALIZATION_VECTOR};

    errorCode = gcry_cipher_open(&handle, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC, 0);

    if(errorCode)
    {
        fprintf(stderr, "An error occurred while opening the cipher handle.\n");
        fprintf(stderr, "Error is %s %s\n", gcry_strsource(errorCode), gcry_strerror(errorCode));
        return NULL;
    }

    errorCode = gcry_cipher_setkey(handle, key, KEY_SIZE);

    if(errorCode)
    {
        fprintf(stderr, "An error occurred while setting the key.\n");
        fprintf(stderr, "Error is %s %s\n", gcry_strsource(errorCode), gcry_strerror(errorCode));
        return NULL;
    }

    errorCode = gcry_cipher_setiv(handle, iv, sizeof(iv));

    if(errorCode)
    {
        fprintf(stderr, "An error occurred while setting the IV.\n");
        fprintf(stderr, "Error is %s %s\n", gcry_strsource(errorCode), gcry_strerror(errorCode));
        return NULL;
    }

    return handle;
}

/*
 * This method creates the MAC handle by providing
 * other information such as the IV, number of iterations
 * and length of the key.
 */
gcry_mac_hd_t createMacHandle(char *key)
{
    gcry_mac_hd_t handle;
    gpg_error_t errorCode;
    char iv[MAC_IV_SIZE] = {INITIALIZATION_VECTOR};

    errorCode = gcry_mac_open(&handle, GCRY_MAC_HMAC_SHA256, 0, NULL);

    if(errorCode)
    {
        fprintf(stderr, "An error occurrred while opening the MAC handle\n");
        fprintf(stderr, "Error: %s %s\n", gcry_strsource(errorCode), gcry_strerror(errorCode));
        return NULL;
    }

    errorCode = gcry_mac_setkey(handle, key, KEY_SIZE);

    if(errorCode)
    {
        fprintf(stderr, "An error occurred while setting the key for MAC.\n");
        fprintf(stderr, "Error is %s %s\n", gcry_strsource(errorCode), gcry_strerror(errorCode));
        return NULL;
    }

    return handle;
}
