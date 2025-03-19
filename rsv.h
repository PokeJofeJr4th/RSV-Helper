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

/// @brief A table of RSV Rows. The size of the `rows` array must be equal to `num_rows`. Each entry in the `rows` array must be a pointer to a valid RsvRow structure
struct RsvTable
{
    int num_rows;
    struct RsvRow *rows[];
};

/// @brief A table of RSV fields. The size of the `fields` array must be equal to `num_fields`. Each entry in the `fields` array must be either `NULL` or a pointer to a valid null-terminated UTF8 string
struct RsvRow
{
    int num_fields;
    char *fields[];
};

/// @brief Read a single field from an RSV file
/// @param file a pointer to the file to read from
/// @param buf a buffer to store the field value in; will be zero-terminated
/// @param max_size the maximum size of field to read before truncating. `buf` must have at least this many bytes available.
/// @return EOF if there is no field and the file has ended; RSV_EOV if the field was successfully read into the buffer; RSV_NV if the field is null; RSV_EOR if there is no field and the row has ended; or RSV_ERROR if the file is formatted incorrectly
int rsv_read_field(FILE *file, char *buf, int max_size);

/// @brief Write a single field to an RSV file
/// @param field a pointer to a zero-terminated string to write to the file
/// @param file a pointer to the file to write to
/// @return File I/O return code
int rsv_write_field(const char *field, FILE *file);

/// @brief Mark the end of a row in an RSV file
/// @param file a pointer to the file to write to
/// @return File I/O return code
int rsv_finish_row(FILE *file);

/// @brief Read a table from a file into an RSV TAble structure
/// @param file a pointer to the file to write to
/// @return a table of the rows and fields in the file. All rows and fields are stored in an allocation directly after the RsvTable structure
struct RsvTable *rsv_read_table(FILE *file);

/// @brief Read a row from a file into an RSV Row structure
/// @param file a pointer to the file to write to
/// @return a row of fields. All fields are stored in a large allocation directly after the RsvRow structure
struct RsvRow *rsv_read_row(FILE *file);

/// @brief Write an RSV Row structure to a file
/// @param row a pointer to the row struct to write data from
/// @param file a pointer to the file to write to
/// @return File I/O return code
int rsv_write_row(struct RsvRow *row, FILE *file);

/// @brief Write an RSV Table structure to a file
/// @param table a pointer to the table struct to write data from
/// @param file a pointer to the file to write to
/// @return File I/O return code
int rsv_write_table(struct RsvTable *table, FILE *file);

#endif