#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define INF 99999

int iterations = 0;

int rand() {
    uint64 temp = uptime();
    uint rand_state = temp % 32768;
    rand_state = (rand_state + iterations) * 21013 + 12345;
    iterations++;
    return (rand_state) % 32768; // Retorna um número aleatório entre 0 e 32767
}

int rand_range(int min_range, int max_range){
    int q = max_range - min_range + 1;
    int rand_number = rand() % q;
    return (rand_number + min_range); // Retorna um número aleatório entre min_range e max_range inclusive
}

void bellman_ford(int **graph, int n, int start) {
    int dist[220];

    for (int i = 0; i < n; i++)
        dist[i] = INF;
    dist[start] = 0;

    for(int i = 0; i < n-1; i++){
        for(int j1 = 0; j1 < n; j1++){
            for(int j2 = 0; j2 < n; j2++){
                if(graph[j1][j2] > 0 && dist[j1] < INF && dist[j2] > dist[j1] + graph[j1][j2])
                    dist[j2] = dist[j1] + graph[j1][j2];
            }
        }
    }
}

int main() {
    int n = rand_range(100, 200); // Número de vértices
    int m = rand_range(50, 400); // Número de arestas

    // printf("n: %d\n m: %d\n", n, m); // Debug (bulwark)

    int **graph = (int **)malloc(n * sizeof(int *)); // Gerar grafo aqui
    for(int i = 0; i < n; i++) {
        graph[i] = (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) {
            graph[i][j] = 0;
        }
    }

    for(int edge_count = 0; edge_count < m; edge_count++) { // Gerando grafo aleatório
        int u = rand_range(0, n - 1);
        int v = rand_range(0, n - 1);

        while(u == v) v = rand_range(0, n - 1);

        if(graph[u][v] == 0) {
            graph[u][v] = rand_range(1, 10); // Peso aleatório
            //printf("Edge %d: u = %d, v = %d, peso = %d\n", edge_count, u, v, graph[u][v]); // Debug (bulwark)
        }
    }

    bellman_ford(graph, n, 0);

    for(int i = 0; i < n; i++) {
        free(graph[i]);
    }
    free(graph);

    exit(0);
}
