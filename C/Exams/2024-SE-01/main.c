#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <err.h>

typedef struct Record {
    uint64_t next;
    uint8_t user_data[504];
} Record;

void markUsedRecords(int fd, int fd_temp, uint64_t start_index) {
    uint64_t current = start_index;
    Record r;

    while (1) {
        off_t offset = current * sizeof(Record);
        if (lseek(fd, offset, SEEK_SET) == -1)
            err(7, "lseek failed on fd");

        ssize_t bytesRead = read(fd, &r, sizeof(Record));
        if (bytesRead != sizeof(Record))
            err(5, "read failed on fd");

        off_t temp_offset = current;
        if (lseek(fd_temp, temp_offset, SEEK_SET) == -1)
            err(7, "lseek failed on fd_temp");

        uint8_t one = 1;
        if (write(fd_temp, &one, 1) != 1)
            err(8, "write failed on fd_temp");

        if (r.next == 0) {
            break; 
        }
        current = r.next;
    }
}

int main(int argc, char* argv[]) {
	if ( argc != 2 ) {
        errx(1, "Expected one argument");
	}

    int fd;
    if ( (fd = open(argv[1], O_RDWR)) == -1 ) {
        err(2, "Error while opening file");
    }

    struct stat st;
    if ( fstat(fd, &st) == -1 ) { err(3, "Errro while stating file"); }

    if ( st.st_size % 512 != 0 ) { errx(4, "Invalid file size"); }
    

    char template[] = "/tmp/mytempfileXXXXXX";

    int fd_temp = mkstemp(template);
    if (fd_temp == -1) {
        err(1, "mkstemp failed");
    }
   
    markUsedRecords(fd, fd_temp, 0);
    
    uint8_t zeroes[512] = {0};
    for (int i = 0; i < st.st_size; i+=512) {
        if (lseek(fd_temp, i, SEEK_SET) == -1 ) { err(7, "Errro while lseeking"); }
        uint8_t value;
        if ( read(fd_temp, &value, sizeof(uint8_t)) == -1 ) { err(5, "Errro while reading"); }
        
        if ( value == 0 ) {
            if (lseek(fd, i, SEEK_SET) == -1 ) { err(7, "Errro while lseeking"); }
            if (write(fd, &zeroes, sizeof(zeroes)) != sizeof(zeroes)) {
                err(8, "Error while writing");
            }
        }
    }

    close(fd);
}