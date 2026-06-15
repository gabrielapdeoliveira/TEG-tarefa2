# Tarefa 2 - Teoria dos Grafos

Repositório desenvolvido para a disciplina de Teoria dos Grafos.

## Descrição

O objetivo do trabalho é construir um grafo não direcionado ponderado a partir de uma base de palavras em português com 4 letras.

Cada palavra da base representa um vértice do grafo. Duas palavras são conectadas por uma aresta quando diferem em exatamente uma posição de caractere.

Exemplo:

```text
MATA -> MALA
```

Nesse caso, as palavras diferem apenas na terceira letra, portanto existe uma aresta entre elas.

## Arquivos do projeto

```text
tarefa2.c
palavras_filtradas_4letras.txt
```

## O que foi implementado

Nesta versão, o programa realiza:

* leitura da base de palavras;
* criação de um vértice para cada palavra válida de 4 letras;
* normalização das palavras para letras minúsculas;
* criação automática das arestas entre palavras que diferem em exatamente uma letra;
* cálculo do peso das arestas usando distância de Jaccard;
* verificação inicial se o grafo é simples ou multigrafo;
* exibição de um resumo inicial do grafo.

## Resultado inicial obtido

Com a base de palavras utilizada, o programa carregou:

```text
Vértices carregados: 1590
Arestas criadas: 7249
Tipo: Grafo simples
```

## Como compilar e executar

### Windows

No terminal, dentro da pasta do projeto, compile com:

```bash
gcc tarefa2.c -o tarefa2
```

Execute com:

```bash
.\tarefa2.exe
```

Caso queira informar o arquivo de palavras manualmente:

```bash
.\tarefa2.exe palavras_filtradas_4letras.txt
```

### Linux

No terminal, dentro da pasta do projeto, compile com:

```bash
gcc tarefa2.c -o tarefa2
```

Execute com:

```bash
./tarefa2
```

Caso queira informar o arquivo de palavras manualmente:

```bash
./tarefa2 palavras_filtradas_4letras.txt
```

## Observação

O arquivo `palavras_filtradas_4letras.txt` deve estar na mesma pasta do código-fonte para que o programa consiga carregar a base de palavras corretamente.

## Próximas etapas

As próximas partes do trabalho envolvem:

* encontrar os componentes conexos do grafo;
* informar o tamanho de cada componente;
* identificar a palavra de maior grau e a palavra de menor grau em cada componente;
* implementar o algoritmo de Dijkstra para ligar duas palavras fornecidas pelo usuário.
