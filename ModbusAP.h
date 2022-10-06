#include "ModbusTCP.h"
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_ADDRESS_NUMBER 65536
#define MAX_REG_WRITE 123
#define MAX_H_REG 125
#define APDU_H_REG_SIZE 5

/**
 * @brief
 *
 * @param ip server IP
 * @param port communication Port
 * @param st_r Starting address, will decrese the value by 1, before calling other function. From 1-65536
 * @param n_r Number of registers, max val = 123
 * @param val Registers value
 * @return Number of written Registers
 */
int Write_multiple_regs(char *server_ip, int port, int st_r, unsigned short n_r, unsigned short *val);

int Read_h_regs(char *server_ip, int port, int st_r, unsigned short n_r, unsigned short *val);
