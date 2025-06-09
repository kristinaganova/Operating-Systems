#include <stdint.h>
#include <err.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_SESSIONS 16384

typedef struct Record {
    uint32_t uid;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t start;
    uint32_t end;
} Record;

typedef struct {
    uint32_t uid;
    uint64_t max_sq_session;
} UserStat;

bool checkFile(const char* path, int size) {
    struct stat st;
    
    if (stat(path, &st) != 0) {
        err(2, "Error while stating file");
    }

    return S_ISREG(st.st_mode) && (st.st_size % size == 0);
}

int openFile(const char* path, int mode, mode_t perms) {
    int fd;

    if ( (fd = open(path, mode, perms)) == -1 ) {
        err(4, "Error while opening file %s", path);
    }

    return fd;
}

void printChar(char c) {
    if (write(1, &c, sizeof(char)) != sizeof(char)) {
        err(5, "Error while writing");
    }
}

void printNum(uint64_t n) {
    if (n == 0) {
        printChar('0');
        return;
    }
    char str[21]; 
    int index = 0;
    while (n > 0) {
        str[index++] = (n % 10) + '0';
        n /= 10;
    }
    for (int i = 0; i < index / 2; i++) {
        char tmp = str[i];
        str[i] = str[index - 1 - i];
        str[index - 1 - i] = tmp;
    }
    if (write(1, str, index) != index) {
        err(5, "Error while writing");
    }
}

double getAverage(uint32_t* arr, int size) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum / size;
}

double getDispersion(uint32_t* arr, int size, double avg) {
    double disp = 0;
    for (int i = 0; i < size; i++) {
        double diff = arr[i] - avg;
        disp += diff * diff;
    }
    return disp / size;
}

int findUserIndex(UserStat* users, int count, uint32_t uid) {
    for (int i = 0; i < count; i++) {
        if (users[i].uid == uid)
            return i;
    }
    return -1;
}

void updateUser(UserStat* users, int* count, uint32_t uid, uint64_t sq_session) {
    int idx = findUserIndex(users, *count, uid);
    if (idx == -1) {
        if (*count < MAX_SESSIONS) {
            users[*count].uid = uid;
            users[*count].max_sq_session = sq_session;
            (*count)++;
        }
    } else {
        if (sq_session > users[idx].max_sq_session) {
            users[idx].max_sq_session = sq_session;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        errx(1, "Expected 2 arguments");
    }

    const char* file = argv[1];

    if (!checkFile(file, sizeof(Record))) {
        errx(3, "Invalid second argument");
    }

    int fd = openFile(file, O_RDONLY, 0);

    Record r;
    uint32_t durations[MAX_SESSIONS];
    uint32_t uids[MAX_SESSIONS];
    int session_count = 0;
    int readBytes = 0;

    while ((readBytes = read(fd, &r, sizeof(r))) == sizeof(r)) {
        if (r.start > r.end) continue;
        if (session_count >= MAX_SESSIONS) break;
        durations[session_count] = r.end - r.start;
        uids[session_count] = r.uid;
        session_count++;
    }
    if (readBytes == -1) {
        err(4, "Error while reading");
    }
    close(fd);

    double avg = getAverage(durations, session_count);
    double disp = getDispersion(durations, session_count, avg);

    UserStat users[MAX_SESSIONS];
    int user_count = 0;

    for (int i = 0; i < session_count; i++) {
        uint64_t sq = (uint64_t)durations[i] * durations[i];
        if ((double)sq > disp) {
            updateUser(users, &user_count, uids[i], sq);
        }
    }

    for (int i = 0; i < user_count; i++) {
        printNum(users[i].uid);
        printChar(' ');
        printNum(users[i].max_sq_session);
        printChar('\n');
    }

    return 0;
}