#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
        if (argc != 2) {
        errx(1, "Invalid number of arguments");
        }

        const char* pipeName = "../pipe.txt";
    int fd = open(pipeName, O_RDONLY);

    if (fd == -1) {
        err(4, "Error while opening file");
    }

    if (dup2(fd, 0) == -1) {
        err(5, "dup2");
    }
    close(fd);

    execlp(argv[1], argv[1], (char*)NULL);
    err(6, "execlp");
}
