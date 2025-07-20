#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>


void create_dir(char *path) {
    if (mkdir(path, 0755) != 0) {
        perror(path);
        exit(1);
    }
}

void write_file(char *path, char *content) {
    FILE *f = fopen(path, "w");
    if (!f) {
        perror(path);
        exit(1);
    }
    fputs(content, f);
    fclose(f);
}

char *read_file(char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        perror(path);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long int file_size = ftell(f);
    rewind(f);

    char *content = malloc(sizeof(char) * file_size);

    if (fread(content, 1, file_size, f) != file_size) {
        perror("fread");
        fclose(f);
        exit(1);
    }
    fclose(f);
    return content;
}


void build_path(char *buffer, size_t buffer_size, char *base, char *subpath) {
    snprintf(buffer, buffer_size, "%s/%s", base, subpath);
}
