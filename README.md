# Analisador de Grafos e Solucionador de Problemas de Roteamento

## Descrição

Este projeto implementa um analisador de grafos para instâncias de problemas de roteamento e um solucionador heurístico. O programa calcula diversas métricas de grafos, incluindo conectividade, graus dos vértices, caminhos mínimos e outras propriedades relevantes (Fase 1). Além disso, ele constrói uma solução inicial utilizando uma heurística gulosa (Fase 2) e a aprimora através de um algoritmo de busca local 2-Opt (Fase 3).

## Fases do Projeto Implementadas

  * **Fase 1: Pré-processamento e Análise de Grafos**
      * Representação do problema usando estruturas de dados de grafos.
      * Implementação da leitura de dados de instâncias padrão.
      * Cálculo de estatísticas e métricas do grafo (incluindo matriz de caminhos mínimos).
  * **Fase 2: Solução Inicial Construtiva**
      * Implementação de uma heurística gulosa (Vizinho Mais Próximo) para construir rotas que respeitem a capacidade dos veículos e garantam que cada serviço seja atendido exatamente uma vez.
  * **Fase 3: Métodos de Melhoria**
      * Implementação de um algoritmo de busca local (2-Opt) para otimizar as rotas geradas na Fase 2, visando reduzir o custo total da solução.

## Instruções de Compilação

O projeto utiliza um `Makefile` para facilitar a compilação.

1.  Navegue até o diretório raiz do projeto:
    ```bash
    cd Etapa1-Projeto
    ```
2.  Para limpar quaisquer arquivos de compilação anteriores (objetos e executáveis):
    ```bash
    make clean
    ```
3.  Para compilar o código fonte e criar o executável `solver`:
    ```bash
    make
    ```

## Instruções de Execução

O executável `solver` processará automaticamente todos os arquivos de instância localizados na pasta `./instancias` e salvará as soluções na pasta `./solucoes`.

1.  Certifique-se de que o executável `solver` foi compilado com sucesso (passo anterior).
2.  Execute o programa a partir do diretório raiz do projeto:
    ```bash
    ./solver
    ```

**Estrutura de diretórios esperada:**

```
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
```

## Saída do Programa

O programa gerará saídas detalhadas no console para cada instância processada, incluindo:

  * Informações de depuração sobre a leitura da instância (depósito, capacidade, número de nós e serviços).
  * Tempos de execução para o cálculo de caminhos mínimos (Fase 1), construção da solução inicial (Fase 2) e otimização com 2-Opt (Fase 3).
  * Custo total da solução inicial e final, e a redução de custo obtida.
  * Um arquivo de solução no formato `sol-nome_da_instancia.dat` será criado na pasta `./solucoes`.

As métricas calculadas e utilizadas internamente incluem:

1.  Quantidade de vértices
2.  Quantidade de arestas
3.  Quantidade de arcos
4.  Vértices requeridos
5.  Arestas requeridas
6.  Arcos requeridos
7.  Densidade do grafo
8.  Componentes conexas
9.  Grau mínimo
10. Grau máximo
11. Intermediação dos vértices
12. Caminho médio
13. Diâmetro do grafo

## Colaboradores

  * Ketlyn Sara - [@kethlynsara](https://www.google.com/search?q=https://github.com/kethlynsara)
  * Luana Peixoto - [@luapxb](https://www.google.com/search?q=https://github.com/luapxb)
