#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>

int openFile(const char* path, int flags, mode_t mode) {
    if ( path == NULL ) errx(2, "Invalid path");

    int fd;
    if ( (fd = open(path, flags, mode)) == -1) {
        err(3, "Errro while opening file");
    }
    return fd;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        errx(1, "Expected 2 arguments");
    }

    int fd_messages = openFile(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0644);
    int fd_stream = openFile(argv[1], O_RDONLY, 0);

    struct stat st;
    if ( fstat(fd_stream, &st) == -1 ) {
        err(9, "Error while stating");
    }

    uint8_t byte;
    int readBytes;
    uint8_t checksum;
    while ( (readBytes = read(fd_stream, &byte, sizeof(uint8_t))) > 0) {
        if ( byte != 0x55 ) continue;
        
        checksum = byte;

        uint8_t N;
        if ( read(fd_stream, &N, sizeof(uint8_t)) == -1 ) {
            err(4, "Error while reading");
        }   
        checksum ^= N;

        int currentPos;

        if ((currentPos = lseek(fd_stream, 0, SEEK_CUR)) == -1 ) {
            err(10, "Error while lseeking");
        }

        if (currentPos + (N - 2) > st.st_size) {
            continue;
        }

        uint8_t arr[(1 << 7)]; // N cannot exceed 2^7
        arr[0] = byte;
        arr[1] = N;

        for ( int i = 2; i < N - 1; i++ ) {
            if ( read(fd_stream, &arr[i], sizeof(uint8_t)) == -1 ) {
                err(4, "Error while reading");
            }
            checksum ^= arr[i];
        }
    
        uint8_t cs;
        if ( read(fd_stream, &cs, sizeof(uint8_t)) == -1 ) {
            err(4, "Error while reading");
        }

        if (cs != checksum) {
            continue;
        }
         
        arr[N-1] = cs;

        if (write(fd_messages, arr, sizeof(uint8_t) * N) == -1) {
            err(6, "Error whil writing");
        }
    }

    if ( readBytes == -1 ) {
        err(4, "Error while reading");
    }

    close(fd_messages);
    close(fd_stream);

    return 0;
}
 
