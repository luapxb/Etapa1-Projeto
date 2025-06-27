#pragma once
#include "grafo.h"
#include "solucao.h"
#include <vector> // Necessário para vector<Servico>
#include <limits> // Para numeric_limits
#include <algorithm> // Para std::sort e std::min (se usado)

// Declaração da função para calcular o custo total de uma rota
int calcularCustoRota(const Rota& rota, const Grafo& g);

// Declaração da função para construir rotas iniciais
vector<Rota> construirRotas(const Grafo& g, const vector<Servico>& servicos);

// Declaração da função para melhorar rotas usando 2-Opt
void melhorarRotas2Opt(vector<Rota>& rotas, const Grafo& g);
