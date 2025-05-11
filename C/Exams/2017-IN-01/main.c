#include <stdint.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>

typedef struct Record {
    uint16_t offset;
    uint8_t len;
    uint8_t reserved;
} Record;

off_t getFileSize(int fd) {
    off_t current = lseek(fd, 0, SEEK_CUR);
    if (current == -1) {
        err(1, "lseek failed to get current position");
    }

    off_t size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
        err(1, "lseek failed to get file size");
    }

    if (lseek(fd, current, SEEK_SET) == -1) {
        err(1, "lseek failed to restore position");
    }

    return size;
}

int main(int argc, char* argv[]) {
    if ( argc != 3 ) {
        errx(1, "Expected 2 arguments");
    }

    int fd1_bin,  fd1_idx;
    const char* bin1 = argv[1];
    const char* idx1 = argv[2];

    if ( (fd1_bin = open(bin1, O_RDONLY)) == -1) err(2, "Error while opening file %s", bin1);

    off_t bin1_size = getFileSize(fd1_bin);
    if ( bin1_size <= 0) errx(3, "Invalid file %s", bin1);

    if ( (fd1_idx = open(idx1, O_RDONLY)) == -1) err(2, "Error while opening file %s", idx1);

    off_t idx1_size = getFileSize(fd1_idx);
    if ( (idx1_size <= 0)  ||  (idx1_size % sizeof(Record) != 0) ) errx(3, "Invalid file %s", idx1);

    int fd2_bin,  fd2_idx;
    const char* bin2 = "f2.bin";
    const char* idx2 = "f2.idx";
    
    if ( (fd2_bin = open(bin2, O_WRONLY | O_TRUNC | O_CREAT, 0644) ) == -1 ) err(2, "Error while opening file %s", bin2);

    if ( (fd2_idx = open(idx2, O_WRONLY | O_TRUNC | O_CREAT, 0644) ) == -1 ) err(2, "Error while opening file %s", idx2);

    Record r;
    int n;

    while ( (n = read(fd1_idx, &r, sizeof(Record))) > 0) {
        if ( ( r.offset + r.len ) * sizeof(uint8_t) > (long unsigned int)bin1_size ) {
            errx(4, "File %s not big enough", bin1);
        }

        uint16_t bin2_size = getFileSize(fd2_bin);
        if ( lseek(fd1_bin, r.offset * sizeof(uint8_t), SEEK_SET) == -1 ) {
            err(5, "Error while seeking 5" );
        }

        uint8_t letter;
        if ( (read(fd1_bin, &letter, sizeof(uint8_t)))  == -1 ) {
            err(6, "Error while reading");
        }

        if (letter >= 'A' && letter <= 'Z') {
            if ( (write(fd2_bin, &letter, sizeof(letter))) == -1 ) {
                err(7, "Error while writing" );
            }
        } else {
            continue;
        }
        
        for ( int i = 1; i < r.len; i++ ) {

            if ( (read(fd1_bin, &letter, sizeof(uint8_t)))  == -1 ) {
                err(6, "Error while reading");
            }

            if ( (write(fd2_bin, &letter, sizeof(letter))) == -1 ) {
                err(7, "Error while writing" );
            }

        }
    
        if ( (write(fd2_idx, &bin2_size, sizeof(uint16_t))) == -1 ) {
            err(7, "Error while writing" );
        }

        if ( (write(fd2_idx, &r.len, sizeof(uint8_t))) == -1 ) {
            err(7, "Error while writing" );
        }
        if ( (write(fd2_idx, &r.reserved, sizeof(uint8_t))) == -1 ) {
            err(7, "Error while writing" );
        }
    }

    if (n == -1) {
        err(6, "Error while reading");
    }

    close(fd1_bin);
    close(fd1_idx);
    close(fd2_bin);
    close(fd2_idx);
}