#include <err.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>

int getLen(long num) {
    if (num == 0) return 1;
    int len = 0;
    while (num > 0) {
        num /= 10;
        len++;
    }
    return len;

}

void writeNumber(int fd, long num, char end) {
    char buff[32];
    int len = getLen(num);

    for (int i = len; i > 0; i--) {
        buff[i] = (num % 10) + '0';
        num /= 10;
    }

    if (write(fd, &buff, len * sizeof(char)) == -1) {
        err(6, "Error while writing");
    }

    if (write(fd, &end, sizeof(char)) == -1) {
         err(6, "Error while writing");
    }
}

int waitChild(void) {
    int status;
    wait(&status);

    int exit_code;

    if (WIFEXITED(status)) {
        exit_code = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        exit_code = 129;
    } else {
        exit_code = 128;
    }

    return exit_code;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        errx(1, "Expected at lest 2 arguments");
    }

    int fd;
    if ((fd = open("./run.log", O_CREAT | O_WRONLY | O_APPEND, 0644)) == -1) {
        err(4, "Errro while opening file");
    }

    int secs = atoi(argv[1]);
    if (secs < 1 || secs > 9) {
        errx(2, "Invalid durration");
    }

    char* command = argv[2];

    char **q_args = &argv[2];
    int count = 0;

    while(1) {
        if (count == 2) {
            break;
        }

        time_t start = time(NULL);
        int pid = fork();

        if (pid == -1) {
            err(2, "fork");
        }

        if (pid == 0) {
            execvp(command, q_args);
            err(3, "Error executing command");
        }


        int exit_code = waitChild();
        time_t end = time(NULL);

        writeNumber(fd, start, ' ');
        writeNumber(fd, end, ' ');
        writeNumber(fd, exit_code, '\n');

        if (exit_code != 0 && end - start < secs) {
            count++;
        } else {
            count = 0;
        }
    }
    close(fd);
}
