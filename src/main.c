#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "init.h"
#include "repo.h"
#include "utils.h"
#include <limits.h>
#include <stdlib.h>
#include "object.h"





int main(int argc, char **argv) {

    if (argc < 2) {
        fprintf(stderr, "Usage: twig <command>\n");
        return 1;
    }

    if (strcmp(argv[1], "init") == 0) {
        printf("init\n");
        char buf[PATH_MAX];
        char *cwd = getcwd(buf, sizeof(buf));
        twig_init(cwd);
    }


    else if (strcmp(argv[1], "check-repo") == 0) {
        printf("status\n");
        char buf[PATH_MAX];
        char *cwd = getcwd(buf, sizeof(buf));
        printf("cwd: %s\n", cwd);

        char *twig_root = find_twig_repo(cwd);
        if (twig_root != NULL) {
            printf("twig root located at: %s\n", twig_root);
        }
        else {
            printf("not a twig repo\n");
        }
    }

    else if (strcmp(argv[1], "add") == 0) {
        if (argc < 3) {
            printf("not enough arguments\n");
        }

        char buf[PATH_MAX];
        char *cwd = getcwd(buf, sizeof(buf));
        printf("cwd: %s\n", cwd);

        char *file_name = argv[2];
        printf("adding file %s\n", file_name);

        // find the repo root
        char *twig_root = find_twig_repo(cwd);
        if (!twig_root) {
            fprintf(stderr, "twig root\n");
        }
        printf("twig root: %s\n", twig_root);

        // get absolute file path
        char *file_path = realpath(file_name, NULL);
        printf("real file path: %s\n", file_path);

        // read file contents
        char *file_contents = read_file(file_path);

        // build blob object
        TwigObject blob;
        blob.type = OBJ_BLOB;
        blob.contents = (unsigned char *) file_contents;
        blob.content_size = strlen(file_contents);

        // write blob object
        twigobject_write(&blob, twig_root);
        printf("wrote blob\n");


    }

}
