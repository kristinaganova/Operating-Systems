
#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void waitChild(void) {
    int status;

    if (wait(&status) == -1) {
        err(4, "wait");
    }

    if (!WIFEXITED(status)) {
        err(5, "child did not terminate correctly");
    }
}

void child(int N, int to , int from) {
    for (int i = 0; i < N; i++) {
        char sig;

        if (read(from, &sig, sizeof(sig)) == -1) {
            err(6, "read");
        }

        if (write(1, "DONG\n", 5) == -1) {
            err(7, "write");
        }

        if (write(to, &sig, sizeof(sig)) == -1) {
            err(7, "write");
        }
    }
}

void parent(int N, int D, int to, int from) {
    for (int i = 0; i < N; i++) {
        if (write(1, "DING ", 5) == -1) {
            err(7, "write");
        }

        char sig = 'a';

        if (write(to, &sig, sizeof(sig)) == -1) {
            err(7, "write");
        }

        if (read(from, &sig, sizeof(sig)) == -1) {
            err(6, "read");
        }

        sleep(D);
    }
}

int main(int argc, char* argv[]) {
        if (argc != 3) {
        errx(1, "expected 2 arguments");
        }

        int N = atoi(argv[1]);

        if (N < 1) {
        errx(1, "Invalid N");
        }

        int D = atoi(argv[2]);
    if (D < 0) {
        errx(1, "Invalid D");
    }

        int pids[2][2];

        if (pipe(pids[0]) == -1) {
        err(2, "pipe");
        }

        if (pipe(pids[1]) == -1) {
        err(2, "pipe");
        }

        pid_t pid = fork();

    if (pid == -1) {
        err(3, "fork");
    }

    if (pid == 0) {
        close(pids[0][1]);
        close(pids[1][0]);

        child(N, pids[1][1], pids[0][0]);

        close(pids[0][0]);
        close(pids[1][1]);

        exit(0); 
    }

        close(pids[0][0]);
        close(pids[1][1]);

        parent(N, D, pids[0][1], pids[1][0]);

    close(pids[0][1]);
    close(pids[1][0]);

    waitChild();
}
