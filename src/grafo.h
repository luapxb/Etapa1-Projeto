#pragma once
#include <vector>
#include <unordered_map>
#include <queue>

using namespace std;

// Estrutura para representar uma aresta no grafo
struct Aresta {
    int destino;
    int custo;
};

// Estrutura para encapsular os dados do grafo
struct Grafo {
    unordered_map<int, vector<Aresta>> adj; // Lista de adjacência do grafo
    vector<vector<int>> distanciasMinimas;  // Matriz de distâncias mínimas
    int deposito;                           // Nó do depósito
    int capacidadeVeiculo;                  // Capacidade máxima do veículo
    int maxIdNo;                           // Para rastrear o ID máximo do nó encontrado
};

// Declarações de funções
void limparDados(Grafo& g);
void dijkstra(const Grafo& g, int origem, vector<int> &dist);
void calcularCaminhosMinimos(Grafo& g);