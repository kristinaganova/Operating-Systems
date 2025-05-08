/*Реализирайте команда cp, работеща с произволен брой подадени входни параметри.*/

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

bool isDirectory(const char* path) {
    struct stat statbuf;

    if (stat(path, &statbuf) == -1) {
        return false;
    }

    if (S_ISDIR(statbuf.st_mode)) {
        return true;
    } else {
        return false;
    }
}

int main(int argc, char* argv[]) {

        if ( argc < 2) {
            errx(1, "at least 2 arguments expected");
        }

    const char* dest = argv[argc - 1];

    if ( !isDirectory(dest)) {
        errx(1, "Last param expected to be a directory");
    }


    for(int count = 1; count < argc - 1; count++) {

        const char* file = argv[count];
        int fd;
        if ( (fd = open(file, O_RDONLY) ) == -1) {
            err(2, "Error opening file %s", file);
        }

        char newFile[1024];
        strcpy(newFile, dest);
        strcat(newFile, "/");
        strcat(newFile, file);
        newFile[strlen(dest) + strlen(file) + 1] = '\0';

        int fd2;
        if ( (fd2 = open(newFile, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU | S_IRWXG | S_IROTH) ) == -1) {
            close(fd);
            err(2, "Error opening file %s", newFile);
        }

        char c;
        ssize_t read_bytes;
        while ( (read_bytes = read(fd, &c, sizeof(c))) > 0 ) {
            if ( write(fd2, &c, sizeof(c)) != 1 ) {
                close(fd);
                close(fd2);
                err(3, "Error while writting");
            }
        }

        if (read_bytes == -1) {
            close(fd);
            close(fd2);
            err(4, "Error while reading");
        }

        close(fd);
        close(fd2);
    }

    exit(0);
}
