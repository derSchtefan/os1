#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>
#include <stdbool.h>
#include "snprintf.h"


static const char characterTable[] = {
    0,    0,    '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',
    '-',  '=',  0,    0x09, 'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
    'o',  'p',  '[',  ']',  0,    0,    'a',  's',  'd',  'f',  'g',  'h',
    'j',  'k',  'l',  ';',  '\'', '`',  0,    '\\', 'z',  'x',  'c',  'v',
    'b',  'n',  'm',  ',',  '.',  '/',  0,    '*',  0x0F, ' ',  0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0x1B, 0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0x0E, 0x1C, 0,    0,    0,
    0,    0,    0,    0,    0,    '/',  0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0,
    0,    0,    0,    0,    0,    0,    0,    0x2C,
};

static const char shiftedCharacterTable[] = {
    0,    0,    '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',
    '_',  '+',  0,    0x09, 'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
    'O',  'P',  '{',  '}',  0,    0,    'A',  'S',  'D',  'F',  'G',  'H',
    'J',  'K',  'L',  ':',  '"',  '~',  0,    '|',  'Z',  'X',  'C',  'V',
    'B',  'N',  'M',  '<',  '>',  '?',  0,    '*',  0x0F, ' ',  0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0x1B, 0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0x0E, 0x1C, 0,    0,    0,
    0,    0,    0,    0,    0,    '?',  0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0,
    0,    0,    0,    0,    0,    0,    0,    0x2C,
};


typedef struct
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t interrupt, error;

    uint32_t eip, cs, eflags;
} interruptFrame;

typedef void (*registered_irq_handler)(interruptFrame *regs);

void registerIRQhandler(uint8_t id, registered_irq_handler handler);


static bool leftShiftDown = false;
static bool rightShiftDown = false;

void keyboard_handler(interruptFrame frame) {
    unsigned char scancode;

    scancode = inportb(0x60);

    if(scancode == 0) {
        return;
    }

    if(scancode == 0x2A){
        leftShiftDown = true;
        return;
    }

    if(scancode == 0xAA){
        leftShiftDown = false;
        return;
    }
    
    if(scancode == 0x36){
        rightShiftDown = true;
        return;
    }

    if(scancode == 0xB6){
        rightShiftDown = false;
        return;
    }

    // figure this out better
    if(scancode & 0x80) {
        return;
    }

    if(scancode >= sizeof(characterTable)) {
        printf(" !%x! ", scancode);
        return;
    }

    bool shifted = leftShiftDown || rightShiftDown;
    char mappedChar = (shifted ? shiftedCharacterTable : characterTable)[scancode];
    

    printf("%c", mappedChar);
    //printf("%X ", (int)scancode);

    // if (scancode & 0x80) {
    //     // TODO: Add handling for shift || alt || control keys
    // } else {
    //     switch (scancode) {
    //         //printf("%c", characterTable[scancode]);
    //     }
    // }
}


void keyboard_install() {
    registerIRQhandler(1, keyboard_handler);
}
