#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "file_info.h"
#include "mime_type.h"

int load_file(const char *filename, const char *parent_directory, FileInfo *file) {

    char *full_path = malloc(strlen(parent_directory) + strlen(filename) + 1);
    if (full_path == NULL) {
        printf("[Error] Could not allocate memory for uri path.\n");
        return -1;
    }
    strcpy(full_path, parent_directory);
    strcat(full_path, filename);

    FILE *fp = fopen(full_path, "rb"); 
    if (fp == NULL) { 
        perror("Send 404. File not found."); 
        free(full_path);
        return -1; 
    } 

    fseek(fp, 0L, SEEK_END); 
    unsigned long file_size = ftell(fp); 
    fseek(fp, 0L, SEEK_SET); 

    char *file_data = malloc(file_size);
    if (file_data == NULL) {
        printf("[Error] Could not allocate memory for file data.\n");
        free(full_path);
        fclose(fp);
        return -1;
    }
    size_t bytes_read = fread(file_data, 1, file_size, fp);
    fclose(fp);

    if (bytes_read < (size_t)file_size) {
        perror("[Error] Could not read file.\n");
        free(full_path);
        free(file_data);
        return -1;
    }

    char *file_mime_type = get_mime_type(filename);

    file->path = full_path;
    file->size = file_size;
    file->data = file_data;
    file->mime_type = file_mime_type;

    return 0;
}


void free_file(FileInfo *file) {
    free(file->path);
    free(file->data);
    file->size = 0;
    file->data = NULL;
    file->mime_type = NULL;
}
