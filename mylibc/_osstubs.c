#include <stdlib.h>
#include <stdarg.h>
#include "stddef.h"

int vsnprintf(char *, size_t, const char *, va_list);

int __sprintf_chk(char * str, int flag, size_t strlen, const char * format, ...)
{
    va_list arglist;
    va_start( arglist, format );

    int rv = vsnprintf(str, strlen, format, arglist);
    
    va_end( arglist );
    return rv;
}

