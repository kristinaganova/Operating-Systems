#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

const char* WORDS[] = { "tic ", "tac ", "toe\n" };

void waitChild(void) {
    int status;
    if (wait(&status) == -1) {
        err(7, "wait");
    }

    if (WEXITSTATUS(status) != 0) {
        err(8, "Did not terminated correctly");
    }
}

void writeToPipe(int from, int to, int WC) {
    int current;

    int readSize;
    while ((readSize = read(from, &current, sizeof(current))) > 0 ) {
        if (current >= WC) {
            break;
        }

        if (write(1, WORDS[current % 3] , 4) == -1) {
            err(5, "write");
        }

        sleep(1);

        current++;
        if (write(to, &current, sizeof(current)) == -1) {
            err(5, "error writing to next pipe");
        }
    }

    if (readSize == -1) {
        err(6, "Error while reading");
    }

    current++;
    if (write(to, &current , sizeof(current)) == -1) {
            err(5, "write last");
    }

    close(from);
    close(to);
}

int main(int argc, char* argv[]) {
        if (argc != 3) {
        errx(1, "Expected 2 arguments: NC and WC");
        }

    int NC = atoi(argv[1]);
    if (NC < 1 || NC > 7) {
        errx(2, "NC should be between 1 and 7");
    }

    int WC = atoi(argv[2]);
    if (WC < 1 || WC > 35) {
        errx(2, "WC should be between 1 and 35");
    }

    int counter = 0;
    int fds[8][2];

    for (int i = 0; i < NC + 1; i++) {
        if (pipe(fds[i]) == -1) {
            err(3, "pipe");
        }
    }

    for (int i = 0; i < NC; i++) {
        int pid = fork();
        if (pid == -1) {
            err(4, "fork");
        }

        if (pid == 0) {
            for (int j = 0; j < NC + 1; j++) {
                if (j == i) {
                    close(fds[j][1]);
                } else if(j == i + 1 ) {
                    close(fds[j][0]);
                } else {
                    close(fds[j][0]);
                    close(fds[j][1]);
                }
            }

            writeToPipe(fds[i][0], fds[i + 1][1], WC);

            exit(0);
        }
    }

    for (int i = 0; i <= NC; i++) {
        if (i == NC) {
            close(fds[i][1]);
        } else if(i == 0) {
            close(fds[i][0]);
        } else {
            close(fds[i][0]);
            close(fds[i][1]);
        }
    }

    if (write(fds[0][1], &counter, sizeof(counter)) == -1) {
        err(5, "write");
    }

    writeToPipe(fds[NC][0], fds[0][1], WC);


    for (int i = 0; i < NC; i++) {
        waitChild();
    }

}
