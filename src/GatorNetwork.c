// GatorNetwork.c
// Author: Rahul Bobhate

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#define BUFFER_SIZE 1024 
#define ACK_MSG "ACK"
#define ACK_SIZE 4 

/*
 * This function transmits the encrypted file
 * from source to destination mentioned
 * in the address and the port number.
 */
void transmit(char *inputFileName, char *address, int port)
{
    int sockfd, sizeRead, sizeWritten, fileSize;
    struct sockaddr_in serverAddress;
    struct hostent *server;
    char *buffer, *inputFileNameHolder, ackBuffer[ACK_SIZE], size[BUFFER_SIZE];
    FILE *file;
 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0) 
    {
        fprintf(stderr, "Error while opening socket\n");
        return;
    } 
    
    server = gethostbyname(address);

    if (server == NULL) 
    {
        fprintf(stderr,"Error while finding the server\n");
        return;
    }
    
    bzero((char *) &serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serverAddress.sin_addr.s_addr, server->h_length);
    serverAddress.sin_port = htons(port);
   
    if (connect(sockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)  
    {
        fprintf(stderr, "Error while connecting to the server.\n");
        return;
    }
   
    inputFileNameHolder = malloc(strlen(inputFileName) + 1);
    strcpy(inputFileNameHolder, inputFileName);
    strcat(inputFileNameHolder, "\0");

    sizeWritten = write(sockfd, inputFileNameHolder, strlen(inputFileNameHolder) + 1);

    if (sizeWritten < 0)
    {
        fprintf(stderr, "Erroor writing the filename to socket\n");
        return;
    }

    file = fopen(inputFileName, "r");
    fseek(file, 0L, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0L, SEEK_SET);
    sprintf(size, "%d", fileSize);

    sizeWritten = write(sockfd, size, strlen(size) + 1); 

    if(sizeWritten < 0)
    {
        fprintf(stderr, "Error occurred while writing the size of file for transmission.\n");
        return;
    }

    buffer = malloc(fileSize);

    bzero(buffer, fileSize);

    sizeRead = fread(buffer, sizeof(char), fileSize, file);
    if(!sizeRead)
    {
        fprintf(stderr, "Error occurred while reading from the file for transmission.\n");
        return;
    }

    sizeWritten = write(sockfd, buffer, sizeRead);

    if(sizeWritten < 0)
    {
        fprintf(stderr, "Error occurred while writing the size of file for transmission.\n");
        return;
    }

    close(sockfd);
    fclose(file);
    free(inputFileNameHolder);
}

/*
 * This method receives the file by
 * listening to a port, accepting incoming
 * connections and reading from the sockets.
 */
void receive(char *inputFileName, int port)
{
    int sockfd, newsockfd, clientLength, sizeRead;
    char buffer[BUFFER_SIZE], *fileSize, *newBuffer;
    struct sockaddr_in serverAddress, clientAddress;
    FILE *file;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0)
    {
        fprintf(stderr, "Error while opeing socket.\n");
        fprintf(stderr, "Program will now exit.\n");
        return; 
    }

    bzero((char *) &serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if(bind(sockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) 
    {
        fprintf(stderr, "Error while binding to the address.\n");
        fprintf(stderr, "Program will now exit.\n");
        return; 
    }

    listen(sockfd, 1);
    clientLength = sizeof(clientAddress);
    newsockfd = accept(sockfd, (struct sockaddr *) &clientAddress, &clientLength);

    if(newsockfd < 0) 
    {
        fprintf(stderr, "Error occurred while accepting connections.\n");
        fprintf(stderr, "Program will now exit.\n");
        return; 
    }

    bzero(buffer, BUFFER_SIZE);

    sizeRead = read(newsockfd, buffer, BUFFER_SIZE);
    inputFileName = malloc(sizeRead);
    strcpy(inputFileName, buffer);
    inputFileName = strtok(inputFileName, "/");
    inputFileName = strtok(NULL, "\0");

    fprintf(stdout, "Name of input file to be received: %s\n", inputFileName);

    sizeRead = read(newsockfd, buffer, BUFFER_SIZE);
    fileSize = malloc(sizeRead);
    strcpy(fileSize, buffer);
    fileSize = strtok(fileSize, "\0");
 
    fprintf(stdout, "Size of the input file is %s\n", fileSize);

    file = fopen(inputFileName, "w");

    while((sizeRead = read(newsockfd, buffer, BUFFER_SIZE)) > 0);
    {
        if(sizeRead < 0)
        {
             fprintf(stderr, "Error while reading from the buffer.\n");
             return;
        }

        fwrite(buffer, sizeof(char), sizeof(buffer), file);
    }
    
    close(newsockfd);
    close(sockfd); 
    fclose(file);

    free(inputFileName);
    free(fileSize);
    free(newBuffer);
}
