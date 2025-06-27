#include "solver.h"
#include <set>
#include <limits> // Para numeric_limits
#include <algorithm> // Para std::min e std::sort
#include <iostream>

using namespace std;

// Calcula o custo total de uma rota, considerando os custos de viagem e os custos dos serviços.
// Retorna numeric_limits<int>::max() se a rota for impossível devido a caminhos não encontrados.
int calcularCustoRota(const Rota& rota, const Grafo& g) {
    if (rota.servicosNaRota.empty()) {
        return 0; // Rota vazia não tem custo
    }

    long long custoTotal = 0; // Usar long long para evitar overflow antes de verificar INT_MAX
    int noAtual = g.deposito;

    for (const auto& servico : rota.servicosNaRota) {
        // Custo de viajar do nó atual para o início do serviço (u)
        // Corrigido: Cast para size_t para evitar warning de signed/unsigned
        if (static_cast<size_t>(noAtual) >= g.distanciasMinimas.size() || 
            static_cast<size_t>(servico.u) >= g.distanciasMinimas[noAtual].size() ||
            g.distanciasMinimas[noAtual][servico.u] == numeric_limits<int>::max()) {
            cout << "      [CalcularCustoRota Debug] Caminho de " << noAtual << " para " << servico.u << " inválido ou infinito." << endl;
            return numeric_limits<int>::max(); // Caminho inválido, rota impossível
        }
        custoTotal += g.distanciasMinimas[noAtual][servico.u];

        // Se o serviço é de aresta/arco (u != v), adiciona o custo de viajar de u para v
        if (servico.u != servico.v) {
            // Corrigido: Cast para size_t para evitar warning de signed/unsigned
            if (static_cast<size_t>(servico.u) >= g.distanciasMinimas.size() || 
                static_cast<size_t>(servico.v) >= g.distanciasMinimas[servico.u].size() ||
                g.distanciasMinimas[servico.u][servico.v] == numeric_limits<int>::max()) {
                 cout << "      [CalcularCustoRota Debug] Caminho interno do serviço " << servico.u << " para " << servico.v << " inválido ou infinito." << endl;
                 return numeric_limits<int>::max(); // Caminho inválido, rota impossível
            }
            custoTotal += g.distanciasMinimas[servico.u][servico.v];
        }
        
        // Adiciona o custo fixo do serviço em si (independentemente do tipo)
        custoTotal += servico.custo;

        // O nó atual para o próximo cálculo é o final do serviço
        noAtual = servico.v;
    }

    // Custo de viajar do último serviço de volta para o depósito
    // Corrigido: Cast para size_t para evitar warning de signed/unsigned
    if (static_cast<size_t>(noAtual) >= g.distanciasMinimas.size() || 
        static_cast<size_t>(g.deposito) >= g.distanciasMinimas[noAtual].size() ||
        g.distanciasMinimas[noAtual][g.deposito] == numeric_limits<int>::max()) {
         cout << "      [CalcularCustoRota Debug] Caminho do último serviço (" << noAtual << ") para o depósito (" << g.deposito << ") inválido ou infinito." << endl;
         return numeric_limits<int>::max(); // Caminho inválido, rota impossível
    }
    custoTotal += g.distanciasMinimas[noAtual][g.deposito];

    // Verifica se o custo total excedeu o limite de int (embora improvável para a maioria das instâncias de teste)
    if (custoTotal > numeric_limits<int>::max()) {
        cout << "      [CalcularCustoRota Debug] Custo total excedeu numeric_limits<int>::max()." << endl;
        return numeric_limits<int>::max(); // Retorna MAX se ocorrer overflow
    }

    return static_cast<int>(custoTotal);
}

