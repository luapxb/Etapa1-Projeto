#include "instancia.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include "grafo.h"
#include "solucao.h"

using namespace std;
namespace fs = filesystem;

void processarInstancia(const string& caminhoArquivo) {
   cout << "Abrindo arquivo: " << caminhoArquivo << endl;
    ifstream arquivo(caminhoArquivo);
    if (!arquivo) {
        cerr << "Erro ao abrir o arquivo!\n";
        exit(1);
    }

    limparDados(); 
    string linha;
    string secao;
    bool headerProcessed = false;

    // Garante que o deposito esta no grafo
    grafo[deposito] = vector<Aresta>(); // Inicializa o depósito no grafo

    while (getline(arquivo, linha)) {
        // Limpar espaços
        linha.erase(0, linha.find_first_not_of(" \t"));
        linha.erase(linha.find_last_not_of(" \t") + 1);

        if (linha.empty()) continue;

        // Processar cabeçalho
        if (!headerProcessed) {
            if (linha.find("Depot Node:") != string::npos) {
                deposito = stoi(linha.substr(linha.find(":") + 1));
            }
            if (linha.find("Capacity:") != string::npos) {
                capacidadeVeiculo = stoi(linha.substr(linha.find(":") + 1));
            }
            if (linha.find("Name:") != string::npos) {
                headerProcessed = true;
            }
            continue;
        }

        // Identificar seções
        if (linha[0] == '#') {
            if (linha.find("#Nodes") != string::npos) secao = "Nodes";
            else if (linha.find("#Required N") != string::npos) secao = "Required N";
            else if (linha.find("#Required E") != string::npos) secao = "Edges";
            else if (linha.find("#Required A") != string::npos) secao = "Arcs";
            else if (linha.find("#Edges") != string::npos) secao = "EDGE";
            else if (linha.find("#Arcs") != string::npos) secao = "ARC";
            continue;
        }
        else if (linha.find("ReN.") != string::npos) { secao = "Required N"; continue; }
        else if (linha.find("ReE.") != string::npos) { secao = "Edges"; continue; }
        else if (linha.find("ReA.") != string::npos) { secao = "Arcs"; continue; }
        else if (linha.find("EDGE") != string::npos) { secao = "EDGE"; continue; }
        else if (linha.find("ARC") != string::npos) { secao = "ARC"; continue; }

        // Processar linhas
        if (secao == "Required N") {
            string nodeId;
            int demanda, custo;
            stringstream ss(linha);
            ss >> nodeId >> demanda >> custo;
            int id = stoi(nodeId.substr(1)); // Remove 'N'

            Servico serv;
            serv.id = (int)servicos.size() + 1;
            serv.tipo = "N";
            serv.u = id;
            serv.v = id;
            serv.custo = custo;
            serv.demanda = demanda;
            servicos.push_back(serv);
        }

        else if (secao == "Edges") {
            string edgeId;
            int u, v, custo, demanda, custoServico;
            stringstream ss(linha);
            ss >> edgeId >> u >> v >> custo >> demanda >> custoServico;

            // Adicionar ao grafo (bidirecional)
            grafo[u].push_back({v, custo});
            grafo[v].push_back({u, custo});

            // Serviço (aresta requerida)
            Servico serv;
            serv.id = (int)servicos.size() + 1;
            serv.tipo = "E";
            serv.u = u;
            serv.v = v;
            serv.custo = custoServico;
            serv.demanda = demanda;
            servicos.push_back(serv);
        }

        else if (secao == "Arcs") {
            string arcId;
            int u, v, custo, demanda, custoServico;
            stringstream ss(linha);
            ss >> arcId >> u >> v >> custo >> demanda >> custoServico;

            // Adicionar ao grafo (direcional)
            grafo[u].push_back({v, custo});

            // Serviço (arco requerido)
            Servico serv;
            serv.id = (int)servicos.size() + 1;
            serv.tipo = "A";
            serv.u = u;
            serv.v = v;
            serv.custo = custoServico;
            serv.demanda = demanda;
            servicos.push_back(serv);
        }

        else if (secao == "EDGE") {
            string edgeId;
            int u, v, custo;
            stringstream ss(linha);
            ss >> edgeId >> u >> v >> custo;

            // Apenas adiciona ao grafo (bidirecional)
            grafo[u].push_back({v, custo});
            grafo[v].push_back({u, custo});
        }

        else if (secao == "ARC") {
            string arcId;
            int u, v, custo;
            stringstream ss(linha);
            ss >> arcId >> u >> v >> custo;

            // Apenas adiciona ao grafo (direcional)
            grafo[u].push_back({v, custo});
        }
    }

    //cout << "Nós no grafo: " << grafo.size() << endl;
    if (grafo.find(deposito) == grafo.end()) {
        cerr << "AVISO: Depósito " << deposito << " não foi incluído no grafo!" << endl;
        grafo[deposito] = vector<Aresta>(); // Força criação se ainda não existir
    }

    // Garantir que todos os nós aparecem no grafo
    for (const auto& [node, _] : grafo) {
        if (grafo.find(node) == grafo.end()) {
            grafo[node] = vector<Aresta>();
        }
    }

    arquivo.close();
}