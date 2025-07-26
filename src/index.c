#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include "index.h"
#include "repo.h"
#include "utils.h"


static void write_u32_be(FILE *stream, uint32_t content) {
    uint32_t be = htonl(content);
    fwrite(&be, sizeof(be), 1, stream);
}

static void write_u16_be(FILE *stream, uint16_t content) {
    uint16_t be = htons(content);
    fwrite(&be, sizeof(be), 1, stream);
}

int index_write_all(Repo *repo, IndexEntry *entries, int num_entries) {

    /* header: 12 bytes
     * 0-3: "DIRC"
     * 4-7: Version (2)
     * 8-11: int (num entries)
     *
     * entry:
     * 4 bytes: file mode (octal in hex representation)
     * 20 bytes: sha1
     * 1 byte: stage num
     * 2 bytes: path length
     * N bytes: path string with NO NULL TERMINATOR
     * P bytes: padding for each entry size (including header) % 8 = 0
     */

    char *index_path = build_path(repo->dottwig_path, "index");

    // write header
    FILE *f = fopen(index_path, "wb");
    fwrite(INDEX_SIGNATURE, sizeof(char), strlen(INDEX_SIGNATURE), f);
    write_u32_be(f, INDEX_VERSION);
    write_u32_be(f, num_entries);

    for (int i = 0; i < num_entries; i++) {
        IndexEntry *entry = &entries[i];

        write_u32_be(f, (uint32_t)entry->mode);
        fwrite(entry->sha1, SHA1_LENGTH, 1, f);
        fwrite(&entry->stage_num, 1, 1, f);
        write_u16_be(f, (uint16_t)entry->path_len);
        fwrite(entry->rel_path, entry->path_len, 1, f);

        long total = ftell(f); // bytes written so far
        int pad  = (8 - (total % 8)) % 8; // needed for padding
        for (int p = 0; p < pad; p++) {
            fputc(0, f);
        }

    }
    fclose(f);
    return 0;


}