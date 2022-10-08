#include "ModbusAP.h"

#define IP "127.0.0.1"
#define PORT 502

int main()
{
    int st_r = 1;
    int size = 2;
    unsigned short val[2] = {4, 20};

    return Read_h_regs(IP, PORT, st_r, size, val);
}