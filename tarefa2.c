#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>

#define TAM_PALAVRA 4
#define MAX_LINHA 100

//estrutura para representar uma aresta saindo de um vértice
typedef struct Vizinho {
    int id;
    double peso; // além do id do destino também guardamos o peso
    struct Vizinho* prox;
} Vizinho;

//estrutura para cada vértice, que representa uma palavra da base de palavras
typedef struct Vertice {
    int id;
    char palavra[TAM_PALAVRA + 1]; // +1 para o caractere nulo
    int grau;
    Vizinho* lista; 
    struct Vertice* prox;
} Vertice;

//estrtura para representar o grafo, que é uma lista encadeada de vértices
typedef struct Grafo {
    Vertice* vertices;
    Vertice** indice; //para acessar vértices pelo id sem percorrer a lista
    int capacidade;
    int n_vertices;
    int n_arestas;
    int n_lacos;
    int n_multiplas;
} Grafo;

//função para criar um grafo vazio
Grafo* criar_grafo() {
    Grafo* g = (Grafo*) malloc(sizeof(Grafo));

    if (g == NULL) {
        printf("Erro de memoria.\n");
        exit(1);
    }

    g->vertices = NULL;

    /*o índice é um vetor que aponta diretamente para os 
    vértices do grafo, permitindo acesso rápido por id.*/
 
    g->capacidade = 64; // capacidade inicial que pode ser aumentada automaticamente se necessário
    g->indice = (Vertice**) malloc(g->capacidade * sizeof(Vertice*));

    if (g->indice == NULL) {
        printf("Erro de memoria.\n");
        exit(1);
    }
    //contadores para inicializar o grafo
    g->n_vertices = 0; 
    g->n_arestas = 0;
    g->n_lacos = 0;
    g->n_multiplas = 0;

    return g;
}

//função para liberar a memória alocada para o grafo
void liberar_grafo(Grafo* g) {
    Vertice* v = g->vertices;

    while (v != NULL) {
        Vizinho* viz = v->lista; //libera a lista de vizinhos de cada vértice

        while (viz != NULL) {
            Vizinho* aux = viz; //guarda o vizinho atual para liberar depois
            viz = viz->prox; //avança para o próximo vizinho antes de liberar o atual
            free(aux);
        }

        Vertice* aux = v;
        v = v->prox; //libera a lista de vértices do grafo
        free(aux);
    }

    free(g->indice); //libera o índice do grafo
    free(g);
}

//função para evitar percorrer a lista encadeada inteira toda vez
Vertice* buscar_por_id(Grafo* g, int id) {
    if (id < 0 || id >= g->n_vertices) {
        return NULL;
    }

    return g->indice[id];
}

/*busca para usar na leitura do arquivo 
com o objetivo de evitar vértices repetidos*/

Vertice* buscar_por_palavra(Grafo* g, char palavra[]) {
    Vertice* atual = g->vertices;

    while (atual != NULL) {
        if (strcmp(atual->palavra, palavra) == 0) {
            return atual;
        }

        atual = atual->prox;
    }

    return NULL;
}

//coloca a palavra em letras minúsculas antes de inserir no grafo
void normalizar_palavra(char palavra[]) {
    for (int i = 0; palavra[i] != '\0'; i++) {
        palavra[i] = tolower((char) palavra[i]);
    }
}

//função para inserir uma nova palavra como vértice no grafo
//a palavra só é inserida se ainda não existir no grafo, caso contrário retorna NULL

Vertice* inserir_v(Grafo* g, char palavra[]) {
    if (buscar_por_palavra(g, palavra) != NULL) {
        return NULL;
    }

    //verifica se o número de vértices atingiu a capacidade do índice, se sim, dobra a capacidade

    if (g->n_vertices == g->capacidade) {
        g->capacidade *= 2;
        g->indice = (Vertice**) realloc(g->indice, g->capacidade * sizeof(Vertice*));

        if (g->indice == NULL) {
            printf("Erro de memoria.\n");
            exit(1);
        }
    }

    Vertice* novo = (Vertice*) malloc(sizeof(Vertice));

    if (novo == NULL) {
        printf("Erro de memoria.\n");
        exit(1);
    }

    /*inicializa o novo vértice com a palavra 
    e o id correspondente ao num. atual de vértices do grafo*/
    novo->id = g->n_vertices;
    strcpy(novo->palavra, palavra);
    novo->grau = 0;
    novo->lista = NULL;

    //aqui o vértice entra no início da lista encadeada
    novo->prox = g->vertices;
    g->vertices = novo;

    /*  além de entrar na lista, o vértice também é salvo no índice
    para recuperar o vértice diretamente pelo id depois.*/

    g->indice[novo->id] = novo;
    g->n_vertices++;
    return novo;
}
    /*função p verificar se uma aresta p determinado 
    destino já existe, além de ser importante p evitar arestas múltiplas*/
