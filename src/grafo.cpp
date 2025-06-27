#include "grafo.h"
#include <iostream>
#include <limits> // Para numeric_limits

// Limpa os dados do grafo
void limparDados(Grafo& g) {
    g.adj.clear();
    g.distanciasMinimas.clear();
    g.deposito = 0;
    g.capacidadeVeiculo = 0;
    g.maxIdNo = 0; // Redefine o ID máximo do nó
}

// Implementação do algoritmo de Dijkstra
void dijkstra(const Grafo& g, int origem, vector<int> &dist) {
    if (g.adj.find(origem) == g.adj.end() && origem != g.deposito) { // Verifica se o nó de origem existe ou é o depósito
        cerr << "Erro: Nó " << origem << " não existe no grafo para Dijkstra!" << endl;
        return;
    }
    
    // Determina o tamanho do vetor de distância com base no maxIdNo
    int n = g.maxIdNo + 1; 
    
    // Inicializa as distâncias com um valor "infinito"
    dist.assign(n, numeric_limits<int>::max());
    dist[origem] = 0; // Distância da origem para ela mesma é 0

    // Fila de prioridade para Dijkstra: {custo_atual, nó}
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
    pq.push({0, origem});

    while(!pq.empty()) {
        int d = pq.top().first;  // Distância atual até 'u'
        int u = pq.top().second; // Nó atual
        pq.pop();

        if (d > dist[u]) continue; // Se já encontrou um caminho mais curto, ignora

        // Itera sobre as arestas adjacentes a 'u'
        if (g.adj.count(u)) { // Garante que 'u' existe na lista de adjacência
            for (auto &aresta : g.adj.at(u)) {
                int v = aresta.destino; // Nó de destino
                int custo = aresta.custo; // Custo da aresta
                
                // Se encontrar um caminho mais curto para 'v'
                if (dist[u] != numeric_limits<int>::max() && dist[u] + custo < dist[v]) {
                    dist[v] = dist[u] + custo; // Atualiza a distância
                    pq.push({dist[v], v});     // Adiciona 'v' à fila de prioridade
                }
            }
        }
    }
}

// Calcula todas as distâncias mínimas entre todos os pares de nós
void calcularCaminhosMinimos(Grafo& g) {
    // O tamanho da matriz de distâncias deve acomodar todos os IDs de nó, até maxIdNo.
    int n = g.maxIdNo + 1;
    g.distanciasMinimas.resize(n);
    for (int i = 0; i < n; i++) {
        // Executa Dijkstra para cada nó como origem
        dijkstra(g, i, g.distanciasMinimas[i]);
    }
}