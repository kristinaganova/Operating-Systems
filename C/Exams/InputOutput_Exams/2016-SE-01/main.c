
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>
#include <fcntl.h>
int main(int argc, char* argv[]) {
    if ( argc != 2 ) {
        errx(1, "Expected 2 arguments");
    }

    const char* file = argv[1];
    int fd;

    if ( (fd = open(file, O_RDWR)) < 0 ) {
        err(2, "Error while opening file %s", file);
    }

    uint32_t bytes[256] = {0};

    uint8_t byte;
    int readBytes;

    while( (readBytes = read(fd, &byte, sizeof(byte))) > 0 ) {
        bytes[byte] += 1;
    }

    if ( readBytes < 0 ) {
        err(3, "Error while reading file %s", file);
    }

    if ( (lseek(fd, 0, SEEK_SET)) < 0 ) {
        err(4, "Error while changin the index");
    }

    for ( uint32_t i = 0; i < 256; i++) {
        uint8_t b = i;
        for ( uint32_t j = 0; j < bytes[i]; j++ ) {
            if ( (write(fd, &b, sizeof(b))) != sizeof(b) ) {
                err(5, "Error while writing in %s", file);
            }
        }
    }

    close(fd);

    exit(0);
}
