#include "ModbusAP.h"

int verify_connection(char *server_ip, int port, int st_r)
{
	if (strlen(server_ip) > 15)
	{
		printf("ERROR, IP has too many characters.\n");
		return -1;
	}

	if ((port < 0) || port > 0xffff)
	{
		printf("ERROR, Port is wrong, off limits.\n");
		return -1;
	}

	if ((st_r < 0) || (st_r > 0xffff))
	{
		printf("ERROR, Invalid address.\n");
		return -1;
	}
	return 0;
}

int Write_multiple_regs(char *server_ip, int port, int st_r, unsigned short n_r, unsigned short *val)
{
	st_r--;
	if (verify_connection(server_ip, port, st_r) < 0)
	{
		return -1;
	}

	if ((n_r < 1) || (n_r > (MAX_ADDRESS_NUMBER - st_r + 1)) || (n_r > MAX_REG_WRITE))
	{
		printf("ERROR, Invalid number of registers.\n");
		return -1;
	}

	// All verifications are done, we don't need to check the values(I think)
	// Assembling the APDU (MODBUS PDU(->protocol data unit))
	int APDUlen;
	APDUlen = 1 + (sizeof(unsigned short) * 2) + 1 + (n_r * sizeof(unsigned short));

	unsigned char APDU[APDUlen];
	int i = 0;

	APDU[0] = (unsigned char)0x10;			// Function code
	APDU[1] = (unsigned char)(st_r >> (8)); // High Byte Address
	APDU[2] = (unsigned char)(st_r & 0xff); // Low Byte Adress
	APDU[3] = 0;
	APDU[4] = (unsigned char)n_r;
	APDU[5] = ((unsigned char)n_r) * 2; // Byte count
	for (i = 0; i < APDU[5]; i += 2)
	{
		printf("i value=%d\n", i);
		APDU[6 + i] = (unsigned char)(val[i / 2] >> (8));
		APDU[6 + i + 1] = (unsigned char)(val[i / 2] & 0xff);
	}

	// Is already verified and working well
	for (i = 0; i < APDUlen; i++)
	{
		printf("APDU[%d]=%u\n", i, APDU[i]);
	}

	printf("Not bad!\n");

	int expected_response_size = 7 + 5;
	char APDU_R[expected_response_size];

	// Incluir aqui chamada a Send_Modbus_request()
	if (Send_Modbus_request(server_ip, port, APDU, APDUlen, APDU_R) < 0)
	{
		printf("Something went wrong\n");
		return -1;
	}

	// verify received values
	for (i = 0; i < (expected_response_size - 7); i++)
	{
		printf("APDU_R[%d]=%u && APDU[%d]=%u\n", i, APDU_R[i], i, APDU[i]);

		if (APDU_R[i] != APDU[i])
		{
			printf("Values don't match\n");
			break;
		}
	}

	if (i == (expected_response_size - 7))
	{
		printf("!!Successfully wrote desired Registers!!\n");
		return n_r;
	}

	if (i == 0 && (APDU_R[0] == (APDU[0] + 0X80)))
	{
		printf("We got an exception\n");

		switch (APDU_R[1])
		{
		case 1:
			printf("ILLEGAL FUNCTION EXCEPTION\n");
			return -1;

		case 2:
			printf("ILEEGAL DATA ADDRESS\n");
			return -1;

		case 3:
			printf("ILLEGAL DATA VALUE\n");
			return -1;
		}
	}

	printf("ERROR, data received didn't match the expected\n");
	return -1;
}

// Seems to work
int Read_h_regs(char *server_ip, int port, int st_r, unsigned short n_r, unsigned short *val)
{
	st_r--;
	if (verify_connection(server_ip, port, st_r) < 0)
	{
		return -1;
	}

	if ((n_r < 1) || (n_r > (MAX_ADDRESS_NUMBER - st_r + 1)) || (n_r > MAX_H_REG))
	{
		printf("ERROR, Invalid number of registers.\n");
		return -1;
	}

	unsigned char APDU[APDU_H_REG_SIZE];
	int i = 0;

	APDU[0] = 0x03;
	APDU[1] = (unsigned char)(st_r >> (8)); // High Byte Address
	APDU[2] = (unsigned char)(st_r & 0xff); // Low Byte Adress
	APDU[3] = 0;							// High byte number of registers
	APDU[4] = (unsigned char)n_r;			// low byte number of registers

	int expected_response_size = 7 + 2 + (n_r * sizeof(unsigned short));
	char APDU_R[expected_response_size];

	for (i = 0; i < APDU_H_REG_SIZE; i++)
	{
		printf("APDU[%d]=%u\n", i, APDU[i]);
	}

	if (Send_Modbus_request(server_ip, port, APDU, APDU_H_REG_SIZE, APDU_R) < 0)
	{
		printf("Something went wrong\n");
		return -1;
	}

	printf("Muy bien coño, let's verify now\n");

	if (APDU_R[0] != APDU[0])
	{
		printf("Function codes don't match\n");
		if (APDU_R[0] == (APDU[0] + 0X80))
		{
			switch (APDU_R[1])
			{
			case 1:
				printf("ILLEGAL FUNCTION EXCEPTION\n");
				return -1;

			case 2:
				printf("ILEEGAL DATA ADDRESS\n");
				return -1;

			case 3:
				printf("ILLEGAL DATA VALUE\n");
				return -1;
			}
		}
		else
		{
			printf("ERROR, data received didn't match the expected\n");
			return -1;
		}
	}

	// This convertion isn't workin and makes no senseeee
	// I had to do an AND idk why, I guess there is more bites that we don't understand somewhere in there
	//  val[(int)(i / 2)] = ((int)APDU_R[2 + i] * 256) + APDU_R[3 + i];

	for (i = 0; i < (int)(APDU_R[1]); i += 2)
	{
		val[(int)(i / 2)] = ((unsigned short)APDU_R[2 + i] << 8) + (APDU_R[3 + i] & 0xff);
		printf("val[%d]=%d\n", (int)(i / 2), val[(int)(i / 2)]);
	}
	printf("!!Successfully Read registers!!\n");

	return n_r;

	return 1;
}
