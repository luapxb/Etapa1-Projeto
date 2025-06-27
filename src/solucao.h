#pragma once
#include <vector>
#include <string>
#include <set>
#include <fstream>

using namespace std;

struct Servico {
    int id;
    string tipo;
    int u, v;
    int custo;
    int demanda;
};

struct Rota {
    vector<Servico> servicosNaRota;
    int demandaTotal;
    int custoTotal;
};

// Declaração da variável global
extern vector<Servico> servicos;

// Declarações de funções
vector<Rota> construirRotas();
void exportarSolucao(const string &nomeArquivo, const vector<Rota> &rotas, int tempoReferencia, int tempoSolucao);