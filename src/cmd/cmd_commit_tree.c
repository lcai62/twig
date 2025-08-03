#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "repo.h"
#include "object.h"
#include "utils.h"
#include "index.h"
#include "commit.h"

int cmd_commit_tree(int argc, char **argv, Repo *repo) {

    static struct option long_opts[] = {
            {"message", required_argument, 0, 'm'},
            {"parent", required_argument, 0, 'p'},
            {0, 0, 0, 0}
    };

    
    int opt;

    char *commit_msg = NULL;
    char **parents;
    int num_parents = 0;
    int max_parents = 0;
    while ((opt = getopt_long(argc, argv, "m:p:", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'm':
                commit_msg = optarg;
                break;
            case 'p':
                if (num_parents == max_parents) {
                    max_parents = max_parents ? max_parents * 2 : 4;
                    parents = realloc(parents, max_parents * sizeof(char *));
                }
                parents[num_parents] = optarg;
                num_parents++;
                break;
            default:
                fprintf(stderr, "Usage: twig commit-tree [-p <parent>] <tree>\n");
                return 1;
        }
    }

    if (optind >= argc || !commit_msg) {
        fprintf(stderr, "Usage: twig commit-tree <tree> [-p <parent] -m <msg>\n");
        return 1;
    }

    char *tree_hex = argv[optind];
    if (!tree_hex || strlen(tree_hex) != SHA1_STR_LENGTH) {
        fprintf(stderr,"commit-tree: tree must be full 40-hex\n");
        return 1;     
    }

    unsigned char sha1[SHA1_LENGTH];
    commit_create(repo, tree_hex, parents, num_parents, commit_msg, sha1);

    char hex[SHA1_STR_LENGTH + 1];
    sha1_to_hex(sha1, hex);
    printf("%s\n", hex);

    return 0;
}