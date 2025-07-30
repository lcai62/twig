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
        fprintf(stderr, "Usage: twig hash-object [-w] <file>\n");
        return 1;
    }

    char *file_name = argv[optind];
    unsigned char sha1[SHA1_LENGTH];

    if (hash_file_as_blob(repo, file_name, write_flag, sha1) != 0) {
        perror("hash-object");
        return 1;
    }


    // convert hash
    char hex_hash[SHA1_LENGTH * 2 + 1];
    sha1_to_hex(sha1, hex_hash);
    printf("%s\n", hex_hash);
    return 0;

}