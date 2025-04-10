#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

using namespace std;

// Estruturas para armazenar os dados do grafo
struct Node {
    int id;
    int demand;
    int service_cost;
};

struct Edge {
    int from;
    int to;
    int traversal_cost;
    int demand;
    int service_cost;
    bool required;
};

struct Arc {
    int from;
    int to;
    int traversal_cost;
    int demand;
    int service_cost;
    bool required;
};

class GraphData {
public:
    string name;
    int optimal_value;
    int num_vehicles;
    int capacity;
    int depot_node;
    
    // Contagens totais
    int total_nodes;
    int total_edges;
    int total_arcs;
    
    // Contagens de elementos requeridos
    int required_nodes;
    int required_edges;
    int required_arcs;
    
    // Dados dos elementos
    vector<Node> nodes;
    vector<Edge> edges;
    vector<Arc> arcs;
    
    // Construtor
    GraphData() : optimal_value(-1), num_vehicles(-1), capacity(0), depot_node(0),
                 total_nodes(0), total_edges(0), total_arcs(0),
                 required_nodes(0), required_edges(0), required_arcs(0) {}
    
    // Função para limpar os dados
    void clear() {
        name.clear();
        optimal_value = -1;
        num_vehicles = -1;
        capacity = 0;
        depot_node = 0;
        total_nodes = 0;
        total_edges = 0;
        total_arcs = 0;
        required_nodes = 0;
        required_edges = 0;
        required_arcs = 0;
        nodes.clear();
        edges.clear();
        arcs.clear();
    }
    
    // Função para ler os dados de um arquivo
    bool readFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Erro ao abrir o arquivo: " << filename << endl;
            return false;
        }
        
        clear(); // Limpa os dados antes de ler um novo arquivo
        
        string line;
        string section;
        
        while (getline(file, line)) {
            // Ignorar linhas vazias
            if (line.empty()) continue;
            
            // Verificar se é uma nova seção
            if (line[0] == '#') {
                section = line.substr(1);
                continue;
            }
            
            // Processar cada seção
            if (section == "Name") {
                name = line;
            }
            else if (section == "Optimal value") {
                optimal_value = stoi(line);
            }
            else if (section == "#Vehicles") {
                num_vehicles = stoi(line);
            }
            else if (section == "Capacity") {
                capacity = stoi(line);
            }
            else if (section == "Depot Node") {
                depot_node = stoi(line);
            }
            else if (section == "#Nodes") {
                total_nodes = stoi(line);
            }
            else if (section == "#Edges") {
                total_edges = stoi(line);
            }
            else if (section == "#Arcs") {
                total_arcs = stoi(line);
            }
            else if (section == "#Required N") {
                required_nodes = stoi(line);
            }
            else if (section == "#Required E") {
                required_edges = stoi(line);
            }
            else if (section == "#Required A") {
                required_arcs = stoi(line);
            }
            else if (section == "ReN.") {
                // Pular o cabeçalho
                getline(file, line); // DEMAND S. COST
                
                // Ler nós requeridos
                for (int i = 0; i < required_nodes; i++) {
                    getline(file, line);
                    istringstream iss(line);
                    string node_id;
                    Node n;
                    
                    iss >> node_id >> n.demand >> n.service_cost;
                    n.id = stoi(node_id.substr(1)); // Remove o 'N' do início
                    nodes.push_back(n);
                }
            }
            else if (section == "ReE.") {
                // Pular o cabeçalho
                getline(file, line); // From N. To N. T. COST DEMAND S. COST
                
                // Ler arestas requeridas
                for (int i = 0; i < required_edges; i++) {
                    getline(file, line);
                    istringstream iss(line);
                    string edge_id, from_str, to_str;
                    Edge e;
                    
                    iss >> edge_id >> from_str >> to_str >> e.traversal_cost >> e.demand >> e.service_cost;
                    e.from = stoi(from_str.substr(1)); // Remove o 'N' do início
                    e.to = stoi(to_str.substr(1));    // Remove o 'N' do início
                    e.required = true;
                    edges.push_back(e);
                }
            }
            else if (section == "EDGE") {
                // Pular o cabeçalho
                getline(file, line); // FROM N. TO N. T. COST
                
                // Ler arestas não requeridas
                for (int i = 0; i < (total_edges - required_edges); i++) {
                    getline(file, line);
                    istringstream iss(line);
                    string edge_id, from_str, to_str;
                    Edge e;
                    
                    iss >> edge_id >> from_str >> to_str >> e.traversal_cost;
                    e.from = stoi(from_str.substr(1)); // Remove o 'N' do início
                    e.to = stoi(to_str.substr(1));    // Remove o 'N' do início
                    e.demand = 0;
                    e.service_cost = 0;
                    e.required = false;
                    edges.push_back(e);
                }
            }
            else if (section == "ReA.") {
                // Pular o cabeçalho
                getline(file, line); // FROM N. TO N. T. COST DEMAND S. COST
                
                // Ler arcos requeridos
                for (int i = 0; i < required_arcs; i++) {
                    getline(file, line);
                    istringstream iss(line);
                    string arc_id, from_str, to_str;
                    Arc a;
                    
                    iss >> arc_id >> from_str >> to_str >> a.traversal_cost >> a.demand >> a.service_cost;
                    a.from = stoi(from_str.substr(1)); // Remove o 'N' do início
                    a.to = stoi(to_str.substr(1));    // Remove o 'N' do início
                    a.required = true;
                    arcs.push_back(a);
                }
            }
            else if (section == "ARC") {
                // Pular o cabeçalho
                getline(file, line); // FROM N. TO N. T. COST
                
                // Ler arcos não requeridos
                for (int i = 0; i < (total_arcs - required_arcs); i++) {
                    getline(file, line);
                    istringstream iss(line);
                    string arc_id, from_str, to_str;
                    Arc a;
                    
                    iss >> arc_id >> from_str >> to_str >> a.traversal_cost;
                    a.from = stoi(from_str.substr(1)); // Remove o 'N' do início
                    a.to = stoi(to_str.substr(1));    // Remove o 'N' do início
                    a.demand = 0;
                    a.service_cost = 0;
                    a.required = false;
                    arcs.push_back(a);
                }
            }
        }
        
        file.close();
        return true;
    }
    
    // Funções para acessar os dados (você pode implementar as suas próprias funções de análise)
    int getTotalNodes() const { return total_nodes; }
    int getTotalEdges() const { return total_edges; }
    int getTotalArcs() const { return total_arcs; }
    int getRequiredNodes() const { return required_nodes; }
    int getRequiredEdges() const { return required_edges; }
    int getRequiredArcs() const { return required_arcs; }
    
    // Você pode adicionar mais funções de acesso conforme necessário
};

int main() {
    GraphData graph;
    
    // Exemplo de uso - substitua pelo caminho do seu arquivo
    if (graph.readFromFile("instancia1.txt")) {
        cout << "Dados lidos com sucesso!" << endl;
        cout << "Nome da instância: " << graph.name << endl;
        cout << "Número total de vértices: " << graph.getTotalNodes() << endl;
        cout << "Número total de arestas: " << graph.getTotalEdges() << endl;
        cout << "Número total de arcos: " << graph.getTotalArcs() << endl;
        cout << "Vértices requeridos: " << graph.getRequiredNodes() << endl;
        cout << "Arestas requeridas: " << graph.getRequiredEdges() << endl;
        cout << "Arcos requeridos: " << graph.getRequiredArcs() << endl;
    } else {
        cout << "Falha ao ler o arquivo." << endl;
    }
    
    return 0;
}