// Constrói um conjunto inicial de rotas usando uma heurística gulosa (vizinho mais próximo).
vector<Rota> construirRotas(const Grafo& g, const vector<Servico>& todosServicos) {
    vector<Rota> rotas;
    set<int> servicosAtendidosIds; // Usa IDs para rastrear serviços já atendidos

    // Cria uma cópia dos serviços para trabalhar, opcionalmente ordenando-os para determinismo
    vector<Servico> servicosDisponiveis = todosServicos;
    // Ordenar serviços por ID para garantir resultados determinísticos se houver serviços com a mesma "distância"
    sort(servicosDisponiveis.begin(), servicosDisponiveis.end(), [](const Servico& a, const Servico& b) {
        return a.id < b.id;
    });

    cout << "    [ConstruirRotas Debug] Total de serviços a serem atendidos: " << todosServicos.size() << endl;

    // Loop principal: continua enquanto houver serviços não atendidos
    while (servicosAtendidosIds.size() < todosServicos.size()) {
        Rota novaRota;
        int noAtualDaRota = g.deposito;
        int demandaAtualDaRota = 0;
        
        cout << "      [ConstruirRotas Debug] Iniciando nova rota do depósito " << g.deposito << ". Serviços atendidos até agora: " << servicosAtendidosIds.size() << endl;

        bool rotaVaziaAposTentativa = true; // Flag para saber se a rota permaneceu vazia após uma tentativa de preenchimento

        // Loop para preencher a rota atual
        while (true) {
            int melhorServicoIdx = -1;
            long long menorCustoAdicional = numeric_limits<long long>::max(); // Usar long long para evitar overflow
            int proxNoPotencial = -1; // Nó final do serviço potencial

            // Encontra o serviço não atendido mais próximo que cabe na rota
            for (size_t i = 0; i < servicosDisponiveis.size(); ++i) {
                const auto& servicoPotencial = servicosDisponiveis[i];
                if (servicosAtendidosIds.count(servicoPotencial.id)) {
                    continue; // Serviço já atendido
                }

                if (demandaAtualDaRota + servicoPotencial.demanda > g.capacidadeVeiculo) {
                    // cout << "        [ConstruirRotas Debug] Serviço " << servicoPotencial.id << " (" << servicoPotencial.demanda << " demanda) excede capacidade. Demanda atual: " << demandaAtualDaRota << ", Capacidade: " << g.capacidadeVeiculo << endl;
                    continue;
                }

                // Calcula o custo de viajar do nó atual para o início do serviço
                // Corrigido: Cast para size_t para evitar warning de signed/unsigned
                if (static_cast<size_t>(noAtualDaRota) >= g.distanciasMinimas.size() || 
                    static_cast<size_t>(servicoPotencial.u) >= g.distanciasMinimas[noAtualDaRota].size()) {
                    // cout << "        [ConstruirRotas Debug] Índices de distanciasMinimas inválidos para nó " << noAtualDaRota << " ou serviço.u " << servicoPotencial.u << endl;
                    continue; // Índice fora dos limites para distanciasMinimas
                }
                int custoViagemParaInicio = g.distanciasMinimas[noAtualDaRota][servicoPotencial.u];
                
                if (custoViagemParaInicio == numeric_limits<int>::max()) {
                    // cout << "        [ConstruirRotas Debug] Caminho do nó " << noAtualDaRota << " para " << servicoPotencial.u << " inviável." << endl;
                    continue; // Caminho inviável
                }

                int custoViagemServico = 0;
                if (servicoPotencial.u != servicoPotencial.v) { // Se for serviço de aresta/arco
                    // Corrigido: Cast para size_t para evitar warning de signed/unsigned
                    if (static_cast<size_t>(servicoPotencial.u) >= g.distanciasMinimas.size() || 
                        static_cast<size_t>(servicoPotencial.v) >= g.distanciasMinimas[servicoPotencial.u].size()) {
                        // cout << "        [ConstruirRotas Debug] Índices de distanciasMinimas inválidos para serviço.u " << servicoPotencial.u << " ou serviço.v " << servicoPotencial.v << endl;
                        continue; // Índice fora dos limites para distanciasMinimas
                    }
                    custoViagemServico = g.distanciasMinimas[servicoPotencial.u][servicoPotencial.v];
                    if (custoViagemServico == numeric_limits<int>::max()) {
                        // cout << "        [ConstruirRotas Debug] Caminho dentro do serviço " << servicoPotencial.u << " para " << servicoPotencial.v << " inviável." << endl;
                        continue; // Caminho inviável
                    }
                }
                
                long long custoAdicionalAtual = (long long)custoViagemParaInicio + custoViagemServico + servicoPotencial.custo;

                if (custoAdicionalAtual < menorCustoAdicional) {
                    menorCustoAdicional = custoAdicionalAtual;
                    melhorServicoIdx = i;
                    proxNoPotencial = servicoPotencial.v;
                }
            }

            if (melhorServicoIdx != -1) {
                // Se um serviço foi adicionado, a rota não está mais vazia
                rotaVaziaAposTentativa = false; 

                const auto& servicoParaAdicionar = servicosDisponiveis[melhorServicoIdx];
                novaRota.servicosNaRota.push_back(servicoParaAdicionar);
                servicosAtendidosIds.insert(servicoParaAdicionar.id);
                demandaAtualDaRota += servicoParaAdicionar.demanda;
                noAtualDaRota = proxNoPotencial;
                // cout << "        [ConstruirRotas Debug] Adicionado serviço " << servicoParaAdicionar.id << " à rota. Demanda atual: " << demandaAtualDaRota << ", Nó atual: " << noAtualDaRota << endl;
            } else {
                // Nenhum serviço pode ser adicionado à rota atual (nem o primeiro, nem os subsequentes)
                cout << "      [ConstruirRotas Debug] Nenhum serviço viável encontrado para adicionar à rota atual. Quebrando loop interno." << endl;
                break; // Sai do loop interno de preenchimento da rota
            }
        }

        // Se nenhuma rota pôde ser construída nesta iteração principal, sai
        if (novaRota.servicosNaRota.empty()) {
            cout << "    [ConstruirRotas Debug] Nenhuma rota pôde ser construída nesta iteração. Quebrando loop principal." << endl;
            break; // Sai do loop principal while (servicosAtendidosIds.size() < todosServicos.size())
        } else {
            novaRota.custoTotal = calcularCustoRota(novaRota, g);
            if (novaRota.custoTotal == numeric_limits<int>::max()) {
                cerr << "    [ConstruirRotas Debug] A rota construída tem custo infinito! Pode haver um problema com os caminhos. Esta rota pode ser inválida." << endl;
                // Se uma rota é construída mas seu custo final é infinito, há um problema com os caminhos.
                // Isso pode levar a resultados incorretos ou 0 se todas as rotas forem assim.
            }
            novaRota.demandaTotal = demandaAtualDaRota;
            rotas.push_back(novaRota);
            cout << "      [ConstruirRotas Debug] Rota finalizada com " << novaRota.servicosNaRota.size() << " serviços. Custo: " << novaRota.custoTotal << ", Demanda: " << novaRota.demandaTotal << endl;
        }
    }

    cout << "    [ConstruirRotas Debug] Total de serviços atendidos: " << servicosAtendidosIds.size() << " de " << todosServicos.size() << endl;
    if (servicosAtendidosIds.size() < todosServicos.size()) {
        cerr << "AVISO: Não foi possível atribuir todos os serviços. Restantes: " << (todosServicos.size() - servicosAtendidosIds.size()) << "." << endl;
    }
    return rotas;
}

