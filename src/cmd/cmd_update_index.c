#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "repo.h"
#include "object.h"
#include "utils.h"
#include "index.h"

int cmd_update_index(int argc, char **argv, Repo *repo) {



//    update-index — maintain staging/index file
//    --add
//    - reads the file contents, and adds it to the index file
//    --cacheinfo
//    - trusts the parameters you pass it, does not read file contents, adds whatever you enter to the index file
//    - only works if path already in the index file
//    --add --cacheinfo
//    - also trusts the parameters you pass it

    static struct option long_options[] = {
            {"add", no_argument, 0, 'a'},
            {"cacheinfo", no_argument, 0, 'c'},
            {0, 0, 0, 0}
    };

    int opt, option_index = 0;
    int add_mode = 0;
    int cacheinfo_mode = 0;

    while ((opt = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'a':
                add_mode = 1;
                break;
            case 'c':
                cacheinfo_mode = 1;
                break;
            default:
                fprintf(stderr, "Usage:\n");
                return 1;
        }
    }

    if (cacheinfo_mode) {
        if (optind + 3 > argc) {
            fprintf(stderr, "Error: --cacheinfo requires <mode> <sha1> <path>\n");
            return 1;
        }

        char *mode_str = argv[optind];
        char *hex_str = argv[optind+1];
        char *path_str = argv[optind+2];

        fprintf(stderr, "%s, %s, %s\n", mode_str, hex_str, path_str);

        IndexEntry entry;
        entry.mode = (int)strtol(mode_str, NULL, 8);
        hex_to_sha1(hex_str, entry.sha1);
        entry.stage_num = NORMAL;
        entry.path_len = strlen(path_str);
        entry.rel_path = strdup(path_str);

        int res = index_write_all(repo, &entry, 1);

    }


//    if (add_mode) {
//        if (optind >= argc) {
//            fprintf(stderr, "Mising file");
//            return 1;
//        }
//
//        const char *file_path = argv[optind];
//        fprintf(stderr, "file path: %s\n", file_path);
//
//    }


}