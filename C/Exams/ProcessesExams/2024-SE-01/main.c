#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <err.h>
#include <stdint.h>

int openFile(const char* filename, mode_t flags, int mode) {
    int fd;

    if ((fd = open(filename, flags, mode)) == -1) {
        err(4, "Error while opening file %s", filename);
    }

    return fd;
}

void waitChild(int pid, char* outfile, uint16_t S, uint8_t* bytes) {
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        err(10, "wait");
    }

    if (!WIFEXITED(status)) {
        int outputFd = openFile(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (write(outputFd, bytes, S) != S) {
            err(9, "write");
        }

        close(outputFd);
        exit(42);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        errx(1, "Expected 3 arguments");
    }

    char* program = argv[1];
    int numTests = atoi(argv[2]);
    char* result = argv[3];

    if (numTests < 0 || numTests > (1 << 8)) {
        errx(2, "Invalid number of tests ");
    }

    int randFd = openFile("/dev/random", O_RDONLY, 0);

    for (int i = 0; i < numTests; i++) {
        uint16_t S;
        if (read(randFd, &S, sizeof(S)) == -1) {
            err(5, "read S");
        }

        if (S < 1) {
            continue;
        }

        uint8_t bytes[65535];
        if (read(randFd, bytes, S) != S) {
            err(5, "read bytes");
        }

        int fds[2];
        if (pipe(fds) == -1) {
            err(6, "pipe");
        }

        int pid = fork();
        if (pid == -1) {
            err(3, "fork");
        }

        if (pid == 0) {
            close(fds[1]);

            if (dup2(fds[0], 0) == -1) {
                err(7, "dup2");
            }
            close(fds[0]);

            int nullFd = openFile("/dev/null", O_WRONLY, 0);

            if (dup2(nullFd, 1) == -1) {
                err(7, "dup2");
            }
            if (dup2(nullFd, 2) == -1) {
                err(7, "dup2");
            }
            close(nullFd);

            close(randFd);
            execlp(program, program, (char*)NULL);
            err(8, "execlp");
        }

        close(fds[0]);

        if (write(fds[1], bytes, S) != S) {
            err(9, "write");
        }
        
        close(fds[1]);

        waitChild(pid, result, S, bytes);
    }

    int outputFd = openFile(result, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    close(randFd);
    close(outputFd);

    exit(0);
}