// Implementa o algoritmo de busca local 2-Opt para melhorar as rotas.
// Recebe um vetor de rotas (por referência, para modificação) e o objeto Grafo.
void melhorarRotas2Opt(vector<Rota>& rotas, const Grafo& g) {
    bool melhoriaEncontrada;
    int iteracao = 0;
    do {
        melhoriaEncontrada = false;
        iteracao++;
        // cout << "      [2-Opt Debug] Iteração: " << iteracao << endl;

        // Itera sobre cada rota para aplicar 2-Opt
        for (size_t rIdx = 0; rIdx < rotas.size(); ++rIdx) {
            Rota& rotaAtual = rotas[rIdx];
            // O 2-Opt precisa de pelo menos 2 serviços para fazer uma troca significativa (i.e., 4 nós/pontos distintos)
            if (rotaAtual.servicosNaRota.size() < 2) continue; 

            // Loop para o primeiro ponto de corte (i): vai de 0 até o penúltimo serviço
            for (size_t i = 0; i < rotaAtual.servicosNaRota.size() - 1; ++i) {
                // Loop para o segundo ponto de corte (j): vai de i + 1 até o último serviço
                for (size_t j = i + 1; j < rotaAtual.servicosNaRota.size(); ++j) {
                    
                    // Cria uma nova sequência de serviços com o segmento invertido
                    vector<Servico> novaSequenciaServicos;
                    
                    // 1. Adiciona serviços do início (índice 0) até o ponto de corte 'i' (inclusive)
                    for (size_t k = 0; k <= i; ++k) {
                        novaSequenciaServicos.push_back(rotaAtual.servicosNaRota[k]);
                    }
                    
                    // 2. Adiciona serviços do ponto de corte 'j' de volta para 'i+1' (em ordem reversa)
                    for (size_t k = j; k > i; --k) {
                        novaSequenciaServicos.push_back(rotaAtual.servicosNaRota[k]);
                    }
                    
                    // 3. Adiciona serviços do ponto de corte 'j+1' até o final da rota (se houver)
                    for (size_t k = j + 1; k < rotaAtual.servicosNaRota.size(); ++k) {
                        novaSequenciaServicos.push_back(rotaAtual.servicosNaRota[k]);
                    }

                    // Cria uma rota temporária com a nova sequência para calcular o custo
                    Rota rotaTeste; 
                    rotaTeste.servicosNaRota = novaSequenciaServicos;
                    // Recalcula demanda total para a rota teste
                    rotaTeste.demandaTotal = 0;
                    for (const auto& s : rotaTeste.servicosNaRota) {
                        rotaTeste.demandaTotal += s.demanda;
                    }

                    // Se a nova demanda excede a capacidade, esta rota teste é inválida
                    if (rotaTeste.demandaTotal > g.capacidadeVeiculo) {
                        // cout << "        [2-Opt Debug] Nova rota (2-opt) excede capacidade." << endl;
                        continue; 
                    }

                    rotaTeste.custoTotal = calcularCustoRota(rotaTeste, g); // Recalcula o custo da rota modificada

                    // Se a nova rota tem um custo menor e é válida (não numeric_limits<int>::max())
                    if (rotaTeste.custoTotal < rotaAtual.custoTotal && rotaTeste.custoTotal != numeric_limits<int>::max()) {
                        // cout << "        [2-Opt Debug] Melhoria encontrada na rota " << rIdx << " de " << rotaAtual.custoTotal << " para " << rotaTeste.custoTotal << endl;
                        rotaAtual = rotaTeste; // Aceita a melhoria: atualiza a rota original
                        melhoriaEncontrada = true; // Marca que uma melhoria foi feita
                        // Reinicia a busca 2-Opt para esta rota (ou globalmente)
                        goto proxima_iteracao_2opt; // Vai para o rótulo para reiniciar o do-while
                    }
                }
            }
        }
        proxima_iteracao_2opt:; // Rótulo para o goto
    } while (melhoriaEncontrada); // Continua enquanto melhorias forem encontradas
}
