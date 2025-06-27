
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_RAM_SIZE 512
#define MAX_REGISTER_COUNT 32
#define FILENAME_SIZE 8

typedef struct Processor {
    uint16_t ram;
    uint16_t count;
    char filename[FILENAME_SIZE];
} Processor;

typedef struct Instruction {
    uint8_t opcode;
    uint8_t op1;
    uint8_t op2;
    uint8_t op3;
} Instruction;

int openFile(const char* filename, mode_t flags, int mode) {
    if (filename == NULL) {
        errx(2, "Filename cannot be null");
    }

    int fd;

    if ((fd = open(filename, flags, mode)) == -1) {
        err(3, "Error while opening file %s", filename);
    }

    return fd;
}

void eval(Instruction* instr, uint8_t* regs, uint8_t* ram, off_t start, int fd) {
    switch(instr->opcode) {
        case 0:
            regs[instr->op1] = regs[instr->op2] & regs[instr->op3];
            break;
        case 1:
            regs[instr->op1] = regs[instr->op2] | regs[instr->op3];
            break;
        case 2:
            regs[instr->op1] = regs[instr->op2] + regs[instr->op3];
            break;
        case 3:
            regs[instr->op1] = regs[instr->op2] * regs[instr->op3];
            break;
        case 4:
            regs[instr->op1] = regs[instr->op2] ^ regs[instr->op3];
            break;
        case 5:
            if (write(1, &regs[instr->op1], 1) != 1) {
                err(4, "write err to stdout");
            }
            break;
        case 6:
            sleep(regs[instr->op1]);
            break;
        case 7:
            regs[instr->op1] = ram[regs[instr->op2]];
            break;
        case 8:
            ram[regs[instr->op2]] = regs[instr->op1];
            break;
        case 9:
            if (regs[instr->op1] != regs[instr->op2]) {
            off_t offset = start + instr->op3 * sizeof(Instruction);
            if (lseek(fd, offset, SEEK_SET) == -1)
                    err(5, "lseek err");
            }
            break;
        case 10:
            regs[instr->op1] = instr->op2;
            break;
        case 11:
            ram[regs[instr->op1]] = instr->op2;
            break;
        default:
                errx(6, "invalid opcode");

    }
}

void execute(Processor* info) {
    uint8_t registers[MAX_REGISTER_COUNT] = {0};
    uint8_t ram[MAX_RAM_SIZE];

    int fd = openFile(info->filename, O_RDWR, 0);

    if (read(fd, registers, info->count) != info->count) {
        err(4, "Error while reading registers");
    }

    if (read(fd, ram, info->ram) != info->ram) {
        err(4, "Error while reading ram");
    }

    off_t start = lseek(fd, 0, SEEK_CUR);
    if (start == -1) {
        err(7, "Error while lseek");
    }

    Instruction instr;
    int readSize;
    while ((readSize = read(fd, &instr, sizeof(instr))) > 0) {
        eval(&instr, registers, ram, start, fd);
    }

    if (readSize == -1) {
        err(4, "Error while reading instructions");
    }

    if (lseek(fd, 0, SEEK_SET) == -1) {
        err(7, "Error while lseek");
    }

    if (write(fd, registers, info->count) != info->count)
        err(8, "write err registers");

    if (write(fd, ram, info->ram) != info->ram)
        err(8, "write err RAM");

    close(fd);
    exit(0);
}

int main(int argc, char* argv[]) {
        if (argc != 2) {
        errx(1, "Expected one argument");
        }

    int inputFd = openFile(argv[1], O_RDONLY, 0);

    Processor processor;

    int readSize;
    while ((readSize = read(inputFd, &processor, sizeof(Processor))) > 0) {

        pid_t pid = fork();
        if (pid == -1) {
            err(6, "fork");
        }

        if (pid == 0) {
            execute(&processor)
        }
    }

    if (readSize == -1) {
        err(4, "Error while reading");
    }
}
