#ifndef _RSV_DEFINED
#define _RSV_DEFINED

#include <stdio.h>

/// @brief RSV Format: End of Value character
#define RSV_EOV (255)
/// @brief RSV Format: Null Value character
#define RSV_NV (254)
/// @brief RSV Format: End of Row Character
#define RSV_EOR (253)
/// @brief RSV error code
#define RSV_ERROR (252)

struct RsvTable
{
    int num_rows;
    struct RsvRow *rows[];
};

struct RsvRow
{
    int num_fields;
    char *fields[];
};

int rsv_read_field(FILE *file, char *buf, int max_size);
int rsv_write_field(const char *field, FILE *file);
int rsv_finish_row(FILE *file);
struct RsvTable *rsv_read_table(FILE *file);
/// @brief Read a row to an RSV structure
/// @param file 
/// @return a row of fields
struct RsvRow *rsv_read_row(FILE *file);
int rsv_write_row(struct RsvRow *row, FILE *file);
int rsv_write_table(struct RsvTable *table, FILE *file);

#endif