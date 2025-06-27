//for the bonus task we can create our own idx file by itterating through the 
//dictionary file and writing the offsets of each word to the new idx file and
//after that the code remains the same

#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <err.h>

int openFile(const char* path, int flags, mode_t mode) {
    if ( path == NULL ) { errx(2, "Invalid path"); }

    int fd;
    if ( (fd = open(path, flags, mode)) == -1 ) {
        err(3, "Error whiel opening file %s", path);
    }

    return fd;
}

off_t getFileSize(int fd) {
    if (fd < 3) { errx(4, "Invalid file descriptor"); }

    struct stat st;
    if (fstat(fd, &st) == -1) { err(5, "Error while fstating"); }
    
    return st.st_size;
}

void lseekSafe(int fd, int pos, int whence) {
    if (fd < 3) { errx(4, "Invalid file descriptor"); }

    if (lseek(fd, pos, whence) == -1 ) {
        err(6, "Error while lseeking");
    }
}

int main(int argc, char* argv[]) {
	if ( argc != 4 ) {
        errx(1, "Expected 3 arguments");
	}

	const char* searched = argv[1];

	int fd_dic = openFile(argv[2], O_RDONLY, 0);
	int fd_idx = openFile(argv[3], O_RDONLY, 0);

    off_t dicSize = getFileSize(fd_dic);
    off_t idxSize = getFileSize(fd_idx);
    int left = 0;
    int right = idxSize / sizeof(uint32_t);

    while (left < right) {
        int mid = left + (right - left)/2;
        lseekSafe(fd_idx, mid * sizeof(uint32_t), SEEK_SET);

        uint32_t offset;
        if ( read(fd_idx, &offset, sizeof(uint32_t)) == -1 ) {
            err(7, "Errro while reading offset");
        }

        if ( dicSize < offset ) { errx(8, "Invalid offset"); }

        lseekSafe(fd_dic, offset + 1, SEEK_SET);

        char word[64];
        int readBytes;
        int idx = 0;
        while ( (readBytes = read(fd_dic, &word[idx], sizeof(char))) > 0) {
            if (word[idx] == '\n' ) {
                word[idx] = '\0';
                break;
            }         
            idx++;
        }
        if ( readBytes == -1 ) {
            err(7, "Errro while reading word" );
        }

        int res = strcmp(word, searched);
        if (res == 0 ) {
            char c;
            while( (readBytes = read(fd_dic, &c, sizeof(char))) > 0 ) {
                if ( c == '\0' ) {
                    break;
                }

                if ( write(1, &c, sizeof(c)) == -1) {
                    err(8, "Errro while writing");
                }
            }
            if ( readBytes == -1 ) {
                err(7, "Errro while reading word" );
            }

            if(write(1, "\n", sizeof(c)) == -1) {
		err(6, "can't write");
	    }

	    close(fd_dic);
	    close(fd_idx);
            exit(0);
        } else if (res > 0) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }

    close(fd_dic);
    close(fd_idx);
    errx(1, "Word %s has no definition", searched);

}
