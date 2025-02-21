
void process_response(int cfd, char *request_line);
char *get_mime_type(char *filename);

FILE *open_file(char *file_path);
unsigned long get_file_size(FILE *opened_file);
char *get_file_data(FILE *opened_file, unsigned long file_size);
void close_file(FILE *opened_file);
