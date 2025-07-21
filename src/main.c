#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include "object.h"
#include "cmd/cmd_init.h"
#include "repo.h"
#include "utils.h"


static struct {
    char *command_name;
    int (*fn)(int, char **, Repo *);
} cmd_table[] = {
        {"init", cmd_init}
};


int main(int argc, char **argv) {

    if (argc < 2) {
        fprintf(stderr, "Usage: twig <command>\n");
        return 1;
    }

    Repo *repo = repo_open(".");

//    if (repo == NULL) {
//        fprintf(stderr, "Not a twig repo\n");
//    }
//    else {
//        fprintf(stderr, "Is a twig repo\n");
//        fprintf(stderr, "root path: %s\ndottwig path %s\nobjects path: %s\nrefs path: %s\n", repo->root_path, repo->dottwig_path, repo->objects_path, repo->refs_path);
//    }


    for (size_t i = 0; i < sizeof(cmd_table) / sizeof(cmd_table[0]); i++) {
        if (strcmp(cmd_table[i].command_name, argv[1]) == 0) {
            return cmd_table[i].fn(argc - 2, argv + 2, repo);
        }
    }
    fprintf(stderr, "Unknown command: %s\n", argv[1]);
    exit(1);
}


//
//
//    else if (strcmp(argv[1], "check-repo") == 0) {
//        printf("status\n");
//        char buf[PATH_MAX];
//        char *cwd = getcwd(buf, sizeof(buf));
//        printf("cwd: %s\n", cwd);
//
//        char *twig_root = find_twig_repo(cwd);
//        if (twig_root != NULL) {
//            printf("twig root located at: %s\n", twig_root);
//        }
//        else {
//            printf("not a twig repo\n");
//        }
//    }
//
//    else if (strcmp(argv[1], "add") == 0) {
//        if (argc < 3) {
//            printf("not enough arguments\n");
//        }
//
//        char buf[PATH_MAX];
//        char *cwd = getcwd(buf, sizeof(buf));
//        printf("cwd: %s\n", cwd);
//
//        char *file_name = argv[2];
//        printf("adding file %s\n", file_name);
//
//        // find the repo root
//        char *twig_root = find_twig_repo(cwd);
//        if (!twig_root) {
//            fprintf(stderr, "twig root\n");
//        }
//        printf("twig root: %s\n", twig_root);
//
//        // get absolute file path
//        char *file_path = realpath(file_name, NULL);
//        printf("real file path: %s\n", file_path);
//
//        // read file contents
//        char *file_contents = read_file(file_path);
//
//        // build blob object
//        TwigObject blob;
//        blob.type = OBJ_BLOB;
//        blob.contents = (unsigned char *) file_contents;
//        blob.content_size = strlen(file_contents);
//
//        // write blob object
//        twigobject_write(&blob, twig_root);
//        printf("wrote blob\n");
//
//
//    }


