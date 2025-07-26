#ifndef TWIG_INDEX_H
#define TWIG_INDEX_H

#define INDEX_SIGNATURE "DIRC"
#define INDEX_VERSION 2

#include "object.h"
#include "repo.h"

typedef enum {
    NORMAL,
    BASE,
    OURS,
    THEIRS
} StageNum;

typedef struct {
    int mode;
    unsigned char sha1[SHA1_LENGTH];
    StageNum stage_num;
    char *rel_path;
    int path_len;
} IndexEntry;

int index_write_all(Repo *repo, IndexEntry *entries, int num_entries);
int index_read_all(Repo *repo, IndexEntry **out_entries, int *out_num_entries);

#endif //TWIG_INDEX_H
