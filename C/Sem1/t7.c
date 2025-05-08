/*Koпирайте файл /etc/passwd в текущата ви
 работна директория и променете разделителят 
 на копирания файл от ":", на "?"*/
 
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char* argv[]) {

    if ( argc != 3 ) {
        errx(1, "Expected 2 arguments");
    }

    int fd;
    const char* file = argv[1];
    if ( (fd = open("/etc/passwd", O_RDONLY) ) == -1 ) {
        err(2, "Error opening file %s" , file);
    }

    const char* output = argv[2];

    int fdOutput;
    if ( (fdOutput = open(output, O_CREAT | O_WRONLY | O_TRUNC, 774) ) == -1 ) {
        err(2, "Error opening file %s", output);
    }

    char c;
    ssize_t read_bytes;

    while ( ( read_bytes = read(fd, &c, sizeof(char))) > 0)  {
        if ( c == ':' ) {
            c = '?';
        }

        if ( (write(fdOutput, &c, sizeof(char))) != 1 ) {
            err(3, "Error while writing" );
        }
    }

    if ( read_bytes == -1 ) {
        err( 4, "Error while reading" );
    }

    close(fd);
    close(fdOutput);

    exit(0);

}
