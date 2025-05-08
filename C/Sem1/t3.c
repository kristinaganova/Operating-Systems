/*Реализирайте команда wc, с един аргумент подаден като входен параметър*/
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdbool.h>

void writeInt(int n) {
    char buff[16];
    int i = 14;
    buff[15] = '\n';

    if ( n == 0 ) {
        buff[i--]='0';
    }
    else {
        while(n > 0 && i >= 0) {
            buff[i--] = '0' + (n%10);
            n/=10;
        }
    }
    write(1, &buff[i+1], 15-i);
}

int main(int argc, char* argv[]) {
        if ( argc != 2) {
            errx(1, "2 arguments expected");
        }

    const char* file = argv[1];

    int fd1;
    if ( (fd1 = open( file, O_RDONLY) ) == -1) {
        err(2, "Error while opening %s", file);
    }

    unsigned char c;
    int chars = 0;
    int words = 0;
    int lines = 0;
        ssize_t read_size;

    bool word = false;
        while( (read_size = read(fd1, &c, sizeof(c))) == sizeof(c)) {
        chars++;

        if ( c == ' ' ) {
            if (word) {
                words++;
                word=false;
            }
        }
        else if (c == '\n') {
            lines = lines + 1;
            if (word) {
                words++;
                word=false;
            }
        }
        else {
            word=true;
        }
    }
        
        if (read_size < 0) {
            close(fd1);
            err(4, "Error while reading");
        }

        writeInt(chars);
        writeInt(words);
        writeInt(lines);
        close(fd1);
}
