#include "ModbusAP.h"

#define IP "127.0.0.1"
#define PORT 2

int main()
{
    int st_r = 300;
    int size = 2;
    unsigned short val[2] = {20, 4};

    return Read_h_regs(IP, PORT, st_r, size, val);
}