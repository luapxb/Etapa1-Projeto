Analisador de Grafos e Solucionador de Problemas de Roteamento
Descrição

Este projeto implementa um analisador de grafos para instâncias de problemas de roteamento e um solucionador heurístico. O programa calcula diversas métricas de grafos, incluindo conectividade, graus dos vértices, caminhos mínimos e outras propriedades relevantes (Fase 1). Além disso, ele constrói uma solução inicial utilizando uma heurística gulosa (Fase 2) e a aprimora através de um algoritmo de busca local 2-Opt (Fase 3).
Fases do Projeto Implementadas

    Fase 1: Pré-processamento e Análise de Grafos

        Representação do problema usando estruturas de dados de grafos.

        Implementação da leitura de dados de instâncias padrão.

        Cálculo de estatísticas e métricas do grafo (incluindo matriz de caminhos mínimos).

    Fase 2: Solução Inicial Construtiva

        Implementação de uma heurística gulosa (Vizinho Mais Próximo) para construir rotas que respeitem a capacidade dos veículos e garantam que cada serviço seja atendido exatamente uma vez.

    Fase 3: Métodos de Melhoria

        Implementação de um algoritmo de busca local (2-Opt) para otimizar as rotas geradas na Fase 2, visando reduzir o custo total da solução.

Instruções de Compilação

O projeto utiliza um Makefile para facilitar a compilação.

    Navegue até o diretório raiz do projeto:

    cd ETAPA1-PROJETO

    Para limpar quaisquer arquivos de compilação anteriores (objetos e executáveis):

    make clean

    Para compilar o código fonte e criar o executável solver:

    make

Instruções de Execução

O executável solver processará automaticamente todos os arquivos de instância localizados na pasta ./instancias e salvará as soluções na pasta ./solucoes.

    Certifique-se de que o executável solver foi compilado com sucesso (passo anterior).

    Execute o programa a partir do diretório raiz do projeto:

    ./solver

Estrutura de diretórios esperada:

ETAPA1-PROJETO/
├── src/
│   ├── grafo.h
│   ├── grafo.cpp
│   ├── instancia.h
│   ├── instancia.cpp
│   ├── solucao.h
│   ├── solucao.cpp
│   ├── solver.h
│   ├── solver.cpp
│   ├── exportador.h
│   └── exportador.cpp
├── instancias/
│   ├── instancia1.dat
│   ├── instancia2.dat
│   └── ...
├── solucoes/ (será criado automaticamente)
├── Makefile
└── main.cpp

Saída do Programa

O programa gerará saídas detalhadas no console para cada instância processada, incluindo:

    Informações de depuração sobre a leitura da instância (depósito, capacidade, número de nós e serviços).

    Tempos de execução para o cálculo de caminhos mínimos (Fase 1), construção da solução inicial (Fase 2) e otimização com 2-Opt (Fase 3).

    Custo total da solução inicial e final, e a redução de custo obtida.

    Um arquivo de solução no formato sol-nome_da_instancia.dat será criado na pasta ./solucoes.

As métricas calculadas e utilizadas internamente incluem:

    Quantidade de vértices

    Quantidade de arestas

    Quantidade de arcos

    Vértices requeridos

    Arestas requeridas

    Arcos requeridos

    Densidade do grafo

    Componentes conexas

    Grau mínimo

    Grau máximo

    Intermediação dos vértices

    Caminho médio

    Diâmetro do grafo

Colaboradores

    Ketlyn Sara - @kethlynsara

    Luana Peixoto - @luapxb