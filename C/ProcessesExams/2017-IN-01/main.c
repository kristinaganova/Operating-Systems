#include <unistd.h>
#include <sys/wait.h>
#include <err.h>
#include <stdio.h>

void waitForChild(void) {
    int status;
    if (wait(&status) == -1) {
        err(4, "wait");
    }

    if (!WIFEXITED(status)) {
        err(5, "Command did not execute correctly");
    }

    if (WEXITSTATUS(status) != 0) {
        err(6, "Command did not finish with status 0");
    }
}

int main(void) {
    int cutToSort[2];
    if (pipe(cutToSort) == -1) {
        err(1, "pipe");
    }

    int pid = fork();
    if (pid == -1) { err(2, "fork"); }
    if (pid == 0) {
        close(cutToSort[0]);
        dup2(cutToSort[1], STDOUT_FILENO); 
        close(cutToSort[1]);
        execlp("cut", "cut", "-d:", "-f7", "/etc/passwd", (char*)NULL);
        err(3, "Error executing cut command");
    }
    close(cutToSort[1]);
    waitForChild();

    int sortToUniq[2];
    if (pipe(sortToUniq) == -1) { err(2, "pipe"); }

    int pid2 = fork();
    if (pid2 == -1) { err(3, "fork"); }
    if (pid2 == 0) {
        close(sortToUniq[0]);
        dup2(cutToSort[0], STDIN_FILENO); 
        dup2(sortToUniq[1], STDOUT_FILENO);
        close(cutToSort[0]);
        close(sortToUniq[1]);
        execlp("sort", "sort", (char*)NULL);
        err(3, "Error executing sort command");
    }
    close(sortToUniq[1]);
    close(cutToSort[0]);  
    waitForChild();

    int uniqToSort[2];
    if (pipe(uniqToSort) == -1) { err(2, "pipe"); }

    int pid3 = fork();
    if (pid3 == -1) { err(3, "fork"); }
    if (pid3 == 0) {
        close(uniqToSort[0]);
        dup2(sortToUniq[0], STDIN_FILENO); 
        dup2(uniqToSort[1], STDOUT_FILENO);
        close(sortToUniq[0]);
        close(uniqToSort[1]);
        execlp("uniq", "uniq", "-c", (char*)NULL);
        err(3, "Error executing uniq command");
    }
    close(uniqToSort[1]);
    close(sortToUniq[0]);  
    waitForChild();

    int pid4 = fork();
    if (pid4 == -1) { err(2, "fork"); }
    if (pid4 == 0) {
        dup2(uniqToSort[0], STDIN_FILENO); 
        close(uniqToSort[0]);
        execlp("sort", "sort", "-n", (char*)NULL);
        err(3, "Error executing final sort command");
    }
    waitForChild();

    return 0;
}