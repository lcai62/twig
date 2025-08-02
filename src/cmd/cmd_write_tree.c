#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "repo.h"
#include "object.h"
#include "utils.h"
#include "index.h"
#include "tree.h"

int cmd_write_tree(int argc, char **argv, Repo *repo)
{
    (void)argc; (void)argv;

    IndexEntry *entries;
    int n;


    if (index_read_all(repo, &entries, &n) != 0) {
        return 1;
    }

    unsigned char sha1[SHA1_LENGTH];
    if (tree_write_from_index(repo, entries, n, sha1) != 0) {
        return 1;
    }

    char hex[SHA1_STR_LENGTH + 1];
    sha1_to_hex(sha1, hex);
    printf("%s\n", hex);

    return 0;
}