#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "repo.h"
#include "object.h"
#include "utils.h"
#include "index.h"


static int print_tree_recursive(Repo *repo, char sha_hex[SHA1_STR_LENGTH], const char *prefix, int recursive) {
    
    /* read object */
    TwigObject *tree_obj = twigobject_read(sha_hex, repo->root_path);
    if (tree_obj == NULL) {
        fprintf(stderr, "ls-tree: invalid object\n");
        exit(1);
    }
    if (tree_obj->type != OBJ_TREE) {
        fprintf(stderr, "ls-tree: invalid object type\n");
        exit(1);
    }


    /* format: 
        <mode as octal><space><name><NULL><20 byte sha1>
    */
    char *curr = (char *)tree_obj->contents;
    char *end = curr + tree_obj->content_size;

    while (curr < end) {

        /* parse mode */
        int mode = (int)strtol(curr, &curr, 8);
        curr++;

        /* parse filename */
        char *filename_start = curr;
        char *filename_end = strchr(curr, '\0');
        int filename_length = filename_end - curr;
        curr += filename_length + 1;

        /* parse sha1 */
        unsigned char sha1[SHA1_LENGTH];
        memcpy(sha1, curr, SHA1_LENGTH);
        curr += SHA1_LENGTH;

        char *type;
        if (mode & 040000) {
            type = "tree";
        }
        else {
            type = "blob";
        }

        char sha1_hex[SHA1_STR_LENGTH + 1];
        sha1_to_hex(sha1, sha1_hex);

        if (!recursive || !(mode & 040000)) {
           printf("%06o %s %s \t%s%.*s\n", mode, type, sha1_hex, prefix, filename_length, filename_start);
        }

        if (recursive && (mode & 040000)) {
            char *next_prefix = malloc(strlen(prefix) + filename_length + 2);
            sprintf(next_prefix, "%s%.*s/", prefix, (int)filename_length, filename_start);
            print_tree_recursive(repo, sha1_hex, next_prefix, recursive);
        }

    }

    return 0;
        

}

int cmd_ls_tree(int argc, char **argv, Repo *repo) {

    static struct option long_opts[] = {
            {"recursive", no_argument, 0, 'r'},
            {0, 0, 0, 0}
    };

    int recursive_flag = 0;
    int opt;
    int opt_index = 0;

    while ((opt = getopt_long(argc, argv, "r", long_opts, &opt_index)) != -1) {
        switch (opt) {
            case 'r':
                recursive_flag = 1;
                break;
            default:
                fprintf(stderr, "Usage: twig ls-tree [-r|--recursive] <tree-hash>\n");
                return 1;
        }
    }

    /* only hex for now */
    char *arg = argv[optind];
    if (!arg || strlen(arg) != SHA1_STR_LENGTH) {
        fprintf(stderr, "ls-tree: only 40-hex sha1\n");
        return 1;
    }

    return print_tree_recursive(repo, arg, "", recursive_flag);

}