int ja_tem_vizinho(Vertice* v, int destino) {
    Vizinho* atual = v->lista;

    while (atual != NULL) {
        if (atual->id == destino) {
            return 1;
        }

        atual = atual->prox;
    }

    return 0;
}

//adiciona um vizinho na lista de adjacência de um vértice
void add_vizinho(Vertice* v, int destino, double peso) {
    Vizinho* novo = (Vizinho*) malloc(sizeof(Vizinho));

    if (novo == NULL) {
        printf("Erro de memoria.\n");
        exit(1);
    }

    novo->id = destino;
    novo->peso = peso;

    novo->prox = v->lista;
    v->lista = novo;

    v->grau++;
}

// adiciona uma aresta não direcionada entre dois vértices
void add_aresta(Grafo* g, int origem, int destino, double peso) {
    Vertice* v_origem = buscar_por_id(g, origem);
    Vertice* v_destino = buscar_por_id(g, destino);

    if (v_origem == NULL || v_destino == NULL) {
        return;
    }

    if (origem == destino) {
        g->n_lacos++;
        return;
    }
    //se a ligação já existe, ela é contada como aresta múltipla

    if (ja_tem_vizinho(v_origem, destino)) {
        g->n_multiplas++;
        return;
    }

    add_vizinho(v_origem, destino, peso);
    add_vizinho(v_destino, origem, peso);

    g->n_arestas++;
}

//função para verificar se duas palavras diferem por exatamente uma letra
int diferem_por_uma_letra(char p1[], char p2[]) {
    int diferencas = 0;

    for (int i = 0; i < TAM_PALAVRA; i++) {
        if (p1[i] != p2[i]) { //compara as letras das duas palavras, contando as diferenças
            diferencas++;

            if (diferencas > 1) { //se já tiver mais de uma diferença, retorna falso imediatamente
                return 0;
            }
        }
    }

    return diferencas == 1;
}

//calcula a distância de Jaccard entre os conjuntos de caracteres das palavras

double calcular_peso_jaccard(char p1[], char p2[]) {
    char u1[TAM_PALAVRA];
    char u2[TAM_PALAVRA];

    int n1 = 0;
    int n2 = 0;
    int inter = 0;

    //função p montar os conjuntos de caracteres únicos de cada palavra
    //OBS: as palavras têm tamanho 4, logo os vetores u1 e u2 possuem no máximo 4 posições

    for (int i = 0; i < TAM_PALAVRA; i++) {
        int achou = 0;

        for (int j = 0; j < n1; j++) {
            if (u1[j] == p1[i]) {
                achou = 1;
                break;
            }
        }

        if (!achou) {
            u1[n1++] = p1[i];
        }

        achou = 0;

        for (int j = 0; j < n2; j++) {
            if (u2[j] == p2[i]) {
                achou = 1;
                break;
            }
        }

        if (!achou) {
            u2[n2++] = p2[i];
        }
    }
    //conta a interseção dos conjuntos de caracteres únicos das duas palavras
    for (int i = 0; i < n1; i++) {
        for (int j = 0; j < n2; j++) {
            if (u1[i] == u2[j]) {
                inter++;
                break;
            }
        }
    }
    //calcula a união dos conjuntos de caracteres únicos das duas palavras
    int uniao = n1 + n2 - inter;

    if (uniao == 0) {
        return 0.0;
    }

    return (double)(uniao - inter) / uniao;
}

//le o arquivo txt e cria um vértice para cada palavra 
int carregar_palavras(Grafo* g, const char* nome_arquivo) {
    FILE* f = fopen(nome_arquivo, "r");

    if (f == NULL) {
        return -1;
    }

    char palavra[MAX_LINHA];
    int carregadas = 0;

    while (fscanf(f, "%99s", palavra) == 1) {
        normalizar_palavra(palavra);

        if (strlen(palavra) == TAM_PALAVRA) {
            if (inserir_v(g, palavra) != NULL) {
                carregadas++;
            }
        }
    }

    fclose(f);
    return carregadas;
}

