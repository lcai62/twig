#ifndef TWIG_UTILS_H
#define TWIG_UTILS_H

#include <stddef.h>

int path_exists_and_is_dir(char *path);
int path_exists_and_is_file(char *path);

void create_dir(char *path);

void write_file(char *path, char *content);
char *read_file(char *path);

char *get_abs_cwd();

char *build_path(const char *base, const char *subpath);

void sha1_to_hex(const unsigned char *hash, char *hex_out);

#endif //TWIG_UTILS_H
