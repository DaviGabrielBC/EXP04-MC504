#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Declaração das variáveis globais do sistema de arquivos (importadas de sysfile.c)
extern uint64 fs_read_time, fs_write_time, fs_unlink_time;
extern int fs_read_count, fs_write_count, fs_unlink_count;

// Função para calcular a eficiência do sistema de arquivos
double calculate_fs_efficiency() {
    // Cálculo dos tempos médios
    double T_read = (double)fs_read_time
    double T_write = (double)fs_write_time 
    double T_delete = (double)fs_unlink_time

    // Soma total dos tempos
    double total_time = T_read + T_write + T_delete;

    // Verificação para evitar divisão por zero
    if (total_time == 0) {
        return 0.0; // Eficiência mínima (sem operações suficientes)
    }

    return 1000 / total_time; // Eficiência do sistema de arquivos
}

// Declaração de variáveis globais para gerenciamento de memória
extern uint64 mem_alloc_time, mem_access_time, mem_free_time;
extern uint64 mem_alloc_count, mem_access_count, mem_free_count;

// Função para calcular o Overhead de Gerenciamento de Memória
double calculate_memory_overhead() {
    // Cálculo dos tempos médios
    double T_alloc = (double)mem_alloc_time
    double T_access = (double)mem_access_time
    double T_free = (double)mem_free_time

    // Soma total dos tempos
    double total_time = T_alloc + T_access + T_free;

    // Verificação para evitar divisão por zero
    if (total_time == 0) {
        return 0.0; // Nenhuma operação suficiente para calcular
    }

    return 1000 / total_time; // Calcula o Overhead
}

// Função para rodar uma rodada de experimentos
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
    while (wait(0) != -1) {} // Aguarda todos os processos terminarem
}

int main() {
    printf("Iniciando experimentos...\n");

    for (int round = 0; round < 30; round++) {
        // Determina a quantidade de tarefas CPU-bound e IO-bound para a rodada
        int cpu_count = 6 + (round % 9);
        int io_count = 20 - cpu_count;

        printf("Rodada %d: %d CPU-bound, %d IO-bound\n", round, cpu_count, io_count);

        // Executa a rodada
        run_experiment(cpu_count, io_count);

        // Calcula a eficiência do sistema de arquivos após a rodada
        double fs_efficiency = calculate_fs_efficiency();
        printf("Eficiência do Sistema de Arquivos (Ef_s): %.6f\n", fs_efficiency);

	// Calcula o Overhead de Gerenciamento de Memória
        double memory_overhead = calculate_memory_overhead();
        printf("Overhead de Gerenciamento de Memória (Mover): %.6f\n", memory_overhead);
    }

    printf("Experimentos finalizados.\n");
    exit(0);
}