/*gera as arestas do grafo comparando as palavras carregadas. como
o arquivo possui apenas palavras, o programa compara as palavras
duas a duas para descobrir quais vértices devem ser conectados.*/
void criar_arestas(Grafo* g) {
    Vertice* v1 = g->vertices;

    while (v1 != NULL) {
        Vertice* v2 = v1->prox; //para evitar comparação com a mesma palavra e comparações repetidas

        while (v2 != NULL) {
            if (diferem_por_uma_letra(v1->palavra, v2->palavra)) {
                double peso = calcular_peso_jaccard(v1->palavra, v2->palavra); //calcula o peso da aresta com base na distância de jaccard entre as palavras
                add_aresta(g, v1->id, v2->id, peso); 
            }

            v2 = v2->prox; //avança para a próxima palavra para comparação
        }

        v1 = v1->prox; 
    }
}

/* teste simples para verificar o funcionamento do programa */
void mostrar_resumo_basico(Grafo* g) {
    printf("\n--- RESUMO INICIAL DO GRAFO ---\n");
    printf("Vertices carregados: %d\n", g->n_vertices);
    printf("Arestas criadas: %d\n", g->n_arestas);

    if (g->n_lacos == 0 && g->n_multiplas == 0) {
        printf("Tipo: Grafo simples\n");
    } else {
        printf("Tipo: Multigrafo\n");
        printf("Lacos: %d\n", g->n_lacos);
        printf("Arestas multiplas: %d\n", g->n_multiplas);
    }
}

void dijkstra(Grafo* g, char str_origem[], char str_destino[]){
    //Normaliza as palavras de entrada para bater com o padrao
    normalizar_palavra(str_origem);
    normalizar_palavra(str_destino);

    Vertice* v_origem = buscar_por_palavra(g, str_origem);
    Vertice* v_destino = buscar_por_palavra(g, str_destino);

    if (!v_origem || !v_destino){
        printf("\nErro: Palavra de origem ou destino nao existe no grafo.\n");
        return;
    }

    int n = g->n_vertices;
    double* dist = (double*)malloc(n * sizeof(double));
    int* ant = (int*)malloc(n * sizeof(int));
    int* visitado = (int*)calloc(n, sizeof(int));

    for (int i = 0; i < n; i++){
        dist[i] = DBL_MAX;
        ant[i] = -1; // -1 significa que nao tem antecessor
    }

    dist[v_origem->id] = 0.0;

    for (int count = 0; count < n; count++){
        double min = DBL_MAX;
        int u = -1;

        //encontra o vertice nao visitado com a menor distancia
        for (int i = 0; i < n; i++){
            if (!visitado[i] && dist[i] <= min){
                min = dist[i];
                u = i;
            }
        }

        //se o menor for infinito ou se chegarmos no destino podemos parar
        if (u == -1 || u == v_destino->id) break;

        visitado[u] = -1;
        Vertice* vert_u = g->indice[u];
        Vizinho* viz = vert_u->lista;

        //relaxamento das arestas
        while (viz != NULL){
            int v = viz->id;
            if (!visitado[v] && dist[u] != DBL_MAX && dist[u] + viz->peso < dist[v]){
                dist[v] = dist[u] + viz->peso;
                ant[v] = u;
            }
            viz = viz->prox;
        }
    }

    printf("\n--- CAMINHO MINIMO (DIJKSTRA)---\n");
    if (dist[v_destino->id] == DBL_MAX){
        printf("Nao existe caminho entre '%s' e '%s'.\n", str_origem, str_destino);
    }else{
        //reconstroi o caminho de tras para a frente usando o vetor de antecessores (ant)
        int*caminho = (int*)malloc(n*sizeof(int));
        int tam_caminho = 0;
        int atual = v_destino->id;

        while (atual != -1){
            caminho[tam_caminho++] = atual;
            atual = ant[atual];
        }

        printf("Caminho de '%s' para '%s':\n", str_origem, str_destino);
        for (int i = tam_caminho - 1; i >= 0; i--){
            printf("%s", g->indice[caminho[i]]->palavra);
            if (i > 0) printf (" -> ");
        }
        printf("\nCusto total (Distancia Jaccard): %.4f\n", dist[v_destino->id]);
        free(caminho);
    }

    free(dist);
    free(ant);
    free(visitado);
}

