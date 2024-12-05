extern void kernel_main(void);

// Symbols defined by the linker script
extern unsigned int _idata;
extern unsigned int _data;
extern unsigned int _edata;
extern unsigned int _stack;
extern unsigned int _bss;
extern unsigned int _ebss;

typedef void (*libc_ctor)(void);

extern libc_ctor* __init_array_start;
extern libc_ctor* __init_array_end;

static void clear_bss(void)
{
    unsigned int *bss_ptr = &_bss;
    while (bss_ptr < &_ebss) {
        *bss_ptr++ = 0;
    }
}

// Global (and static local) variables initialized to non-zero value
static void copy_data(void)
{
    unsigned int *src_data_ptr = &_idata;
    unsigned int *dst_data_ptr = &_data;
    while (dst_data_ptr < &_edata) {
        *dst_data_ptr++ = *src_data_ptr++;
    }
}

void __libc_init_array()
{
    for(libc_ctor* i = &__init_array_start; (i < &__init_array_end) && (i != 0); i++ ) {
        if(i) (*i)();
    }
}

__attribute((used, section(".entry")))
void _entry_point(void) {
    copy_data();
    clear_bss();
    __libc_init_array();
    kernel_main();

    // We should never return from main, but just in case we do
    while(1);
}
