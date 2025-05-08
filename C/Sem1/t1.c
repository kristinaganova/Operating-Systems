//Копирайте съдържанието на файл1 във файл2

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
    int fd1;
    int fd2;
    char c;

    if (argc != 3) {
        errx(1, "3 arguments expected");
    }

    const char* source = argv[1];
    const char* dest = argv[2];

    fd1 = open(source, O_RDONLY);
    if(fd1 == -1) {
        err(2, "Error while opening %s", source);
    }

    if( (fd2 = open(dest, O_CREAT | O_WRONLY | O_TRUNC, S_IWUSR | S_IRUSR) ) == -1 ) {        
        close(fd1);
        err(2, "Error while opening %s", dest);
    }

    int read_size;
    while ( (read_size = read(fd1, &c, 1)) == 1)
    {
        if (write(fd2, &c, 1) < 0){
            close(fd1);
            close(fd2);
            err(3, "Error while writing");
        }
    }

    if (read_size < 0 ) {
        err(3, "Error while reading");
    }
    if (close(fd1) < 0 || close(fd2) < 0) {
        err(4, "Error while closing files");
    }

    exit(0);
}
