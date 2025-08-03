#ifndef TWIG_COMMIT_H
#define TWIG_COMMIT_H

#include "repo.h"
#include "object.h"

int commit_create(Repo *repo, char *tree_hex, char **parents, int num_parents, char *commit_message, unsigned char out_sha1[SHA1_LENGTH]);
 
#endif //TWIG_COMMIT_H

