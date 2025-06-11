#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <err.h>

typedef struct Pair {
    uint32_t offset;
    uint32_t len;
} Pair;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        errx(1, "Expected 3 arguments");
    }

    int fd1, fd2, fd3;
    const char* f1 = argv[1];
    const char* f2 = argv[2];
    const char* f3 = argv[3];

    if ( (fd1 = open(f1, O_RDONLY)) == -1 ) {
        err(2, "Error while opening file %s", f1);
    }

    if ( (fd2 = open(f2, O_RDONLY)) == -1) {
        err(2, "Error while opening file %s", f2);
    }

    if ( (fd3 = open(f3, O_WRONLY | O_TRUNC | O_CREAT, 644)) == -1) {
        err(2, "Error while opening file %s", f2);
    }

    off_t file1_len;
    if ( (file1_len = lseek(fd1,0, SEEK_END)) <= 0) {
        errx(3, "Invalid file %s", f1);
    }

    if (lseek(fd1, 0, SEEK_SET) == -1) {
        err(5, "Failed to seek in %s", f1)         
    } 

    if ( file1_len % sizeof(Pair) != 0 ) { 
        errx(3, "Invalid file %s", f1);
    }

    off_t file2_len;
    if ( (file2_len = lseek(fd2,0, SEEK_END)) <= 0) {
        errx(3, "Invalid file %s", f2);
    }

    
    if (lseek(fd2,0, SEEK_SET) == -1) {
        err(5, "Failed to seek in %s", f2);
    }

    if ( file2_len % sizeof(uint32_t) != 0 ) { 
        errx(3, "Invalid file %s", f2);
    }

    if ( file1_len > file2_len ) {
        errx(4, "File 2 should be bigger than file 1");
    }

    Pair pair; 
    int readBytes;
    while ( (readBytes = read(fd1, &pair, sizeof(pair))) == sizeof(pair)) {
        uint32_t num; 
        if ( (pair.offset + pair.len)* sizeof(uint32_t) > (uint32_t)file2_len) {
            errx(5, "f2 not big enough");
        }

        if (lseek(fd2, pair.offset * sizeof(uint32_t), SEEK_SET) == -1) {
            err(5, "Failed to seek in %s", f2);
        }

        for ( uint32_t i = 0 ; i < pair.len; i++ ) {    
            if ( (read(fd2, &num, sizeof(num))) < 0 ) {
                err(6, "Error while reading from %s", f2);
            }
            if ( (write(fd3, &num, sizeof(num))) < 0) {
                err(7, "Error while writing in %s", f3);
            }
        }
    }

    if ( readBytes < 0 ) {
        err(8, "Error while reading pairs");
    }

    close(fd1);
    close(fd2);
    close(fd3);

    exit(0);
}