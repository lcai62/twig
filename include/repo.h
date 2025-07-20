#ifndef TWIG_REPO_H
#define TWIG_REPO_H

int path_exists_and_is_dir(char *path);
int path_exists_and_is_file(char *path);

/**
 * Finds the Twig root, if it exists
 * @param path
 * @return Path to the root of the twig repo, NULL if it does not exist
 */
char *find_twig_repo(char *path);



#endif //TWIG_REPO_H
