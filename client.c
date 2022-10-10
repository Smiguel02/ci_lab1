#include "ModbusAP.h"

#define IP "127.0.0.1"
#define PORT 502

int main()
{

    unsigned short A, B, C, D;

    int st_r = 7 + 1;
    int n_r = 2;
    unsigned short val[2] = {0};
    int lenght;

    lenght = Read_h_regs(IP, PORT, st_r, n_r, val);

    if (lenght < 0)
    {
        printf("Non Modbus error!\n");
        return -1;
    }
    if (lenght > 0)
    {
        printf("An Exception occurred!\n");
        return lenght;
    }
    else
    {
        printf("Read successfully\n");
    }

    A = val[0];
    B = val[1];

    // Read C

    st_r = 159 + 1;
    n_r = 1;

    lenght = Read_h_regs(IP, PORT, st_r, n_r, val);

    if (lenght < 0)
    {
        printf("Non Modbus error!\n");
        return -1;
    }
    if (lenght > 0)
    {
        printf("An Exception occurred!\n");
        return lenght;
    }
    else
    {
        printf("Read successfully\n");
    }

    C = val[0];

    // Write Registers
    val[0] = 55;

    if (!C)
    {
        D = 5555;
    }
    else
        D = A + B;

    val[1] = D;

    st_r = 150 + 1;
    n_r = 2;
    lenght = Write_multiple_regs(IP, PORT, st_r, n_r, val);

    if (lenght < 0)
    {
        printf("Non Modbus error!\n");
        return -1;
    }
    if (lenght > 0)
    {
        printf("An Exception occurred!\n");
        return lenght;
    }
    else
    {
        printf("Read successfully\n");
    }

    char ip[15] = "10.227.113.1";
    st_r = 152 + 1;
    n_r = 1;
    val[0] = val[1];

    lenght = Write_multiple_regs(ip, PORT, st_r, n_r, val);

    if (lenght < 0)
    {
        printf("Non Modbus error!\n");
        return -1;
    }
    if (lenght > 0)
    {
        printf("An Exception occurred!\n");
        return lenght;
    }
    else
    {
        printf("Read successfully\n");
    }

    return 0;
}