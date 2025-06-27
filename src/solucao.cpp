#include "solucao.h"
#include "grafo.h" 
#include <iostream>
#include <set> // Para std::set

// Definição da variável global para armazenar todos os serviços
vector<Servico> servicos;

// Constrói um conjunto inicial de rotas (heurística gulosa simples)
vector<Rota> construirRotas(const Grafo& g) { // Agora recebe o objeto Grafo
    vector<Rota> rotas;
    set<int> servicosAtendidos; // Conjunto para rastrear serviços já atendidos

    // Itera sobre todos os serviços disponíveis
    for (const Servico &s : servicos) {
        if (servicosAtendidos.count(s.id)) continue; // Se o serviço já foi atendido, pula

        Rota novaRota; // Cria uma nova rota
        novaRota.servicosNaRota.push_back(s); // Adiciona o serviço atual à nova rota
        novaRota.demandaTotal = s.demanda;    // Atualiza a demanda total da rota
        novaRota.custoTotal = s.custo;        // Atualiza o custo total da rota
        servicosAtendidos.insert(s.id);       // Marca o serviço como atendido

        // Tenta adicionar outros serviços à rota atual enquanto a capacidade permitir
        for (const Servico &outro : servicos) {
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

// Exporta a solução para um arquivo no formato especificado
void exportarSolucao(const string &nomeArquivo, const vector<Rota> &rotas, int tempoReferencia, int tempoSolucao, const Grafo& g) {
    ofstream saida(nomeArquivo); // Abre o arquivo de saída
    if (!saida.is_open()) {
        cerr << "Erro ao abrir arquivo de saída: " << nomeArquivo << endl;
        return;
    }

    int custoTotalGeral = 0;
    for (const Rota &r : rotas) {
        custoTotalGeral += r.custoTotal; // Soma o custo de todas as rotas
    }

    saida << custoTotalGeral << "\n";      // Custo total de todas as rotas
    saida << rotas.size() << "\n";          // Número de rotas
    saida << tempoReferencia << "\n";      // Tempo de referência (geralmente 0)
    saida << tempoSolucao << "\n";         // Tempo de execução do algoritmo

    int idRota = 1;
    for (const Rota &r : rotas) {
        // Formato da linha da rota:
        // 0 1 <ID Rota> <Demanda Total> <Custo Total> <Número de Elementos na Rota + 2> (D <Depósito>,1,1) (S <ID Serviço>,<U>,<V>) ... (D <Depósito>,1,1)
        saida << "0 1 " << idRota << " " << r.demandaTotal << " " << r.custoTotal << " " 
              << r.servicosNaRota.size() + 2 << " (D " << g.deposito << ",1,1) "; // Início da rota no depósito

        for (const Servico &s : r.servicosNaRota) {
            saida << "(S " << s.id << "," << s.u << "," << s.v << ") "; // Serviços na rota
        }
        
        saida << "(D " << g.deposito << ",1,1)\n"; // Fim da rota no depósito
        idRota++;
    }
}