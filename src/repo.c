#include "repo.h"
#include "utils.h"
#include <limits.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>


int path_exists_and_is_dir(char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;
    }
    return S_ISDIR(st.st_mode);
}

int path_exists_and_is_file(char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;
    }
    return S_ISREG(st.st_mode);
}

int is_curr_twig_repo(char *path) {

    // check if .twig directory exists
    char buffer[PATH_MAX];
    build_path(buffer, PATH_MAX, path, ".twig");
    if (!path_exists_and_is_dir(buffer)) {
        return 0;
    }

    build_path(buffer, PATH_MAX, path, ".twig/HEAD");
    if (!path_exists_and_is_file(buffer)) {
        return 0;
    }

    build_path(buffer, PATH_MAX, path, ".twig/objects");
    if (!path_exists_and_is_dir(buffer)) {
        return 0;
    }

    build_path(buffer, PATH_MAX, path, ".twig/refs/heads");
    if (!path_exists_and_is_dir(buffer)) {
        return 0;
    }

    build_path(buffer, PATH_MAX, path, ".twig/refs/tags");
    if (!path_exists_and_is_dir(buffer)) {
        return 0;
    }

    return 1;
}

char *find_twig_repo(char *path) {

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
