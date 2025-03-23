#include "rsv.h"

#include <stdlib.h>

struct RsvTable *rsv_read_table(FILE *file)
{
    // the total number of rows in the file
    int num_rows = 0;
    // the total number of fields in the file
    int num_fields = 0;
    // the total number of null fields in the file
    int num_nulls = 0;
    rewind(file);
    for (int c = fgetc(file); c != EOF; c = fgetc(file))
    {
        if (c == RSV_EOR)
        {
            num_rows++;
        }
        if (c == RSV_EOV)
        {
            num_fields++;
        }
        if (c == RSV_NV)
        {
            num_nulls++;
        }
    }
    // number of bytes in the file
    long file_size = ftell(file);
    if (file_size == -1)
        return NULL;
    rewind(file);
    // printf("%lli bytes - %i rows - %i fields\n", file_size, num_rows, num_fields);
    // number of bytes in the RsvTable structure
    long table_size = sizeof(struct RsvTable) + num_rows * sizeof(struct RsvRow *);
    // number of bytes in all RsvRow structures combined
    long row_size = num_rows * sizeof(struct RsvRow *) + num_fields * sizeof(char *);
    // number of bytes for the null-terminated strings
    long data_size = file_size - num_nulls * 2 - num_rows;
    // block large enough for all relevant data
    char *base_ptr = malloc(table_size + row_size + data_size);
    // the block starts with an RsvTable; this will never change
    struct RsvTable *table = (struct RsvTable *)base_ptr;
    table->num_rows = num_rows;
    // after the RsvTable are the RsvRows. This pointer represents the row currently being worked on
    struct RsvRow *current_row = (struct RsvRow *)(base_ptr + table_size);
    // after the rows are the actual bytes of the file.
    char *data_ptr = base_ptr + table_size + row_size;
    // this is the pointer to the pointer in the RsvTable array of the RsvRow we're currently writing to
    struct RsvRow **current_row_array = table->rows;
    // this is the pointer to the pointer in the fields array of the RsvRow we're currently writing representing the field we're currently writing to
    char **current_field = current_row->fields;
    // this is the first character of the field we're currently working on
    char *field_tmp = NULL;

    // printf("Table @ %p - Row @ %p - Field @ %p - Data @ %p\n", table, current_row, current_field, data_ptr);
    for (int c = fgetc(file); c != EOF; c = fgetc(file))
    {
        if (c == EOF)
            break;
        if (field_tmp == NULL)
        {
            field_tmp = data_ptr;
            current_row->num_fields = 0;
        }
        if (c == RSV_EOR)
        {
            *current_row_array++ = current_row;
            // the next row will start at the end of this row's fields
            current_row = (struct RsvRow *)(current_field);
            current_field = current_row->fields;
            field_tmp = NULL;
        }
        else if (c == RSV_EOV)
        {
            *data_ptr++ = 0;
            *current_field++ = field_tmp;
            current_row->num_fields++;
            field_tmp = data_ptr;
        }
        else if (c == RSV_NV)
        {
            if (fgetc(file) != RSV_EOV)
                return NULL;
            *current_field++ = NULL;
            current_row->num_fields++;
            field_tmp = data_ptr;
        }
        else
        {
            *data_ptr++ = c;
        }
    }
    return table;
}

struct RsvRow *rsv_read_row(FILE *file)
{
    // the total number of fields in the row
    int num_fields = 0;
    // the number of null fields in the row
    int num_nulls = 0;
    fpos_t start_pos, end_pos;
    if (fgetpos(file, &start_pos) != 0)
        return NULL;
    // count the EOF chars
    for (int c = fgetc(file); c != EOF && c != RSV_EOR; c = fgetc(file))
    {
        if (c == RSV_EOV)
        {
            num_fields++;
        }
        else if (c == RSV_NV)
        {
            num_nulls++;
        }
    }
    if (fgetpos(file, &end_pos) != 0)
        return NULL;
    if (fsetpos(file, &start_pos) != 0)
        return NULL;
    long file_size = end_pos - start_pos;
    // printf("%lli bytes - %i fields\n", file_size, num_fields);
    // number of bytes in the RsvRow structure
    long row_size = sizeof(struct RsvRow *) + num_fields * sizeof(char *);
    // block large enough for all relevant data
    char *base_ptr = malloc(row_size + file_size - 2 * num_nulls);
    // after the RsvTable are the RsvRows. This pointer represents the row currently being worked on
    struct RsvRow *row = (struct RsvRow *)(base_ptr);
    // after the rows are the actual bytes of the file.
    char *data_ptr = base_ptr + row_size;
    // this is the pointer to the pointer in the fields array of the RsvRow we're currently writing representing the field we're currently writing to
    char **current_field = row->fields;
    // this is the first character of the field we're currently working on
    char *field_tmp = data_ptr;
    row->num_fields = 0;
    int c;
    // printf("Table @ %p - Row @ %p - Field @ %p - Data @ %p\n", table, current_row, current_field, data_ptr);
    for (c = fgetc(file); c != EOF; c = fgetc(file))
    {
        if (c == RSV_EOR)
        {
            break;
        }
        else if (c == RSV_EOV)
        {
            *data_ptr++ = 0;
            *current_field++ = field_tmp;
            row->num_fields++;
            field_tmp = data_ptr;
        }
        else if (c == RSV_NV)
        {
            if (fgetc(file) != RSV_EOV)
                return NULL;
            *data_ptr++ = 0;
            *data_ptr++ = 0;
            *current_field++ = NULL;
            row->num_fields++;
            field_tmp = data_ptr;
        }
        else
        {
            *data_ptr++ = c;
        }
    }
    if (c == EOF)
    {
        return NULL;
    }
    return row;
}

int rsv_write_row(struct RsvRow *row, FILE *file)
{
    int result;
    for (int i = 0; i < row->num_fields; i++)
    {
        char *field = row->fields[i];
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
        if (fputc(RSV_EOV, file) == EOF)
            return EOF;
    }
    return fputc(RSV_EOR, file);
}

int rsv_write_table(struct RsvTable *table, FILE *file)
{
    for (int i = 0; i < table->num_rows; i++)
    {
        if (rsv_write_row(table->rows[i], file) == EOF)
            return EOF;
    }
    return 0;
}