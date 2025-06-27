#include "solver.h"
#include <set> // Para std::set

using namespace std;

// Constrói um conjunto inicial de rotas (heurística gulosa simples)
// Agora recebe o objeto Grafo e o vetor de Servicos como argumentos
vector<Rota> construirRotas(const Grafo& g, const vector<Servico>& servicos) {
    vector<Rota> rotas;
    set<int> servicosAtendidos; // Conjunto para rastrear serviços já atendidos

    // Itera sobre todos os serviços disponíveis
    for (const auto& serv : servicos) {
        if (servicosAtendidos.count(serv.id)) continue; // Se o serviço já foi atendido, pula

        Rota novaRota; // Cria uma nova rota
        novaRota.servicosNaRota.push_back(serv); // Adiciona o serviço atual à nova rota
        novaRota.demandaTotal = serv.demanda;    // Atualiza a demanda total da rota
        novaRota.custoTotal = serv.custo;        // Atualiza o custo total da rota
        servicosAtendidos.insert(serv.id);       // Marca o serviço como atendido

        // Tenta adicionar outros serviços à rota atual enquanto a capacidade permitir
        for (const auto& outro : servicos) {
            if (servicosAtendidos.count(outro.id)) continue; // Se o outro serviço já foi atendido, pula
            
            // Verifica se adicionar o próximo serviço excede a capacidade do veículo
            if (novaRota.demandaTotal + outro.demanda <= g.capacidadeVeiculo) { // Usa g.capacidadeVeiculo do objeto Grafo
                novaRota.servicosNaRota.push_back(outro);  // Adiciona o outro serviço
                novaRota.demandaTotal += outro.demanda;     // Atualiza demanda
                novaRota.custoTotal += outro.custo;         // Atualiza custo
                servicosAtendidos.insert(outro.id);        // Marca o outro serviço como atendido
            }
        }

        rotas.push_back(novaRota); // Adiciona a rota completa ao conjunto de rotas
    }

    return rotas; // Retorna todas as rotas construídas
}