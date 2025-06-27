#include "solver.h"
#include <set>

using namespace std;

vector<Rota> construirRotas(const Grafo& grafo, const vector<Servico>& servicos) {
    vector<Rota> rotas;
    set<int> servicosAtendidos;

    for (const auto& serv : servicos) {
        if (servicosAtendidos.count(serv.id)) continue;

        Rota novaRota;
        novaRota.servicosNaRota.push_back(serv);
        novaRota.demandaTotal = serv.demanda;
        novaRota.custoTotal = serv.custo;
        servicosAtendidos.insert(serv.id);

        for (const auto& outro : servicos) {
            if (servicosAtendidos.count(outro.id)) continue;
            if (novaRota.demandaTotal + outro.demanda <= grafo.capacidadeVeiculo) {
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