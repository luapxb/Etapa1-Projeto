#include "exportador.h"
#include <fstream>
#include <iostream>
#include "grafo.h" // Inclui grafo.h para a struct Grafo

using namespace std;

// Exporta a solução para um arquivo no formato especificado
void exportarSolucao(const string& nomeArquivo, const vector<Rota>& rotas, int tempoSolucao, const Grafo& g) {
    ofstream saida(nomeArquivo); // Abre o arquivo de saída
    if (!saida.is_open()) {
        cerr << "Erro ao abrir arquivo de saída: " << nomeArquivo << endl;
        return;
    }

    int custoTotalGeral = 0;
    for (const auto& r : rotas) {
        custoTotalGeral += r.custoTotal; // Soma o custo de todas as rotas
    }

    saida << custoTotalGeral << "\n";      // Custo total de todas as rotas
    saida << rotas.size() << "\n";          // Número de rotas
    saida << "0\n";                         // Tempo de referência (valor fixo 0)
    saida << tempoSolucao << "\n";         // Tempo de execução do algoritmo

    int idRota = 1;
    for (const auto& r : rotas) {
        // Formato da linha da rota:
        // 0 1 <ID Rota> <Demanda Total> <Custo Total> <Número de Elementos na Rota + 2> (D <Depósito>,1,1) (S <ID Serviço>,<U>,<V>) ... (D <Depósito>,1,1)
        saida << "0 1 " << idRota << " " << r.demandaTotal << " "
              << r.custoTotal << " " << r.servicosNaRota.size() + 2 << " ";
        
        saida << "(D " << g.deposito << ",1,1) "; // Início da rota no depósito (usando o depósito correto)
        for (const auto& s : r.servicosNaRota) {
            saida << "(S " << s.id << "," << s.u << "," << s.v << ") "; // Serviços na rota
        }
        saida << "(D " << g.deposito << ",1,1)\n"; // Fim da rota no depósito (usando o depósito correto)
        idRota++;
    }
}