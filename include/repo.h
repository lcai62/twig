#ifndef TWIG_REPO_H
#define TWIG_REPO_H

#include <limits.h>


typedef struct {
    char *root_path;
    char *dottwig_path;
    char *objects_path;
    char *refs_path;
} Repo;


/**
 * Finds the Twig root, if it exists
 * @param rel_path
 * @return Returns the repo struct, filled in if rel_path is a twig repository, NULL otherwise
 */
Repo *repo_open(char *rel_path);



#endif //TWIG_REPO_H
