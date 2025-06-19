#include <stdint.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>

typedef struct Triple {
    char filename[8];
    uint32_t offset;
    uint32_t length;
} Triple;

int getFileSize(int fd) {
    struct stat st;

    if (fstat(fd, &st) == -1) {
        err(2, "Cannot fstat");
    }

    return st.st_size;
}

void processTriple(Triple *triple, int writeFd) {
    int fileFd;
    if ((fileFd = open(triple->filename, O_RDONLY)) == -1) {
        err(3, "Errro while opening file %s", triple->filename);
    }

    long unsigned int fileSize = getFileSize(fileFd);
    if (fileSize < (triple->offset + triple->length) * sizeof(uint16_t)) {
        errx(4, "Invalid file");
    }

    if (lseek(fileFd, triple->offset * sizeof(uint16_t), SEEK_SET) == -1) {
        err(8, "Cannot lseek");
    }

    uint16_t res = 0;
    uint16_t num;
    for (uint32_t j = 0; j < triple->length; j++) {
        if (read(fileFd, &num, sizeof(num)) != sizeof(num)) {
            err(7, "Error while reading numbers in file %s", triple->filename);
        }

        res ^= num;
    }

    if (write(writeFd, &res, sizeof(res)) == -1) {
        err(9, "Error writing result");
    }

    close(fileFd);
}

void waitForChildren(int numChildren) {
    for (int i = 0; i < numChildren; i++) {
        int status;
        if (wait(&status) == -1) {
            err(10, "Error while waiting for child");
        }

        if (!WIFEXITED(status)) {
            err(10, "Process did not terminate correctly");
        }

        if (WEXITSTATUS(status) != 0) {
            err(10, "Invalid exit status");
        }
    }
}

void outputResult(uint16_t res) {
    char str[6];

    int len = snprintf(str, sizeof(str), "%X", res);

    if (write(1, str, len) == -1) {
        err(9, "write");
    }

    if (write(1, "\n", 1) == -1) {
        err(9, "write");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        errx(1, "Expected 1 argument");
    }

    int fd;
    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        err(3, "open");
    }

    int size = getFileSize(fd);
    if ((size % 8 != 0) || (size / sizeof(Triple) > 8)) {
        errx(4, "Invalid file size");
    }

    int pids[2];
    if (pipe(pids) == -1) {
        err(6, "pipe");
    }

    Triple triple;
    for (long unsigned int i = 0; i < (size / sizeof(Triple)); i++) {
        if (read(fd, &triple, sizeof(triple)) != sizeof(triple)) {
            err(7, "read triple");
        }

        pid_t pid = fork();

        if (pid == -1) {
            err(5, "fork");
        }

        if (pid == 0) {
            close(pids[0]);
            processTriple(&triple, pids[1]);
            close(pids[1]);
            _exit(0);
        }
    }

    close(pids[1]);

    waitForChildren(size / sizeof(Triple));

    uint16_t res = 0;
    uint16_t num;
    int readSize;
    while ((readSize = read(pids[0], &num, sizeof(num))) > 0) {
        res ^= num;
    }

    outputResult(res);

    close(pids[0]);
    close(fd);
    return 0;
}
