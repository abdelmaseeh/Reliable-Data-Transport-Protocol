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

#define MAX_PACKET_LENGTH 500

using namespace std;

struct packet {
 /*header*/
 	uint16_t cksum;
	uint16_t len;
	uint32_t seqno;
	/*data*/
	char data[MAX_PACKET_LENGTH - 8];
};

struct ack_packet {
	uint16_t cksum;
	uint16_t len;
	uint32_t ackno;
};

int getArguments(int &max, int &seed, double &p) {
	ifstream inFile;
	inFile.open("server.in");
	if (!inFile) {
    	cerr << "Unable to open file datafile.txt";
    	exit(1);   // call system to stop
	}
	int portnum;
	inFile >> portnum;
	inFile >> max;
	inFile >> seed;
	inFile >> p;
	return portnum;
}

bool dropPacket(unsigned int seed, double probability)
{
	srand(seed) ;
	return ((double) rand() / (RAND_MAX)) < probability ; 
}

vector<char> readFile(string filename)
{
    ifstream ifs(filename, ios::binary|ios::ate);

    ifstream::pos_type pos = ifs.tellg();

    vector<char>  result(pos);

    ifs.seekg(0, ios::beg);
    ifs.read(&result[0], pos);
    return result;
}

vector<packet> getFilePackets(string fileName)
{
	vector<packet> packets ;
	cout << "begin read file \n" ;
	vector<char> file_data = readFile(fileName) ;
	cout << "file readed\n" ;
	int index = 0 ;
	uint32_t i = 0  ;
	vector<char>::iterator it = file_data.begin();
	cout << "begin iterate\n" ;
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
		char data[MAX_PACKET_LENGTH - 8];
		//copy(it, it+len, data)  ;

		// assinging data and header to packet to be send
		packet p  ;
		p.cksum = 0 ;  // checksum
		p.len = len ;  // length
		p.seqno = i%2 ; // sequence number
		copy(it, it+len, p.data) ; // data

		packets.push_back(p) ;
		index += len ;
		it += len ;
	}

	return packets ;
}

string getfilename(struct packet &p) {
	string str(p.data);
	return str;
}

int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in echoClntAddr;
	unsigned int cliAddrLen;
	//struct packet echoBuffer;
	unsigned short echoServPort;
	int recvMsgSize;

	int max_window, seed;
    double p;
	echoServPort = getArguments(max_window, seed, p);
	cout << echoServPort << endl;

	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		printf("socket() failed");

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	echoServAddr.sin_port = htons(echoServPort);

	if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		printf("bind() failed");

	cout << "bind" << endl;
	//cout << echoServAddr.sin_addr.s_addr << endl;
	for (int i=0;i < 1; i++) {/* Run forever */
	//while(true){
		char packetBuffer[MAX_PACKET_LENGTH] ;
		cliAddrLen = sizeof(echoClntAddr);
		if ((recvMsgSize = recvfrom(sock, packetBuffer, MAX_PACKET_LENGTH, 0, (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
			printf("recvfrom() failed\n");
		printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr)) ;
		cout <<  "recvMsgSizes "<< recvMsgSize << endl;
		struct packet * file_pckt = (packet*) packetBuffer;
		string filename = getfilename(*file_pckt);
		cout << "file name : " << filename<< endl;
		int proccess  = fork();
		cout << "proccess number" << proccess <<endl;
		if (!proccess)
		{
			cout << "getFilePackets B" << endl;
			vector<packet> packets = getFilePackets(filename);
			cout << "getFilePackets A" << endl;
			int index = 0;
			cout << "size :" << packets.size() << endl;
			while (index < packets.size()) {
				cout << "seqno" << packets[index].seqno << endl;
				cout << "size packet" << packets[index].len << endl;
				if (sendto(sock, &packets[index], recvMsgSize, 0, (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) < 0)
				{
					printf("sendto() sent a different number of bytes than expected");
				}
				else {
					// while not timeout
					recvMsgSize = 0;
					while (recvMsgSize == 0) //not time out 
					{
						recvMsgSize = recvfrom(sock, packetBuffer, MAX_PACKET_LENGTH, 0, (struct sockaddr *) &echoClntAddr, &cliAddrLen);
						cout << recvMsgSize << endl;
						printf("increment time out");
					}
					if (recvMsgSize > 0)
						index++;

				}
				cout << "index = " << index<< endl; 
			}
			cout << "exit" << endl;
			exit(0);
		}
	}
}