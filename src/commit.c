#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "tree.h"
#include "utils.h"
#include "commit.h"


#define APPEND(fmt, ...)                                                            \
    do {                                                                            \
        int _n = snprintf(NULL, 0, fmt, ##__VA_ARGS__); /* rest of the arguments */ \
        while (curr + _n + 1 > buffer_size) {                                       \
            buffer_size *= 2;                                                       \
            buffer = realloc(buffer, buffer_size);                                  \
        }                                                                           \
        curr += sprintf(buffer + curr, fmt, ##__VA_ARGS__);                         \
    } while(0)


static char *create_authors(void) {
    char *name = getenv("GIT_AUTHOR_NAME") ?: "default";
    char *email = getenv("GIT_AUTHOR_EMAIL") ?: "default@default.com";

    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);

    /* timezone */
    char tz[6];
    int off = (int)tm.tm_gmtoff;
    sprintf(tz, "%+03d%02d", off/3600, abs(off / 60) % 60);

    int len = snprintf(NULL, 0, "%s <%s> %ld %s", name, email, (long)t, tz);
    char *ret = malloc(len + 1); 

    snprintf(ret, len + 1, "%s <%s> %ld %s", name, email, (long)t, tz);
    return ret;
}


int commit_create(Repo *repo, char *tree_hex, char **parents, int num_parents, char *commit_message, unsigned char out_sha1[SHA1_LENGTH]) {

    int buffer_size = 1024;
    int curr = 0;

    char *buffer = malloc(buffer_size);

    APPEND("tree %s\n", tree_hex);

    for (int i = 0; i < num_parents; i++) {
        APPEND("parent %s\n", parents[i]);
    }
    
    char *signature = create_authors();
    APPEND("author %.*s\n", (int)strlen(signature), signature);
    APPEND("committer %.*s\n\n", (int)strlen(signature), signature);

    APPEND("%s\n", commit_message);

    TwigObject commit_obj;
    commit_obj.type = OBJ_COMMIT;
    commit_obj.content_size = curr;
    commit_obj.contents = (unsigned char *)buffer;

    twigobject_write(&commit_obj, repo->root_path);
    twigobject_hash(&commit_obj, out_sha1);

    return 0;

}