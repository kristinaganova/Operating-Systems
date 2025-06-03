#include <err.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct Header {
    uint32_t magic;
    uint32_t packet_count;
    uint64_t original_size;
} Header;

int openFile(const char* path, int flags, mode_t mode) {
    if (path == NULL) { errx(1, "Invalid path"); }

    int fd;

    if ((fd = open(path, flags, mode)) == -1) {
        err(2, "Errro while opening file %s", path);
    }

    return fd;
}

void checkHeader(Header* h, int fd, uint32_t magic) {
    if ( read(fd, h, sizeof(Header)) == -1 ) {
        err(3, "Error while reading");
    }

    if (h->magic != magic) {
        errx(1, "Invalid magic number of header");
    }
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
        errx(1, "Expected 2 arguments");
	}

    int fd_compressed = openFile(argv[1], O_RDONLY, 0);
    int fd_result = openFile(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
    Header h;
    checkHeader(&h, fd_compressed, 0x21494D46);

    uint8_t byte;
    int readBytes;
    while ( (readBytes = read(fd_compressed, &byte, sizeof(uint8_t))) > 0 ) { //could use header.packet_count to limit the number of packets read
        int N = (byte & 0x7F);
        int type = ((byte >> 7) & 1);

        if (type != 0 && type != 1) { errx(1, "Invalid package type"); }

        if (type == 0) {
            uint8_t arr[256];
            if (read(fd_compressed, &arr, (N + 1) * sizeof(uint8_t)) == -1) {
                err(4, "Error while reading");
            }

            if (write(fd_result, &arr, N + 1) != N + 1) {
                err(5, "Errro while reading");
            } 
        } else {
            uint8_t toRep;
            if (read(fd_compressed, &toRep, sizeof(uint8_t)) == -1) {
                err(4, "Error while reading");
            }
    
            for (int i = 0; i < N + 1; i++) {
                if (write(fd_result, &toRep, 1) != 1) {
                    err(5, "Errro while reading");
                } 
            }
        }

    }
    
    //should check if result size is equal to original_size from header
    if (readBytes == -1) {
        err(4, "Errow while reading");
    }
    
    close(fd_compressed);
    close(fd_result);
}
