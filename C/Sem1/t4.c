/*Реализирайте команда cat, работеща с произволен брой подадени входни параметри.*/

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdbool.h>

int main(int argc, char* argv[]) {
        if ( argc < 2) {
            errx(1, "at least 2 arguments expected");
        }

    for(int count = 1; count < argc; count++) {
        const char* file = argv[count];
        int fd;
        if ( (fd = open(file, O_RDONLY) ) == -1){
            err(2, "Error opening file %s", file);
        }

        int read_bytes;
        char c;
        while ( (read_bytes = read(fd, &c, sizeof(c))) > 0 ) {
            if ( write(1, &c, sizeof(c)) < 0) {
                close(fd);
                err(3, "Error while writting");
            }
        }

        if (read_bytes == -1) {
            close(fd);
            err(4, "Error while reading");
        }

        char newline='\n';
        if(( write(fd, &newline, 1)) != 1) {
            close(fd);
            err(5, "Error ending file content");
        }

        close(fd);
    }

    exit(0);
}
