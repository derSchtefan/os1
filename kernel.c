#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "snprintf.h"

char* booted_msg = "Kernel message printed";



int outstr_c(int line, char *str) {
    char *video_mem = (char*)(0x000b8000+(line*80*2));
    while(*str != 0) {
        *(video_mem++) = *(str++);
        *(video_mem++) = 0x02;
    }    
}

static unsigned int _putchar_pos = 0;

void setloc(unsigned int l) {
    _putchar_pos = l;
}

void putchar_(char c) {
    if(_putchar_pos < 80*25) {
        char *video_mem = (char*)(0x000b8000+(_putchar_pos*2));
        *(video_mem++) = c;
        *(video_mem++) = 0x03;
    }
}

int __errno = 0;
int *__errno_location(){
    return &__errno;
}

__attribute((used, constructor))
static void pre_init(void) {
    outstr_c(5, "pre-init");
}

extern unsigned int __heap;

void kernel_main() {
    outstr_c(4, booted_msg);

    char *video_mem = (char*)(0x000b8000);
    memset(video_mem, 0, 8);

    int flux = 4;
    char* f;
    int resu = asnprintf(&f, 100, "Fluxu: %i", flux);
    outstr_c(6, f);
    free(f);


    char * x = malloc(3000);
    free(x);
    outstr_c(7, "allocd");

    while(1);
}


