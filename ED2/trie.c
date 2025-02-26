/**
 * @file trie.c
 * @brief Implementação de uma estrutura de dados Trie para indexação de texto com suporte a caracteres acentuados
 * 
 * @author Eduardo Brito
 * @author Eric Cesconetti
 * @author Gabriel Vargas 
 * @author Paulo Albuquerque
 * @date 25/02/2025
 *
 * Esta implementação fornece um sistema de indexação de texto baseado em Trie com os seguintes recursos:
 * - Suporte para caracteres acentuados (conjunto de caracteres Latin-1)
 * - Armazenamento e recuperação de palavras sem distinção entre maiúsculas e minúsculas
 * - Armazenamento das posições das palavras no texto
 * - Preservação da palavra original (mantém acentos na saída)
 * - Otimização baseada em HashMap para buscas de palavras-chave
 * - Recursos de representação visual da árvore
 *
 * Componentes Principais:
 * @struct TrieNode - Estrutura básica do nó da Trie
 * @struct KeywordEntry - Estrutura de entrada para o HashMap de palavras-chave
 * @struct KeywordHashMap - Estrutura HashMap para buscas otimizadas de palavras-chave
 * @struct WordEntry - Estrutura auxiliar para saída ordenada de palavras
 *
 * Funções Principais:
 * - trie_create_node(): Cria um novo nó Trie
 * - trie_insert(): Insere uma palavra com sua posição
 * - trie_search(): Busca uma palavra e retorna suas posições
 * - criar_indice_trie(): Cria um índice Trie a partir de palavras e posições
 * - criar_indice_trie_texto(): Cria um índice Trie diretamente do texto
 * - imprimir_trie_arvore(): Imprime uma representação visual da Trie
 * - imprimir_indice_trie(): Imprime o índice em ordem alfabética
 *
 * Gerenciamento de Memória:
 * - Toda memória alocada dinamicamente é gerenciada com operações apropriadas de liberação
 * - Expansões de array são tratadas automaticamente quando necessário
 * - Limpeza de memória é realizada por trie_destroy()
 *
 * Considerações de Desempenho:
 * - Busca de palavras: O(m) onde m é o comprimento da palavra
 * - Complexidade espacial: O(TAMANHO_ALFABETO * N) onde N é o total de nós
 * - Otimização HashMap: O(1) caso médio para buscas de palavras-chave
 * - Ordenação da lista de palavras: O(n log n) onde n é o número de palavras
 *
 * @note Esta implementação é projetada para lidar com codificação Latin-1
 * @note Todas as palavras são armazenadas em minúsculas mas a forma original é preservada
 * @note A indexação de posição começa em 1
 */
#include "trie.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static char normalize_char(char c) {
    unsigned char uc = (unsigned char)c;
    switch (uc) {
        case 0xE1: case 0xE0: case 0xE2: case 0xE3: case 0xE4: case 0xE5: // á, à, â, ã, ä, å
          return 'a';
        case 0xE9: case 0xE8: case 0xEA: case 0xEB: // é, è, ê, ë
        case 0xC9: case 0xC8: case 0xCA: case 0xCB: // É, È, Ê, Ë
            return 'e';
        case 0xED: case 0xEC: case 0xEE: case 0xEF: // í, ì, î, ï
        case 0xCD: case 0xCC: case 0xCE: case 0xCF: // Í, Ì, Î, Ï
            return 'i';
        case 0xF3: case 0xF2: case 0xF4: case 0xF5: case 0xF6: // ó, ò, ô, õ, ö
        case 0xD3: case 0xD2: case 0xD4: case 0xD5: case 0xD6: // Ó, Ò, Ô, Õ, Ö
            return 'o';
        case 0xFA: case 0xF9: case 0xFC: case 0xFB: // ú, ù, ü, û
        case 0xDA: case 0xD9: case 0xDC: case 0xDB: // Ú, Ù, Ü, Û
            return 'u';
        case 0xE7: // ç
        case 0xC7: // Ç
            return 'c';
        case 0xF1: // ñ
        case 0xD1: // Ñ
            return 'n';
        default:
            return tolower(uc);
    }
}

