#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void DieWithError(char *errorMessage);

int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in echoServAddr; 
	struct sockaddr_in echoClntAddr; 
	unsigned int cliAddrLen;
	char echoBuffer[ECHOMAX];
	unsigned short echoServPort;
	int recvMsgSize;
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <UDP SERVER PORT>\n", argv[0]) ;
		exit(l);
	}
	[]

	echoServPort = atoi(argv[l]) ; 
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWitb_Error("socket() failed");

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = htonI(INADDR_ANY);
	echoServAddr.sin_port = htons(echoServPort);

	if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < O)
		DieWithError("bind() failed");


	for (;;) /* Run forever */
	{
		cliAddrLen = sizeof(echoClntAddr);
		if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
			DieWithError("recvfrom() failed") ;

		printf("Handling client %s\n", inet_ntoa(echoClntAddr, sin_addr)) ;

		if (sendto(sock, echoBuffer, recvMsgSize, O, (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != recvMsgSize)
			DieWithError("sendto() sent a different number of bytes than expected");

	}
	/* NOT REACHED */
}

