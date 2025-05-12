/* Напишете програма, която приема точно 2 аргумента. Първият може да бъде 
само --min, --max или --print (вижте man 3 strcmp). Вторият аргумент е двоичен 
файл, в който има записани цели неотрицателни двубайтови числа (uint16_t - вижте man stdint.h). 
Ако първият аргумент е:

--min - програмата отпечатва кое е най-малкото число в двоичния файл.
--max - програмата отпечатва кое е най-голямото число в двоичния файл.
--print - програмата отпечатва на нов ред всяко число.
Използвайте двоичния файл binary/dump или генерирайте сами такъв (правилно подравнен).
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>

void checkFile(const char* file) {
    struct stat st;

    if ( stat(file, &st) == -1 ) {
        err(3, "Error while stat file %s", file);
    }

    if ( st.st_size == 0 ) {
        errx(4, "File %s is empty", file);
    }

    if ( (st.st_size % sizeof(uint16_t) ) != 0 ) {
        errx(5, "File should conatin only uint16_t numbers");
    }
}

uint16_t getMin(uint16_t a, uint16_t b) {
    return ( a < b ) ? a : b ;
}

uint16_t  getMax(uint16_t a, uint16_t b) {
    return ( a > b ) ? a : b ;
}

uint16_t getCompared(int fd, uint16_t (*comp)(uint16_t, uint16_t)) {
    uint16_t N;

    if ( (read(fd, &N, sizeof(N))) == -1 ) {
        err(7, "Error while reading");
    }

    uint16_t number;
    int read_bytes;

    while ( (read_bytes = read(fd, &number, sizeof(number)) ) == sizeof(number) ) {
        N = comp(N, number);
    }

    if ( read_bytes == -1 ) {
        err( 8, "Error while reading" );
    }

    return N;
}

int main(int argc, char* argv[]) {

    if ( argc != 3 ) {
        errx(1, "Expected 2 arguments");
    }

    const char* option = argv[1];
    const char* min = "--min", *max = "--max", *print = "--print";
    if ( (strcmp(option, min) != 0) &&
         (strcmp(option, max) != 0) &&
         (strcmp(option, print) != 0) ) {
        errx(2, "Invalid option");
    }

    const char* file = argv[2];
    checkFile(file);
    int fd;

    if ( (fd = open(file, O_RDONLY)) == -1) {
        err(6, "Error while opening file %s", file);
    }

    if ( strcmp(option, min) == 0 ) {
        uint16_t minimum = getCompared(fd, getMin );

        if ( (write(1, &minimum, sizeof(minimum))) != sizeof(minimum ) ) {
            err(9, "Error while writing" );
        }

    } else if ( strcmp(option, max) == 0 ) {
        uint16_t maximum = getCompared(fd, getMax );

        if ( (write(1, &maximum, sizeof(maximum))) != sizeof(maximum) ) {
            err(9, "Error while writing" );
        }

    } else if ( strcmp(option, print) == 0 ) {
        int read_bytes;
        uint16_t num;
        while ( (read_bytes = read(fd, &num, sizeof(num))) != sizeof(num) ) {
            if ( (write(1, &num, sizeof(num))) != sizeof(num) ) {
                err(9, "Error while writing" );
            }
        }

        if ( read_bytes == -1 ) {
            err(8, "Error while reading");
        }
    }

    close(fd);
    exit(0);
}
