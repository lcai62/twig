#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include "repo.h"
#include "object.h"
#include "utils.h"

int cmd_update_ref(int argc, char **argv, Repo *repo) {

    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: twig update-ref <ref> <newsha> [<oldsha>]\n");
        return 1;
    }

    const char *refname  = argv[1];
    const char *new_sha  = argv[2];
    const char *old_sha  = (argc == 4) ? argv[3] : NULL;

    if (strlen(new_sha) != SHA1_STR_LENGTH || (old_sha && strlen(old_sha) != SHA1_STR_LENGTH)) {
        fprintf(stderr,"update-ref: sha1 must be 40 hex chars\n");
        return 1;
    }
    /* 
    variable ref_path to be set
    
    if head ->  
        if head is detatched (has 40 byte sha) -> we can just perform our update and return
        if head is pointing to a ref -> set ref_path

    now ref path is set -> refs/heads/<file>
    we can perform our update and return
    -> open the file
    -> check the sha1 hash inside if it matches old sha
    -> write new sha
    */
    char *ref_path;

    if (strcmp(refname, "HEAD") == 0) {

        char *head_path = build_path(repo->dottwig_path, "HEAD");
        FILE *f = fopen(head_path, "r");
        if (!f) {
            perror("fopen");
            return 1;
        }

        char *line = NULL;
        size_t len = 0;

        if (getline(&line, &len, f) == -1) {
            fclose(f);
            free(line);
            return 1;
        }
        fclose(f);

        line[strcspn(line, "\n")] = '\0';
        
        if (strncmp(line, "ref: ", 5) == 0) {
            ref_path = build_path(repo->dottwig_path, line + 5);
        }
        else {
            ref_path = head_path;
        }
        
    }
    else {
        ref_path = build_path(repo->dottwig_path, refname);
    }

    /* ref path is set */
    fprintf(stderr, "ref path is %s\n", ref_path);

    if (old_sha) {
        char *current_sha = NULL;
        size_t len_current_sha = 0;

        FILE *f = fopen(ref_path, "r");
        if (!f) {
            perror("fopen");
            return 1;
        }


        if (getline(&current_sha, &len_current_sha, f) == -1) {
            fclose(f);
            return 1;
        }
        fclose(f);

        current_sha[strcspn(current_sha, "\n")] = '\0';
        
        if (strcmp(current_sha, old_sha) != 0) {
            fprintf(stderr,"update-ref: ref is %s but expected %s\n", current_sha, old_sha);
            return 1;
        }

    }

    /* make sure parents exist and create if they dont */
    char *parent_dir = dirname(strdup(ref_path));
    _mkdir(parent_dir);
    
    /* atomic write */
    char lock_path[strlen(ref_path) + 6]; /* .lock\0 */
    sprintf(lock_path, "%s.lock", ref_path);

    int fd = open(lock_path, O_WRONLY | O_CREAT | O_EXCL, 0666); /* write only, create if doesnt exist, fail if already exists */
    if (fd < 0) {
        return 1;
    }

    size_t sha_len = strlen(new_sha);
    write(fd, new_sha, sha_len);
    write(fd, "\n", 1);
    fsync(fd); 
    close(fd);

    if (rename(lock_path, ref_path) != 0) {
        unlink(lock_path);
        return 1;
    }
    return 0;


    




}