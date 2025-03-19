#include "rsv.h"

#include "rsv_table.c"

/// @brief Read a single field from an RSV file
/// @param file the file to read from
/// @param buf the buffer to read into. The buffer must have space for at least max_size + 1 bytes.
/// @param max_size the maximum number of bytes to read
/// @return the character describing. If this is RSV_EOF, the file has ended and there is no next field. If this is RSV_EOR, the row has ended and there is no next field. If this is RSV_NV, the field is null. If this is RSV_EOV, the field was read successfully.
int rsv_read_field(FILE *file, char *buf, int max_size)
{
    int c = fgetc(file);
    // if we're at the end of a file, return -2
    if (c == EOF)
    {
        *buf = 0;
        return EOF;
    }
    // if we're at the end of a row, return -1
    else if (c == RSV_EOR)
    {
        *buf = 0;
        return RSV_EOR;
    }
    // if this is a null field, return 0
    else if (c == RSV_NV)
    {
        *buf = 0;
        // if the next character isn't EOV, scream
        if (fgetc(file) != RSV_EOV)
        {
            return -3;
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

/// @brief Write a field to an RSV file
/// @param field the string to write to the file
/// @param file the file to write to
int rsv_write_field(const char *field, FILE *file)
{
    if (field == NULL)
    {
        fputc(RSV_NV, file);
    }
    else
    {
        fputs(field, file);
    }
    return fputc(RSV_EOV, file);
}

/// @brief Mark the end of a row in an RSV file. This function should be called after EVERY row, including the last one
/// @param file the file to write to
int rsv_finish_row(FILE *file)
{
    return fputc(RSV_EOR, file);
}
