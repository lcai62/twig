#ifndef TWIG_UTILS_H
#define TWIG_UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

bool path_exists_and_is_dir(char *path);
bool path_exists_and_is_file(char *path);

int create_dir(char *path);

int write_file(char *path, char *content);
char *read_file(char *path);

char *get_abs_cwd();

char *build_path(const char *base, const char *subpath);

int sha1_to_hex(const unsigned char *hash, char *hex_out);
int hex_to_sha1(const char *hex_str, unsigned char *sha1_out);

uint32_t mode_for_path(const char *path);

void _mkdir(const char *dir);

#endif //TWIG_UTILS_H
