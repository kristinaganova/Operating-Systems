#include <stdint.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>

typedef struct Record {
    uint16_t offset;
    uint8_t original;
    uint8_t new;
} Record;

int openFile(const char* name, int mode, mode_t perms) {
    int fd = open(name, mode, perms);

    if (fd < 0) {
        err(2, "Error while opening file %s", name);
    }

    return fd;
}

off_t getFileSize(int fd) {
    int current;
    int result;

    if ( (current = lseek(fd, 0, SEEK_CUR)) == -1 ) {
        err(3, "Error while seeking");
    }

    if ( (result = lseek(fd, 0, SEEK_END)) == -1 ) {
        err(3, "Error while seeking");
    }

    if ( lseek(fd, current, SEEK_SET) == -1 ) {
        err(3, "Error while seeking");
    }

    return result;
}

int main(int argc, char* argv[]) {
    if ( argc != 4 ) errx(1, "Expected 3 arguments");

    const char* f1 = argv[1];
    const char* f2 = argv[2];
    const char* patch = argv[3];

    int fd1 = openFile(f1, O_RDONLY, 0);
    int fd2 = openFile(f2, O_RDONLY, 0);
    int fd3 = openFile(patch, O_WRONLY | O_TRUNC | O_CREAT, 0644);

    if ( getFileSize(fd1) != getFileSize(fd2) ) {
        errx(4, "Files should be the same sizes" );
    }

    uint8_t byte1, byte2;

    uint16_t index = 0;

    while ( (read(fd1, &byte1, sizeof(byte1)) == sizeof(byte1))
         && (read(fd2, &byte2, sizeof(byte2)) == sizeof(byte2)) ) {
        if (byte1 != byte2) {
            if ( (write(fd3, &index, sizeof(index)) ) == -1 ) {
                err(5, "Error while writing");
            }
            if ( (write(fd3, &byte1, sizeof(uint8_t)) ) == -1 ) {
                err(5, "Error while writing");
            }
            if ( (write(fd3, &byte2, sizeof(uint8_t)) ) == -1 ) {
                err(5, "Error while writing");
            }
        }
        else {
            index++;
            continue;
        }

        index++;
    }

        close(fd1);
        close(fd2);
        close(fd3);
}
