#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <err.h>
#include <sys/stat.h>

int openFile(const char* filename, int flags, mode_t perms) {
    if (filename == NULL) errx(2, "Invalid filename");

    int fd = open(filename, flags, perms);
    if (fd == -1) {
        err(3, "Error while opening file %s", filename);
    }

    return fd;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        errx(1, "Usage: ./program input.scl input.sdl output.sdl");
    }

    int fd_scl = openFile(argv[1], O_RDONLY, 0);
    int fd_sdl = openFile(argv[2], O_RDONLY, 0);
    int fd_result = openFile(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    //to do 
    // add validation for the input files with stat

    uint8_t scl_byte;
    ssize_t scl_read;

    while ((scl_read = read(fd_scl, &scl_byte, sizeof(scl_byte))) == sizeof(scl_byte)) {
        for (uint8_t mask = 1 << 7; mask > 0; mask >>= 1) {
            uint16_t value;
            ssize_t sdl_read = read(fd_sdl, &value, sizeof(value));
            if (sdl_read == -1) {
                err(4, "Error while reading SDL");
            } else if (sdl_read == 0) {
                errx(4, "SDL file is too short for the given SCL bits");
            }

            if ((scl_byte & mask) != 0) {
                if (write(fd_result, &value, sizeof(value)) != sizeof(value)) {
                    err(5, "Error while writing to output");
                }
            }
        }
    }

    if (scl_read == -1) {
        err(6, "Error while reading SCL");
    }

    close(fd_scl);
    close(fd_sdl);
    close(fd_result);

    return 0;
}