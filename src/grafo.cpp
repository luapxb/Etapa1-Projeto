#include "grafo.h"
#include <iostream>

// Definição das variáveis globais
unordered_map<int, vector<Aresta>> grafo;
vector<vector<int>> distanciasMinimas;
int deposito = 0;
int capacidadeVeiculo = 0;

void limparDados() {
    grafo.clear();
    distanciasMinimas.clear();
}

void dijkstra(int origem, vector<int> &dist) {
    if (grafo.find(origem) == grafo.end()) {
        cerr << "Erro: Nó " << origem << " não existe no grafo!" << endl;
        return;
    }
    int n = grafo.size();
    dist.assign(n, 1e9);
    dist[origem] = 0;
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
    pq.push({0, origem});

    while(!pq.empty()) {
        int d = pq.top().first;
        int u = pq.top().second;
        pq.pop();
        if (d > dist[u]) continue;

        for (auto &aresta : grafo[u]) {
            int v = aresta.destino;
            int custo = aresta.custo;
            if (dist[u] + custo < dist[v]) {
                dist[v] = dist[u] + custo;
                pq.push({dist[v], v});
            }
        }
    }
}

void calcularCaminhosMinimos() {
    int n = grafo.size();
    distanciasMinimas.resize(n);
    for (int i = 0; i < n; i++) {
        dijkstra(i, distanciasMinimas[i]);
    }
}