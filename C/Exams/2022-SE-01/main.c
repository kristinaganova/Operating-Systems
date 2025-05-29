#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>

typedef struct HeaderData {
    uint32_t magic;
    uint32_t count;
} HeaderData;

typedef struct HeaderComparator {
    uint32_t magic1;
    uint16_t magic2;
    uint16_t reserved;
    uint64_t count;
} HeaderComparator;

typedef struct Hexad {
    uint16_t type;
    uint16_t reserved1;
    uint16_t reserved2;
    uint16_t reserved3;
    uint32_t offset1;
    uint32_t offset2;
} Hexad;

off_t checkHeader(int fd) {
    HeaderData h;

    if ( read(fd, &h, sizeof(h)) == -1 ) {
        err(3, "Error while reading");
    }

    if (h.magic != 0x21796F4A) {
        errx(4, "Invalid data file magic number");
    }

    struct stat st;

    if ( fstat(fd, &st) == -1 ) {
        err(5, "Error while stating");
    }

    if ( ((st.st_size - sizeof(HeaderData)) / sizeof(uint64_t)) != h.count) {
        errx(4, "Invalid data file size");
    }
    
    return st.st_size;
}

void checkHeaderComp(int fd) {
    HeaderComparator h;

    if ( read(fd, &h, sizeof(h)) == -1 ) {
        err(3, "Error while reading");
    }

    if ( h.magic1 != 0xAFBC7A37 || h.magic2 != 0x1C27 ) {
        errx(4, "Invalid data file magic number");
    }

    struct stat st;

    if ( fstat(fd, &st) == -1 ) {
        err(5, "Error while stating");
    }

    if ( ((st.st_size - sizeof(HeaderComparator)) % sizeof(Hexad))!= 0 ) {
        errx(4, "Invalid data file size");
    }
    
}

int openFile(const char* path, int flags, mode_t mode) {
    if ( path == NULL ) errx(2, "Invalid path");

    int fd;
    if ( (fd = open(path, flags, mode)) == -1 ) {
        err(3, "Error while opening file %s", path);
    }

    return fd;
}

int main(int argc, char* argv[]) {
	if ( argc != 3 ) {
        errx(1, "Invalid number of arguments");
	}

    int fd_data = openFile(argv[1], O_RDWR, 0);
    int fd_comp = openFile(argv[2], O_RDONLY, 0);

    checkHeaderComp(fd_comp);
    off_t size_data = checkHeader(fd_data);

    Hexad h;
    int readBytes;

    while ( (readBytes = read(fd_comp, &h, sizeof(h))) > 0 ) {
        if ( h.type != 0 && h.type != 1) {
            errx(4, "Invalid type");
        }

        if (h.reserved1 != 0 || h.reserved2 != 0 || h.reserved3 != 0) {
            errx(4, "Invalid file format");
        }
        
        off_t offset1 = sizeof(HeaderData) + h.offset1 * sizeof(uint64_t);

        if ( offset1 > size_data) errx(8, "Invalid offset %d", h.offset1);

        if (lseek(fd_data, offset1, SEEK_SET)== -1 ) {
            err(6, "Error while lseeking");
        }

        // could be a function
        uint64_t num1;
        if ( read(fd_data, &num1, sizeof(uint64_t)) == -1 ) {
            err(3, "Error while reading");
        }

        off_t offset2 = sizeof(HeaderData) + h.offset2 * sizeof(uint64_t);

        if ( offset2 > size_data) errx(8, "Invalid offset %d", h.offset2);

        if (lseek(fd_data, offset2, SEEK_SET)== -1 ) {
            err(6, "Error while lseeking");
        }
        //

        uint64_t num2;
        if ( read(fd_data, &num2, sizeof(uint64_t)) == -1 ) {
            err(3, "Error while reading");
        }

        if ( (h.type == 0 && num1 > num2) || (h.type == 1 && num1 < num2) ) {
            if (lseek(fd_data, offset2, SEEK_SET) == -1 ) {
                err(6, "Error while lseeking");
            }

            if ( write(fd_data, &num1, sizeof(uint64_t)) != sizeof(uint64_t)) {
                err(7, "Errro while writing");
            }

            if (lseek(fd_data, offset1, SEEK_SET) == -1 ) {
                err(6, "Error while lseeking");
            }
            
            if ( write(fd_data, &num2, sizeof(uint64_t)) != sizeof(uint64_t)) {
                err(7, "Errro while writing");
            }
        }
    }
    
    if ( readBytes == -1 ) {
        err(3, "Error while reading");
    }

    close(fd_data);
    close(fd_comp);
}
