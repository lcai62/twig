#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include "object.h"
#include "cmd/cmd_init.h"
#include "cmd/cmd_hash_object.h"
#include "cmd/cmd_cat_file.h"
#include "cmd/cmd_update_index.h"
#include "cmd/cmd_ls_files.h"
#include "cmd/cmd_ls_tree.h"
#include "cmd/cmd_add.h"
#include "cmd/cmd_write_tree.h"
#include "cmd/cmd_commit_tree.h"
#include "cmd/cmd_update_ref.h"
#include "repo.h"
#include "utils.h"


static struct {
    char *command_name;
    int (*fn)(int, char **, Repo *);
} cmd_table[] = {
        {"init", cmd_init},
        {"hash-object", cmd_hash_object},
        {"cat-file", cmd_cat_file},
        {"update-index", cmd_update_index},
        {"ls-files", cmd_ls_files},
        {"ls-tree", cmd_ls_tree},
        {"add", cmd_add},
        {"write-tree", cmd_write_tree},
        {"commit-tree", cmd_commit_tree},
        {"update-ref", cmd_update_ref},
};


int main(int argc, char **argv) {

    if (argc < 2) {
        fprintf(stderr, "Usage: twig <command>\n");
        return 1;
    }

    Repo *repo = repo_open(".");


    for (size_t i = 0; i < sizeof(cmd_table) / sizeof(cmd_table[0]); i++) {
        if (strcmp(cmd_table[i].command_name, argv[1]) == 0) {
            return cmd_table[i].fn(argc - 1, argv + 1, repo);
        }
    }
    fprintf(stderr, "Unknown command: %s\n", argv[1]);
    exit(1);
}



