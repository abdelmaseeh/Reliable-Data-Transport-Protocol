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

#define ECHOMAX 500
#define MAX_PACKET_SIZE 500
using namespace std;

struct ack_packet
{
    uint16_t cksum; /* Optional bonus part */
    uint16_t len;
    uint32_t ackno;
};

/* Data-only packets */
struct packet
{
    /* Header */
    uint16_t cksum; /* Optional bonus part */
    uint16_t len;
    uint32_t seqno;
    /* Data */
    char data[500]; /* Not always 500 bytes, can be less */
};

// Global Variables
unsigned short server_port;
unsigned short client_port;
char *serv_IP;;
char *file_name;
int sock; /* Socket descriptor */
struct sockaddr_in server_address; /* Echo server address */
struct sockaddr_in from_add; /* Source address of echo */
unsigned int fromSize;
int window_size;

// Functions Prototypes
void read_client_file();
void handle_stope_and_wait();
void add_packet_to_file(char [], int );
void handle_selective_repeat();
// Main
int main(int argc, char *argv[])
{
    read_client_file();
    char *sent_string;
    char received_buffer[MAX_PACKET_SIZE + 1];
    int sent_stringLen;
    int respStringLen;
    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        printf( "socket () failed\n") ;
    /* Construct the server address structure */
    memset(&server_address, 0, sizeof(server_address)); /* Zero out structure */
    server_address.sin_family = AF_INET; /* Internet addr family */
    server_address.sin_addr.s_addr = inet_addr(serv_IP); /* Server IP address */
    // cout << "HI1 " << server_address.sin_addr.s_addr<<endl;
    server_address.sin_port = htons(server_port); /* Server port */

    cout << "Server IP: \n" << serv_IP;
    cout << "Server port : \n" <<server_port;
    cout << "Socket Created\n";
    //***********************************************Socket Created ***********************************************************//

    // SENDING FILE NAME:
    struct packet file_name_pckt;
    file_name_pckt.len = strlen(file_name);
    memcpy(file_name_pckt.data, file_name, (strlen(file_name) + 1) * sizeof *file_name);
    cout << "Data: " << file_name_pckt.data << endl;
    if (sendto(sock, &file_name_pckt, sizeof(file_name_pckt), 0, (struct sockaddr *) &server_address, sizeof(server_address)) != sizeof(file_name_pckt))
    {
        printf("sendto() sent a different number of bytes than expected\n");
    }
    else
    {
        cout << "Client Sent File Name" << endl;
    }








/*
    // Receiving Acknowledgement
    // Recv a response
    struct ack_packet *file_name_ack;
    fromSize = sizeof(from_add) ;
    if ((respStringLen = recvfrom(sock, received_buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *) &from_add, &fromSize)) < 0 )
    {
        printf("recvfrom() failed\n") ;
    }
    else
    {
        cout << "Client Received File Name Acknowledgement" << endl;
    }

    if (server_address.sin_addr.s_addr != from_add.sin_addr.s_addr)
    {
        fprintf(stderr,"Error: received a packet from unknown source.\n %d\n%d\n", server_address.sin_addr.s_addr, from_add.sin_addr.s_addr);
        exit(1);
    }
    // null-terminate the received data
    received_buffer[respStringLen] = '\0' ;
    printf("Received: %s\n", received_buffer); // Print the  echoed arg
    file_name_ack = (ack_packet*) received_buffer;

    // Convert received to ack_packet


    */

    handle_stope_and_wait();
    close(sock);
    exit(0);
}

void handle_stope_and_wait()
{
    int expected_seq_no = 0;
    struct packet *file_pckt;
    struct ack_packet ack;
    char received_buffer[MAX_PACKET_SIZE + 1];
    int recievedLength;
    while (true)
    {
        cout << "Waiting for a file packet..." << endl;
        if ((recievedLength = recvfrom(sock, received_buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *) &from_add, &fromSize)) < 0 ) {
            printf("recvfrom() failed\n");
        } else {
            cout << "Received a File Packet" << endl;
            received_buffer[recievedLength] = '\0';
            file_pckt = (packet*) received_buffer;
            add_packet_to_file(file_pckt->data, file_pckt->len);
            if (file_pckt->seqno == expected_seq_no)
            {
                cout << "expected_seq_no" ;
                ack.ackno = expected_seq_no;
                expected_seq_no = !expected_seq_no;

                if (sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr *) &server_address, sizeof(server_address)) != sizeof(ack))
                    printf("Failed to send ack \n");
                else
                    cout << "Sent Ack" << endl;
            }
            else {
                cout << "field to send ack" << endl;
            }

        }
    }

}

void handle_selective_repeat()
{


}

void add_packet_to_file(char data[], int data_size)
{
    ofstream myfile;
    myfile.open(file_name);
    char *temp = NULL;
    temp = (char*)malloc((data_size+1) * sizeof *temp);
    memcpy(temp,(char*)data, data_size);
    myfile.write((char *)temp, data_size);
    free(temp);
}

void read_client_file()
{
    ifstream file("client.in");
    string str;

    // getting server IP
    getline(file, str);
    serv_IP = (char*)malloc(str.length() * sizeof *serv_IP);
    memcpy(serv_IP, str.c_str(), str.length() + 1);


    // getting server_port
    getline(file, str);
    server_port = atoi(str.c_str());

    // getting client_port
    getline(file, str);
    client_port = atoi(str.c_str());

    // getting file_name
    getline(file, str);
    file_name = (char*)malloc(str.length() * sizeof *file_name);
    memcpy(file_name, str.c_str(), str.length() + 1);

    // getting window Size
    getline(file, str);
    window_size = atoi(str.c_str());

    file.close();
}
