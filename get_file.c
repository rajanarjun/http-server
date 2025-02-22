#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "utilities.h"

FILE *open_file(char *file_path)
{
    FILE *file = fopen(file_path, "rb"); 
    if (file == NULL) { 
        perror("Send 404. File not found."); 
        return NULL; 
    } 

    return file;
}

unsigned long get_file_size(FILE *opened_file)
{
    fseek(opened_file, 0L, SEEK_END); 
    unsigned long file_size = ftell(opened_file); 
    fseek(opened_file, 0L, SEEK_SET); 

    return file_size;
}

char *get_file_data(FILE *opened_file, unsigned long file_size)
{
    char *file_data = malloc(file_size);
    size_t bytes_read = fread(file_data, 1, file_size, opened_file);
    
    return file_data;
}

void close_file(FILE *opened_file)
{
    fclose(opened_file);
}




