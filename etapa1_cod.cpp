#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <cmath>
#include <climits>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

struct Aresta {
    int from, to, traversal_cost, demand, service_cost;
    bool required;
};

struct Arco {
    int from, to, traversal_cost, demand, service_cost;
    bool required;
};

class Grafo {
private:
    vector<Aresta> arestas;
    vector<Arco> arcos;
    set<int> vertices;
    set<int> vertices_requeridos;
    
    void construirAdjacencia(map<int, vector<int>>& adj) {
        adj.clear();
        for (int v : vertices) adj[v] = vector<int>();
        
        for (const Aresta& e : arestas) {
            adj[e.from].push_back(e.to);
            adj[e.to].push_back(e.from);
        }
        
        for (const Arco& a : arcos) {
            adj[a.from].push_back(a.to);
        }
    }

    void DFS(int v, map<int, bool>& visitados, const map<int, vector<int>>& adj) {
        visitados[v] = true;
        for (int u : adj.at(v)) {
            if (!visitados[u]) DFS(u, visitados, adj);
        }
    }

public:
    // Métricas básicas
    int qtdVertices() const { return vertices.size(); }
    int qtdArestas() const { return arestas.size(); }
    int qtdArcos() const { return arcos.size(); }
    int qtdVerticesRequeridos() const { return vertices_requeridos.size(); }
    
    int qtdArestasRequeridas() const {
        int count = 0;
        for (const Aresta& e : arestas) 
            if (e.required) count++;
        return count;
    }
    
    int qtdArcosRequeridos() const {
        int count = 0;
        for (const Arco& a : arcos)
            if (a.required) count++;
        return count;
    }
    
    double densidade() {
        int n = qtdVertices();
        int m = qtdArestas() + qtdArcos();
        return n <= 1 ? 0.0 : (1.0 * m) / (n * (n - 1));
    }
    
    int componentesConectados() {
        map<int, vector<int>> adj;
        construirAdjacencia(adj);
        
        map<int, bool> visitados;
        for (int v : vertices) visitados[v] = false;
        
        int componentes = 0;
        for (int v : vertices) {
            if (!visitados[v]) {
                DFS(v, visitados, adj);
                componentes++;
            }
        }
        return componentes;
    }
    
    int grauMinimo() {
        map<int, vector<int>> adj;
        construirAdjacencia(adj);
        
        int min_grau = INT_MAX;
        for (const auto& par : adj) {
            int grau = par.second.size();
            min_grau = min(min_grau, grau);
        }
        return min_grau == INT_MAX ? 0 : min_grau;
    }
    
    int grauMaximo() {
        map<int, vector<int>> adj;
        construirAdjacencia(adj);
        
        int max_grau = 0;
        for (const auto& par : adj) {
            int grau = par.second.size();
            max_grau = max(max_grau, grau);
        }
        return max_grau;
    }

    void processarLinha(const string& linha, const string& secao) {
        if (linha.empty()) return;
        istringstream iss(linha);
        string token;

        if (secao == "Nodes") {
            // Formato: N1, N2, etc. ou apenas números
            string node_id;
            if (iss >> node_id) {
                int id = 0;
                if (node_id[0] == 'N') {
                    id = stoi(node_id.substr(1));
                } else {
                    id = stoi(node_id);
                }
                vertices.insert(id);
            }
        }
        else if (secao == "Required N") {
            // Formato: N4    1    1
            string node_id;
            int demand, service_cost;
            if (iss >> node_id >> demand >> service_cost) {
                int id = 0;
                if (node_id[0] == 'N') {
                    id = stoi(node_id.substr(1));
                } else {
                    id = stoi(node_id);
                }
                vertices_requeridos.insert(id);
                vertices.insert(id);
            }
        }
        else if (secao == "Edges") {
            // Formato: E1    2    3    18    1    19
            string edge_id, from_str, to_str;
            Aresta e;
            if (iss >> edge_id >> from_str >> to_str >> e.traversal_cost >> e.demand >> e.service_cost) {
                e.from = from_str[0] == 'N' ? stoi(from_str.substr(1)) : stoi(from_str);
                e.to = to_str[0] == 'N' ? stoi(to_str.substr(1)) : stoi(to_str);
                e.required = true;
                arestas.push_back(e);
                vertices.insert(e.from);
                vertices.insert(e.to);
            }
        }
        else if (secao == "EDGE") {
            // Formato: EDGE FROM N. TO N. T. COST (cabeçalho) ou linhas de dados
            if (linha.find("FROM N.") == string::npos) { // Se não for cabeçalho
                string edge_id, from_str, to_str;
                Aresta e;
                if (iss >> edge_id >> from_str >> to_str >> e.traversal_cost) {
                    e.from = from_str[0] == 'N' ? stoi(from_str.substr(1)) : stoi(from_str);
                    e.to = to_str[0] == 'N' ? stoi(to_str.substr(1)) : stoi(to_str);
                    e.demand = e.service_cost = 0;
                    e.required = false;
                    arestas.push_back(e);
                    vertices.insert(e.from);
                    vertices.insert(e.to);
                }
            }
        }
        else if (secao == "Arcs") {
            // Formato: A1    1    2    13    1    14
            string arc_id, from_str, to_str;
            Arco a;
            if (iss >> arc_id >> from_str >> to_str >> a.traversal_cost >> a.demand >> a.service_cost) {
                a.from = from_str[0] == 'N' ? stoi(from_str.substr(1)) : stoi(from_str);
                a.to = to_str[0] == 'N' ? stoi(to_str.substr(1)) : stoi(to_str);
                a.required = true;
                arcos.push_back(a);
                vertices.insert(a.from);
                vertices.insert(a.to);
            }
        }
        else if (secao == "ARC") {
            // Formato: ARC FROM N. TO N. T. COST (cabeçalho) ou linhas de dados
            if (linha.find("FROM N.") == string::npos) { // Se não for cabeçalho
                string arc_id, from_str, to_str;
                Arco a;
                if (iss >> arc_id >> from_str >> to_str >> a.traversal_cost) {
                    a.from = from_str[0] == 'N' ? stoi(from_str.substr(1)) : stoi(from_str);
                    a.to = to_str[0] == 'N' ? stoi(to_str.substr(1)) : stoi(to_str);
                    a.demand = a.service_cost = 0;
                    a.required = false;
                    arcos.push_back(a);
                    vertices.insert(a.from);
                    vertices.insert(a.to);
                }
            }
        }
    }
};

