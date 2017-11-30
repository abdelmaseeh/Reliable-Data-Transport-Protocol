#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <bits/stdc++.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <sys/time.h>

#define ECHOMAX 255

using namespace std;

void read_input();


/* for sockaddr_in and inet_addr() */
/* for atoi() */
/* for memset() */
/* for close() */
/* Longest string to echo */
/* External error handling function */
int main(int argc, char *argv[])
{
    int sock; /* Socket descriptor */
    struct sockaddr_in server_address; /* Echo server address */
    struct sockaddr_in client_address; /* Source address of echo */
    unsigned short server_port;
    unsigned int fromSize;
    char *servlP;
    char *sent_string;
    char recived_buffer[ECHOMAX+1];
    int sent_stringLen;
    int respStringLen;

    if ((argc < 3) || (argc > 4)) /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[0]);
        exit(1);
    }
    servlP = argv[1] ;
    sent_string = argv[2] ;
    /* First arg' server IP address (dotted quad)*/
    /*  Second  arg'  string  to  echo  */
    if ((sent_stringLen = strlen(sent_string)) > ECHOMAX) /* Check input length */
        printf("Echo word too long\n");
    if (argc == 4)
        server_port = atoi(argv[3]) ; /* Use given port, if any */
    else
        server_port = 7; /* 7 is the well-known port for the echo service */
    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        printf( "socket () failed\n") ;
    /* Construct the server address structure */
    memset(&server_address, 0, sizeof(server_address)); /* Zero out structure */
    server_address.sin_family = AF_INET; /* Internet addr family */
    server_address.sin_addr.s_addr = inet_addr(servlP); /* Server IP address */
    server_address.sin_port = htons(server_port); /* Server port */
    /* Send the string to the server */
    if (sendto(sock, sent_string, sent_stringLen, 0, (struct sockaddr *)
               &server_address, sizeof(server_address)) != sent_stringLen)
        printf("sendto() sent a different number of bytes than expected\n");
    /* Recv a response */
    fromSize = sizeof(client_address) ;
    if ((respStringLen = recvfrom(sock, recived_buffer, ECHOMAX, 0, (struct sockaddr *) &client_address, &fromSize)) != sent_stringLen)
        printf("recvfrom() failed\n") ;
    if (server_address.sin_addr.s_addr != client_address.sin_addr.s_addr)
    {
        fprintf(stderr,"Error: received a packet from unknown source.\n");
        exit(1);
    }
    /* null-terminate the received data */
    recived_buffer[respStringLen] = '\0' ;
    printf("Received: %s\n", recived_buffer); /* Print the  echoed arg */
    close(sock);
    exit(0);
}
