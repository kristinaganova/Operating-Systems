
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <stdbool.h>

#define  BUFF_SIZE 1024
char buff[BUFF_SIZE] = {'\0'};
bool seen[256] = { false };

void getUserInput(char* str) {
    if ( (read(0, str, BUFF_SIZE)) == -1)  {
        err(2, "Error while reading");
    }
}

void handleDelete(const char* str) {
    getUserInput(buff);

    for ( size_t i = 0; i < strlen(str); i++ ) {
        seen[(int)str[i]] = true;
    }

    for ( size_t i = 0; i < strlen(buff); i++ ) {
        if (seen[(int)buff[i]] == true ) {
            continue;
        }

        if ( (write(1, &buff[i], sizeof(char))) != sizeof(char) ) {
            err(3, "Error while writing");
        }

    }
}

bool contains (const char* str, char c) {
    for ( size_t i = 0; i < strlen(str); i++) {
        if ( str[i] == c ) {
            return true;
        }
    }

    return false;
}

void handleDuplicates(const char* str) {

    getUserInput(buff);

    for (size_t i = 0; i < strlen(buff); i++) {
        if ( seen[(int)buff[i]] == true && contains(str, buff[i]) ) {
            continue;
        }

        if ( (write(1, &buff[i], sizeof(char))) != sizeof(char) ) {
            err(3, "Error while writing");
        }

        seen[(int)buff[i]] = true;
    }

}

void handleReplace(const char* toReplace, const char* replaceWith) {
    if (strlen(toReplace) != strlen(replaceWith)) {
        errx(4, "Strings should have the same length");
    }

    int indices[256] =  {-1};
    for ( size_t i = 0; i < strlen(toReplace); i++ ) {
        seen[(int)toReplace[i]] = true;
        indices[(int)toReplace[i]] = i;
    }

    getUserInput(buff);

    for ( size_t i = 0; i < strlen(buff); i++) {
        if ( seen[(int)buff[i]] == true ) {

            if ( (write(1, &replaceWith[indices[(int)buff[i]]], sizeof(char))) != sizeof(char) ) {
                err(3, "Error while writing");
            }

            continue;
        }

        if ( (write(1, &buff[i], sizeof(char))) != sizeof(char) ) {
            err(3, "Error while writing");
        }
    }


}

int main(int argc, char* argv[]) {
    if ( argc < 2 ) {
        errx(1, "At least 2 arguments expected");
    }


    if ( strcmp(argv[1], "-d") == 0 ) {
        if ( argc != 3 ) {
            errx(1, "Invalid arguments count for option %s", argv[1] );
        }
        handleDelete(argv[2]);
    }
    else if (  strcmp(argv[1], "-s") == 0 ) {
        if ( argc != 3 ) {
            errx(1, "Invalid arguments count for option %s", argv[1] );
        }
        handleDuplicates(argv[2]);
    }
    else {
        if ( argc != 3 ) {
            errx(1, "Invalid arguments count for no options");
        }

        handleReplace(argv[1], argv[2]);

    }
}
