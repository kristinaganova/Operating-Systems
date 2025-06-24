#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <err.h>
#include <sys/wait.h>

void terminate(pid_t pids[], int count) {
    for (int i = 0; i < count; i++) {
        if (pids[i] != 0) {
            int status;
            pid_t result = waitpid(pids[i], &status, WNOHANG);
            if (result == 0) {
                kill(pids[i], SIGTERM);
                terminated++;
            }
        }
    }
}

int main(int argc, char* argv[]) {
        if (argc == 1) {
        errx(26, "Expected at least 1 program");
        }

        int fds[2];
        if (pipe(fds) == -1) {
        err(26, "pipe");
        }

        pid_t pids[20] = {0};

        for (int i = 1; i < argc; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            err(26, "fork");
        }

        if (pid == 0) {
            close(fds[0]);
            dup2(fds[1], 1);
            close(fds[1]);
            execlp(argv[i], argv[i], (char*)NULL);
            err(26, "exec");
        } else {
            pids[i - 1] = pid;
        }
        }

        close(fds[1]);

    int exitStatus = 1;
        char c;
        int readSize;
        char output[1024];
        int index = 0;
        while ((readSize = read(fds[0], &c, sizeof(c))) > 0)  {
        if (c == '\n') {
            output[index] = '\0';
            index = 0;

            if (strcmp(output, "found it!") == 0) {
                terminate(pids, argc - 1);
                exitStatus = 0;
                break;
            }
        }

        output[index++] = c;
        }

        for (int i = 0; i < argc - 1; i++)  {
        wait(NULL);
        }

        exit(exitStatus);
}
