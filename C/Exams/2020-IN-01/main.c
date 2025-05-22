#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <err.h>
#include <sys/stat.h>

typedef struct Header {
    uint32_t magic;
    uint8_t header_version;
    uint8_t data_version;
    uint16_t count;
    uint32_t reserved1;
    uint32_t reserved2;
} Header;

typedef struct Type0 {
    uint16_t offset;
    uint8_t original;
    uint8_t new;
} Type0;

typedef struct Type1 {
    uint32_t offset;
    uint16_t original;
    uint16_t new;
} Type1;

int openFile(const char* filename, int flags, mode_t mode) {
    if (filename == NULL) errx(2, "Invalid filename");

    int fd;
    if ( (fd = open(filename, flags, mode)) == -1) {
        err(2, "Error while opening file %s", filename);
    }
    return fd;
}

void copyFile(int fd_from, int fd_to) {
    char c[1024];
    ssize_t readSize;

    while ((readSize = read(fd_from, c, sizeof(c))) > 0) {
        ssize_t written = 0;
        while (written < readSize) {
            ssize_t w = write(fd_to, c + written, readSize - written);
            if (w == -1) err(10, "Write error");
            written += w;
        }
    }

    if (readSize == -1) {
        err(3,"Read error");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        errx(1, "Expected 3 arguments");
    }
    
    const char* patch = argv[1];
    const char* f1 = argv[2];
    const char* f2 = argv[3];

    int fd_patch = openFile(patch, O_RDONLY, 0);
    int fd1 = openFile(f1, O_RDONLY, 0);
    int fd2 = openFile(f2, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    Header h;
    if (read(fd_patch, &h, sizeof(Header)) != sizeof(Header)) {
        err(3, "Error reading patch header");
    }

    if (h.magic != 0xEFBEADDE) {
        errx(4, "Invalid magic value");
    }
    if (h.header_version != 0x01) {
        errx(5, "Invalid header version");
    }

    struct stat st;
    if (fstat(fd_patch, &st) == -1) {
        err(6, "Error stating patch file");
    }

    off_t patch_data_size = st.st_size - sizeof(Header);

    copyFile(fd1, fd2);

    close(fd2);
    fd2 = openFile(f2, O_RDWR, 0);

    if (h.data_version == 0x00) {
        if (patch_data_size != h.count * sizeof(Type0)) {
            errx(7, "Invalid patch file size for data version 0x00");
        }

        Type0 t;
        for (uint16_t i = 0; i < h.count; i++) {
            if (read(fd_patch, &t, sizeof(Type0)) != sizeof(Type0)) {
                err(3, "Error reading patch entry");
            }

            off_t offset_bytes = (off_t)t.offset * sizeof(uint8_t);

            if (lseek(fd2, offset_bytes, SEEK_SET) == -1) {
                err(8, "Error seeking output file");
            }

            uint8_t current;
            if (read(fd2, &current, sizeof(uint8_t)) != sizeof(uint8_t)) {
                err(3, "Error reading output file");
            }

            if (current != t.original) {
                errx(9, "Original byte mismatch at offset %u", t.offset);
            }

            if (lseek(fd2, offset_bytes, SEEK_SET) == -1) {
                err(8, "Error seeking output file for writing");
            }

            if (write(fd2, &t.new, sizeof(uint8_t)) != sizeof(uint8_t)) {
                err(10, "Error writing patched byte");
            }
        }
    } else if (h.data_version == 0x01) {
        if (patch_data_size != h.count * sizeof(Type1)) {
            errx(6, "Invalid patch file size for data version 0x01");
        }

        Type1 t;
        for (uint16_t i = 0; i < h.count; i++) {
            if (read(fd_patch, &t, sizeof(Type1)) != sizeof(Type1)) {
                err(3, "Error reading patch entry");
            }

            off_t offset_bytes = (off_t)t.offset * sizeof(uint16_t);

            if (lseek(fd2, offset_bytes, SEEK_SET) == -1) {
                err(9, "Error seeking output file");
            }

            uint16_t current;
            if (read(fd2, &current, sizeof(uint16_t)) != sizeof(uint16_t)) {
                err(3, "Error reading output file");
            }

            if (current != t.original) {
                errx(10, "Original word mismatch at offset %u", t.offset);
            }

            if (lseek(fd2, offset_bytes, SEEK_SET) == -1) {
                err(9, "Error seeking output file for writing");
            }

            if (write(fd2, &t.new, sizeof(uint16_t)) != sizeof(uint16_t)) {
                err(11, "Error writing patched word");
            }
        }
    } else {
        errx(12, "Unsupported data version: %u", h.data_version);
    }

    close(fd_patch);
    close(fd1);
    close(fd2);

    return 0;
}