#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "mime_type.h"

#define DEFAULT_MIME_TYPE "text/html"

char *get_mime_type(const char *filename)
{
    char *file_ext = strchr(filename, '.');
    if (file_ext == NULL) { return DEFAULT_MIME_TYPE; }

    if (strcmp(file_ext, ".html") == 0) { return "text/html"; }
    if (strcmp(file_ext, ".jpeg") == 0 || strcmp(file_ext, ".jpg") == 0) { return "image/jpg"; }
    if (strcmp(file_ext, ".txt") == 0) { return "text/plain"; }
    if (strcmp(file_ext, ".png") == 0) { return "image/png"; }
    if (strcmp(file_ext, ".ico") == 0) { return "image/x-icon"; }
    
    else { return DEFAULT_MIME_TYPE; }
}
