#ifndef GET_FILE_H_
#define GET_FILE_H_

typedef struct {
    char *path;
    unsigned long size;
    char *data;
    char *mime_type;
} FileInfo;

int load_file(const char *filename, const char *parent_directory, FileInfo *file);
void free_file(FileInfo *file);

#endif 
