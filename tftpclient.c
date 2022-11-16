#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>        
#include <string.h>        
#include <signal.h>         
#include <unistd.h>
#include <strings.h>


#define SERV_UDP_PORT   70361     
#define SERV_HOST_ADDR "127.0.0.1" 
#define DATA_SIZE 		512
#define PACKET_SIZE		516
#define MODE           "octet" 

char *progname;
char *requestType;
char *fileName;
struct sockaddr_in *serv_addrPtr;

// Send request packet
void sendRRQ(int sockfd) {
	char buffer[PACKET_SIZE];
	struct sockaddr_in serv_addr;

	// set values to serv_addr
	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERV_UDP_PORT);

	// clear buffer
	bzero(buffer, sizeof(buffer));

	// create read request packet
	unsigned short opCode = 1; // RRQ opcode
	unsigned short *opCodePtr = (unsigned short*) buffer; 

	// convert the opcode from host to network byte 
	*opCodePtr = htons(opCode); 
	opCodePtr++; 

	strcpy(buffer + 2, fileName);

	unsigned char end = '\0';
	memcpy(buffer + strlen(fileName) + 2, &end, 1);

	strcpy(buffer + strlen(fileName) + 3, MODE);
	
	memcpy(buffer + strlen(fileName) + strlen(MODE) + 3, &end, 1);

	// Send to the server
	sendto(sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	printf("[CLIENT] Sent RRQ packet to server for file: %s\n", fileName);
}

void sendWRQ(int sockfd) {
	char buffer[PACKET_SIZE];
	struct sockaddr_in serv_addr;
	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERV_UDP_PORT);
	bzero(buffer, sizeof(buffer));

	unsigned short opCode = 2;
	unsigned short *opCodePtr = (unsigned short*) buffer; 
	*opCodePtr = htons(opCode); 
	opCodePtr++; 

	strcpy(buffer + 2, fileName);
	
	unsigned char end = '\0';
	memcpy(buffer + strlen(fileName) + 2, &end, 1);

	strcpy(buffer + strlen(fileName) + 3, MODE);

	memcpy(buffer + strlen(fileName) + strlen(MODE) + 3, &end, 1);

	sendto(sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	printf("[CLIENT] Sent WRQ packet to server for file: %s\n", fileName);
}

void sendDataPacket(unsigned short blockNum, char blockData[], int sockfd) {
	char packet[PACKET_SIZE];
	unsigned short opCode = 3;
	bzero(packet, sizeof(packet));

	// construct packet to send
	// convert/add opcode (+2 bytes)
	unsigned short* packetPtr = (unsigned short*)packet;
	*packetPtr = htons(opCode);
	packetPtr++;

	// convert/add blockNum at packet[2] (+4 bytes)
	//unsigned long* blockNumPtr = packet[2];
	*packetPtr = htonl(blockNum);
	blockNum++;

	// add data into packet[6]
	memcpy(&packet[6], blockData, DATA_SIZE);

	// send packet
	sendto(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr*)&serv_addrPtr,
		   sizeof(serv_addrPtr));
	//printf("[CLIENT] Sent DATA packet to server for block number: %lu\n", blockNum);
}

void sendAckPacket(unsigned short blockNum, int sockfd) {
	char packet[PACKET_SIZE];
	unsigned short opCode = 4;
	bzero(packet, sizeof(packet));

	// construct packet to send
	// convert/add opcode (+2 bytes)
	unsigned short* packetPtr = (unsigned short*)packet;
	*packetPtr = htons(opCode);
	packetPtr++;

	// convert/add blockNum at packet[2] (+4 bytes)
	*packetPtr = htons(blockNum);

	// send packet
	
	serv_addrPtr->sin_family = AF_INET;
	serv_addrPtr->sin_addr.s_addr = htonl(INADDR_ANY); 
	serv_addrPtr->sin_port = htons(SERV_UDP_PORT); 

	sendto(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr*)&serv_addrPtr,
		sizeof(serv_addrPtr));
	printf("[CLIENT] Sent ACK packet to server for block number: %u\n", blockNum);	
}

