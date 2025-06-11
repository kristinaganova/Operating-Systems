#include <unistd.h>
#include <sys/wait.h>
#include <err.h>
#include <stdio.h>
#include <string.h>

void waitChild(void) {
    int status;
    if (wait(&status) == -1) {
        err(5, "wait");
    }

    if (!WIFEXITED(status)) {
        err(6, "Command did not terminate correctly");
    }

    if (WEXITSTATUS(status) != 0) {
        err(7, "Not correct exit status");
    }
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        errx(1, "Expected one or no arguments");
    }

    const char* command = "echo"; 
    if (argc == 2) {
        command = argv[1];
    }

    char byte;
    char buff[5]; 
    int readBytes;
    int index = 0;
    
    while ((readBytes = read(0, &byte, sizeof(byte))) > 0) {
        if (byte == ' ' || byte == '\n' || byte == '\t') {
            buff[index] = '\0';  
            
            if (strlen(buff) > 4) {
                errx(2, "Invalid argument length: word exceeds 4 characters");
            }

            if (strlen(buff) > 0) {
                int pid = fork();
                if (pid == -1) { err(3, "fork"); }

                if (pid == 0) {
                    execlp(command, command, buff, (char*)NULL);
                    err(3, "Error executing command");
                }

                waitChild();  
            }
            index = 0;  
        } else {
            buff[index] = byte;
            index++;
        }
    }

    if (index > 0) {
        buff[index] = '\0';
        if (strlen(buff) > 4) {
            errx(2, "Invalid argument length: word exceeds 4 characters");
        }
        
        int pid = fork();
        if (pid == -1) { err(3, "fork"); }

        if (pid == 0) {
            execlp(command, command, buff, (char*)NULL);
            err(3, "Error executing command");
        }

        waitChild(); 
    }

    if (readBytes == -1) {
        err(4, "read");
    }

    return 0;
}