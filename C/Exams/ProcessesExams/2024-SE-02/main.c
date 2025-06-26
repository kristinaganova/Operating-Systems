#include <err.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct program {
    char* programName;
    pid_t pid;
    bool isDone;
} program;

int getIndex(program* programs, int size, pid_t searched) {
    for (int i = 0; i < size; i++) {
        if (!programs[i].isDone && programs[i].pid == searched) {
            return i;
        }
    }

    return -1;
}

void executeCommand(char* programName, program* programs, int index) {
    pid_t pid = fork();

    if (pid == -1) {
        err(2, "Error while forking");
    }
   
    programs[index].programName = programName;
    programs[index].isDone = false;;
    programs[index].pid = pid;
 
    if (pid == 0) {
        execlp(programName, programName, (char*)NULL);
        err(3, "Error while executing program %s", programName);
    }
}

int main(int argc, char* argv[]) {
	if (argc < 2 || argc > 11) {
        errx(1, "Expected between 1 and 10 arguments");
	}

	program programs[10];

	for (int i = 1; i < argc; i++) {
        executeCommand(argv[i], programs, i - 1);	
	}

    int count = 0;
    while (1) {
        if (count == argc - 1) break;

        int status;
        pid_t pid;

        if ((pid = wait(&status)) == -1) {
            err(4, "Errro while waiting");
        }

        int index = getIndex(programs, argc -1, pid);
        
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != 0) {
                executeCommand(programs[index].programName, programs, index);
            } else {
                programs[index].isDone = true;
                count++;
            }
        } else {
            programs[index].isDone = true;
            for (int i = 0; i < argc - 1; i++) {
                if (!programs[i].isDone) {
                    if (kill(programs[i].pid, SIGTERM) == -1) {
                        err(5, "kill");
                    }
                }
            }

            while (wait(NULL) > 0) {}

            exit(index + 1);
        }
    }
}
