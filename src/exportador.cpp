#include "exportador.h"
#include <fstream>
#include <iostream>

using namespace std;

void exportarSolucao(const string& nomeArquivo, const vector<Rota>& rotas, int tempoSolucao) {
    ofstream saida(nomeArquivo);
    if (!saida.is_open()) {
        cerr << "Erro ao abrir arquivo de saída: " << nomeArquivo << endl;
        return;
    }

    int custoTotal = 0;
    for (const auto& r : rotas) {
        custoTotal += r.custoTotal;
    }

    saida << custoTotal << "\n";
    saida << rotas.size() << "\n";
    saida << "0\n"; // Tempo referência
    saida << tempoSolucao << "\n";

    int idRota = 1;
    for (const auto& r : rotas) {
        saida << "0 1 " << idRota << " " << r.demandaTotal << " " 
              << r.custoTotal << " " << r.servicosNaRota.size() + 2 << " ";
        
        saida << "(D " << rotas[0].servicosNaRota[0].u << ",1,1) ";
        for (const auto& s : r.servicosNaRota) {
            saida << "(S " << s.id << "," << s.u << "," << s.v << ") ";
        }
        saida << "(D " << rotas[0].servicosNaRota[0].u << ",1,1)\n";
        idRota++;
    }
}