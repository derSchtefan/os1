//#include <string.h>
//#include <stdio.h>

char* booted_msg = "Kernel message printed";

int outstr_c(int line, char *str) {
    char *video_mem = (char*)(0x000b8000+(line*80));
    while(*str != 0) {
        *(video_mem++) = *(str++);
        *(video_mem++) = 0x02;
    }    
}


void kernel_main() {
    outstr_c(4, booted_msg);
    while(1);
}

