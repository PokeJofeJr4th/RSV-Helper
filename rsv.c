#include "rsv.h"

#include "rsv_table.c"
#include <stdarg.h>

int rsv_read_field(FILE *file, char *buf, int max_size)
{
    int c = fgetc(file);
    // if we're at the end of a file, return -2
    if (c == EOF)
    {
        *buf = 0;
        return EOF;
    }
    // if we're at the end of a row, return RSV_EOR
    else if (c == RSV_EOR)
    {
        *buf = 0;
        return RSV_EOR;
    }
    // if this is a null field, return RSV_NV
    else if (c == RSV_NV)
    {
        *buf = 0;
        // if the next character isn't EOV, return the error
        if (fgetc(file) != RSV_EOV)
        {
            return RSV_ERROR;
        }
        return RSV_NV;
    }
    // if the first character is an end of value, return the empty string
    else if (c == RSV_EOV)
    {
        *buf = 0;
        return RSV_EOV;
    }
    char *limit = buf + max_size;
    *limit = 0;
    // start at the beginning of the string
    char *tmp = buf;
    *tmp = c;
    // write to the buffer until we hit and EOV or go past the limit
    while ((unsigned char)(*++tmp = (c = fgetc(file))) != RSV_EOV && tmp < limit)
        ;
    while (c != RSV_EOV && c != RSV_EOR && c != EOF)
        c = fgetc(file);
    *tmp = 0;
    return RSV_EOV;
}

int rsv_fmt_field(FILE *file, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int vf_result = rsv_vfmt_field(file, fmt, args);
    va_end(args);
    return vf_result;
}

int rsv_vfmt_field(FILE *file, const char *fmt, va_list args)
{
    if (vfprintf(file, fmt, args) < 0)
        return EOF;
    return fputc(RSV_EOV, file);
}

int rsv_write_field(const char *field, FILE *file)
{
    int result;
    if (field == NULL)
    {
        result = fputc(RSV_NV, file);
    }
    else
    {
        result = fputs(field, file);
    }
    if (result == EOF)
        return EOF;
    return fputc(RSV_EOV, file);
}

int rsv_finish_row(FILE *file)
{
    return fputc(RSV_EOR, file);
}
