#include "rsv.c"

int main()
{
    FILE *file = fopen("test.rsv", "w");
    rsv_write_field("1234", file);
    rsv_write_field(NULL, file);
    rsv_finish_row(file);
    rsv_write_field("Jake", file);
    rsv_write_field(";-;-;;;", file);
    rsv_write_field("", file);
    rsv_finish_row(file);
    fclose(file);

    file = fopen("../moku.rsv", "r");
    char my_buf[7];
    while (1)
    {
        int result = rsv_read_field(file, my_buf, 6);
        if (result == EOF)
        {
            puts("EOF");
            break;
        }
        else if (result == RSV_EOR)
        {
            puts("EOR");
        }
        else if (result == RSV_NV)
        {
            puts("NULL");
        }
        else
        {
            printf("FIELD: %s\n", my_buf);
        }
    }

    rewind(file);

    struct RsvTable *table = rsv_read_table(file);

    for (int i = 0; i < table->num_rows; i++)
    {
        putchar('[');
        struct RsvRow *row = table->rows[i];
        for (int i = 0; i < row->num_fields; i++)
        {
            printf(" <%s> ", row->fields[i]);
        }
        printf("]\n");
    }

    rewind(file);

    struct RsvRow *row;

    while (1)
    {
        row = rsv_read_row(file);
        if (row == NULL)
        {
            break;
        }
        putchar('[');
        for (int i = 0; i < row->num_fields; i++)
        {
            printf(" <%s> ", row->fields[i]);
        }
        printf("]\n");
    }

    fclose(file);
}
