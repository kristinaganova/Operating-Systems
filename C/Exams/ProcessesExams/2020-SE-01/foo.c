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
        const char* fileName = argv[1];

    //if (mkfifo(pipeName, 0666) == -1) {
    //    err(2, "mkfifo");
        //}


        int fd = open(pipeName, O_WRONLY);

        if (fd == -1) {
            err(4, "Error while opening file");
        }

        if (dup2(fd, 1) == -1) {
            err(5, "dup2");
        }
        close(fd);

        execlp("cat", "cat", fileName, (char*)NULL);
        err(6, "execlp");
}
