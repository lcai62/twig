#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/sha.h>
#include "utils.h"

#define ASSERT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        exit(1); \
    } \
} while (0)


void setup() {
    system("rm -rf /tmp/twig_test tmp");
}


void test_path_exists_and_is_dir() {
    // absolute paths
    system("rm -rf /tmp/twig_test");
    ASSERT(path_exists_and_is_dir("/tmp/twig_test") == false);
    system("mkdir /tmp/twig_test");
    ASSERT(path_exists_and_is_dir("/tmp/twig_test") == true);

    // relative paths
    system("mkdir -p tmp"); 
    system("rm -rf tmp/twig_test");
    ASSERT(path_exists_and_is_dir("tmp/twig_test") == false);
    system("mkdir tmp/twig_test");
    ASSERT(path_exists_and_is_dir("tmp/twig_test") == true);
}

void test_path_exists_and_is_file() {
    char file[] = "/tmp/twig_test/file.txt";

    system("rm -rf /tmp/twig_test/file.txt");
    ASSERT(path_exists_and_is_file(file) == false);
    
    FILE *f = fopen(file, "w");
    fputs("a", f);
    fclose(f);
    ASSERT(path_exists_and_is_file(file) == true);
}

void test_create_dir() {

    char dir[] = "/tmp/twig_test/test_dir";
    system("rm -rf /tmp/twig_test/test_dr");
    ASSERT(!path_exists_and_is_dir(dir));

    create_dir(dir);
    ASSERT(path_exists_and_is_dir(dir));

}


void test_write_file() {
    char *file = "/tmp/twig_test/file.txt";
    system("rm -f /tmp/twig_test/file.txt");

    write_file(file, "hello world");
    ASSERT(path_exists_and_is_file(file));

    FILE *f = fopen(file, "r");
    char buf[32];
    fgets(buf, sizeof(buf), f);
    fclose(f);
    ASSERT(strcmp(buf, "hello world") == 0);
}


void test_read_file() {
    char *file = "/tmp/twig_test/file.txt";
    system("rm -f /tmp/twig_test/file.txt");

    FILE *f = fopen(file, "w");
    fputs("hello world", f);
    fclose(f);

    char *content = read_file(file);
    ASSERT(strncmp(content, "hello world", 11) == 0);
    free(content);
}


void test_get_abs_cwd() {
    char *cwd = getcwd(NULL, 0);
    char *abs_cwd = get_abs_cwd();
    ASSERT(strcmp(cwd, abs_cwd) == 0);
    free(cwd);
    free(abs_cwd);
}

void test_build_path() {
    char *full = build_path("/tmp", "twig_build");
    ASSERT(strcmp(full, "/tmp/twig_build") == 0);
    free(full);
}

void test_sha1_conversions() {
    // no error
    const char *msg = "hello";
    unsigned char digest[SHA_DIGEST_LENGTH];
    SHA1((const unsigned char *)msg, strlen(msg), digest);

    char hex[41];
    sha1_to_hex(digest, hex);
    ASSERT(strlen(hex) == 40);

    unsigned char back[SHA_DIGEST_LENGTH];
    hex_to_sha1(hex, back);
    ASSERT(memcmp(digest, back, SHA_DIGEST_LENGTH) == 0);

    // null inputs
    ASSERT(sha1_to_hex(NULL, hex) == -1);
    ASSERT(sha1_to_hex(digest, NULL) == -1);
    ASSERT(hex_to_sha1(NULL, back) == -1);
    ASSERT(hex_to_sha1(hex, NULL) == -1);

    // invalid length
    ASSERT(hex_to_sha1("abcd", back) == -1); // too short
    char long_hex[100] = {0};
    memset(long_hex, 'a', 99);
    ASSERT(hex_to_sha1(long_hex, back) == -1); // too long

    // invalid characters
    ASSERT(hex_to_sha1("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz", back) == -1);
}


void test_mode_for_path() {
    char *file = "/tmp/twig_mode_file.txt";
    system("rm -f /tmp/twig_mode_file.txt");
    FILE *f = fopen(file, "w");
    fputs("mode", f);
    fclose(f);
    ASSERT(mode_for_path(file) == 0100644);

    system("chmod +x /tmp/twig_mode_file.txt");
    ASSERT(mode_for_path(file) == 0100755);

    const char *dir = "/tmp/twig_test/twig_mode_dir";
    system("rm -rf /tmp/twig_test/twig_mode_dir && mkdir /tmp/twig_test/twig_mode_dir");
    ASSERT(mode_for_path(dir) == 0040000);

    char *link = "/tmp/twig_test/twig_mode_symlink";
    system("rm -f /tmp/twig_test/twig_mode_symlink");
    symlink(file, link);
    ASSERT(mode_for_path(link) == 0120000);

}

void cleanup() {
    system("rm -rf /tmp/twig_test tmp");
}


int main() {
    setup();
    test_path_exists_and_is_dir();
    test_path_exists_and_is_file();
    test_read_file();
    test_write_file();
    test_create_dir();
    test_build_path();
    test_get_abs_cwd();
    test_sha1_conversions();
    test_mode_for_path();
    cleanup();
}