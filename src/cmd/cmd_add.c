#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "repo.h"
#include "object.h"
#include "utils.h"
#include "index.h"
#include "cmd/cmd_update_index.h"

int cmd_add(int argc, char **argv, Repo *repo) {

    if (argc < 2) {
        fprintf(stderr, "Usage: twig add <file>...\n");
        return 1;
    }

    char *new_argv[argc + 2];
    new_argv[0] = "update-index";
    new_argv[1] = "--add";
    for (int i = 1; i < argc; i++) {
        new_argv[i + 1] = argv[i];
    }
    return cmd_update_index(argc + 1, new_argv, repo);
}