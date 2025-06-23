#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>

void waitChild(void) {
    int status;

    if (wait(&status) == -1) {
        err(6, "wait");
    }

    if (!WIFEXITED(status)) {
        err(6, "Proccess was killed");
    }

    if (WEXITSTATUS(status) != 0) {
        err(6, "Exit status not 0");
    }
}

int openFile(const char* filename, mode_t flags, int mode) {
    int fd;
    if ((fd = open(filename, flags, mode)) == -1) {
        err(5, "open");
    }

    return fd;
}

void executeMd5sum(char* filename) {
    int fds[2];

    if (pipe(fds) == -1) {
        err(3, "pipe");
    }

    int pid = fork();
    if (pid == -1) {
        err(4, "fork");
    }

    if (pid == 0) {
        close(fds[0]);
        dup2(fds[1], 1);
        close(fds[1]);

        execlp("md5sum", "md5sum", filename, (char*)NULL);
        err(5, "exec");
    }
    close(fds[1]);

    char hashFilename[1024];
    strcpy(hashFilename, filename);
    strcat(hashFilename, ".hash");

    int hashFd = openFile(hashFilename, O_WRONLY | O_TRUNC | O_CREAT, 0644); 

    char byte;
    int bytesRead;
    while ((bytesRead = read(fds[0], &byte, sizeof(byte))) > 0) {
        if (byte == ' ') {
            break;
        }

        if (write(hashFd, &byte, bytesRead) == -1) {
            err(6, "write");
        }
    }

    byte = '\n';
    if (write(hashFd, &byte, sizeof(byte)) == -1) {
        err(6, "write");
    }

    close(fds[0]);
    close(hashFd);

}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        errx(1, "Expected 1 argument: Directory name");
    }

    struct stat st;
    if (stat(argv[1], &st) == -1) {
        err(2, "Error using stat");
    }
    if (!S_ISDIR(st.st_mode)) {
        errx(1, "Argument is not a directory");
    }

    int pipeFds[2];
    if (pipe(pipeFds) == -1) {
        err(3, "pipe");
    }

    int pid = fork();
    if (pid == -1) {
        err(4, "fork");
    }

    if (pid == 0) {
        close(pipeFds[0]);
        dup2(pipeFds[1], 1);
        close(pipeFds[1]);

        int fd = openFile("/dev/null", O_WRONLY);
        dup2(fd, 2);
        close(pid);

        execlp("find", "find", argv[1], "-type", "f", "-not", "-name", "*hash", "-print0", (char*)NULL);
        err(5, "exec");
    }
    close(pipeFds[1]);
    waitChild();

    int count = 0;
    char filename[1024];
    int index = 0;
    char byte;
    while(read(pipeFds[0], &byte, sizeof(byte)) > 0) {
        if (byte == '\0') {
             count++;
             filename[index] = '\0';
             executeMd5sum(filename);
             index = 0;
        } else {
            filename[index++] = byte;
        }
    }
    close(pipeFds[0]);

    for (int i = 0; i < count; i++) {
        waitChild();
    }
}
