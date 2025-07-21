#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>



void create_dir(char *abs_path) {
    if (mkdir(abs_path, 0755) != 0) {
        perror(abs_path);
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


char *build_path(const char *base, const char *subpath) {
    size_t len = strlen(base) + strlen(subpath) + 2; // '/' and '\0'
    char *result = malloc(len);
    snprintf(result, len, "%s/%s", base, subpath);
    return result;
}

char *get_abs_cwd() {
    char buf[PATH_MAX];
    char *cwd = getcwd(buf, sizeof(buf));
    return realpath(cwd, NULL);
}