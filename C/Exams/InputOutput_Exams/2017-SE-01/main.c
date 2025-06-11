#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <err.h>

void numToStr(int num, char *str) {
    int idx = 0;
    if (num == 0) {
        str[idx++] = '0';
    } else {
        int temp = num;
        while (temp > 0) {
            str[idx++] = (temp % 10) + '0';
            temp /= 10;
        }
    }
    str[idx++] = ' ';
    str[idx] = '\0';

    int left = 0, right = idx - 1;
    while (left < right) {
        char tmp = str[left];
        str[left] = str[right];
        str[right] = tmp;
        left++;
        right--;
    }
}

void process_and_write(char c, char prev, int* newLineCount, bool numeric) {
    if (prev == '\n' && numeric) {
        char numStr[16];
        numToStr(*newLineCount, numStr);
        if (write(1, numStr, strlen(numStr)) != (ssize_t)strlen(numStr)) {
            err(4, "Error while writing");
        }
    }

    if (c == '\n') {
        (*newLineCount)++;
    }

    if (write(1, &c, sizeof(char)) != sizeof(char)) {
        err(4, "Error while writing");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        errx(1, "Expected at least 1 argument");
    }

    bool numeric = false;
    int start = 1;

    if (strcmp(argv[1], "-n") == 0) {
        numeric = true;
        start++;
    }

    int newLineCount = 1;
    for (int i = start; i < argc; i++) {
        const char *current = argv[i];
        char buff[1024];

        if (strcmp(current, "-") == 0) {
            ssize_t readBytes;
            while ((readBytes = read(0, buff, sizeof(buff))) > 0) {
                char prev = '\n';
                for (ssize_t j = 0; j < readBytes; j++) {
                    process_and_write(buff[j], prev, &newLineCount, numeric);
                    prev = buff[j];
                }
            }
            if (readBytes == -1) {
                err(2, "Error while reading from stdin");
            }
            continue;
        }

        int fd = open(current, O_RDONLY);
        if (fd == -1) {
            err(3, "Error while opening file %s", current);
        }

        ssize_t readBytes;
        char c;
        char prev = '\n';
        while ((readBytes = read(fd, &c, sizeof(char))) == sizeof(char)) {
            process_and_write(c, prev, &newLineCount, numeric);
            prev = c;
        }

        if (readBytes == -1) {
            err(5, "Error while reading in %s", current);
        }

        close(fd);
    }

    return 0;
}
