#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	if (argc != 2) { errx(1, "Expected 1 argument"); }

	int fd;
	if ((fd = open(argv[1], O_RDONLY)) == -1) { err(2, "Errro while opening file %s", argv[1]); }

    int pipeFds[2];
    if (pipe(pipeFds) == -1) { err(3, "Errro while creating pipe"); }
	
	int pid = fork();
	if (pid == -1) { err(5, "Error while forking"); }
    if (pid == 0) {
        close(pipeFds[0]);
        if (dup2(pipeFds[1], 1) == -1) { err(6, "Error while redirecting stdout to pipe"); }
        close(pipeFds[1]);

        execlp("cat", "cat", argv[1], (char*)NULL);
        err(4, "Errro while executing command cat");
    }

	pid = fork();
	if (pid == -1) { err(5, "Error while forking"); }
    if (pid == 0) {
        close(pipeFds[1]);
        if (dup2(pipeFds[0], 0) == -1) { err(6, "Error while redirecting stdin from pipe"); }
        close(pipeFds[0]);
        
        execlp("sort", "sort",  (char*)NULL);
        err(4, "Errro while executing command sort");
    }
    
    close(pipeFds[0]);
    close(pipeFds[1]);
    
    int status;
    if (wait(&status) == -1) { err(4, "Error while waiting for child"); }
    
    if ( !WIFEXITED(status) || (WEXITSTATUS(status) != 0)) { err(5, "Process did not finish correctly"); }
    
    if (wait(&status) == -1) { err(4, "Error while waiting for child"); }
    
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) { err(5, "Process did not finish correctly"); }

    close(fd);
}
