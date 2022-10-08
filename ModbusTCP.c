#include "ModbusTCP.h"

// tbh I am proud af of this function
// Already prints error message
int read_buffer(int socket_fd, char *APDU_R, int size)
{
	int len = 0;

	len = recv(socket_fd, APDU_R, sizeof(unsigned char) * size, 0);
	if (len <= 0)
	{
		printf("ERROR, couldn't receive info\n");
		return -1;
	}
	else if (len < size)
	{
		printf("BRUH, should read %d Bytes, trying to read the rest\n", size);
		return len + read_buffer(socket_fd, APDU_R, size - len);
	}
	else if (len > size)
	{
		printf("Read more than expected, BRUHHH, len->%d\n", len);
		return -1;
	}

	return len;

	len = 0;
}

// returns socket initiator file
// if error returns -1
int initialize_server_connection(char *ip, int port)
{
	int socket_fd, len = 0;
	struct sockaddr_in client;

	client.sin_family = AF_INET;
	inet_aton(ip, &client.sin_addr);
	client.sin_port = htons(port);

	printf("Trying to create connection...\n");

	// initialize socket
	if (socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
	{
		printf("Socket created\n");
	}
	else
	{
		printf("Error creating socket\n");
		return -1;
	}

	// Connect to the server
	if (connect(socket_fd, (struct sockaddr *)&client, sizeof(client)) < 0)
	{
		printf("Failed to connect with server\n");
		close(socket_fd);
		return -1;
	}
	else
	{
		printf("Connection successful\n");
	}

	return socket_fd;
}

int Send_Modbus_request(char *ip, int port, unsigned char *APDU, int APDUlen, unsigned char *APDU_R)
{
	int MBAP_size = APDUlen + 7;
	unsigned char MBAP[MBAP_size];
	static unsigned short trans_id = 0; // Increments every use, program doesn't forget

	MBAP[0] = (unsigned char)(trans_id >> 8);		 // High byte transaction identifier
	MBAP[1] = (unsigned char)(trans_id & 0xff);		 // Low byte transaction identifier
	MBAP[2] = 0;									 // Modbus protocol Identifier, always 0
	MBAP[3] = 0;									 // Modbus protocol Identifier, always 0
	MBAP[4] = (unsigned char)((APDUlen + 1) >> 8);	 // High byte rest of the packet lenght
	MBAP[5] = (unsigned char)((APDUlen + 1) & 0xff); // Low byte rest of the packet lenght
	MBAP[6] = 0xff;									 // Might need to change this one later

	// Kids, don't use strcat()
	for (int i = 0; i < APDUlen; i++)
	{
		MBAP[7 + i] = APDU[i];
	}

	// Initialize server connection
	int socket_fd;

	socket_fd = initialize_server_connection(ip, port);

	if (socket_fd < 0)
	{
		return -1;
	}

	// Verified until here
	//  Now we can communicate with the server

	int len = 0;

	// Send request
	len = send(socket_fd, MBAP, MBAP_size, 0);
	if (!len)
	{
		printf("ERROR, couldn't send data\n");
		return -1;
	}

	// Receive request, first we receive MBAP, then we go fot the Data
	// Reads what we want
	len = read_buffer(socket_fd, APDU_R, 6);
	if (len != 6)
	{
		return -1;
	}

	// verify readings
	for (int i = 0; i < 4; i++)
	{
		if (APDU_R[i] != MBAP[i])
		{
			printf("ERROR, APDU_R[%d] != MBAP[%d]\n", i, i);
		}
	}

	// I have a pressentimento que isto não vai dar
	int data_lenght = (int)(APDU_R[4] << 8) + (int)(APDU_R[5] & 0xff);

	len = 0;
	len = read_buffer(socket_fd, &APDU_R[6], data_lenght);
	if (len != data_lenght)
	{
		return -1;
	}

	for (int i = 0; i < (6 + data_lenght); i++)
	{
		printf("APDU_R[%d]=%u\n", i, APDU_R[i]);
	}

	// Just complicating here, should ahve used a buffer and then give the values to APDU_R
	// Now as I don't know how to use string functions, I will use a loop :/
	// This works now
	for (int i = 0; i < (data_lenght - 1); i++)
	{
		APDU_R[i] = APDU_R[i + 7];
		printf("APDU_R[%i]=%u\n", i, APDU_R[i]);
	}

	// close socket
	close(socket_fd);

	return 0;
}