TrieNode* trie_create_node() {
    TrieNode* node = malloc(sizeof(TrieNode));
    if (!node) {
        fprintf(stderr, "Erro ao criar nó\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 26; i++) node->children[i] = NULL;
    node->is_end_of_word = 0;
    node->occurrences = malloc(10 * sizeof(int));
    node->num_occurrences = 0;
    node->max_occurrences = 10;
    node->original_word = NULL; // Inicializa a palavra original
    return node;
}

void trie_insert(TrieNode* root, const char* word, int position) {
    TrieNode* current = root;
    char original_word[MAX_WORD_SIZE];
    strncpy(original_word, word, MAX_WORD_SIZE - 1);
    original_word[MAX_WORD_SIZE - 1] = '\0'; // Garante terminação nula

    while (*word) {
        char normalized = normalize_char(*word);
        int index = normalized - 'a';
        
        if (index < 0 || index >= 26) {
            word++;
            continue;
        }
        
        if (current->children[index] == NULL) {
            current->children[index] = trie_create_node();
        }
        
        current = current->children[index];
        word++;
    }
    
    current->is_end_of_word = 1;
    
    // Armazena a palavra original (com acentos) no nó final
    if (current->original_word == NULL) {
        current->original_word = strdup(original_word);
        if (!current->original_word) {
            fprintf(stderr, "Erro ao copiar palavra original\n");
            exit(EXIT_FAILURE);
        }
    }

    // Expande o array de ocorrências se necessário
    if (current->num_occurrences >= current->max_occurrences) {
        current->max_occurrences *= 2;
        current->occurrences = (int*)realloc(current->occurrences, 
            current->max_occurrences * sizeof(int));
    }
    
    current->occurrences[current->num_occurrences++] = position;
}

int* trie_search(TrieNode* root, const char* word, int* num_occurrences) {
    TrieNode* current = root;
    
    while (*word) {
        char normalized = normalize_char(*word);
        int index = normalized - 'a';
        
        if (index < 0 || index >= 26 || current->children[index] == NULL) {
            *num_occurrences = 0;
            return NULL;
        }
        
        current = current->children[index];
        word++;
    }
    
    if (current != NULL && current->is_end_of_word) {
        *num_occurrences = current->num_occurrences;
        return current->occurrences;
    }
    
    *num_occurrences = 0;
    return NULL;
}

void _trie_traverse(TrieNode* node, char* prefix, int level,
                  char*** words, int*** positions, int** num_positions, 
                  int* num_words, int* max_words) {
    if (node == NULL) return;
    
    if (node->is_end_of_word) {
        if (*num_words >= *max_words) {
            *max_words *= 2;
            *words = (char**)realloc(*words, *max_words * sizeof(char*));
            *positions = (int**)realloc(*positions, *max_words * sizeof(int*));
            *num_positions = (int*)realloc(*num_positions, *max_words * sizeof(int));
        }
        
        // Usa a palavra original armazenada, não o prefixo normalizado
        (*words)[*num_words] = strdup(node->original_word);
        
        (*positions)[*num_words] = (int*)malloc(node->num_occurrences * sizeof(int));
        memcpy((*positions)[*num_words], node->occurrences, node->num_occurrences * sizeof(int));
        (*num_positions)[*num_words] = node->num_occurrences;
        
        (*num_words)++;
    }
    
    for (int i = 0; i < 26; i++) {
        if (node->children[i]) {
            prefix[level] = 'a' + i;
            prefix[level + 1] = '\0';
            _trie_traverse(node->children[i], prefix, level + 1, 
                         words, positions, num_positions, num_words, max_words);
        }
    }
}

void trie_get_all_words(TrieNode* root, char* prefix, char*** words, int*** positions, 
                      int** num_positions, int* num_words, int* max_words) {
    *num_words = 0;
    _trie_traverse(root, prefix, 0, words, positions, num_positions, num_words, max_words);
}

void trie_destroy(TrieNode* root) {
    if (root == NULL) return;
    
    for (int i = 0; i < 26; i++) {
        if (root->children[i]) {
            trie_destroy(root->children[i]);
        }
    }
    
    free(root->occurrences);
    free(root->original_word); // Libera a palavra original
    free(root);
}

// Verifica se uma palavra deve ser incluída no índice
// Retorna 1 se a palavra deve ser incluída, 0 caso contrário
int should_include_word(const char* word) {
    // Palavras muito curtas (como "a", "e", "o") são conjunções, artigos ou preposições
    // Nesta implementação, incluímos TODAS as palavras, independentemente do tamanho ou função gramatical
    return strlen(word) > 0; // Inclui todas as palavras não vazias
}

// Função para verificar se uma palavra é uma stopword
// Neste caso, como queremos incluir todas as palavras, esta função sempre retorna 0
int is_stopword(const char* word) {
    // No caso de querer filtrar stop words, poderíamos adicionar:
    // Lista de palavras muito comuns como "e", "ou", "mas", "pois", etc.
    // return (strcmp(word, "e") == 0 || strcmp(word, "de") == 0 ... );
    
    // Mas como queremos TODAS as palavras:
    (void)word; // Silencia o warning de parâmetro não utilizado
    return 0; // Nenhuma palavra é considerada stopword
}

// Nova função de busca binária para encontrar palavras em um array ordenado
// Retorna o índice se encontrar, -1 caso contrário
int binary_search_word(char* palavras[], int num_palavras, const char* palavra) {
    int left = 0;
    int right = num_palavras - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcasecmp(palavras[mid], palavra);
        
        if (cmp == 0) {
            return mid; // Encontrou a palavra
        }
        
        if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return -1; // Não encontrou
}

// Função auxiliar para ordenar um array de palavras e manter os índices de posições
void sort_palavras_com_posicoes(char* palavras[], int* posicoes[], int num_palavras) {
    // Cria um array temporário para armazenar os índices originais
    typedef struct {
        char* palavra;
        int* posicoes;
    } PalavraComPosicao;
    
    PalavraComPosicao* temp = malloc(num_palavras * sizeof(PalavraComPosicao));
    if (!temp) {
        fprintf(stderr, "Erro ao alocar memória para ordenação\n");
        return;
    }
    
    // Preenche o array temporário
    for (int i = 0; i < num_palavras; i++) {
        temp[i].palavra = palavras[i];
        temp[i].posicoes = posicoes[i];
    }
    
    // Algoritmo de ordenação simples (insertion sort) - O(n²) no pior caso
    // Para manter complexidade melhor, poderíamos implementar um quicksort ou mergesort
    for (int i = 1; i < num_palavras; i++) {
        PalavraComPosicao key = temp[i];
        int j = i - 1;
        
        while (j >= 0 && strcasecmp(temp[j].palavra, key.palavra) > 0) {
            temp[j + 1] = temp[j];
            j = j - 1;
        }
        temp[j + 1] = key;
    }
    
    // Copia de volta para os arrays originais
    for (int i = 0; i < num_palavras; i++) {
        palavras[i] = temp[i].palavra;
        posicoes[i] = temp[i].posicoes;
    }
    
    free(temp);
}

// Versão sem HashMap para criar_indice_trie
int criar_indice_trie(TrieNode** root, char* palavras[], int* posicoes[], int num_palavras, 
                    char keywords[][MAX_WORD_SIZE], int num_keywords) {
    if (*root == NULL) {
        *root = trie_create_node();
    } else {
        trie_destroy(*root);
        *root = trie_create_node();
    }

    // Ordenar o array de palavras para usar busca binária - O(n log n)
    sort_palavras_com_posicoes(palavras, posicoes, num_palavras);
    
    // Processar as palavras-chave - O(k log n), onde k é o num de keywords
    for (int i = 0; i < num_keywords; i++) {
        int j = binary_search_word(palavras, num_palavras, keywords[i]);
        if (j >= 0) {
            // Palavra-chave encontrada
            for (int k = 1; k <= posicoes[j][0]; k++) {
                trie_insert(*root, keywords[i], posicoes[j][k]);
            }
        }
    }
    
    return 1;
}

// Função auxiliar para tokenizar texto em palavras
int tokenize_text(const char* text, char*** words, int*** positions) {
    int max_words = 1000;  // Tamanho inicial
    *words = (char**)malloc(max_words * sizeof(char*));
    *positions = (int**)malloc(max_words * sizeof(int*));
    
    if (!*words || !*positions) {
        fprintf(stderr, "Erro ao alocar memória para tokenização\n");
        return 0;
    }
    
    int word_count = 0;
    int pos = 1;  // Começamos na posição 1
    
    const char* start = text;
    int in_word = 0;
    
    while (*text) {
        char c = *text;
        
        // Verifica se é um caractere de palavra
        if (isalnum((unsigned char)c) || c == '-' || ((unsigned char)c > 127)) {
            if (!in_word) {
                start = text;
                in_word = 1;
            }
        } else {
            if (in_word) {
                // Fim da palavra
                int len = text - start;
                if (len > 0) {
                    // Aumenta o array se necessário
                    if (word_count >= max_words) {
                        max_words *= 2;
                        *words = (char**)realloc(*words, max_words * sizeof(char*));
                        *positions = (int**)realloc(*positions, max_words * sizeof(int*));
                        
                        if (!*words || !*positions) {
                            fprintf(stderr, "Erro ao realocar memória para tokenização\n");
                            return 0;
                        }
                    }
                    
                    // Aloca palavra
                    (*words)[word_count] = (char*)malloc((len + 1) * sizeof(char));
                    strncpy((*words)[word_count], start, len);
                    (*words)[word_count][len] = '\0';
                    
                    // Aloca posição (posicoes[0] guarda o número de posições)
                    (*positions)[word_count] = (int*)malloc(2 * sizeof(int));
                    (*positions)[word_count][0] = 1;  // Número de posições
                    (*positions)[word_count][1] = pos;  // A posição
                    
                    word_count++;
                    pos++;  // Incrementa posição para a próxima palavra
                }
                in_word = 0;
            }
        }
        
        text++;
    }
    
    // Verifica se acabou em uma palavra
    if (in_word) {
        int len = text - start;
        if (len > 0) {
            if (word_count >= max_words) {
                max_words *= 2;
                *words = (char**)realloc(*words, max_words * sizeof(char*));
                *positions = (int**)realloc(*positions, max_words * sizeof(int*));
            }
            
            (*words)[word_count] = (char*)malloc((len + 1) * sizeof(char));
            strncpy((*words)[word_count], start, len);
            (*words)[word_count][len] = '\0';
            
            (*positions)[word_count] = (int*)malloc(2 * sizeof(int));
            (*positions)[word_count][0] = 1;
            (*positions)[word_count][1] = pos;
            
            word_count++;
        }
    }
    
    return word_count;
}

// Nova função para processar texto diretamente
int criar_indice_trie_texto(TrieNode** root, const char* texto, char keywords[][MAX_WORD_SIZE], int num_keywords) {
    char** palavras = NULL;
    int** posicoes = NULL;
    
    int num_palavras = tokenize_text(texto, &palavras, &posicoes);
    if (num_palavras == 0) return 0;
    
    int resultado = criar_indice_trie(root, palavras, posicoes, num_palavras, keywords, num_keywords);
    
    // Libera memória alocada
    for (int i = 0; i < num_palavras; i++) {
        free(palavras[i]);
        free(posicoes[i]);
    }
    free(palavras);
    free(posicoes);
    
    return resultado;
}

// Nova função para representação visual da Trie
void imprimir_trie_arvore_recursivo(TrieNode* node, char* prefix, int is_last, char* path, int level) {
    if (node == NULL) return;
    
    // Imprime o prefixo atual
    printf("%s", prefix);
    
    // Imprime a conexão ├── ou └── 
    if (is_last) {
        printf("└──");
        // Atualiza o prefixo para o próximo nível
        strcat(prefix, "    ");
    } else {
        printf("├──");
        // Atualiza o prefixo para o próximo nível
        strcat(prefix, "│   ");
    }
    
    // Imprime a letra atual (se não for raiz)
    if (level > 0) {
        char letra = path[level-1];
        printf(" %c", letra);
    } else {
        printf(" [Raiz]");
    }
    
    // Se for fim de palavra, imprime a palavra completa e suas ocorrências
    if (node->is_end_of_word) {
        printf(" -> %s (", node->original_word);
        for (int i = 0; i < node->num_occurrences; i++) {
            printf("%d", node->occurrences[i]);
            if (i < node->num_occurrences - 1) printf(", ");
        }
        printf(")");
    }
    printf("\n");
    
    // Conta quantos filhos o nó tem para determinar qual é o último
    int count_children = 0;
    for (int i = 0; i < 26; i++) {
        if (node->children[i] != NULL) {
            count_children++;
        }
    }
    
    // Percorre os filhos
    int current_child = 0;
    for (int i = 0; i < 26; i++) {
        if (node->children[i] != NULL) {
            // Adiciona a letra atual ao caminho
            path[level] = 'a' + i;
            path[level + 1] = '\0';
            
            // Chama recursivamente para este filho
            imprimir_trie_arvore_recursivo(
                node->children[i], 
                prefix, 
                ++current_child == count_children, 
                path, 
                level + 1
            );
        }
    }
    
    // Restaura o prefixo original ao retornar
    prefix[strlen(prefix) - 4] = '\0';
}

void imprimir_trie_arvore(TrieNode* root) {
    if (root == NULL) {
        printf("Árvore Trie não foi criada.\n");
        return;
    }
    
    printf("\n=== Representação em Árvore da Trie ===\n");
    char prefix[1000] = "";
    char path[MAX_WORD_SIZE] = "";
    imprimir_trie_arvore_recursivo(root, prefix, 1, path, 0);
}

// Função de comparação para qsort
int compare_words(const void* a, const void* b) {
    return strcasecmp(*(const char**)a, *(const char**)b);
}

// Estrutura auxiliar para ordenação
typedef struct {
    char* word;
    int* positions;
    int num_positions;
} WordEntry;

// Função de comparação para qsort com estruturas
int compare_word_entries(const void* a, const void* b) {
    const WordEntry* entry_a = (const WordEntry*)a;
    const WordEntry* entry_b = (const WordEntry*)b;
    return strcasecmp(entry_a->word, entry_b->word);
}

// Função auxiliar para buscar palavra em uma lista de palavras
// Complexidade O(n), mas poderia ser melhorada com busca binária se a lista estiver ordenada
int find_word_in_array(char** words, int num_words, const char* word) {
    // Presume que words[] esteja ordenado alfabeticamente
    return binary_search_word(words, num_words, word);
}

void imprimir_indice_trie(TrieNode* root) {
    if (root == NULL) {
        printf("Índice trie não foi criado.\n");
        return;
    }
    
    printf("\n=== Índice Trie ===\n");
    
    char prefix[MAX_WORD_SIZE] = "";
    char** words = (char**)malloc(10 * sizeof(char*));
    int** positions = (int**)malloc(10 * sizeof(int*));
    int* num_positions = (int*)malloc(10 * sizeof(int));
    int num_words = 0;
    int max_words = 10;
    
    trie_get_all_words(root, prefix, &words, &positions, &num_positions, &num_words, &max_words);
    
    // Cria array de estruturas para ordenação mais eficiente
    WordEntry* entries = (WordEntry*)malloc(num_words * sizeof(WordEntry));
    if (!entries) {
        printf("Erro ao alocar memória para ordenação.\n");
        
        // Liberação de memória
        for (int i = 0; i < num_words; i++) {
            free(words[i]);
            free(positions[i]);
        }
        free(words);
        free(positions);
        free(num_positions);
        return;
    }
    
    // Preenche estruturas
    for (int i = 0; i < num_words; i++) {
        entries[i].word = words[i];
        entries[i].positions = positions[i];
        entries[i].num_positions = num_positions[i];
    }
    
    // Ordena com qsort (complexidade n*log(n))
    qsort(entries, num_words, sizeof(WordEntry), compare_word_entries);
    
    // Cria um array auxiliar com apenas as palavras para busca binária
    char** sorted_words = (char**)malloc(num_words * sizeof(char*));
    if (!sorted_words) {
        printf("Erro ao alocar memória para array de palavras ordenadas.\n");
        // Liberação de memória
        for (int i = 0; i < num_words; i++) {
            free(entries[i].word);
            free(entries[i].positions);
        }
        free(entries);
        free(num_positions);
        free(words);
        free(positions);
        return;
    }
    
    // Preenche o array auxiliar com palavras ordenadas
    for (int i = 0; i < num_words; i++) {
        sorted_words[i] = entries[i].word;
    }
    
    // Obtém as palavras-chave que foram utilizadas para criar o índice
    char (*keywords)[MAX_WORD_SIZE] = get_keywords_trie();
    int num_keywords = get_num_keywords_trie();
    
    // Imprime as palavras em ordem alfabética
    for (int i = 0; i < num_words; i++) {
        printf("%s: ", entries[i].word);
        for (int j = 0; j < entries[i].num_positions; j++) {
            printf("%d", entries[i].positions[j]);
            if (j < entries[i].num_positions - 1) printf(", ");
        }
        printf("\n");
    }
    
    // Verifica quais palavras-chave não foram encontradas usando busca binária - O(k log n)
    if (num_keywords > 0) {
        for (int i = 0; i < num_keywords; i++) {
            // Usa busca binária em vez de busca linear
            int found_index = binary_search_word(sorted_words, num_words, keywords[i]);
            if (found_index == -1) {
                printf("%s: Não foi encontrada no texto.\n", keywords[i]);
            }
        }
    }
    
    // Liberação de memória
    free(sorted_words);
    for (int i = 0; i < num_words; i++) {
        free(entries[i].word);
        free(entries[i].positions);
    }
    free(entries);
    free(num_positions);
    free(words);
    free(positions);
}