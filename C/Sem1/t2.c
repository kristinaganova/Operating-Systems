/*
Реализирайте команда head без опции 
(т.е. винаги да извежда на стандартния изход само първите 10 реда 
от съдържанието на файл подаден като първи параматър)
*/

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char* argv[]) {
        if ( argc != 2) {
            errx(1, "2 arguments expected");
        }

    const char* file=argv[1];
    int fd1;
        if ( (fd1 = open( file, O_RDONLY) ) == -1) {
            err(2, "Error while opening %s", file);
        }

    unsigned char c;
        ssize_t read_size;
    int cnt = 0;

        while( (read_size = read(fd1, &c, sizeof(c))) == sizeof(c)) {
        if (write(1, &c, sizeof(c)) < 0 ) {
            close(fd1);
            err(3, "Error while writing");
        }

        if (c == '\n') {
            cnt = cnt + 1;
            if (cnt == 10) {
                break;
            }
        }
        }

        if (read_size < 0) {
            close(fd1);
            err(4, "Error while reading");
        }

        if (close(fd1) < 0) {
            err(5, "Error while closing file");
        }
        exit(0);
}
