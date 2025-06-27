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
    // Determine o tamanho do vetor de distância com base no maxIdNo.
    // Garante que 'n' seja pelo menos 1 para alocar 'dist' corretamente,
    // mesmo que maxIdNo seja -1 ou 0 e não haja nós explícitos além do depósito.
    int n = g.maxIdNo + 1;
    if (n <= 0) n = 1; // Garante que n é pelo menos 1 para dist.assign

    // Esta verificação é importante. Se 'origem' for um ID de nó inválido
    // para a matriz que será criada (tamanho 'n'), preenche com max() e retorna.
    if (origem < 0 || origem >= n || (g.adj.find(origem) == g.adj.end() && origem != g.deposito)) {
        dist.assign(n, numeric_limits<int>::max());
        return;
    }
    
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
                // Verifica também se 'v' está dentro dos limites da matriz 'dist'
                if (dist[u] != numeric_limits<int>::max() && static_cast<size_t>(v) < dist.size() && dist[u] + custo < dist[v]) { 
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
    // Ajuste: Se o grafo está vazio (apenas o depósito ou nenhum nó), garanta que 'n' seja pelo menos 1.
    // Isso evita redimensionar para 0 se maxIdNo for -1 e previne falhas de alocação/acesso.
    if (n <= 0) n = 1; 

    g.distanciasMinimas.resize(n);
    for (int i = 0; i < n; i++) {
        // Executa Dijkstra para cada nó como origem
        dijkstra(g, i, g.distanciasMinimas[i]);
    }
}
