#pragma once
#include "solucao.h"
#include "grafo.h" // Inclui grafo.h para a struct Grafo

// Declaração da função para exportar a solução
void exportarSolucao(const string& nomeArquivo, const vector<Rota>& rotas, int tempoReferencia, int tempoSolucao, const Grafo& g);
