#ifndef TWIG_TREE_H
#define TWIG_TREE_H

#include <stddef.h>
#include "repo.h"
#include "index.h"
#include "object.h"

typedef struct {
    int mode;
    char *file_name;
    unsigned char sha1[SHA1_LENGTH];
} TreeEntry;


int tree_write_from_index(Repo *repo, IndexEntry *entries, int num_entries, unsigned char out_sha1[SHA1_LENGTH]);


#endif //TWIG_TREE_H
