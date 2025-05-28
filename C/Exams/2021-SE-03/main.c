#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <err.h>
#include <sys/stat.h>

int openFile(const char* filename, int flags, mode_t perms) {
    if (filename == NULL) errx(2, "Invalid filename");

    int fd = open(filename, flags, perms);
    if (fd == -1) {
        err(3, "Error while opening file %s", filename);
    }

    return fd;
}

int main(int argc, char* argv[]) {  
    if ( argc != 3 ) {
        errx(1, "Expected 2 arguments");
    }

    int fd_input = openFile(argv[1], O_RDONLY, 0);
    int fd_output = openFile(argv[2], O_CREAT | O_RDWR | O_TRUNC, 0644);

    uint16_t piece;
    int readSize;
    while ( (readSize = read(fd_input, &piece, sizeof(uint16_t))) > 0) {
        uint8_t result = 0;
        for (int i = 7; i >= 0; i--) { 
            uint16_t bits = (piece >> (i * 2)) & 3;

            result <<= 1;

            if ( bits == 2 ) {
                result |= 1;
            } else if ( bits == 1) {
                result |= 0;
            } else {
                errx(6, "Invalid Manchester encoding at segment %d: %u", i, bits);
            }
        }

        if (write(fd_output, &result, sizeof(uint8_t)) == -1) {
            err(5, "Error while writing");
        }
    }

    if ( readSize == -1) {
        err(4, "Error while reading");
    }
    
    close(fd_input);
    close(fd_output);
    return 0;
}
