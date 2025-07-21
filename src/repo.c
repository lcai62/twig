#include "repo.h"
#include "utils.h"
#include <limits.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int is_curr_twig_repo(char *path);
static char *find_twig_repo(char *path);

Repo *repo_open(char *rel_path) {
    char *abs_path = realpath(rel_path, NULL);

    char *twig_root = find_twig_repo(abs_path);
    if (twig_root == NULL) {
        return NULL;
    }

    char *dottwig_path = build_path(twig_root, ".twig");
    char *objects_path = build_path(twig_root, ".twig/objects");
    char *refs_path = build_path(twig_root, ".twig/refs");

    Repo *repo = malloc(sizeof(Repo));
    repo->root_path = twig_root;
    repo->dottwig_path = dottwig_path;
    repo->objects_path = objects_path;
    repo->refs_path = refs_path;

    return repo;
}

/* helpers */
static int is_curr_twig_repo(char *path) {

    // check if .twig directory exists
    char *buffer;
    buffer = build_path(path, ".twig");
    if (!path_exists_and_is_dir(buffer)) {
        return 0;
    }

    buffer = build_path(path, ".twig/HEAD");
    if (!path_exists_and_is_file(buffer)) {
        return 0;
    }

    buffer = build_path(path, ".twig/objects");
    if (!path_exists_and_is_dir(buffer)) {
        return 0;
    }

    buffer = build_path(path, ".twig/refs/heads");
    if (!path_exists_and_is_dir(buffer)) {
        return 0;
    }

    buffer = build_path(path, ".twig/refs/tags");
    if (!path_exists_and_is_dir(buffer)) {
        return 0;
    }

    return 1;
}

static char *find_twig_repo(char *path) {

    char curr_path[strlen(path) + 1];
    strncpy(curr_path, path, strlen(path));
    curr_path[strlen(path)] = '\0';

    while (1) {
        if (is_curr_twig_repo(curr_path)) {
            return strdup(curr_path); // curr_path is stack allocated
        }

        char *last_slash = strrchr(curr_path, '/');
        if (last_slash == NULL) {
            break;
        }

        if (last_slash == curr_path) {
            // we are at the root directory
            curr_path[1] = '\0';

            // edge case: root is twig repo
            if (is_curr_twig_repo(curr_path)) {
                return strdup(curr_path);
            }
            break;
        }
        else {
            *last_slash = '\0';
        }

    }

    return NULL;

}


