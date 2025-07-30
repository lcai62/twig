#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "utils.h"
#include "cmd/cmd_init.h"
#include "repo.h"
#include "index.h"
#include <arpa/inet.h>

int cmd_init(int argc, char **argv, Repo *repo) {

    // shut the compiler up
    (void) argc;
    (void) argv;

    char *abs_cwd = get_abs_cwd();

    if (repo != NULL) {
        printf("Reinitialized existing Twig repository in %s\n", abs_cwd);
        return 0;
    }

    char *root = build_path(abs_cwd, ".twig");
    char *objects = build_path(root, "objects");
    char *refs = build_path(root, "refs");

    char *heads = build_path(refs, "heads");
    char *tags = build_path(refs, "tags");

    char *head_file = build_path(root, "HEAD");

    create_dir(root);
    create_dir(objects);
    create_dir(refs);
    create_dir(heads);
    create_dir(tags);
    write_file(head_file, "ref: refs/heads/master\n");

    // TODO: this is so bad
    FILE *f = fopen(".twig/index", "wb");
    if (!f) {
        perror("init index");
        exit(1);
    }
    fwrite("DIRC", 1, 4, f);

    // write_u32_be(f, 2);
    uint32_t be = htonl(2);
    fwrite(&be, sizeof(be), 1, f);
    // write_u32_be(f, 0);
    be = htonl(0);
    fwrite(&be, sizeof(be), 1, f);

    fclose(f);

    printf("Initialized empty Twig repository in %s\n", abs_cwd);

    free(root);
    free(objects);
    free(refs);
    free(head_file);
    free(abs_cwd);

    return 0;

}