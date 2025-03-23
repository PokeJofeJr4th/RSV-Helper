## RSV-Helper

An [RSV](https://github.com/Stenway/RSV-Specification) library for C.

### Usage

The simplest way to use this library is with the table functions. The following snippet prints an RSV file in CSV format.

```c
#include "rsv.h"
#include <stdio.h>

int main() {
    FILE *file = fopen("my_rsv_file", "r");
    struct RsvTable *table = rsv_read_table(file);
    print_table(table);
    fclose(file);
}


void print_table(struct RsvTable *table) {
    for (int i = 0; i<table->num_rows; i++) {
        struct RsvRow *row = table->rows[i];
        for (int j=0; j<row->num_fields; j++) {
            if (j != 0) {
                putc(',');
            }
            char *field = row->fields[j];
            if (field == NULL) {
                printf("(null)");
            } else {
                printf("%s", field);
            }
        }
        putc('\n');
    }
}
```
