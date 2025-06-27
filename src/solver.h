#pragma once
#include "grafo.h"
#include "solucao.h"

// Declaração da função para construir rotas
// Recebe o objeto Grafo e o vetor de Servicos
vector<Rota> construirRotas(const Grafo& g, const vector<Servico>& servicos);