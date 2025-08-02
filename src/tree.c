#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include "tree.h"
#include "utils.h"


/* serializes entries first to last into a tree object, and return its hash */
static void build_flat_tree_node(Repo *repo, IndexEntry *entries, int first, int last, unsigned char sha1_out[SHA1_LENGTH]) {

    int payload_length = 0;
    for (int i = first; i < last; i++) {
        payload_length += snprintf(NULL, 0, "%o ", entries[i].mode);
        payload_length += entries[i].path_len + 1; // null terminator
        payload_length += SHA1_LENGTH;
    }

    unsigned char *buffer = malloc(payload_length);
    unsigned char *curr = buffer;

    for (int i = first; i < last; i++) {
        int n = sprintf((char *)curr, "%o ", entries[i].mode);
        curr += n;

        memcpy(curr, entries[i].rel_path, entries[i].path_len);
        curr += entries[i].path_len;

        *curr = '\0';
        curr++;


        memcpy(curr, entries[i].sha1, SHA1_LENGTH);
        curr += SHA1_LENGTH;
    }

    assert(curr == buffer + payload_length);


    TwigObject tree_obj;
    tree_obj.type = OBJ_TREE;
    tree_obj.contents = buffer;
    tree_obj.content_size = payload_length;


    twigobject_write(&tree_obj, repo->root_path);
    twigobject_hash(&tree_obj, sha1_out);
    free(buffer);

}

/* assumes that entries are qsorted */
static void write_tree_recursive_helper(Repo *repo, IndexEntry *entries, int first, int last, unsigned char sha1_out[SHA1_LENGTH]) {

    bool split_needed = false;
    for (int i = first; i < last && !split_needed; i++) {
        if (strchr(entries[i].rel_path, '/')) {
            split_needed = true;
        }
    }

    /* we are at a leaf, build the flat tree*/
    if (!split_needed) {
        build_flat_tree_node(repo, entries, first, last, sha1_out);
        return;
    }

    /* worst case: all entries are sub trees */
    TreeEntry *curr_entries = malloc((last - first) * sizeof(TreeEntry));
    int curr_entries_count = 0;
    
    int i = first;
    while (i < last) {
        char *slash = strchr(entries[i].rel_path, '/');

        /* file exists at current directory */
        if (!slash) {
            curr_entries[curr_entries_count].mode = entries[i].mode;
            curr_entries[curr_entries_count].file_name = strdup(entries[i].rel_path);
            memcpy(curr_entries[curr_entries_count].sha1, entries[i].sha1, SHA1_LENGTH);
            curr_entries_count++;
            i++;
            continue;
        }


        /* file in subdirectory */
        int dir_len = slash - entries[i].rel_path;

        /* find rows that share this subdirectory -> if exists, guaranteed to be next due to qsort precond */
        int j = i;
        while (
            j < last && 
            strncmp(entries[j].rel_path, entries[i].rel_path, dir_len) == 0 && // directory matches
            entries[j].rel_path[dir_len] == '/' // slash is at the same place
        ) {
            j++;
        }

        /* remove dir name for next recursive call */
        for (int k = i; k < j; k++) {
            entries[k].rel_path += dir_len + 1;
            entries[k].path_len -= dir_len + 1;
        }

        unsigned char child_sha[SHA1_LENGTH];
        write_tree_recursive_helper(repo, entries, i, j, child_sha);

        /* restore subdirectory paths */
        for (int k = i; k < j; k++) {
            entries[k].rel_path -= dir_len + 1;
            entries[k].path_len += dir_len + 1;
        }

        curr_entries[curr_entries_count].mode = 040000;
        curr_entries[curr_entries_count].file_name = strndup(entries[i].rel_path, dir_len);
        memcpy(curr_entries[curr_entries_count].sha1, child_sha, SHA1_LENGTH);

        curr_entries_count++;
        i = j;
    }

    /* finally, we have to build ourself into a tree node */
    IndexEntry *temp = calloc(curr_entries_count, sizeof(IndexEntry));
    for (int i = 0; i < curr_entries_count; i++) {
        temp[i].mode = curr_entries[i].mode;
        temp[i].rel_path = curr_entries[i].file_name;
        temp[i].path_len = strlen(curr_entries[i].file_name);
        memcpy(temp[i].sha1, curr_entries[i].sha1, SHA1_LENGTH);
        // stage num not needed
    }

    build_flat_tree_node(repo, temp, 0, curr_entries_count, sha1_out);

    for (int k = 0; k < curr_entries_count; k++) {
        free(curr_entries[k].file_name);
    }

    free(temp);
    free(curr_entries);

}

static int compare_index_paths(const void *a, const void *b) {
    const IndexEntry *temp1 = a, *temp2 = b;
    return strcmp(temp1->rel_path, temp2->rel_path);
}

int tree_write_from_index(Repo *repo, IndexEntry *entries, int num_entries, unsigned char out_sha1[SHA1_LENGTH]) {

    /* no merge conflicts */
    for (int i = 0; i < num_entries; i++) {
        if (entries[i].stage_num != NORMAL){
            fprintf(stderr, "twig: unmerged paths exists");
            return 1;
        }
    }

    qsort(entries, num_entries, sizeof(IndexEntry), compare_index_paths);
    
    write_tree_recursive_helper(repo, entries, 0, num_entries, out_sha1);

    return 0;
}