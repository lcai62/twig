#include "utils.h"
#include "object.h"
#include <stdlib.h>
#include <stdio.h>

#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>
#include <errno.h>


bool path_exists_and_is_dir(char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    return S_ISDIR(st.st_mode);
}

bool path_exists_and_is_file(char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    return S_ISREG(st.st_mode);
}

int create_dir(char *abs_path) {
    if (mkdir(abs_path, 0755) != 0) {
        return -1;
    }
    return 0;
}

int write_file(char *path, char *content) {
    FILE *f = fopen(path, "w");
    if (!f) {
        return -1;
    }
    if (fputs(content, f) == EOF) {
        int err = errno;
        fclose(f);
        errno = err;
        return -1;
    }
    fclose(f);
    return 0;
}

char *read_file(char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        return NULL;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return NULL;
    }

    long int file_size = ftell(f);
    if (file_size < 0) {
        fclose(f);
        return NULL;
    }

    rewind(f);

    char *content = malloc(sizeof(char) * file_size);
    if (!content) {
        fclose(f);
        return NULL;
    }


    size_t size_read = fread(content, 1, (size_t)file_size, f);
    if (size_read != (size_t)file_size) {
        free(content);
        fclose(f);
        return NULL;
    }

    fclose(f);
    return content;
}


char *build_path(const char *base, const char *subpath) {
    if (!base || !subpath) {
        errno = EINVAL;
        return NULL;
    }

    size_t len = strlen(base) + strlen(subpath) + 2; // '/' and '\0'
    char *result = malloc(len);
    if (!result) {
        return NULL;
    }
    snprintf(result, len, "%s/%s", base, subpath);
    return result;
}

char *get_abs_cwd() {
    char buf[PATH_MAX];
    char *cwd = getcwd(buf, sizeof(buf));
    if (!cwd) return NULL;
    return realpath(cwd, NULL);
}

int sha1_to_hex(const unsigned char *sha1_hash, char *hex_out) {
    if (!sha1_hash || !hex_out) {
        return -1;
    }

    for (int i = 0; i < SHA1_LENGTH; i++) {
        sprintf(hex_out + i * 2, "%02x", sha1_hash[i]);
    }
    hex_out[SHA1_LENGTH * 2] = '\0';
    return 0;
}

int hex_to_sha1(const char *hex_str, unsigned char *sha1_out) {
    if (!hex_str || !sha1_out) {
        errno = EINVAL;
        return -1;
    }

    size_t len = strlen(hex_str);
    if (len != SHA1_LENGTH * 2) {
        errno = EINVAL;
        return -1;
    }

    for (int i = 0; i < SHA1_LENGTH; i++) {
        unsigned int byte;
        if (sscanf(hex_str + (i * 2), "%2x", &byte) != 1) {
            errno = EINVAL;
            return -1;
        }
        sha1_out[i] = (unsigned char)byte;
    }
    return 0;
}


uint32_t mode_for_path(const char *path) {
    struct stat st;
    if (lstat(path, &st) != 0) {
        errno = ENOENT;
        return 0;
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