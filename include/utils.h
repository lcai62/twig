#ifndef TWIG_UTILS_H
#define TWIG_UTILS_H

#include <stddef.h>


void create_dir(char *path);

void write_file(char *path, char *content);
char *read_file(char *path);

char *get_abs_cwd();

char *build_path(const char *base, const char *subpath);

#endif //TWIG_UTILS_H
