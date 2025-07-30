#include "utils.h"
#include "object.h"
#include <stdlib.h>
#include <stdio.h>

#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>


int path_exists_and_is_dir(char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;
    }
    return S_ISDIR(st.st_mode);
}

int path_exists_and_is_file(char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;
    }
    return S_ISREG(st.st_mode);
}

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


    size_t size_read = fread(content, 1, (size_t)file_size, f);
    if (size_read != (size_t)file_size) {
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

void sha1_to_hex(const unsigned char *sha1_hash, char *hex_out) {
    for (int i = 0; i < SHA1_LENGTH; i++) {
        sprintf(hex_out + i * 2, "%02x", sha1_hash[i]);
    }
    hex_out[SHA1_LENGTH * 2] = '\0';
}

void hex_to_sha1(const char *hex_str, unsigned char *sha1_out) {
    if (strlen(hex_str) != SHA1_LENGTH * 2) {
        sha1_out = NULL;
        fprintf(stderr, "--cacheinfo cannot add\n");
        exit(1);
    }

    for (int i = 0; i < SHA1_LENGTH; i++) {
        unsigned int byte;
        if (sscanf(hex_str + (i * 2), "%2x", &byte) != 1) {
            fprintf(stderr, "--cacheinfo cannot add\n");
            exit(1);
        }
        sha1_out[i] = (unsigned char)byte;
    }
}


uint32_t mode_for_path(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        fprintf(stderr, "mode_for_path: file not found\n");
        exit(1);
    }

    if (S_ISLNK(st.st_mode)) {
        return 0120000; // symb link
    }
    if (S_ISDIR(st.st_mode)) {
        return 0040000; // directory
    }

    // executable file
    if (st.st_mode & 0111) {
        return 0100755;
    }

    return 0100644; // reg file
}