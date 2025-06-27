#pragma once
#include <vector>
#include <string>
#include <set>
#include <fstream>
#include "grafo.h" // Inclui grafo.h para a struct Grafo

using namespace std;

// Estrutura para representar um serviço (nó, aresta ou arco requerido)
struct Servico {
    int id;
    string tipo; // Tipo de serviço: "N" (nó), "E" (aresta), "A" (arco)
    int u, v;    // Nós envolvidos no serviço (u=v para nós)
    int custo;   // Custo de atendimento do serviço
    int demanda; // Demanda do serviço
};

// Estrutura para representar uma rota de veículo
struct Rota {
    vector<Servico> servicosNaRota; // Serviços atendidos nesta rota
    int demandaTotal;               // Demanda total acumulada na rota
    int custoTotal;                 // Custo total da rota
};

// Declaração da variável global para armazenar todos os serviços
extern vector<Servico> servicos;

// Declarações de funções
vector<Rota> construirRotas(const Grafo& g); // Modificada para receber o objeto Grafo
void exportarSolucao(const string &nomeArquivo, const vector<Rota> &rotas, int tempoReferencia, int tempoSolucao, const Grafo& g); // Modificada para receber o objeto Grafo