//funcao que verificar o tipo de grafo
void verificar_tipo_grafo(Grafo* g){
    printf("\n--- TIPO DE GRAFO ---\n");
    if (g->n_lacos == 0 && g->n_multiplas == 0){
        printf("O grafo eh SIMPLES.\n");
    }else {
        printf("O grafo eh MULTIGRAFO.\n");
        if (g->n_lacos > 0) printf(" -> Ocorrencias de lacos: %d\n", g->n_lacos);
        if (g->n_multiplas > 0) printf(" -> Ocorrencias de arestas multiplas: %d\n", g->n_multiplas);
    }
}

//funcao auxiliar de busca em profundidade (dfs)
void dfs (Grafo* g, int u, int visitado[], int* tamanho, Vertice** v_max, Vertice** v_min){
    visitado[u] = 1;
    (*tamanho)++; //vai contar mais vertice
    Vertice* v = g->indice[u];

    //atualiza os vertices de maior e menor grau do componente
    if (*v_max == NULL || v->grau > (*v_max)->grau){
        *v_max = v;
    }
    if (*v_min == NULL || v->grau < (*v_min)->grau){
        *v_min = v;
    }
    
    Vizinho* viz = v->lista;
    while (viz != NULL){
        if (!visitado[viz->id]){
            dfs(g, viz->id, visitado, tamanho, v_max, v_min);
        }
        viz = viz->prox;
    }
}

//funcao principal para analisar os componentes
void analisar_componentes (Grafo* g){
    int* visitado = (int*)calloc(g->n_vertices, sizeof(int));
    int num_componentes = 0;

    FILE* arq = fopen("componentes.csv", "w");

    if (arq != NULL) { 
        fprintf(arq, "componente,tamanho,palavra_central,grau_max,palavra_menor_grau,grau_min\n");
    }

    printf("\n--- COMPONENTES CONEXOS ---\n");
    for (int i = 0; i < g->n_vertices; i++){
        if (!visitado[i]){
            num_componentes++;
            int tamanho = 0;
            Vertice* v_max = NULL;
            Vertice* v_min = NULL;

            //inicia a busca para mapear todo o componente conhecido a "i"
            dfs(g,i, visitado, &tamanho, &v_max, &v_min);

            printf("Componente %d: Tamanho = %d\n", num_componentes, tamanho);
            if (v_max) printf(" -> Palavra Central (Grau Max): %s (Grau %d)\n", v_max->palavra, v_max->grau);
            if (v_min) printf(" -> Menos Conexoes  (Grau Min): %s (Grau %d)\n", v_min->palavra, v_min->grau);
    
            if (arq != NULL) { // NOVO
                fprintf(
                    arq,
                    "%d,%d,%s,%d,%s,%d\n",
                    num_componentes,
                    tamanho,
                    v_max->palavra,
                    v_max->grau,
                    v_min->palavra,
                    v_min->grau
                );
            }
        }
    }

    
    printf("Total de Componentes Conexos: %d\n", num_componentes);

      if (arq != NULL) { 
        fclose(arq);
    }
    free(visitado);
}

void exportar_graus(Grafo* g) {
    FILE* arquivo = fopen("graus.csv", "w");

    if (arquivo == NULL) {
        printf("Erro ao criar graus.csv\n");
        return;
    }

    fprintf(arquivo, "palavra,grau\n");

    Vertice* atual = g->vertices;

    while (atual != NULL) {
        fprintf(arquivo, "%s,%d\n", atual->palavra, atual->grau);
        atual = atual->prox;
    }

    fclose(arquivo);

    printf("\nArquivo graus.csv exportado com sucesso!\n");
}

int main(int argc, char *argv[]) {
    Grafo* g = criar_grafo();

    int carregadas = -1;

    if (argc > 1) {
        carregadas = carregar_palavras(g, argv[1]);
    } else {
        carregadas = carregar_palavras(g, "palavras_filtradas_4letras.txt");
    }

    if (carregadas <= 0) {
        printf("nao foi possivel carregar a base de palavras\n");
        printf("verifique se o arquivo esta na mesma pasta do codigo\n");

        liberar_grafo(g);
        return 1;
    }

    //criar as arestas
    criar_arestas(g);
    mostrar_resumo_basico(g);

    //analise do grafo
    verificar_tipo_grafo(g);

    //analise de componentes conexos
    analisar_componentes(g);

    //exportar os graus dos vertices para um arquivo CSV
    exportar_graus(g);

    //teste de dijkstra
    char origem[20], destino[20];
    printf("\nDigite a palavra de origem (4 letras): ");
    scanf("%19s", origem);
    printf("\nDigite a palavra de destino (4 letras): ");
    scanf("%19s", destino);

    dijkstra(g, origem, destino);
    
    liberar_grafo(g);
    return 0;
}