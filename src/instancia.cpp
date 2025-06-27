#include "instancia.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include "solucao.h" // 'servicos' é global aqui
#include <algorithm> // Necessário para std::max
#include <cstring>

using namespace std;
namespace fs = filesystem;

// 'servicos' é uma variável global, precisa ser 'extern' em solucao.h
extern vector<Servico> servicos;

// Função auxiliar para remover espaços em branco de uma string
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// Processa o arquivo de instância para popular o grafo e os serviços
void processarInstancia(const string& caminhoArquivo, Grafo& g) {
   cout << "Abrindo arquivo: " << caminhoArquivo << endl;
    ifstream arquivo(caminhoArquivo);
    if (!arquivo) {
        cerr << "Erro ao abrir o arquivo! Certifique-se de que o arquivo '" << caminhoArquivo << "' existe e pode ser lido.\n";
        exit(1);
    }

    limparDados(g); // Limpa os dados do grafo antes de processar uma nova instância
    servicos.clear(); // Limpa o vetor global de serviços também
    string linha;
    string secaoAtual = ""; // Variável para a seção que estamos processando
    bool dataSectionStarted = false; // Flag para indicar se já entramos nas seções de dados

    while (getline(arquivo, linha)) {
        string linhaTrimmed = trim(linha); // Usa a função trim para limpar espaços

        if (linhaTrimmed.empty()) continue;

        // Tenta parsear Depot Node e Capacity em qualquer linha antes das seções de dados
        // Usamos find de "Depot Node" e "Capacity" seguidos de um ':',
        // e depois o valor. Isso é mais robusto.
        size_t posDepot = linhaTrimmed.find("Depot Node:");
        if (posDepot != string::npos) {
            string valorStr = linhaTrimmed.substr(posDepot + strlen("Depot Node:"));
            g.deposito = stoi(trim(valorStr));
            g.maxIdNo = max(g.maxIdNo, g.deposito);
            continue; // Vai para a próxima linha
        }
        
        size_t posCapacity = linhaTrimmed.find("Capacity:");
        if (posCapacity != string::npos) {
            string valorStr = linhaTrimmed.substr(posCapacity + strlen("Capacity:"));
            g.capacidadeVeiculo = stoi(trim(valorStr));
            continue; // Vai para a próxima linha
        }

        // Ignora outras linhas informativas do cabeçalho que não precisamos parsear
        if (linhaTrimmed.find("Name:") != string::npos ||
            linhaTrimmed.find("Optimal value:") != string::npos ||
            linhaTrimmed.find("#Vehicles:") != string::npos ||
            linhaTrimmed.find("#Nodes:") != string::npos || // Estes #Nodes/Edges/Arcs são info, não a seção de dados em si
            linhaTrimmed.find("#Edges:") != string::npos ||
            linhaTrimmed.find("#Arcs:") != string::npos ||
            linhaTrimmed.find("#Required N:") != string::npos ||
            linhaTrimmed.find("#Required E:") != string::npos ||
            linhaTrimmed.find("#Required A:") != string::npos ||
            linhaTrimmed.find("the data is based on the CARP instance") != string::npos) 
        {
            continue; 
        }

        // Verifica se é o início de uma nova seção de dados.
        // A ordem dos 'if' é importante para que as palavras-chave do cabeçalho
        // sejam sempre verificadas antes das dos marcadores de seção.
        string proximaSecao = "";
        if (linhaTrimmed[0] == '#') { // Linhas que começam com # (ex: #Nodes, #Required N)
            if (linhaTrimmed.find("#Required N") != string::npos) proximaSecao = "Required N";
            else if (linhaTrimmed.find("#Required E") != string::npos) proximaSecao = "Edges";
            else if (linhaTrimmed.find("#Required A") != string::npos) proximaSecao = "Arcs";
            else if (linhaTrimmed.find("#Nodes") != string::npos || // 'Nodes' é uma seção de dados?
                     linhaTrimmed.find("#Edges") != string::npos ||
                     linhaTrimmed.find("#Arcs") != string::npos)
            {
                 // Estas seções podem ter dados simples de arestas/arcos não requeridos.
                 // Precisamos diferenciar entre a linha de contagem e a seção real.
                 // Pelo seu exemplo, "EDGE" e "ARC" sem '#' indicam a seção real.
                 // Os '#Nodes', '#Edges', '#Arcs' no cabeçalho são apenas contadores.
                 // A seção real é marcada por "EDGE" ou "ARC" (sem o # inicial).
                 // Para este tipo de arquivo, as seções de dados começam com ReN., ReE., ReA. ou EDGE/ARC.
                 // Se o # indicar uma seção de dados, ele não deve ser ignorado.
                 // O formato dos seus arquivos de instância é um pouco ambíguo sobre isso.
                 // Vamos manter a regra que as linhas ReN., ReE., ReA. e EDGE/ARC (sem #)
                 // marcam o início das seções de dados, como nos exemplos fornecidos.
                 continue; // Ignora contadores no cabeçalho
            }
        }
        // Marcadores de seção que não começam com '#'
        else if (linhaTrimmed.find("ReN.") != string::npos) { proximaSecao = "Required N"; }
        else if (linhaTrimmed.find("ReE.") != string::npos) { proximaSecao = "Edges"; }
        else if (linhaTrimmed.find("ReA.") != string::npos) { proximaSecao = "Arcs"; }
        else if (linhaTrimmed.find("EDGE") == 0 && linhaTrimmed.length() <= 6) { // "EDGE" como marcador de seção
             proximaSecao = "EDGE";
        }
        else if (linhaTrimmed.find("ARC") == 0 && linhaTrimmed.length() <= 5) { // "ARC" como marcador de seção
             proximaSecao = "ARC";
        }

        if (!proximaSecao.empty()) {
            secaoAtual = proximaSecao; // Atualiza a seção atual
            dataSectionStarted = true; // A partir daqui, estamos nas seções de dados
            continue; // Já processamos a linha como um marcador de seção, não como dado
        }

        // Se chegamos aqui e 'dataSectionStarted' é verdadeiro, a linha é um dado da seção atual
        if (dataSectionStarted) {
            // Processar linhas de dados
            if (secaoAtual == "Required N") {
                string idNoStr;
                int demanda, custo;
                stringstream ss(linhaTrimmed); // Use linhaTrimmed
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
            } else if (secaoAtual == "Edges") { // Required Edges
                string idArestaStr;
                int u, v, custo, demanda, custoServico;
                stringstream ss(linhaTrimmed); // Use linhaTrimmed
                ss >> idArestaStr >> u >> v >> custo >> demanda >> custoServico;

                // Adiciona ao grafo (bidirecional)
                g.adj[u].push_back({v, custo});
                g.adj[v].push_back({u, custo});
                g.maxIdNo = max(g.maxIdNo, max(u, v));

                // Serviço (aresta requerida)
                Servico serv;
                serv.id = (int)servicos.size() + 1;
                serv.tipo = "E";
                serv.u = u;
                serv.v = v;
                serv.custo = custoServico;
                serv.demanda = demanda;
                servicos.push_back(serv);
            } else if (secaoAtual == "Arcs") { // Required Arcs
                string idArcoStr;
                int u, v, custo, demanda, custoServico;
                stringstream ss(linhaTrimmed); // Use linhaTrimmed
                ss >> idArcoStr >> u >> v >> custo >> demanda >> custoServico;

                // Adiciona ao grafo (direcional)
                g.adj[u].push_back({v, custo});
                g.maxIdNo = max(g.maxIdNo, max(u, v));

                // Serviço (arco requerido)
                Servico serv;
                serv.id = (int)servicos.size() + 1;
                serv.tipo = "A";
                serv.u = u;
                serv.v = v;
                serv.custo = custoServico;
                serv.demanda = demanda;
                servicos.push_back(serv);
            } else if (secaoAtual == "EDGE") { // Other Edges (not required)
                string edgeId;
                int u, v, custo;
                stringstream ss(linhaTrimmed); // Use linhaTrimmed
                ss >> edgeId >> u >> v >> custo;

                // Apenas adiciona ao grafo (bidirecional)
                g.adj[u].push_back({v, custo});
                g.adj[v].push_back({u, custo});
                g.maxIdNo = max(g.maxIdNo, max(u, v));
            } else if (secaoAtual == "ARC") { // Other Arcs (not required)
                string arcId;
                int u, v, custo;
                stringstream ss(linhaTrimmed); // Use linhaTrimmed
                ss >> arcId >> u >> v >> custo;

                // Apenas adiciona ao grafo (direcional)
                g.adj[u].push_back({v, custo});
                g.maxIdNo = max(g.maxIdNo, max(u, v));
            }
        }
    }

    // Final check for deposit node inclusion in maxIdNo
    g.maxIdNo = max(g.maxIdNo, g.deposito);
    
    // Garante que o depósito está no grafo, mesmo que não haja arestas incidentes a ele
    g.adj[g.deposito];

    arquivo.close();
}
