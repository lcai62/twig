#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "repo.h"
#include "object.h"
#include "utils.h"
#include "index.h"

int cmd_ls_files(int argc, char **argv, Repo *repo) {

    static struct option long_opts[] = {
            {"stage", no_argument, 0, 's'},
            {0, 0, 0, 0}
    };

    int show_stage = 0;
    int opt;
    int opt_index = 0;

    while ((opt = getopt_long(argc, argv, "", long_opts, &opt_index)) != -1) {
        switch (opt) {
            case 's':
                show_stage = 1;
                break;
            default:
                fprintf(stderr, "Usage: twig ls-files [--stage]\n");
                return 1;
        }
    }

    IndexEntry *entries;
    int n;
    index_read_all(repo, &entries, &n);

    for (int i = 0; i < n; ++i) {
        if (show_stage) {
            char hex[SHA1_LENGTH*2 + 1];
            sha1_to_hex(entries[i].sha1, hex);
            printf("%06o %s %d\t%s\n",
                   entries[i].mode, hex,
                   entries[i].stage_num,
                   entries[i].rel_path);
        } else {
            puts(entries[i].rel_path);
        }
    }

    return 0;

}