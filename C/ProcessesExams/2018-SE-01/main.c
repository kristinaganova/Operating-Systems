#include <unistd.h>
#include <err.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    if (argc != 2) { errx(1, "Expected one argument"); }

    struct stat st;
    if (stat(argv[1], &st) == -1) { err(2, "stat"); }

    if (!S_ISDIR(st.st_mode)) { errx(3, "not a directory"); }

    int findToSort[2];

    if (pipe(findToSort) == -1) { err(5, "pipe"); }

    int pid = fork();

    if (pid == -1) { err(4, "fork");}

    if (pid == 0) {
        close(findToSort[0]);
        dup2(findToSort[1], 1);
        close(findToSort[1]);
        int devnull = open("/dev/null", O_WRONLY);
        if (devnull == -1) {
            err(7, "open /dev/null");
        }
        dup2(devnull, 2);
        close(devnull);
        execlp("find", "find", argv[1], "-type", "f" , "-printf", "%T@ %f\n" , (char*)NULL);
        err(6, "Error while executing command find");
    }
    close(findToSort[1]);
    wait(NULL);

    int sortToHead[2];
    if (pipe(sortToHead) == -1) { err(5, "pipe"); }

    int pid1 = fork();

    if (pid1 == -1) { err(4, "fork");}

    if (pid1 == 0) {
        close(sortToHead[0]);
        dup2(findToSort[0], 0);
        close(findToSort[0]);
        dup2(sortToHead[1], 1);
        close(sortToHead[1]);
        execlp("sort", "sort", "-nr", (char*)NULL);
        err(6, "Error while executing command sort");
    }
    close(sortToHead[1]);
    close(findToSort[0]);
    wait(NULL);

    int pid2 = fork();
    if (pid2 == -1) { err(4, "fork");}

    if(pid2 == 0) {
        dup2(sortToHead[0], 0);
        close(sortToHead[0]);
        execlp("head", "head", "-n1", (char*)NULL);
        err(6, "Error while executing command head");
    }
    close(sortToHead[0]);
    wait(NULL);
}
