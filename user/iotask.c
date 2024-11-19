#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

unsigned int rand_state = 1; // Estado inicial para o gerador

int rand() {
    rand_state = rand_state * 1103515245 + 12345;
    return (rand_state / 65536) % 32768; // Retorna um número entre 0 e 32767
}

char randchar() {
    return 'A' + (rand() % 26);
}


void write_file(char *filename) {
    int fd = open(filename, O_CREATE | O_RDWR);
    if (fd < 0) {
        printf("Error creating file\n");
        exit(1);
    }
    for (int i = 0; i < 100; i++) {
        char line[101];
        for (int j = 0; j < 100; j++) {
            line[j] = randchar();
        }
        line[100] = '\n';
        write(fd, line, 101);
    }
    close(fd);
}

void permute_file(char *filename) {
    // Ler linhas, permutar e sobrescrever (implementação simplificada para ideia geral)
}

void delete_file(char *filename) {
    unlink(filename);
}

int main() {
    char *filename = "testfile.txt";
    write_file(filename);
    permute_file(filename);
    delete_file(filename);
    exit(0);
}
