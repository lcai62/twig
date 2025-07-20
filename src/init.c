#include "object.h"
#include "utils.h"
#include "init.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>


void twig_init(char *path) {

    create_dir(".twig");

    create_dir(".twig/objects");
    create_dir(".twig/refs");
    create_dir(".twig/refs/heads");
    create_dir(".twig/refs/tags");

    write_file(".twig/HEAD", "ref: refs/heads/master\n");

    printf("Initialized empty Twig repository in %s\n", path);

}