#include <err.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Pair {
    uint16_t start;
    uint16_t len;
} Pair;

typedef struct Header {
    uint32_t unused1;
    uint16_t count;
    uint16_t unused2;
    uint64_t unused3;
} Header;

int openFile(const char* filename, int flags, mode_t perms) {
    if (filename == NULL) errx(2, "Invalid filename");

    int fd = open(filename, flags, perms);
    if (fd == -1) {
        err(3, "Error while opening file %s", filename);
    }

    return fd;
}

void copyInterval(int src_fd, int dst_fd, Pair p, ssize_t typeSize) {
    if (lseek(src_fd, 16 + p.start * typeSize, SEEK_SET) == -1) {
        err(5, "lseek failed");
    }

    uint8_t buf[8];
    for (int i = 0; i < p.len; i++) {
        if (read(src_fd, buf, typeSize) != typeSize) {
            err(4, "read failed");
        }
        if (write(dst_fd, buf, typeSize) != typeSize) {
            err(6, "write failed");
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        errx(1, "Expected 6 file arguments");
    }

    int affix = openFile(argv[1], O_RDONLY, 0);
    int postfix = openFile(argv[2], O_RDONLY, 0);
    int prefix = openFile(argv[3], O_RDONLY, 0);
    int infix = openFile(argv[4], O_RDONLY, 0);
    int suffix = openFile(argv[5], O_RDONLY, 0);
    int crucifix = openFile(argv[6], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    Header h;
    if (read(affix, &h, sizeof(Header)) != sizeof(Header)) {
        err(4, "Error while reading affix header");
    }

    int setCount = h.count / 8;

    for (int i = 0; i < setCount; i++) {
        Pair pairs[4];
        if (read(affix, &pairs, sizeof(pairs)) != sizeof(pairs)) {
            err(4, "Error while reading pairs");
        }

        copyInterval(postfix, crucifix, pairs[0], sizeof(uint32_t));
        copyInterval(prefix, crucifix, pairs[1], sizeof(uint8_t));
        copyInterval(infix, crucifix, pairs[2], sizeof(uint16_t));
        copyInterval(suffix, crucifix, pairs[3], sizeof(uint64_t));
    }

    close(affix);
    close(postfix);
    close(prefix);
    close(infix);
    close(suffix);
    close(crucifix);
    return 0;
}
