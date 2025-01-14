#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>
#include "snprintf.h"

char* booted_msg = "Kernel ready";

void clearscr(){
    char *video_mem = (char*)(0x000b8000);
    memset(video_mem, 0, 80*25*2);
}

int outstr_c(int line, char *str) {
    char *video_mem = (char*)(0x000b8000+(line*80*2));
    while(*str != 0) {
        *(video_mem++) = *(str++);
        *(video_mem++) = 0x02;
    }    
}

void printf_loc(int line, const char*fmt, ...) {
    va_list arglist;
    va_start( arglist, fmt );

    char* msg;
    vasnprintf(&msg, 1024, fmt, arglist);
    outstr_c(line, msg);
    free(msg);
    
    va_end( arglist );
}

static unsigned int _putchar_pos = 0;

void setloc(unsigned int l) {
    _putchar_pos = l;
}


void putchar_(unsigned char c) {
    printf_loc(18, "%X  ", (unsigned int)c);

    if(c == '\n') {
        _putchar_pos += 80;
        _putchar_pos = _putchar_pos - (_putchar_pos % 80);
        // todo: scroll
        return;
    }
    if(c == '\r') {
        _putchar_pos = _putchar_pos % 80;
        return;
    }

    if(_putchar_pos < 80*25) {
        char *video_mem = (char*)(0x000b8000+(_putchar_pos*2));
        *(video_mem++) = c;
        *(video_mem++) = 0x03;
        _putchar_pos++;
    }    
    // todo: scroll
}

int putchar(int c) {
    putchar_(c); 
}

int __errno = 0;
int *__errno_location(){
    return &__errno;
}

__attribute((used, constructor))
static void pre_init(void) {
    outstr_c(5, "pre-init");
}

extern volatile unsigned int __heap;
extern volatile int last_int_no;
extern volatile int int_call_count;

void execute_cpp();

void idt_init();

void fff(){
    asm volatile ("int $0x80");
    asm volatile ("sti");
}
void keyboard_install() ;


void kernel_main() {
    /*
    printf("Nein: %i\n", 12);
    printf("Doch: %i\n", 12);



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

    execute_cpp();

    printf("Nein: %i", 12);
    */

    clearscr();

    idt_init();
    outstr_c(4, booted_msg);

    /*
    int a = 1;
    int b = 0;
    int c = a / b;
    */  
    

    fff();

    keyboard_install();

    while(1){
        printf_loc(20, "Last Int: %i     ", last_int_no);
        printf_loc(21, "Int Count: %i     ", int_call_count);
        printf_loc(22, "BRK: %x     ", __heap);
        register void *esp asm ("esp");
        printf_loc(23, "ESP: %x     ", esp);
    }
}


