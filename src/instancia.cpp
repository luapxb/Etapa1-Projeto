#include "instancia.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include "solucao.h" // 'servicos' é global aqui
#include <algorithm> // Necessário para std::max

using namespace std;
namespace fs = filesystem;

// 'servicos' é uma variável global, precisa ser 'extern' em solucao.h
extern vector<Servico> servicos;

// Processa o arquivo de instância para popular o grafo e os serviços
void processarInstancia(const string& caminhoArquivo, Grafo& g) {
   cout << "Abrindo arquivo: " << caminhoArquivo << endl;
    ifstream arquivo(caminhoArquivo);
    if (!arquivo) {
        cerr << "Erro ao abrir o arquivo!\n";
        exit(1);
    }

    limparDados(g); // Limpa os dados do grafo antes de processar uma nova instância
    servicos.clear(); // Limpa o vetor global de serviços também
    string linha;
    string secao;
    bool cabecalhoProcessado = false;

    // Garante que o depósito está no grafo
    g.adj[g.deposito] = vector<Aresta>(); // Inicializa o depósito na lista de adjacência
    g.maxIdNo = max(g.maxIdNo, g.deposito); // Atualiza o ID máximo do nó com o depósito

    while (getline(arquivo, linha)) {
        // Limpar espaços em branco do início e fim da linha
        linha.erase(0, linha.find_first_not_of(" \t"));
        linha.erase(linha.find_last_not_of(" \t") + 1);

        if (linha.empty()) continue; // Ignora linhas vazias

        // Processa o cabeçalho do arquivo
        if (!cabecalhoProcessado) {
            if (linha.find("Depot Node:") != string::npos) {
                g.deposito = stoi(linha.substr(linha.find(":") + 1));
                g.maxIdNo = max(g.maxIdNo, g.deposito); // Atualiza o ID máximo do nó
            }
            if (linha.find("Capacity:") != string::npos) {
                g.capacidadeVeiculo = stoi(linha.substr(linha.find(":") + 1));
            }
            if (linha.find("Name:") != string::npos) {
                cabecalhoProcessado = true;
            }
            continue;
        }

        // Identifica as seções do arquivo
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

        // Processa as linhas de dados com base na seção atual
        if (secao == "Required N") {
            string idNoStr;
            int demanda, custo;
            stringstream ss(linha);
            ss >> idNoStr >> demanda >> custo;
            int id = stoi(idNoStr.substr(1)); // Remove 'N' do ID do nó

            Servico serv;
            serv.id = (int)servicos.size() + 1;
            serv.tipo = "N";
            serv.u = id;
            serv.v = id;
            serv.custo = custo;
            serv.demanda = demanda;
            servicos.push_back(serv);

            g.adj[id]; // Garante que o nó existe na lista de adjacência
            g.maxIdNo = max(g.maxIdNo, id); // Atualiza o ID máximo do nó
        }

        else if (secao == "Edges") {
            string idArestaStr;
            int u, v, custo, demanda, custoServico;
            stringstream ss(linha);
            ss >> idArestaStr >> u >> v >> custo >> demanda >> custoServico;

            // Adiciona ao grafo (bidirecional)
            g.adj[u].push_back({v, custo});
            g.adj[v].push_back({u, custo});
            g.maxIdNo = max(g.maxIdNo, max(u, v)); // Atualiza o ID máximo do nó

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
            string idArcoStr;
            int u, v, custo, demanda, custoServico;
            stringstream ss(linha);
            ss >> idArcoStr >> u >> v >> custo >> demanda >> custoServico;

            // Adiciona ao grafo (direcional)
            g.adj[u].push_back({v, custo});
            g.maxIdNo = max(g.maxIdNo, max(u, v)); // Atualiza o ID máximo do nó

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
            string idArestaStr;
            int u, v, custo;
            stringstream ss(linha);
            ss >> idArestaStr >> u >> v >> custo;

            // Apenas adiciona ao grafo (bidirecional)
            g.adj[u].push_back({v, custo});
            g.adj[v].push_back({u, custo});
            g.maxIdNo = max(g.maxIdNo, max(u, v)); // Atualiza o ID máximo do nó
        }

        else if (secao == "ARC") {
            string idArcoStr;
            int u, v, custo;
            stringstream ss(linha);
            ss >> idArcoStr >> u >> v >> custo;

            // Apenas adiciona ao grafo (direcional)
            g.adj[u].push_back({v, custo});
            g.maxIdNo = max(g.maxIdNo, max(u, v)); // Atualiza o ID máximo do nó
        }
    }

    // Verifica se o nó do depósito foi realmente incluído
    if (g.adj.find(g.deposito) == g.adj.end()) {
        cerr << "AVISO: Depósito " << g.deposito << " não foi explicitamente incluído no grafo, forçando criação!" << endl;
        g.adj[g.deposito] = vector<Aresta>(); // Força criação se ainda não existir
    }
    g.maxIdNo = max(g.maxIdNo, g.deposito); // Atualização final do ID máximo com o depósito

    arquivo.close();
}