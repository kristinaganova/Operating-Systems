//not wroking

#include <stdint.h>
#include <string.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct Header {
    uint16_t magic;
    uint16_t ver;
    uint16_t cp;
    uint16_t co;
} Header;

typedef struct Preamble {
    uint16_t v1;
    uint16_t v2;
    uint16_t v3;
} Preamble;

typedef struct Object {
    uint32_t ctime;
    uint16_t opt;
    uint16_t parent_id;
    uint32_t size;
    uint32_t ssize;
} Object;

void checkHeader(Header* h, int fd, uint16_t magic, uint16_t version) {
    if ( read(fd, h, sizeof(*h)) == -1 ) {
        err(3, "Error while reading");
    }

    if ( h->magic != magic || h->ver != version ) {
        errx(4, "Invalid magic or version");
    }
}

int main(int argc, char* argv[]) {
    if ( argc != 2 ) {
        errx(1, "Expected 2 arguments");
    }

    int fd = open(argv[1], O_RDONLY);
    
    if ( fd == -1 ) {
        err(2, "Error while opening file");
    }

    Header h;
    checkHeader(&h, fd, 0x6963, 0x6e73);
    
    Preamble p;
    for ( int i = 0; i < h.cp; i++) {
        if ( read(fd, &p, sizeof(p)) == -1 ) {
            err(3, "Error while reading preamble");
        }
    }

    Object objs[100];

    for (int i = 0; i < h.co; i++) {
        Object o;
        if ( read(fd, &o, sizeof(Object)) == -1 ) {
            err(3, "Erro while reading");
        }
        objs[i] = o;
    }

    int upperSum = 0;
    int lowerSum = 0;
    for ( int i = 0; i < h.co; i++ ) {
        if ( (objs[i].opt & (1 << 14)) != 0 ) {
            errx(4, "Invalid opt version");
        }
        
        if ( (objs[i].opt & (1 << 15)) != 0 ) continue;
        
        if ( objs[i].ssize > objs[i].size ) errx(4, "Invalid sizes");
        
        int xi = objs[i].ssize / objs[i].size;
        int wi = objs[i].size;
        upperSum += (xi*wi);
        lowerSum += wi;

        uint16_t parent = objs[i].parent_id;
        for (int j = 0; j < h.co; j++ ) {
            if ( objs[j].parent_id != parent) continue;
            if ( (objs[i].opt & (1 << 15)) == 0 ) continue;
            
            int xj = objs[j].ssize / objs[j].size;
            int wj = objs[j].size;
            upperSum += (xj*wj);
            lowerSum += wj;

            if ( objs[j].parent_id == 0 ) break;
            parent = objs[j].parent_id;
        }

    }

    int res = upperSum / lowerSum;
    char str[10];
    int len = 0;
    while (res > 0 ) {
        str[len] = res % 10;
        res /= 10;
        len++;
    }
    str[len] = '\0';
    for (int i = 0; i < len / 2; i++ ) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }

    if ( write(1, &str, len * sizeof(char)) != (ssize_t)(len * sizeof(char))) {
        err(6, "Error while writing");
    }

    close(fd);
}