void processDataPacket(char buffer[], int sockfd) {
	// create new file with filename
	FILE *fp;
	fp = fopen(fileName, "wb");

	// read packet data
	char dataBuf[DATA_SIZE];
	bzero(dataBuf, sizeof(dataBuf));
	char* dataPtr = (char*)buffer;
	dataPtr = &buffer[4];
	memcpy(&dataBuf[0], dataPtr, DATA_SIZE);

	// read data into file
	fseek(fp, 0, SEEK_SET);
	//fwrite(fp, 1, sizeof(dataBuf), fp);
	fprintf(fp, "%s", dataBuf);
	bzero(buffer, PACKET_SIZE);
	fclose(fp);

	// send ack packet
	unsigned short* blockNumPtr = (unsigned short*)buffer;
	blockNumPtr += 2;
	unsigned short blockNum = ntohs(*blockNumPtr);
	sendAckPacket(blockNum, sockfd);

	// if data size < 512, terminate connection	
	// step 2 (to be implement)
}

void processAckPacket(char buffer[], int sockfd) {
	// read block number
	unsigned short* blockNumPtr = (unsigned short*)buffer;
	blockNumPtr += 2;
	unsigned short ackBlockNum = ntohs(*blockNumPtr);
	// read block blockNum + 1 from file
	// if ack is for block 0, first data block is sent
	// check if last block
	unsigned short blockNum = ackBlockNum + 1;

	// read correct block of data from file
	FILE *fp;
	fp = fopen(fileName, "rb");

	// calculate total number of blocks
	fseek(fp, 0, SEEK_END);
	long int size = ftell(fp);
	unsigned short lastBlockNum = size / 512;

	// reset fp to beginning
	fseek(fp, 0, SEEK_SET);
	
	char blockBuf[DATA_SIZE];
	bzero(blockBuf, sizeof(blockBuf));
	if (blockNum <= lastBlockNum) {
		int fileOffset = blockNum * DATA_SIZE;

		fseek(fp, fileOffset, SEEK_SET);
		fread(blockBuf, 1, strlen(blockBuf), fp);
	}

	fclose(fp);

	// send data block
	sendDataPacket(blockNum, blockBuf, sockfd);
	// wait for ack or send again
}

void processResponse(char buffer[], int sockfd) {
	// parse opcode
	unsigned short* opCodePtrRcv = (unsigned short*)buffer;
	unsigned short opCodeRcv = ntohs(*opCodePtrRcv);

	switch (opCodeRcv) {
		case 3:
			// data packet
			processDataPacket(buffer, sockfd);
		case 4:
			// ack packet
			processAckPacket(buffer, sockfd);
		case 5:
			// error packet
			break;
		/*default:
			printf(" unexpected or unknown packet type\n");*/
	}
}

int main(int argc, char *argv[]) {
	// Checking for valid number of arguments (progname, requestType, fileName)
	if(argc != 3) {
		printf("Usage: ./tftpclient [-r or -w] [File Name]\n");
		exit(1);
	}

	// -------------------------- Initializing Socket -------------------------
	// Defining necessary variables
	int sockfd;
	struct sockaddr_in cli_addr;
	progname = argv[0];     // program name
	requestType = argv[1];  // RRQ or WRQ
	fileName = argv[2];     // filename


	// Socket descriptor
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("%s: Failed to open datagram socket\n", progname);
		exit(1);
	}

	// Setting values to cli_addr
	memset(&cli_addr, '\0', sizeof(cli_addr));
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
	cli_addr.sin_port = htons(SERV_UDP_PORT);

	// ----------------------- Sending request to server ----------------------
	// [opcode | Filename | 0 | Mode | 0]
	if(strcmp(argv[1], "-r") == 0) { // RRQ 
		sendRRQ(sockfd);
	} else if(strcmp(argv[1], "-w") == 0) { // WRQ
		sendWRQ(sockfd);
	} else {
		printf("Invalid request type: %s\n", requestType);
		printf("Request type option 'r' for RRQ or 'w' for WRQ\n");
	}

	// ------------------- Receiving response from server ---------------------
	char buffer[PACKET_SIZE];
	struct sockaddr_in serv_addr;
	int s, length;

	bzero(buffer, PACKET_SIZE);
	s = recvfrom(sockfd, (char*)buffer, PACKET_SIZE, MSG_WAITALL,
				(struct sockaddr*)&serv_addr, &length);
	serv_addrPtr = &serv_addr;
	processResponse(buffer, sockfd);

	return 0;
}