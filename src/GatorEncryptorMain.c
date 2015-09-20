// GatorEncryptorMain.c
// Author: Rahul Bobhate

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <gcrypt.h>
#include <GatorEncryptor.h>
#include <GatorNetwork.h>

#define BLOCK_LENGTH 8
#define ENC_EXTENSION ".uf"
#define PASSWORD_MAX_LENGTH 1024
#define ADDRESS_LENGTH 256

/*
 * This is the main method of the gatorenc utility.
 * The arguments passed to the utility should contain
 * at least one input file name.
 * The following is a sample input for local mode.
 *  
 *    ./gatorenc hello.txt -l
 *
 * Similarly, the gatorenc utility can also be used for
 * network communication. The following is sample arguments
 * for network mode.
 *
 *    ./gatorenc hello.txt -d 192.0.0.12:12345
 *
 */ 
int main(int argc, char *argv[])
{
    int i, mode = 0, port;
    char *inputFileName, *outputFileName, line[BLOCK_LENGTH + 1], password[PASSWORD_MAX_LENGTH], address[ADDRESS_LENGTH] = {0x0}, *portString;
    FILE *inputFile, *outputFile;

    fprintf(stdout, "Gator Encryption utility started.\n");

    if(argc == 1)
    {
        // We need atleast the name of the file to start
        // If the input file is not specified. Stop the program. 
        fprintf(stdout, "No argument passed. The program requires an input file to proceed.\n");
        fprintf(stdout, "Exiting.\n");
        return 0; 
    }
    else if(argc > 2 && (argc == 3 && strcmp(argv[2], "-l") == 0))
    {
        mode = 0;
    }
    else if(argc != 2 && strcmp(argv[2], "-d") == 0)
    {
        mode = 1;
        strncpy(address, argv[3], (int)strcspn(argv[3], ":"));        
        portString = strtok(argv[3], ":"); 
        portString = strtok(NULL, "\0");
        port = atoi(portString);
    }
 
    inputFileName = argv[1];
    fprintf(stdout, "Input file specified: %s\n", inputFileName); 

    outputFileName = malloc(strlen(inputFileName) + strlen(ENC_EXTENSION) + 1);
    strcpy(outputFileName, inputFileName);
    strcat(outputFileName, ENC_EXTENSION);
    fprintf(stdout, "Output file name: %s\n", outputFileName);

    if((access(outputFileName, F_OK) != -1))
    {
        fprintf(stderr, "The file %s already exists.\n Exiting now.\n", outputFileName);
        return 0;
    }

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

    fprintf(stdout, "Password: ");
    fgets(password, sizeof(password), stdin);
    printf("\n");

    encrypt(inputFile, outputFile, password);

    fclose(inputFile);
    fclose(outputFile);

    if(mode)
    {
        transmit(outputFileName, address, port);
    }

    return 0;
}
