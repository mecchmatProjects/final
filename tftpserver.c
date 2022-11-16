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

#define SERV_UDP_PORT 70361      
#define DATA_SIZE     512	     
#define PACKET_SIZE   516
#define MODE          "octet"    

char *progname;

void sendACK(unsigned short blockNum, unsigned short opCode, int sockfd) {
	char packet[PACKET_SIZE];
	struct sockaddr_in cli_addr;
	bzero(packet, sizeof(packet));

	unsigned short *packetPtr = (unsigned short*)packet;
	*packetPtr = htons(opCode);
	packetPtr++;

	unsigned short *blockNumPtr = packetPtr;
	*blockNumPtr = htons(blockNum);
	
	int ackNum = blockNum - 1;

	sendto(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr*)&cli_addr,
	       sizeof(cli_addr));
	printf("[SERVER] Sent ACK #%d\n", ackNum);
}

void sendDataPacket(unsigned short blockNum, char blockData[], int sockfd, struct sockaddr_in cli_addr) {
	char packet[PACKET_SIZE];
	unsigned short opCode = 3;
	bzero(packet, sizeof(packet));

	unsigned short* packetPtr = (unsigned short*)packet;
	*packetPtr = htons(opCode);
	packetPtr++;

	*packetPtr = htonl(blockNum);
	blockNum++;

	memcpy(&packet[6], blockData, DATA_SIZE);

	sendto(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr*)&cli_addr,
		   sizeof(cli_addr));
	printf("[SERVER] Sent DATA packet to server for block number: %d\n", blockNum);
}

void processDataPacket(char buffer[], char filename[], unsigned short blockNum, int sockfd) {
	unsigned short opCode = 4;
	
	FILE *fp;
	fp = fopen(filename, "w+t");

	char dataBuf[DATA_SIZE];
	bzero(dataBuf, sizeof(dataBuf));
	char* dataPtr = (char*)buffer;
	memcpy(&dataBuf[0], dataPtr + 4, DATA_SIZE);

	fprintf(fp, "%s", dataBuf);
	bzero(buffer, PACKET_SIZE);
	fclose(fp);
	
	blockNum++;
	sendACK(blockNum, opCode, sockfd); 
}

void processRRQ(int sockfd, char filename[], struct sockaddr_in cli_addr) {
	FILE *fp;
	fp = fopen(filename, "rb");

	unsigned short opCode = 3;   
	unsigned short blockNum = 1;  
	int msgLen;

	printf("[SERVER] Sent DATA block #%d\n", blockNum);
	fseek(fp, 0, SEEK_END);
	int dataSize = ftell(fp);
	
	fseek(fp, 0, SEEK_SET);

	printf("Size of %s file: %d bytes\n", filename, dataSize);

	char dataPacket[PACKET_SIZE];
	bzero(dataPacket, sizeof(dataPacket));

	opCode = htons(opCode);
	blockNum = htons(blockNum);

	// Add op code and block num to data buffer
	memcpy(dataPacket, &opCode, 2);
	memcpy(dataPacket + 2, &blockNum, 2);

	// Add file data to data packet
	fread(dataPacket + 4, 1, PACKET_SIZE, fp);

	// Send data packet to client
	msgLen = sendto(sockfd, dataPacket, PACKET_SIZE, 0, (const struct sockaddr*)&cli_addr,
	       sizeof(cli_addr));
	fclose(fp);
}



void text_mode_transfer(FILE * src_file, int data_sock,
			struct sockaddr_in cli_addr)
{
	// incoming message buffer
	char buffer[BUFSIZ];

	// message opcode
	uint16_t opcode;

	// received message length
	int recv_len;

	// client address length
	size_t cli_size = sizeof(cli_addr);

	// single char store buffer
	char c;

	// chars counter: the first 4 bytes of the transfer buffer
	// must be left empty for the opcode and block number
	int i = 4;

	// data transfer blocks counter
	uint16_t block_counter = 1;

