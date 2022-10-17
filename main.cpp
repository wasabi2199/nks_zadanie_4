#include <stdio.h>
#include <cstdint>
#define NR 7
#include <time.h>
#include <cstdlib>

uint16_t SB[16] = {
        0x5, 0x6, 0x2, 0x0,
        0x7, 0xB, 0x3, 0xF,
        0xC, 0x1, 0x4, 0x8,
        0x9, 0xD, 0xA, 0xE
};

uint16_t transp(uint16_t x)
{
    uint16_t y = 0;

    y ^= ((x) & 0x8421);        // 1000 0100 0010 0001
    y ^= ((x) & 0x0842) << 3;    // 0000 1000 0100 0010
    y ^= ((x) & 0x0084) << 6;    // 0000 0000 1000 0100
    y ^= ((x) & 0x0008) << 9;    // 0000 0000 0000 1000
    y ^= ((x) & 0x1000) >> 9;    // 0001 0000 0000 0000
    y ^= ((x) & 0x2100) >> 6;    // 0010 0001 0000 0000
    y ^= ((x) & 0x4210) >> 3;    // 0100 0010 0001 0000

    return y;
}

uint16_t subst(uint16_t x)
{
    uint16_t y = 0;

    y = SB[x & 0xf];
    y ^= SB[(x >> 4) & 0xf] << 4;
    y ^= SB[(x >> 8) & 0xf] << 8;
    y ^= SB[(x >> 12) & 0xf] << 12;

    return y;
}

uint16_t SPN_encrypt(uint16_t pt, uint16_t key_exp)
{
    uint16_t x;
    int i;

    x = pt ^ key_exp;

    for (i = 1; i < NR; i++)
    {
        x = subst(x);
        x = transp(x);
        x = x ^ key_exp;
    }

    x = subst(x);
    x = x ^ key_exp;

    return x;
}


int main()
{
    unsigned int k = 0xdeadbeef;
    unsigned int x = 0;
    unsigned int y = 0;
    unsigned int t = 0;
    double clk = -clock();
    uint16_t key;
    srand(time(NULL));
    FILE *f;
    char fname[100];

    for(int i = 0; i < 100; i++){
        sprintf(fname, "%04i.dat", i);
        f = fopen(fname, "wb");
        for (k = 0; k < 8; k++)
        {
            key = rand();
            for(x = 0; x < 0xffff; x++)
            {
                /*if(x % 256 == 0){
                    key = rand();
                }*/
                y ^= SPN_encrypt(x, key);
                putc(y & 0xff, f);
                putc((y & 0xff00) >> 8, f);
            }
        }
        fclose(f);
    }

    clk += clock();

    printf("%04x, T = %0.6lf s\n", y, clk/CLOCKS_PER_SEC);
    return 0;
}