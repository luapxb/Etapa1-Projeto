#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <filesystem>
#include <chrono>
#include <set>

using namespace std::chrono;
using namespace std;
namespace fs = std::filesystem;

struct Aresta {
    int destino;
    int custo;
};

struct Servico {
    int id;
    string tipo; // "A" para arco, "E" para aresta
    int u, v;
    int custo;
    int demanda;
};

// Grafo - mapa de nó para lista de adjacência (destino, custo)
unordered_map<int, vector<Aresta>> grafo;
vector<Servico> servicos;

int deposito = 0;
int capacidadeVeiculo = 0;

// Caminhos mínimos
vector<vector<int>> distanciasMinimas;

void limparDados() {
    grafo.clear();
    servicos.clear();
    distanciasMinimas.clear();
}

// Ler instância do diretorio de instancias
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

    // Garantir que todos os nós aparecem no grafo
    for (const auto& [node, _] : grafo) {
        if (grafo.find(node) == grafo.end()) {
            grafo[node] = vector<Aresta>();
        }
    }

    arquivo.close();
}


// Função para calcular caminhos mínimos entre todos os nós com Dijkstra
void dijkstra(int origem, vector<int> &dist) {
    int n = (int)grafo.size();
    dist.assign(n, 1e9);
    dist[origem] = 0;
    using pii = pair<int,int>;
    priority_queue<pii, vector<pii>, greater<pii>> pq;
    pq.push({0, origem});

    while(!pq.empty()) {
        int d = pq.top().first;
        int u = pq.top().second;
        pq.pop();
        if (d > dist[u]) continue;

        for (auto &aresta : grafo[u]) {
            int v = aresta.destino;
            int custo = aresta.custo;
            if (dist[u] + custo < dist[v]) {
                dist[v] = dist[u] + custo;
                pq.push({dist[v], v});
            }
        }
    }
}

void calcularCaminhosMinimos() {
    int n = (int)grafo.size();
    distanciasMinimas.resize(n);
    for (int i = 0; i < n; i++) {
        dijkstra(i, distanciasMinimas[i]);
    }
}

// Estrutura para rotas construídas
struct Rota {
    vector<Servico> servicosNaRota;
    int demandaTotal = 0;
    int custoTotal = 0;
};

// Algoritmo construtivo - aproximação gulosa básica
vector<Rota> construirRotas() {
    vector<Rota> rotas;
    set<int> servicosAtendidos; // para evitar atender serviço mais de uma vez

    for (const Servico &s : servicos) {
        if (servicosAtendidos.count(s.id)) continue;

        Rota novaRota;
        novaRota.servicosNaRota.push_back(s);
        novaRota.demandaTotal = s.demanda;
        novaRota.custoTotal = s.custo; // custo serviço

        servicosAtendidos.insert(s.id);

        // Tentando encaixar mais serviços respeitando capacidade
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

// Salvar a solução no arquivo de saida
void salvarSolucao(const string &nomeArquivo, const vector<Rota> &rotas, int tempoReferencia, int tempoSolucao) {
    ofstream saida(nomeArquivo);
    if (!saida.is_open()) {
        cerr << "Erro ao abrir arquivo de saída: " << nomeArquivo << endl;
        return;
    }

    int custoTotal = 0;
    int totalRotas = (int)rotas.size();

    // Calcula custo total
    for (const Rota &r : rotas) {
        custoTotal += r.custoTotal;
    }

    saida << custoTotal << "\n";
    saida << totalRotas << "\n";
    saida << tempoReferencia << "\n";
    saida << tempoSolucao << "\n";

    int idRota = 1;
    for (const Rota &r : rotas) {
        int visitas = (int)r.servicosNaRota.size() + 2; // +2 pois inclui depósito no início e fim
        saida << "0 1 " << idRota << " " << r.demandaTotal << " " << r.custoTotal << " " << visitas << " ";

        // Início no depósito (D 0,1,1)
        saida << "(D 0,1,1) ";

        // Para cada serviço na rota, imprime no formato (S id,u,v)
        for (const Servico &s : r.servicosNaRota) {
            saida << "(S " << s.id << "," << s.u << "," << s.v << ") ";
        }

        // Fim no depósito (D 0,1,1)
        saida << "(D 0,1,1)\n";

        idRota++;
    }

    saida.close();
}

int main() {
    string pastaInstancias = "./instancias";
    string pastaSolucoes = "./solucoes";

    if (!fs::exists(pastaSolucoes)) {
        fs::create_directory(pastaSolucoes);
    }

    cout << "Iniciando processamento das instancias..." << endl;

    for (const auto &entry : fs::directory_iterator(pastaInstancias)) {
        if (!entry.is_regular_file()) continue;

        string caminhoInstancia = entry.path().string();
        string nomeArquivo = entry.path().filename().string();

        cout << "Processando instância: " << nomeArquivo << endl;

        // Medir tempo de execução

        processarInstancia(caminhoInstancia);
        auto inicioReferencia = high_resolution_clock::now();
        calcularCaminhosMinimos();
        auto fimReferencia = high_resolution_clock::now();
        int duracaoReferencia = duration_cast<milliseconds>(fimReferencia - inicioReferencia).count();

        auto inicioSolucao = high_resolution_clock::now();
        vector<Rota> rotas = construirRotas();
        auto fimSolucao = high_resolution_clock::now();
        int duracaoSolucao = duration_cast<milliseconds>(fimSolucao - inicioSolucao).count();

        // Gerar nome do arquivo de saída
        string caminhoSaida = pastaSolucoes + "/sol-" + nomeArquivo;

        salvarSolucao(caminhoSaida, rotas, duracaoReferencia, duracaoSolucao);

        cout << "Instância " << nomeArquivo << " processada. Tempo: " << duracaoSolucao << " ms." << endl;
    }

    cout << "Processamento finalizado." << endl;
    return 0;
}
