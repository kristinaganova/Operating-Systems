#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <err.h>
#include <string.h>
#include <stdlib.h>

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool checkStr(const char* str) {
    if ( strlen(str) != 3 ) {
        return false;
    }

    return isDigit(str[0]) && (str[1] == '-') && isDigit(str[2]);
}

void printFields(const char* howMuch, const char* file, char sep) {
    int fd;
    if ( (fd = open(file, O_RDONLY)) == -1 ) {
        err(2, "Error while opening file %s", file);
    }

    int start;
    int end;

    if (strlen(howMuch) == 1 ) {
        start = atoi(howMuch);
        end = start;
    } else if (strlen(howMuch) == 3 && checkStr(howMuch)) {
        start = howMuch[0] - '0';
        end = (howMuch[2] - '0');
    } else {
        return;
    }

    char c;
    int readBytes;
    int count = 1;

    while ( (readBytes = read(fd, &c, sizeof(char))) == sizeof(char) ) {
        if ( c == '\n' ) {
            count = 1;
            if ( (write(1, &c, sizeof(c))) != sizeof(c)) {
                err(5, "Error while writing");
            }
                continue;
        }

        if ( count >= start && count <= end) {
            if ( (write(1, &c, sizeof(c))) != sizeof(c)) {
                err(5, "Error while writing");
            }
        }

        if(sep == c || sep == '\0') {
            count++;
        }
    }

    if (readBytes == -1) {
        err(6, "Error while reading");
    }

    close(fd);
}

int main(int argc, char* argv[]) {
        if (argc < 4) {
        errx(1, "Insufficient arguments");
    }

        if ( strcmp(argv[1], "-c") == 0 ) {
        if ( !(strlen(argv[2]) == 1 && isDigit(*argv[2])) && !checkStr(argv[2]) ) {
            errx(2, "Invalid second argument for option -c");
        }

        printFields(argv[2], argv[argc -1], '\0');
        }
        else if ( strcmp(argv[1], "-d") == 0) {
        char sep = argv[2][0];
        if ( strcmp(argv[3], "-f" ) != 0) {
            errx(3, "Third param should be option -f for option -d");
        }

        if ( !(strlen(argv[4]) == 1 && isDigit(*argv[4])) && !checkStr(argv[4]) ) {
            errx(4, "Invalid fourth argument for option -d");
        }

        printFields(argv[4], argv[argc - 1], sep );
        }
        else {
        errx(5, "Invalid options") ;
        }

        exit(0);
}
