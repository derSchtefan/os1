//#include <string.h>
//#include <stdio.h>

char* andi = "And I miss you. And I want you, tonite";
static char* xandi = "Nargi nargi baby. Loving in devotion";

static char y[100] = {0xDE, 0xEA, 0xBE, 0xEF, 0xDE, 0xEA, 0xBE, 0xEF, 0xDE, 0xEA, 0xBE, 0xEF, 0xDE, 0xEA, 0xBE, 0xEF};

int klax(int i) {
    static char x[100] = {0xDE, 0xEA, 0xBE, 0xEF, 0xDE, 0xEA, 0xBE, 0xEF, 0xDE, 0xEA, 0xBE, 0xEF, 0xDE, 0xEA, 0xBE, 0xEF};
    static char yy[100];
    x[ 0x20 ] = 1;    
    yy[ 0x20 ] = 1;    
    y[0x40] = 8 + x[i] + yy[i+1];
    return i*4;
}


void kernel_main() {
    //memset(y, 0x55, 0x35);
    //snprintf(y, 100, "Vulgo: %i", 4);
    y[80] = (char) klax(1);
    for(int i = 0; i< 20; i++) {
        y[i] = (char) klax(y[i]);
    }

    char *base = (char*)0x000b8000;
    base[0] = andi[0];
    base[2] = andi[1];
    base[4] = xandi[6];
}