	int MAX = 512;
	// until the EOF is found in the opened file
	do {
		// retrieve next char from the source file
		c = fgetc(src_file);

		// avoid adding the EOF to the transfer buffer
		if (c != EOF)
		{
			// copy retrieved char in the transfer buffer
			buffer[i] = c;

			// increase chars counter
			i++;
		}

		// check if either the maximum number of chars or the
		// EOF has been reached
		if (i == MAX + 4 || c == EOF)
		{
			// if so, send the data message to the client
			// opcode = 3 (= DATA)
			opcode = htons(3);

			// prepare block number
			uint16_t block = htons(block_counter);

			// copy opcode to the transfer buffer
			memcpy(buffer, &opcode, 2);

			// copy block_number to the transfer buffer
			memcpy(buffer + 2, &block, 2);

			// send transfer buffer to the client
			recv_len =
			    sendto(data_sock,
				   buffer,
				   i,
				   MSG_CONFIRM,
				   (const struct sockaddr *) &cli_addr,
				   sizeof(cli_addr));

			// check for errors

			// if debugging is enabled
			if (1)
			{
				// print debugging info log message
				printf
				    ("Data packet with block number %d sent. Waiting "
				     "to receive ACK response.", block_counter);
				//child_log(INFO, log_message);
			}

			// clear transfer buffer
			memset(buffer, 0, BUFSIZ);

			// wait for ACK response from the client
			recv_len = recvfrom(data_sock, (char *)
					    buffer,
					    BUFSIZ,
					    MSG_WAITALL,
					    (struct sockaddr *) &cli_addr,
					    (socklen_t *) & cli_size);

			// check for errors
			//check_errno(recv_len,
			//	    "Error while receiving ACK packet.");

			// retrieve block number from transfer buffer
			memcpy(&block, (uint16_t *) & buffer[2], 2);

			// deserialize block number
			block = ntohs(block);

			// if debugging is enabled
			if (1)
			{
				// print debugging info log message
				printf
				    ("ACK response received for block number: %d.", block);
				//child_log(INFO, log_message);
			}

			// before sending next data message, check ack message block number
			if (block != block_counter)
			{
				// print a warning error log
				//child_log(ERROR,
				//	  "Sent block number and ACK packet block number do not "
				//	  "match. Transfer cancelled.");

				// cancel tranfer, close source file
				fclose(src_file);

				// shutdown transfer socket
				shutdown(data_sock, SHUT_RDWR);

				// close transfer socket
				close(data_sock);

				// exit with error
				//exit(-1);
			}

			// reset chars counter for new transfer
			i = 4;

			// increase block number counter
			block_counter++;
		}
	}
	while (c != EOF);
}



// TODO: fix write request
void processWRQ(int sockfd, char filename[], struct sockaddr_in cli_addr) {
	FILE *fp;

	//unsigned short opCode = 4;
//	unsigned short blockNum = 1;
//	char dataBuf[DATA_SIZE];
//	socklen_t cli_size = sizeof(cli_addr);

	//sendACK(blockNum, opCode, sockfd);

	int data_sock = socket(AF_INET, SOCK_DGRAM, 0);

	text_mode_transfer(fp, data_sock, cli_addr);


	// file transfer completed, clear transfer buffer
	//memset(buffer, 0, BUFSIZE);

	// close source file
	fclose(fp);

	// shutdown transfer socket
	shutdown(data_sock, SHUT_RDWR);

	// close transfer socket
	close(data_sock);


	//processDataPacket(dataBuf, filename, blockNum, sockfd);
}

int main(int argc, char *argv[]) {
	// -------------------------- Initializing Socket -------------------------
	// Defining necessary variables
	int sockfd, nBytes;
	struct sockaddr_in serv_addr, cli_addr;
	char buffer[PACKET_SIZE];
	progname = argv[0];

	// Socket descriptor
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("%s: Failed to open datagram socket\n", progname);
		exit(1);
	}

	// Setting values to serv_addr
	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	serv_addr.sin_port = htons(SERV_UDP_PORT); 
	
	// -------------------------------- Binding -------------------------------
	if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("%s: Failed to bind to socket\n", progname);
		exit(2);
	}

	// ------------------------ Listen for data packets -----------------------
	printf("[SERVER] Waiting for client request\n");
	
	for( ; ; ) {
		socklen_t cli_size = sizeof(cli_addr);
		bzero(buffer, DATA_SIZE);
		recvfrom(sockfd, buffer, DATA_SIZE, 0, (struct sockaddr*)&cli_addr, &cli_size);

		// op code
		unsigned short *opCodePtrRcv = (unsigned short*)buffer;
		unsigned short opCodeRcv = ntohs(*opCodePtrRcv);

		char fileNameRcv[20]; 
		strcpy(fileNameRcv, buffer + 2);
		
		unsigned short* blockNumPtr = (unsigned short*)buffer;
		blockNumPtr += 2;
		unsigned short blockNum = ntohs(*blockNumPtr);
		
		char *dataPtrRcv = buffer;
		dataPtrRcv += 4;
		
		switch (opCodeRcv) {
		case 1:
			printf("Received RRQ packet from client\n");
			printf("Opcode: %d\n", opCodeRcv);
			printf("File name received: %s\n", fileNameRcv);
			processRRQ(sockfd, fileNameRcv, cli_addr);
			break;
		case 2: 
			printf("Received WRQ packet from client\n");
			printf("Opcode: %d\n", opCodeRcv);
			printf("File name received: %s\n", fileNameRcv);
			processWRQ(sockfd, fileNameRcv, cli_addr);

			break;
		case 3:
			sendDataPacket(blockNum, dataPtrRcv, sockfd, cli_addr);
			break;
		case 4: 
			processDataPacket(buffer, fileNameRcv, blockNum, sockfd);
			break;
		case 5: 
			// ERROR
			break;
		default:
			printf("[ERROR] Invalid op code: %d\n", opCodeRcv);
			break;
		}
		printf("----------------------------------------------------------\n");
	}
	return 0;
}
