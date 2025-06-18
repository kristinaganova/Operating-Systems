#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
        if (argc != 3) {
        errx(1, "Expected 2 arguments");
        }

        char* inputFile = argv[1];
        char* outputFile = argv[2];

        int pids[2];
        if (pipe(pids) == -1) {
        err(2, "pipe");
        }

        int pid = fork();

        if (pid == -1) {
        err(3, "fork");
        }

        if (pid == 0) {
        close(pids[0]);
        dup2(pids[1], 1);
        close(pids[1]);

        execlp("cat", "cat", inputFile, (char *)NULL);
        err(4, "Error executing command cat");
        }
    close(pids[1]);

    int fd;
    if ((fd = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0644)) == -1) {
        err(6, "open");
    }

    int readBytes;
    uint8_t byte;
    while ((readBytes = read(pids[0], &byte, sizeof(byte))) > 0) {
        if (byte != 0x7D) continue;

        if ( read(pids[0], &byte, sizeof(byte)) == -1) {
            err(5, "read");
        }

        byte ^= 0x20;

        if (byte != 0x00 && byte != 0xFF && byte != 0x55 && byte != 0x7D) {
            errx(7, "Invalid data");
        }

        if (write(fd, &byte, sizeof(byte)) != sizeof(byte)) {
            err(8, "write");
        }
    }

    if (readBytes == -1) {
        err(5, "read");
    }

    close(fd);
    close(pids[0]);
}
