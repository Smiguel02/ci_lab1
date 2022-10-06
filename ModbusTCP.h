#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

int Send_Modbus_request(char *ip, int port, unsigned char *APDU, int APDUlen, unsigned char *APDU_R);