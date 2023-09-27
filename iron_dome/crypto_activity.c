#include <stdio.h>
#include <openssl/sha.h>
#include <string.h>

int main() {
    unsigned char data[SHA256_DIGEST_LENGTH];
    char str[] = "test";
    int i;

    while(1) {
        SHA256((unsigned char*)&str, strlen(str), data);
    }
    return 0;
}