#include "kernel/types.h"
#include "user/user.h"

#define INF 99999

void dijkstra(int graph[200][200], int n, int start) {
    int dist[200], visited[200];
    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        visited[i] = 0;
    }
    dist[start] = 0;

    for (int count = 0; count < n - 1; count++) {
        int min = INF, u = -1;
        for (int i = 0; i < n; i++) {
            if (!visited[i] && dist[i] <= min) {
                min = dist[i];
                u = i;
            }
        }
        visited[u] = 1;
        for (int v = 0; v < n; v++) {
            if (!visited[v] && graph[u][v] && dist[u] != INF && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
            }
        }
    }
}

int main() {
    int graph[200][200]; // Gerar grafo aqui
    int n = 150; // Número de vértices
    dijkstra(graph, n, 0);
    exit(0);
}
