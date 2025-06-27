#pragma once
#include <vector>
#include <unordered_map>
#include <queue>

using namespace std;

struct Aresta {
    int destino;
    int custo;
};

// Declaração das variáveis globais
extern unordered_map<int, vector<Aresta>> grafo;
extern vector<vector<int>> distanciasMinimas;
extern int deposito;
extern int capacidadeVeiculo;

// Declarações de funções
void limparDados();
void dijkstra(int origem, vector<int> &dist);
void calcularCaminhosMinimos();