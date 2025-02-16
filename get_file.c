#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "utilities.h"

char *read_file(char *file_path)
{
    FILE *file = fopen(file_path, "rb"); 
    if (file == NULL) { 
        perror("Send 404. File not found."); 
        return NULL; 
    } 
 
    fseek(image_file, 0L, SEEK_END); 
    unsigned long file_size = ftell(image_file); 
    fseek(image_file, 0L, SEEK_SET); 

    char *file_data = (char *)malloc(file_size);

    size_t bytes_read = fread(file_data, 1, file_size, file);
    fclose(file);
    
    return file_data;
}
