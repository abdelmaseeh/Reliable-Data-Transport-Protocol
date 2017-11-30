#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <bits/stdc++.h>
#include <sys/ioctl.h>

#define ECHOMAX 255
#define MAX_PACKET_LENGTH 512

using namespace std;

struct packet {
 /*header*/
 	uint16_t cksum;
	uint16_t len;
	uint32_t seqno;
	/*data*/
	char data[MAX_PACKET_LENGTH];
};

struct ack_packet {
	uint16_t cksum;
	uint16_t len;
	uint32_t ackno;
};

bool dropPacket(unsigned int seed, double probability)
{
	srand(seed) ;
	return ((double) rand() / (RAND_MAX)) < probability ; 
}

vector<char> readFile(string fileName)
{
    ifstream ifs(fileName, ios::binary|ios::ate);
    ifstream::pos_type pos = ifs.tellg();
    vector<char>  res(pos);

    ifs.seekg(0, ios::beg);
    ifs.read(&res[0], pos);
    return res;
}

vector<packet> getFilePackets(string fileName)
{
	vector<packet> packets ;
	vector<char> file_data = readFile(fileName) ;
	int index = 0 ;
	uint32_t i = 0  ;
	vector<char>::iterator it = file_data.begin();

	for (; i < file_data.size()/MAX_PACKET_LENGTH + 1; i++)
	{
		uint16_t len;

		if (index + MAX_PACKET_LENGTH < file_data.size())
		{
				len = MAX_PACKET_LENGTH ;
		}
		else if (file_data.size() - index > 0)
		{
			len = file_data.size() - index ;
		}
		else
		{
			break ;
		}

		// assigning data
		char data[MAX_PACKET_LENGTH];
		//copy(it, it+len, data)  ;

		// assinging data and header to packet to be send
		packet p  ;
		p.cksum = 0 ;  // checksum
		p.len = len ;  // length
		p.seqno = i ; // sequence number
		copy(it, it+len, p.data) ; // data

		packets.push_back(p) ;
		index += len ;
		it += len ;
	}

	return packets ;
}

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
		exit(1);
	}

	echoServPort = atoi(argv[1]);
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		printf("socket() failed");

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	echoServAddr.sin_port = htons(echoServPort);

	if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		printf("bind() failed");


	for (;;) /* Run forever */
	{
		cliAddrLen = sizeof(echoClntAddr);
		if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
			printf("recvfrom() failed") ;

		printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr)) ;
		if (!fork())
		{
			if (sendto(sock, echoBuffer, recvMsgSize, 0, (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != recvMsgSize)
			{
				printf("sendto() sent a different number of bytes than expected");
			}
		}
	}
}
