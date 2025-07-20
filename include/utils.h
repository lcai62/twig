#ifndef TWIG_UTILS_H
#define TWIG_UTILS_H

#include <stddef.h>


void create_dir(char *path);

void write_file(char *path, char *content);
char *read_file(char *path);

void build_path(char *buffer, size_t buffer_size, char *base, char *subpath);

#endif //TWIG_UTILS_H
