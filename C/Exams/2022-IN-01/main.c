#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/stat.h>

typedef struct __attribute__((packed)) {
    uint16_t magic;
    uint16_t filetype;
    uint32_t count;
} Header;

#define MAGIC 0x5A4D

int openFile(const char* path, int flags, mode_t mode) {
    if (path == NULL) errx(2, "Invalid path");

    int fd = open(path, flags, mode);
    if (fd == -1) err(3, "Error while opening file: %s", path);

    return fd;
}

void checkHeader(int fd, int expected_type, uint32_t* out_count) {
    Header h;
    if (read(fd, &h, sizeof(Header)) != sizeof(Header)) {
        err(4, "Error reading header");
    }

    if (h.magic != MAGIC) {
        errx(5, "Invalid magic number: expected 0x5A4D");
    }

    if (h.filetype != expected_type) {
        errx(6, "Unexpected file type: got %d, expected %d", h.filetype, expected_type);
    }

    if (out_count) *out_count = h.count;
}

void checkSize(int fd, size_t item_size, uint32_t expected_count) {
    struct stat st;
    if (fstat(fd, &st) == -1) err(7, "fstat failed");

    off_t expected = sizeof(Header) + (off_t)item_size * expected_count;
    if (st.st_size != expected) {
        errx(8, "Invalid file size: expected %lld, got %lld", (long long)expected, (long long)st.st_size);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        errx(1, "Usage: %s list.bin data.bin out.bin", argv[0]);
    }

    int fd_list = openFile(argv[1], O_RDONLY, 0);
    int fd_data = openFile(argv[2], O_RDONLY, 0);
    int fd_out  = openFile(argv[3], O_RDWR | O_CREAT | O_TRUNC, 0644);

    uint32_t count_list, count_data;
    checkHeader(fd_list, 1, &count_list);
    checkHeader(fd_data, 2, &count_data);

    checkSize(fd_list, sizeof(uint16_t), count_list);
    checkSize(fd_data, sizeof(uint32_t), count_data);

    Header out_header = { .magic = MAGIC, .filetype = 3, .count = 0 };
    if (write(fd_out, &out_header, sizeof(out_header)) != sizeof(out_header)) {
        err(9, "Error writing header to out.bin");
    }

    for (uint32_t i = 0; i < count_list; i++) {
        uint16_t position, target;

        if (read(fd_list, &position, sizeof(uint16_t)) == -1) {
            err(10, "Error reading position");
        }

        if (read(fd_list, &target, sizeof(uint16_t)) == -1) {
            err(10, "Error reading value");
        }

        off_t data_offset = sizeof(Header) + (off_t)position * sizeof(uint32_t);
        if (lseek(fd_data, data_offset, SEEK_SET) == -1) {
            err(11, "Error seeking in data.bin");
        }

        uint32_t value;
        if (read(fd_data, &value, sizeof(uint32_t)) == -1) {
            err(12, "Error reading from data.bin");
        }

        off_t out_offset = sizeof(Header) + (off_t)target * sizeof(uint64_t);
        if (lseek(fd_out, out_offset, SEEK_SET) == -1) {
            err(13, "Error seeking in out.bin");
        }

        uint64_t val64 = (uint64_t)value;
        if (write(fd_out, &val64, sizeof(uint64_t)) != sizeof(uint64_t)) {
            err(14, "Error writing to out.bin");
        }

        if ((uint32_t)(target + 1) > out_header.count) {
            out_header.count = target + 1; 
        }
    }

    if (lseek(fd_out, 0, SEEK_SET) == -1) {
        err(15, "Error seeking to start of out.bin to write header");
    }

    if (write(fd_out, &out_header, sizeof(out_header)) != sizeof(out_header)) {
        err(16, "Error writing final header to out.bin");
    }

    close(fd_list);
    close(fd_data);
    close(fd_out);

    return 0;
}