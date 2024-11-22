#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int iterations = 0;

int rand(){
    unsigned int rand_state = uptime() + iterations; // (bulwark)
    rand_state = rand_state % 21013; // Estado inicial para o gerador
    rand_state = (rand_state) * 12047 + 12345;
    iterations++;
    return (rand_state) % 32768; // Retorna um número entre 0 e 32767
}

char randchar(){
    char chars[] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789";
    return chars[rand() % 62];
}

void write_file(char *filename){
    int fd = open(filename, O_CREATE | O_RDWR);
    if(fd < 0){
        printf("Error creating file\n");
        exit(1);
    }
    for(int i = 0; i < 100; i++){
        char line[101];
        for(int j = 0; j < 100; j++){
            line[j] = randchar();
        }
        line[100] = '\0';
        write(fd, line, 101);
    }
    close(fd);
}

void permute_file(char *filename){
    // Ler linhas, permutar e sobrescrever (implementação simplificada para ideia geral)
    int fd;
    for(int i = 0; i < 50; i++){ // Seleciona 2 linhas quaisquer e as troca de posição (x50)
        int line1 = rand() % 100;
        int line2 = rand() % 100;

        while(line1 == line2) line2 = rand() % 100; // Pouco provável

        // Recuperamos o conteúdo da line1 e da line2 e colocamos isso em 2 strings temporárias
        fd = open(filename, O_RDONLY);
        char line1_temp[101] = {0};
        for(int j = 0; j < line1; j++) // Avança o file pointer até o endereço desejado (bulwark)
            read(fd, line1_temp, 101);
        close(fd);

        fd = open(filename, O_RDONLY); // Reinicia o file pointer (bulwark)
        char line2_temp[101] = {0};
        for(int j = 0; j < line2; j++)
            read(fd, line2_temp, 101);
        close(fd);

        //Agora escrevemos o conteúdo de line1_temp na posição line2 e vice-versa
        char garbage[101];

        fd = open(filename, O_RDWR);
        for(int j = 0; j < line2-1; j++)
            read(fd, garbage, 101);
        write(fd, line1_temp, 101); // posição da linha line2
        close(fd);

        fd = open(filename, O_RDWR);
        for(int j = 0; j < line1-1; j++)
            read(fd, garbage, 101);
        write(fd, line2_temp, 101); // posição da linha line1
        close(fd);
    }
}

void delete_file(char *filename){
    unlink(filename);
}

int main(){
    char *filename = "testfile.txt";
    write_file(filename);
    permute_file(filename);
    delete_file(filename);
    exit(0);
}
