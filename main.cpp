#include "src/grafo.h"
#include "src/solucao.h"
#include "src/instancia.h"
#include <filesystem>
#include <chrono>
#include <iostream>

using namespace std;
using namespace chrono;
namespace fs = filesystem;

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

        processarInstancia(caminhoInstancia);
        auto inicioReferencia = high_resolution_clock::now();
        calcularCaminhosMinimos();
        auto fimReferencia = high_resolution_clock::now();
        auto duracaoReferencia = duration_cast<milliseconds>(fimReferencia - inicioReferencia).count();

        auto inicioSolucao = high_resolution_clock::now();
        auto rotas = construirRotas();
        auto fimSolucao = high_resolution_clock::now();
        auto duracaoSolucao = duration_cast<milliseconds>(fimSolucao - inicioSolucao).count();

        string caminhoSaida = pastaSolucoes + "/sol-" + nomeArquivo;
        exportarSolucao(caminhoSaida, rotas, duracaoReferencia, duracaoSolucao);

        cout << "Instância " << nomeArquivo << " processada. Tempo: " << duracaoSolucao << " ms." << endl;
    }

    cout << "Processamento finalizado." << endl;
    return 0;
}