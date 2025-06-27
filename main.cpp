#include "src/grafo.h"
#include "src/solucao.h"
#include "src/instancia.h"
#include "src/solver.h" // Incluir solver.h para as funções construirRotas e melhorarRotas2Opt
#include "src/exportador.h" // Incluir exportador.h para exportarSolucao
#include <filesystem>
#include <chrono>
#include <iostream>
#include <vector> // Para vector<Servico>

using namespace std;
using namespace chrono;
namespace fs = filesystem;

// Variável global para armazenar os serviços, definida em solucao.cpp
extern vector<Servico> servicos; 

int main() {
    string pastaInstancias = "./instancias";
    string pastaSolucoes = "./solucoes";

    // Cria o diretório de soluções se ele não existir
    if (!fs::exists(pastaSolucoes)) {
        fs::create_directory(pastaSolucoes);
        cout << "Diretório de soluções criado: " << pastaSolucoes << endl;
    }

    cout << "Iniciando processamento das instancias..." << endl;

    // Itera sobre os arquivos na pasta de instâncias
    for (const auto &entrada : fs::directory_iterator(pastaInstancias)) {
        if (!entrada.is_regular_file()) continue; // Ignora se não for um arquivo regular

        string caminhoInstancia = entrada.path().string();
        string nomeArquivo = entrada.path().filename().string();

        cout << "\n--- Processando instância: " << nomeArquivo << " ---" << endl;

        Grafo g; // Cria uma nova instância do grafo para cada arquivo

        // --- ETAPA 1: Pré-processamento ---
        cout << "  [Etapa 1] Processando dados da instância e construindo grafo..." << endl;
        processarInstancia(caminhoInstancia, g); 

        // Adições de Debug em main.cpp
        cout << "  [Debug Main] Depósito: " << g.deposito << ", Capacidade do Veículo: " << g.capacidadeVeiculo << endl;
        cout << "  [Debug Main] Número de nós no grafo (adj list size): " << g.adj.size() << endl;
        cout << "  [Debug Main] ID Máximo de Nó (g.maxIdNo): " << g.maxIdNo << endl;
        cout << "  [Debug Main] Número de Serviços lidos (global 'servicos' size): " << servicos.size() << endl;

        auto inicioCalculoCaminhos = high_resolution_clock::now();
        calcularCaminhosMinimos(g); 
        auto fimCalculoCaminhos = high_resolution_clock::now();
        auto duracaoCalculoCaminhos = duration_cast<milliseconds>(fimCalculoCaminhos - inicioCalculoCaminhos).count();
        cout << "  [Etapa 1] Caminhos mínimos calculados em " << duracaoCalculoCaminhos << " ms." << endl;
        
        // Verificação de debug da matriz de distâncias mínimas
        // (Apenas se o depósito for um índice válido na matriz)
        if (g.deposito >= 0 && static_cast<size_t>(g.deposito) < g.distanciasMinimas.size() && 
            static_cast<size_t>(g.deposito) < g.distanciasMinimas[g.deposito].size()) {
             cout << "  [Debug Main] Distância do depósito (" << g.deposito << ") para depósito: " << g.distanciasMinimas[g.deposito][g.deposito] << endl;
        }
        // Tentativa de verificar distância para o nó 1, se existir e índices forem válidos
        if (g.deposito >= 0 && static_cast<size_t>(g.deposito) < g.distanciasMinimas.size() && 
            1 < g.distanciasMinimas[g.deposito].size()) { // Nó 1 é um índice válido
             cout << "  [Debug Main] Distância do depósito (" << g.deposito << ") para nó 1: " << g.distanciasMinimas[g.deposito][1] << endl;
        }


        // --- ETAPA 2: Solução Inicial (Algoritmo Construtivo) ---
        cout << "  [Etapa 2] Construindo solução inicial (heurística gulosa)..." << endl;
        auto inicioSolucaoConstrutiva = high_resolution_clock::now();
        vector<Rota> rotasIniciais = construirRotas(g, servicos); 
        auto fimSolucaoConstrutiva = high_resolution_clock::now();
        auto duracaoSolucaoConstrutiva = duration_cast<milliseconds>(fimSolucaoConstrutiva - inicioSolucaoConstrutiva).count();
        cout << "  [Etapa 2] Solução inicial construída em " << duracaoSolucaoConstrutiva << " ms. Rotas: " << rotasIniciais.size() << endl;

        int custoTotalInicial = 0;
        for(const auto& r : rotasIniciais) {
            custoTotalInicial += r.custoTotal;
        }
        cout << "  [Etapa 2] Custo Total Inicial: " << custoTotalInicial << endl;

        // --- ETAPA 3: Métodos de Melhoria (2-Opt) ---
        cout << "  [Etapa 3] Melhorando rotas com 2-Opt..." << endl;
        auto inicioMelhoria = high_resolution_clock::now();
        melhorarRotas2Opt(rotasIniciais, g); // A função 2-Opt modifica 'rotasIniciais' diretamente
        auto fimMelhoria = high_resolution_clock::now();
        auto duracaoMelhoria = duration_cast<milliseconds>(fimMelhoria - inicioMelhoria).count();
        cout << "  [Etapa 3] Melhoria com 2-Opt concluída em " << duracaoMelhoria << " ms." << endl;

        int custoTotalFinal = 0;
        for(const auto& r : rotasIniciais) {
            custoTotalFinal += r.custoTotal;
        }
        cout << "  [Etapa 3] Custo Total Final (após 2-Opt): " << custoTotalFinal << endl;
        cout << "  [Etapa 3] Redução de Custo: " << (custoTotalInicial - custoTotalFinal) << endl;

        // --- Exportar Solução Final ---
        long long duracaoTotalSolucao = duracaoSolucaoConstrutiva + duracaoMelhoria;
        string caminhoSaida = pastaSolucoes + "/sol-" + nomeArquivo;
        
        // Chamada para a exportarSolucao do exportador.h/.cpp
        exportarSolucao(caminhoSaida, rotasIniciais, duracaoCalculoCaminhos, duracaoTotalSolucao, g); 

        cout << "Instância " << nomeArquivo << " processada. Tempo Total da Solução (Construtivo + Melhoria): " << duracaoTotalSolucao << " ms." << endl;
        cout << "Solução exportada para: " << caminhoSaida << endl;
    }

    cout << "\nProcessamento finalizado." << endl;
    return 0;
}
