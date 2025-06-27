#include "solucao.h"
#include "grafo.h"
#include <iostream>

// Definição da variável global
vector<Servico> servicos;

vector<Rota> construirRotas() {
    vector<Rota> rotas;
    set<int> servicosAtendidos;

    for (const Servico &s : servicos) {
        if (servicosAtendidos.count(s.id)) continue;

        Rota novaRota;
        novaRota.servicosNaRota.push_back(s);
        novaRota.demandaTotal = s.demanda;
        novaRota.custoTotal = s.custo;
        servicosAtendidos.insert(s.id);

        for (const Servico &outro : servicos) {
            if (servicosAtendidos.count(outro.id)) continue;
            if (novaRota.demandaTotal + outro.demanda <= capacidadeVeiculo) {
                novaRota.servicosNaRota.push_back(outro);
                novaRota.demandaTotal += outro.demanda;
                novaRota.custoTotal += outro.custo;
                servicosAtendidos.insert(outro.id);
            }
        }

        rotas.push_back(novaRota);
    }

    return rotas;
}

void exportarSolucao(const string &nomeArquivo, const vector<Rota> &rotas, int tempoReferencia, int tempoSolucao) {
    ofstream saida(nomeArquivo);
    if (!saida.is_open()) {
        cerr << "Erro ao abrir arquivo de saída: " << nomeArquivo << endl;
        return;
    }

    int custoTotal = 0;
    for (const Rota &r : rotas) {
        custoTotal += r.custoTotal;
    }

    saida << custoTotal << "\n";
    saida << rotas.size() << "\n";
    saida << tempoReferencia << "\n";
    saida << tempoSolucao << "\n";

    int idRota = 1;
    for (const Rota &r : rotas) {
        saida << "0 1 " << idRota << " " << r.demandaTotal << " " << r.custoTotal << " " 
              << r.servicosNaRota.size() + 2 << " (D 0,1,1) ";
        
        for (const Servico &s : r.servicosNaRota) {
            saida << "(S " << s.id << "," << s.u << "," << s.v << ") ";
        }
        
        saida << "(D 0,1,1)\n";
        idRota++;
    }
}