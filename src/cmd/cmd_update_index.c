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

    IndexEntry *entries;
    int num_entries;
    index_read_all(repo, &entries, &num_entries);

    if (cacheinfo_mode) {
        if (optind + 3 > argc) {
            fprintf(stderr, "Error: --cacheinfo requires <mode> <sha1> <path>\n");
            return 1;
        }

        char *mode_str = argv[optind];
        char *hex_str = argv[optind+1];
        char *path_str = argv[optind+2];

        // find existing entry in file
        int found = -1;
        for (int i = 0; i < num_entries; i++) {
            if (strcmp(entries[i].rel_path, path_str) == 0) {
                found = i;
                break;
            }
        }

        IndexEntry new_entry;
        new_entry.mode = (int) strtol(mode_str, NULL, 8);
        hex_to_sha1(hex_str, new_entry.sha1);
        new_entry.stage_num = NORMAL;
        new_entry.path_len = (int) strlen(path_str);
        new_entry.rel_path = strdup(path_str);

        if (found >= 0) {
            entries[found].mode = new_entry.mode;
            memcpy(entries[found].sha1, new_entry.sha1, SHA1_LENGTH);
        }
        else if (add_mode) {
            entries = realloc(entries, (num_entries + 1) * sizeof(IndexEntry));
            entries[num_entries] = new_entry;
            num_entries++;
        }
        else { // error cond
            fprintf(stderr, "Path '%s' not in index (use --add to insert)\n", path_str);
            return 1;
        }
    }

    else if (add_mode) {

        if (optind >= argc) {
            fprintf(stderr, "Error: --add requires <file>\n");
            return 1;
        }

        // loop through every file
        for (int i = optind; i < argc; i++) {

            char *file_name = argv[i];

            /* hash and write blob */
            unsigned char sha1[SHA1_LENGTH];
            if (hash_file_as_blob(repo, file_name, 1, sha1) != 0) {
                perror("hash-object");
                continue;
            }

            /* Check if already exists in index */
            int found = -1;
            for (int j = 0; j < num_entries; j++) {
                if (strcmp(entries[j].rel_path, file_name) == 0) { 
                    found = j; 
                    break; 
                }
            }

            if (found >= 0) {
                memcpy(entries[found].sha1, sha1, SHA1_LENGTH);
                entries[found].mode = mode_for_path(file_name);
            } else {
                entries = realloc(entries, (num_entries + 1) * sizeof(IndexEntry));
                entries[num_entries].mode = mode_for_path(file_name);
                memcpy(entries[num_entries].sha1, sha1, SHA1_LENGTH);
                entries[num_entries].stage_num = NORMAL;
                entries[num_entries].rel_path = strdup(file_name);
                entries[num_entries].path_len = strlen(file_name);
                num_entries++;
            }
        }
    }

    int res = index_write_all(repo, entries, num_entries);

    return res;

}