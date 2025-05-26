# Analisador de Grafos

## Descrição

Este projeto implementa um analisador de grafos para instâncias de problemas de roteamento. O programa calcula diversas métricas de grafos, incluindo conectividade, graus dos vértices, caminhos mínimos e outras propriedades relevantes.

## Instruções de Compilação
1. Navegue até o diretório do projeto:
```bash
cd ETAPA1-PROJETO
```
2. Compile o código fonte:
```bash
g++ -o etapa1_cod etapa1cod.cpp 
```

## Instruções de Execução
Forma básica:
```bash
./etapa1_cod instancias/nome_da_instancia.dat
```

Exemplos:
```bash
./etapa1_cod instancias/grafo_simples.dat
./etapa1_cod ../instancias/grafo_complexo.dat
```

Opções disponíveis:
```bash
./etapai_cod -h       # Mostra ajuda
./etapai_cod --help   # Mostra ajuda
```

## Saída do Programa
O programa gera um relatório com 13 métricas do grafo:

    1. Quantidade de vértices

    2. Quantidade de arestas

    3. Quantidade de arcos

    4. Vértices requeridos

    5. Arestas requeridas

    6. Arcos requeridos

    7. Densidade do grafo

    8. Componentes conectados

    9. Grau mínimo

    10. Grau máximo

    11. Intermediação dos vértices

    12. Caminho médio

    13. Diâmetro do grafo

## Colaboradores
- Ketlyn Sara - @kethlynsara
- Luana Peixoto - @luapxb