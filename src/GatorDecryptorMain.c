// GatorDecryptor.c 
// Author: Rahul Bobhate

#include <stdio.h>
#include <string.h>
#include <gcrypt.h>
#include <unistd.h>
#include <GatorDecryptor.h>
#include <GatorNetwork.h>

#define BLOCK_LENGTH 8
#define ENC_EXTENSION ".uf"
#define PASSWORD_MAX_LENGTH 1024

/*
 * This is the main method of the gatordec utility.
 * There is no need of passing  arguments if not used 
 * in local mode.
 * The following is a sample input for local mode.
 *  
 *    ./gatordec -l hello.txt.uf 
 *
 * Similarly, the gatordec utility can also be used for
 * network communication. The following is sample arguments
 * for network mode.
 *
 *    ./gatordec 12345
 *
 */
int main(int argc, char *argv[])
{
    int i, mode, port;
    char *inputFileName, *outputFileName, line[BLOCK_LENGTH + 1], password[PASSWORD_MAX_LENGTH];
    FILE *inputFile, *outputFile;

    fputs("Gator Decryption utility started.\n", stdout);

    if(argc > 1 && (strcmp(argv[1], "-l") == 0))
    {
        inputFileName = argv[2];
        fprintf(stdout, "Input file specified: %s\n", inputFileName); 
    }
    else
    {
        port = atoi(argv[1]);
        receive(inputFileName, port);
    }

    outputFileName = malloc(strlen(inputFileName) - strlen(ENC_EXTENSION) + 1);
    strncpy(outputFileName, inputFileName, strlen(inputFileName) - strlen(ENC_EXTENSION) +2);
    fprintf(stdout, "Output file name: %s\n", outputFileName);

    inputFile = fopen(inputFileName, "r");
    if(!inputFile)
    {
        fprintf(stdout, "Unable to open file %s. Program will now exit.\n", inputFileName);
        return 0;
    }

    outputFile = fopen(outputFileName, "w");
    if(!outputFile)
    {
        fprintf(stdout, "Cannot create outputFile %s. Program will now exit.\n", outputFileName);
        return 0; 
    }

    if((access(outputFileName, F_OK) != -1)) 
    {
        fprintf(stderr, "The file %s already exists.\n Exiting now.\n", outputFileName);
        return 0;
    }

    fprintf(stdout, "Password: ");
    fgets(password, sizeof(password), stdin);
    fprintf(stdout, "\n");

    initializeGcrypt();

    decrypt(inputFile, outputFile, password); 

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
