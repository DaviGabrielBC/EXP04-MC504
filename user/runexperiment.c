#include "kernel/types.h"
#include "user/user.h"

void run_experiment(int cpu_count, int io_count) {
    for (int i = 0; i < cpu_count; i++) {
        if (fork() == 0) {
            exec("cpu_task", 0);
            exit(0);
        }
    }
    for (int i = 0; i < io_count; i++) {
        if (fork() == 0) {
            exec("io_task", 0);
            exit(0);
        }
    }
    while (wait(0) != -1) {} // Espera todos os processos terminarem
}

int main() {
    for (int round = 0; round < 30; round++) {
        int cpu_count = 6 + (round % 9);
        int io_count = 20 - cpu_count;
        printf("Round %d: %d CPU-bound, %d IO-bound\n", round, cpu_count, io_count);
        run_experiment(cpu_count, io_count);
    }
    exit(0);
}
