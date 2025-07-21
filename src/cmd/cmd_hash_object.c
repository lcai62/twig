#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "repo.h"
#include "object.h"
#include "utils.h"

int cmd_hash_object(int argc, char **argv, Repo *repo) {

    int write_flag = 0;

    int opt;

    while ((opt = getopt(argc, argv, "w")) != -1) {
        switch (opt) {
            case 'w':
                write_flag = 1;
                break;
            default:
                fprintf(stderr, "Usage: twig hash-object [-w] <file>\n");
                return 1;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected file path after options\n");
        fprintf(stderr, "Usage: twig hash-object [-w] <file>\n");
        return 1;
    }

    char *file_name = argv[optind];

    // check if file exists
    if (!path_exists_and_is_file(file_name)) {
        perror(file_name);
        return 1;
    }

    char *file_path = realpath(file_name, NULL);

    // read file contents
    char *file_contents = read_file(file_path);

    // build blob object
    TwigObject blob;
    blob.type = OBJ_BLOB;
    blob.contents = (unsigned char *) file_contents;
    blob.content_size = strlen(file_contents);

    // calculate hash
    unsigned char hash[SHA1_LENGTH];
    twigobject_hash(&blob, hash);

    char hex_hash[SHA1_LENGTH * 2 + 1];
    sha1_to_hex(hash, hex_hash);

    printf("%s\n", hex_hash);

    if (write_flag) {
        twigobject_write(&blob, repo->root_path);
    }

    free(file_contents);
    free(file_path);

    return 0;

}