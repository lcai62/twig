#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "repo.h"
#include "object.h"
#include "utils.h"

int cmd_cat_file(int argc, char **argv, Repo *repo) {

    int pretty_flag = 0;

    int opt;

    while ((opt = getopt(argc, argv, "p")) != -1) {
        switch (opt) {
            case 'p':
                pretty_flag = 1;
                break;
            default:
                fprintf(stderr, "Usage: twig cat-file [-p] <hash>\n");
                return 1;
        }
    }

    if (!pretty_flag || optind >= argc) { // TODO: temp for now
        fprintf(stderr, "Usage: twig cat-file [-p] <hash>\n");
        return 1;
    }

    char *hash = argv[optind];
    if (strlen(hash) != SHA1_STR_LENGTH) {
        fprintf(stderr, "Invalid hash\n");
        return 1;
    }

    // construct paths
    // .twig/objects/xx/yyyy... path
    char *objects_dir = ".twig/objects";

    char *objects_path = build_path(repo->root_path, objects_dir);

    char subdir[3];
    strncpy(subdir, hash, 2);
    subdir[2] = '\0';

    char *file_name = hash + 2; // file name of object is hash without first byte

    char *dir_path = build_path(objects_path, subdir);
    char *file_path = build_path(dir_path, file_name);

    // check if file exists
    if (!path_exists_and_is_file(file_path)) {
        perror(file_path);
        return 1;
    }

    TwigObject *twigObject = twigobject_read(hash, repo->root_path);
    if (twigObject == NULL) {
        exit(1);
    }

    printf("%s\n", twigObject->contents);

    return 0;
}