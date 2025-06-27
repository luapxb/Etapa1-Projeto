#include "src/grafo.h"
#include "src/solucao.h"
#include "src/instancia.h"
#include "src/solver.h" // Incluir solver.h para a função construirRotas
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
    }

    cout << "Iniciando processamento das instancias..." << endl;

    // Itera sobre os arquivos na pasta de instâncias
    for (const auto &entrada : fs::directory_iterator(pastaInstancias)) {
        if (!entrada.is_regular_file()) continue; // Ignora se não for um arquivo regular

        string caminhoInstancia = entrada.path().string();
        string nomeArquivo = entrada.path().filename().string();

        cout << "Processando instância: " << nomeArquivo << endl;

        Grafo g; // Cria uma nova instância do grafo para cada arquivo

        // --- ETAPA 1: Pré-processamento ---
        // processarInstancia agora exige um objeto Grafo
        processarInstancia(caminhoInstancia, g); 

        auto inicioReferencia = high_resolution_clock::now();
        // calcularCaminhosMinimos agora exige um objeto Grafo
        calcularCaminhosMinimos(g); 
        auto fimReferencia = high_resolution_clock::now();
        auto duracaoReferencia = duration_cast<milliseconds>(fimReferencia - inicioReferencia).count();

        // --- ETAPA 2: Solução Inicial (Algoritmo Construtivo) ---
        auto inicioSolucao = high_resolution_clock::now();
        // construirRotas agora exige um objeto Grafo e o vetor de servicos
        auto rotas = construirRotas(g, servicos); 
        auto fimSolucao = high_resolution_clock::now();
        auto duracaoSolucao = duration_cast<milliseconds>(fimSolucao - inicioSolucao).count();

        // Constrói o caminho completo para o arquivo de saída
        string caminhoSaida = pastaSolucoes + "/sol-" + nomeArquivo;
        
        // exportarSolucao agora exige um objeto Grafo
        exportarSolucao(caminhoSaida, rotas, duracaoReferencia, duracaoSolucao, g); 

        cout << "Instância " << nomeArquivo << " processada. Tempo da Solução: " << duracaoSolucao << " ms." << endl;
    }

    cout << "Processamento finalizado." << endl;
    return 0;
}