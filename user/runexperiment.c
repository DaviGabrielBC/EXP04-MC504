#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "user/user.h"
#define SCALE 1000

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
    //In order to calculate the normalized throughput (bulwark)
    int min_through = (1 << 20); // 2^20 is probably sufficiently big
    int max_through = 0;
    int mean_through = 0;

    for (int round = 0; round < 30; round++) {
        int cpu_count = 6 + (round % 9);
        int io_count = 20 - cpu_count;
        printf("Round %d: %d CPU-bound, %d IO-bound\n", round, cpu_count, io_count);
        run_experiment(cpu_count, io_count);

        int curr_through = throughput();
        if(curr_through < min_through) min_through = curr_through;
        if(curr_through > max_through) max_through = curr_through;
        mean_through += curr_through;
    }
    // Calculating the fairness (bulwark)
    uint64 experiment_fairness = fairness();

    //Calculating the throughput (bulwark)
    uint64 experiment_throughput;
    mean_through = mean_through / 30;
    uint64 temp_throughput = mean_through - min_through;
    temp_throughput = temp_throughput * SCALE;
    experiment_throughput = temp_throughput / (max_through - min_through);

    //Calculating the memoverhead (bulwark)
    uint64 experiment_memoverhead = memoverhead();

    //Calculating the fsefficiency (bulwark)
    uint64 experiment_fsefficiency = fsefficiency();

    uint64 experiment_total_efficiency = experiment_fairness +
                                         experiment_fsefficiency +
                                        experiment_memoverhead +
                                        experiment_throughput;

    printf("Total System Performance = %ld\n", experiment_total_efficiency);
    exit(0);
}