void processarInstancia(const string& nomeArquivo, Grafo& grafo) {
    string caminhoArquivo = "../instancias/" + nomeArquivo;
    cout << "Abrindo arquivo: " << caminhoArquivo << endl;
    ifstream arquivo(caminhoArquivo);
    if (!arquivo) {
        cerr << "Erro ao abrir o arquivo!\n";
        return;
    }

    string linha;
    string secao;
    bool headerProcessed = false;
    
    while (getline(arquivo, linha)) {
        // Remover espaços em branco no início e fim
        linha.erase(0, linha.find_first_not_of(" \t"));
        linha.erase(linha.find_last_not_of(" \t") + 1);
        
        if (linha.empty()) continue;
        
        // Processar o cabeçalho inicial
        if (!headerProcessed) {
            if (linha.find("Name:") != string::npos) {
                headerProcessed = true;
            }
            continue;
        }
        
        // Identificar seções
        if (linha[0] == '#') {
            if (linha.find("#Nodes") != string::npos) {
                secao = "Nodes";
            }
            else if (linha.find("#Required N") != string::npos) {
                secao = "Required N";
            }
            else if (linha.find("#Required E") != string::npos) {
                secao = "Edges";
            }
            else if (linha.find("#Required A") != string::npos) {
                secao = "Arcs";
            }
            else if (linha.find("#Edges") != string::npos) {
                secao = "EDGE";
            }
            else if (linha.find("#Arcs") != string::npos) {
                secao = "ARC";
            }
            continue;
        }
        else if (linha.find("ReN.") != string::npos) {
            secao = "Required N";
            continue;
        }
        else if (linha.find("ReE.") != string::npos) {
            secao = "Edges";
            continue;
        }
        else if (linha.find("ReA.") != string::npos) {
            secao = "Arcs";
            continue;
        }
        else if (linha.find("EDGE") != string::npos) {
            secao = "EDGE";
            continue;
        }
        else if (linha.find("ARC") != string::npos) {
            secao = "ARC";
            continue;
        }
        
        grafo.processarLinha(linha, secao);
    }
    
    arquivo.close();
}

int main() {
    Grafo grafo;
    string nomeArquivo;
    // Lista automaticamente os arquivos .dat
    // listarArquivosDat();

    cout << "\nDigite o nome da instancia: ";
    getline(cin, nomeArquivo);

    // if (!arquivoExiste(nomeArquivo)) {
    //  cerr << "\nERRO: Arquivo '" << nomeArquivo << "' nao encontrado!\n";
    //  listarArquivosDat();
    // return 1;
    //}
    cout << "\n=== Processando: " << nomeArquivo << " ===\n";

    processarInstancia(nomeArquivo, grafo);

    cout << "\n=== Metricas da Instancia ===\n";
    cout << "1. Quantidade de vertices: " << grafo.qtdVertices() << endl;
    cout << "2. Quantidade de arestas: " << grafo.qtdArestas() << endl;
    cout << "3. Quantidade de arcos: " << grafo.qtdArcos() << endl;
    cout << "4. Quantidade de vertices requeridos: " << grafo.qtdVerticesRequeridos() << endl;
    cout << "5. Quantidade de arestas requeridas: " << grafo.qtdArestasRequeridas() << endl;
    cout << "6. Quantidade de arcos requeridos: " << grafo.qtdArcosRequeridos() << endl;
    cout << "7. Densidade do grafo: " << grafo.densidade() << endl;
    cout << "8. Componentes conectados: " << grafo.componentesConectados() << endl;
    cout << "9. Grau minimo dos vertices: " << grafo.grauMinimo() << endl;
    cout << "10. Grau maximo dos vertices: " << grafo.grauMaximo() << endl;

    return 0;
}