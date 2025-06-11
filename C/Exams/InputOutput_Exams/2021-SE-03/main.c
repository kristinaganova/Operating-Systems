#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <err.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

void write_all(int fd, const void *buf, size_t count) {
    const char *ptr = (const char *)buf;
    size_t total_written = 0;

    while (total_written < count) {
        ssize_t written = write(fd, ptr + total_written, count - total_written);
        if (written == -1) {
            err(7, "Error while writing");
        }
        total_written += written;
    }
}

uint16_t swap_uint16(uint16_t val) {
    return (val << 8) | (val >> 8);
}

bool isLittleEndian(void) {
    unsigned int x = 1;
    return (*(char*)&x == 1);
}

int uint_to_str(uint32_t num, char* buf) {
    int i = 0;
    char temp[10]; 

    if (num == 0) {
        buf[0] = '0';
        return 1;
    }

    while (num > 0) {
        temp[i++] = '0' + (num % 10);
        num /= 10;
    }

    for (int j = 0; j < i; j++) {
        buf[j] = temp[i - j - 1];
    }
    return i;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        errx(1, "Expected 2 arguments: input_file output_file");
    }

    int fd_input = open(argv[1], O_RDONLY);
    if (fd_input == -1) err(2, "Opening input file");

    int fd_output = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd_output == -1) {
        err(3, "Opening output file");
    }

    struct stat st;
    if (fstat(fd_input, &st) == -1) {
        err(4, "Stat input file");
    }

    if (st.st_size % sizeof(uint16_t) != 0) {
        errx(5, "Input file size not multiple of 2");
    }

    uint32_t n = st.st_size / sizeof(uint16_t);
    if (n > 524288) {
        errx(6, "Input file too large");
    }

    bool littleEndian = isLittleEndian();

    const char* include_header = "#include <stdint.h>\n\n";
    write_all(fd_output, include_header, strlen(include_header));

    const char* arrN_start = "const uint32_t arrN = ";
    write_all(fd_output, arrN_start, strlen(arrN_start));

    char buf[20];
    int len = uint_to_str(n, buf);
    write_all(fd_output, buf, len);

    const char* semicolon_newline = ";\n";
    write_all(fd_output, semicolon_newline, 2);

    const char* arr_start = "const uint16_t arr[] = {";
    write_all(fd_output, arr_start, strlen(arr_start));

    uint16_t num;
    bool first = true;
    ssize_t bytes_read;

    while ((bytes_read = read(fd_input, &num, sizeof(num))) == sizeof(num)) {
        if (!littleEndian) {
            num = swap_uint16(num);
        }

        if (!first) {
            write_all(fd_output, ", ", 2);
        } else {
            first = false;
        }

        len = uint_to_str(num, buf);
        write_all(fd_output, buf, len);
    }

    if (bytes_read == -1) {
        err(8, "Reading input file");
    }

    const char* arr_end = "};\n";
    write_all(fd_output, arr_end, strlen(arr_end));

    close(fd_input);
    close(fd_output);

    return 0;
}