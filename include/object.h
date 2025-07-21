#ifndef TWIG_OBJECT_H
#define TWIG_OBJECT_H

#include <stddef.h>

#define SHA1_LENGTH 20
#define SHA1_STR_LENGTH 40

enum TwigObjectType {
    OBJ_BLOB,
    OBJ_TREE,
    OBJ_COMMIT,
    OBJ_TAG
};

typedef struct {
    enum TwigObjectType type;
    unsigned char *contents;
    size_t content_size;
} TwigObject;

/**
 * Returns the SHA1 hash of an object, with the header included
 *
 * @param object The TwigObject to hash
 * @param out_hash Pointer to a 20-byte address to write the hash to
 */
void twigobject_hash(TwigObject *object, unsigned char *out_hash);


/**
 * Writes the TwigObject to disk
 * @param object The TwigObject to write
 * @param twig_root The root location of the twig repository
 */
void twigobject_write(TwigObject *object, char *twig_root);


TwigObject *twigobject_read(char *hash, char *twig_root);

/**
 * Converts a TwigObjectType into its string representation in the header
 */
char *object_to_string(enum TwigObjectType type);

#endif //TWIG_OBJECT_H
