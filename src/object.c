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
#include "repo.h"
#include <errno.h>


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
    sha1_to_hex(object_hash, hex_hash);

    // check directory and file exists
    // .twig/objects/xx/yyyy... path
    char *objects_dir = ".twig/objects";

    char *objects_path = build_path(twig_root, objects_dir);

    char subdir[3];
    strncpy(subdir, hex_hash, 2);
    subdir[2] = '\0';

    char *file_name = hex_hash + 2; // file name of object is hash without first byte

    char *dir_path = build_path(objects_path, subdir);
    char *file_path = build_path(dir_path, file_name);

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


TwigObject *twigobject_read(char *hex_hash, char *twig_root) {

    // construct paths
    // .twig/objects/xx/yyyy... path
    char *objects_dir = ".twig/objects";

    char *objects_path = build_path(twig_root, objects_dir);

    char subdir[3];
    strncpy(subdir, hex_hash, 2);
    subdir[2] = '\0';

    char *file_name = hex_hash + 2; // file name of object is hash without first byte

    char *dir_path = build_path(objects_path, subdir);
    char *file_path = build_path(dir_path, file_name);



    // read compressed object
    FILE *f = fopen(file_path, "rb");
    if (!f) {
        perror(file_path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long compressed_size = ftell(f);
    rewind(f);

    Bytef *compressed_contents = malloc(compressed_size);
    fread(compressed_contents, 1, compressed_size, f);
    fclose(f);

    z_stream zs = {0};
    zs.next_in  = compressed_contents; /* input pointer */
    zs.avail_in = compressed_size; /* input len */

    if (inflateInit(&zs) != Z_OK) {
        return NULL;
    }

    size_t cap = 8192; // 8kib
    size_t used = 0;
    unsigned char *deflated_contents = malloc(cap);

    int status;
    do {
        if (used == cap) {
            cap *= 2;
            deflated_contents = realloc(deflated_contents, cap);
        }

        zs.next_out = deflated_contents + used;
        zs.avail_out = cap - used;

        status = inflate(&zs, Z_NO_FLUSH);
        used = cap - zs.avail_out;

    } while (status == Z_OK);

    inflateEnd(&zs);
    free(compressed_contents);


    if (status != Z_STREAM_END) {
        return NULL;
    }

    // parse the header
    size_t header_len = 0;

    while(header_len < used && deflated_contents[header_len] != '\0') {
        header_len++;
    }

    if (header_len == used) {
        fprintf(stderr, "corrupt object\n");
        return NULL;
    }

    char *space = memchr(deflated_contents, ' ', header_len);
    if (!space) {
        fprintf(stderr, "corrupt object\n");
        return NULL;
    }

    size_t type_len = space - (char *)deflated_contents;

    enum TwigObjectType obj_type;
    if (type_len == 4 && !memcmp(deflated_contents, "blob", 4)) {
        obj_type = OBJ_BLOB;
    }
    else if (type_len == 4 && !memcmp(deflated_contents, "tree", 4)) {
        obj_type = OBJ_TREE;
    }
    else if (type_len == 6 && !memcmp(deflated_contents, "commit", 4)) {
        obj_type = OBJ_COMMIT;
    }
    else if (type_len == 3 && !memcmp(deflated_contents, "tag", 4)) {
        obj_type = OBJ_TAG;
    }

    int payload_length = used - (header_len + 1); // for null character
    unsigned char *payload = deflated_contents + header_len + 1;

    // sanity check
    int size_start = strcspn((char *)deflated_contents, " ") + 1;
    int declared_payload_length = strtoul((char *)deflated_contents + size_start, NULL, 10);

    if (declared_payload_length != payload_length) {
        fprintf(stderr, "corrupt object\n");
        return NULL;
    }

    TwigObject *twigObject = malloc(sizeof(TwigObject));
    twigObject->content_size = payload_length;
    twigObject->type = obj_type;
    twigObject->contents = malloc(payload_length);
    memcpy(twigObject->contents, payload, payload_length);

    return twigObject;

}


int hash_file_as_blob(Repo *repo, char *file_name, int write, unsigned char *out_sha1) {
    if (!path_exists_and_is_file(file_name)) {
        perror(file_name);
        return -1;
    }

    char *file_path = realpath(file_name, NULL);
    char *file_contents = read_file(file_path);

    TwigObject blob;
    blob.type = OBJ_BLOB;
    blob.contents = (unsigned char *) file_contents;
    blob.content_size = strlen(file_contents);

    twigobject_hash(&blob, out_sha1);

    if (write) {
        twigobject_write(&blob, repo->root_path);
    }

    free(file_contents);
    return 0;
}
