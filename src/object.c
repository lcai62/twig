#include "object.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "utils.h"
#include <openssl/sha.h>
#include <zlib.h>
#include <sys/stat.h>


char *object_to_string(enum TwigObjectType type) {
    switch (type) {
        case OBJ_BLOB:
            return "blob";
        case OBJ_TREE:
            return "tree";
        case OBJ_COMMIT:
            return "commit";
        case OBJ_TAG:
            return "tag";
        default:
            return NULL;
    }
}

void twigobject_hash(TwigObject *object, unsigned char *out_hash) {
    assert(object != NULL);
    assert(out_hash != NULL);

    char *header_type = object_to_string(object->type);
    size_t content_size = object->content_size;

    int header_length = snprintf(NULL, 0, "%s %zu", header_type, content_size);
    unsigned char *buffer = malloc(header_length + content_size + 1);
    snprintf((char *)buffer, header_length + 1, "%s %zu", header_type, object->content_size);
    buffer[header_length] = '\0';

    memcpy(buffer + header_length + 1, object->contents, object->content_size);

    SHA1(buffer, header_length + 1 + content_size, out_hash);

    free(buffer);

}


void twigobject_write(TwigObject *object, char *twig_root) {
    assert(object != NULL);

    // find sha1 hash
    unsigned char object_hash[SHA1_LENGTH];
    twigobject_hash(object, object_hash);



    // format as hex string
    char hex_hash[SHA1_LENGTH * 2 + 1];
    for (int i = 0; i < SHA1_LENGTH; i++) {
        sprintf(hex_hash + i * 2, "%02x", object_hash[i]);
    }
    hex_hash[SHA1_LENGTH * 2] = '\0';
    printf("object hash: %s\n", hex_hash);

    // check directory and file exists
    // .twig/objects/xx/yyyy... path
    char dir_path[PATH_MAX];
    char file_path[PATH_MAX];
    char objects_path[PATH_MAX];

    char *objects_dir = ".twig/objects";

    build_path(objects_path, PATH_MAX, twig_root, objects_dir);
    fprintf(stderr, "objects dir: %s\n", dir_path);

    char subdir[3];
    strncpy(subdir, hex_hash, 2);
    subdir[2] = '\0';

    char *file_name = hex_hash + 2; // file name of object is hash without first byte

    build_path(dir_path, PATH_MAX, objects_path, subdir);
    build_path(file_path, PATH_MAX, dir_path, file_name);

    fprintf(stderr, "dir path: %s\n", dir_path);
    fprintf(stderr, "file path: %s\n", file_path);

    // create dir if needed
    struct stat st;
    if (stat(dir_path, &st) != 0) {
        if (mkdir(dir_path, 0755) != 0) {
            perror("mkdir");
            exit(1);
        }
    }

    // create full buffer
    char *object_type = object_to_string(object->type);
    int header_len = snprintf(NULL, 0, "%s %zu", object_type, object->content_size);
    int full_size = header_len + 1 + object->content_size;

    char *buffer = malloc(full_size);

    // create header
    snprintf(buffer, header_len + 1, "%s %zu", object_type, object->content_size);
    buffer[header_len] = '\0';
    memcpy(buffer + header_len + 1, object->contents, object->content_size);

    // write compressed object
    uLongf compressed_size = compressBound(full_size);
    Bytef *compressed_contents = malloc(compressed_size);

    compress(compressed_contents, &compressed_size, (Bytef *)buffer, full_size);
    free(buffer);

    FILE *f = fopen(file_path, "wb");
    if (fwrite(compressed_contents, 1, compressed_size, f) != compressed_size) {
        perror("fwrite");
        fclose(f);
        free(compressed_contents);
        exit(1);
    }

    fclose(f);
    free(compressed_contents);

}


