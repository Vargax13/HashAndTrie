/**
 * @file hash.c
 * @brief Implementação de uma tabela hash para indexação de palavras em texto.
 *
 * Este arquivo contém a implementação de uma tabela hash que armazena palavras
 * e suas posições de ocorrência em um texto. A tabela utiliza endereçamento
 * aberto com sondagem linear para resolver colisões e redimensiona 
 * automaticamente quando atinge um fator de carga de 0.7.
 *
 * Características principais:
 * - Função hash FNV-1a para melhor distribuição
 * - Sondagem linear para resolução de colisões
 * - Redimensionamento automático
 * - Suporte a múltiplas ocorrências por palavra
 * - Busca case-insensitive
 *
 * Funções principais:
 * - hash_create: Cria uma nova tabela hash
 * - hash_insert: Insere uma palavra e sua posição
 * - hash_search: Busca uma palavra e retorna suas posições
 * - hash_destroy: Libera a memória da tabela
 * - criar_indice_hash: Cria índice a partir de palavras-chave
 * - imprimir_indice_hash: Mostra índice em ordem alfabética
 * - imprimir_estrutura_hash: Visualiza estrutura interna
 * - imprimir_hash_arvore: Visualiza como árvore
 *
 * @see hash.h para definições das estruturas e interfaces
 */
#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* Cria uma nova tabela hash */
HashTable* hash_create(int size) {
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    if (ht == NULL) {
        fprintf(stderr, "Erro de alocação de memória para HashTable\n");
        exit(EXIT_FAILURE);
    }

    ht->size = size;
    ht->entries = 0;
    ht->table = (HashEntry*)calloc(size, sizeof(HashEntry));
    if (ht->table == NULL) {
        free(ht);
        fprintf(stderr, "Erro de alocação de memória para HashEntry\n");
        exit(EXIT_FAILURE);
    }

    // Inicializa todas as entradas
    for (int i = 0; i < size; i++) {
        ht->table[i].word = NULL;
        ht->table[i].occurrences = NULL;
        ht->table[i].num_occurrences = 0;
        ht->table[i].max_occurrences = 0;
    }

    return ht;
}

/* Função de hash melhorada (FNV-1a hash) */
unsigned int hash_function(const char* word, int size) {
    unsigned int hash = 2166136261u;  // Valor inicial (offset) padrão do FNV-1a de 32 bits
    for (; *word; word++) {
        hash ^= tolower(*word);
        hash *= 16777619u;  // Número primo usado pelo FNV-1a para multiplicação
    }
    return hash % size;
}

/* Redimensiona a tabela hash */
int hash_resize(HashTable* ht) {
    int old_size = ht->size;
    HashEntry* old_table = ht->table;
    int new_size = old_size * 2 + 1;
    
    // Criar nova tabela e inicializar todas as entradas
    HashEntry* new_table = (HashEntry*)calloc(new_size, sizeof(HashEntry));
    if (new_table == NULL) return 0;
    
    for (int i = 0; i < new_size; i++) {
        new_table[i].word = NULL;
        new_table[i].occurrences = NULL;
        new_table[i].num_occurrences = 0;
        new_table[i].max_occurrences = 0;
    }
    
    // Reinsere as entradas antigas uma por uma
    for (int i = 0; i < old_size; i++) {
        if (old_table[i].word != NULL) {
            unsigned int index = hash_function(old_table[i].word, new_size);
            unsigned int initial_index = index;
            
            // Encontra nova posição usando sondagem linear
            while (new_table[index].word != NULL) {
                index = (index + 1) % new_size;
                if (index == initial_index) {
                    // Falha na realocação, restaura estado original
                    for (int j = 0; j < new_size; j++) {
                        if (new_table[j].word != NULL) {
                            free(new_table[j].word);
                            free(new_table[j].occurrences);
                        }
                    }
                    free(new_table);
                    return 0;
                }
            }
            
            // Copia a entrada para a nova posição
            new_table[index].word = old_table[i].word;
            new_table[index].occurrences = old_table[i].occurrences;
            new_table[index].num_occurrences = old_table[i].num_occurrences;
            new_table[index].max_occurrences = old_table[i].max_occurrences;
        }
    }
    
    // Atualiza a tabela hash
    free(old_table);  // Libera apenas a tabela, não os dados
    ht->table = new_table;
    ht->size = new_size;
    
    return 1;
}

/* Insere uma palavra na tabela hash */
void hash_insert(HashTable* ht, const char* word, int position) {
    if (!ht || !word) return;
    
    // Redimensiona se necessário
    if (ht->entries > ht->size * 0.7) {
        if (!hash_resize(ht)) {
            fprintf(stderr, "Falha ao redimensionar a tabela hash\n");
            return;
        }
    }

    unsigned int index = hash_function(word, ht->size);
    unsigned int initial_index = index;

    // Busca posição usando sondagem linear
    while (ht->table[index].word != NULL && strcasecmp(ht->table[index].word, word) != 0) {
        index = (index + 1) % ht->size;
        if (index == initial_index) {
            fprintf(stderr, "Tabela hash cheia!\n");
            return;
        }
    }

    // Nova entrada
    if (ht->table[index].word == NULL) {
        ht->table[index].word = strdup(word);
        if (ht->table[index].word == NULL) {
            fprintf(stderr, "Erro de alocação de memória para palavra\n");
            return;
        }

        ht->table[index].max_occurrences = 10;
        ht->table[index].occurrences = (int*)malloc(10 * sizeof(int));
        if (ht->table[index].occurrences == NULL) {
            fprintf(stderr, "Erro de alocação de memória para ocorrências\n");
            free(ht->table[index].word);
            ht->table[index].word = NULL;
            return;
        }
        ht->entries++;
    }

    // Verifica se posição já existe (duplicata)
    for (int i = 0; i < ht->table[index].num_occurrences; i++) {
        if (ht->table[index].occurrences[i] == position) {
            return;  // Não insere duplicatas
        }
    }

    // Expande array se necessário
    if (ht->table[index].num_occurrences >= ht->table[index].max_occurrences) {
        int new_size = ht->table[index].max_occurrences * 2;
        int* new_arr = (int*)realloc(ht->table[index].occurrences, new_size * sizeof(int));
        if (new_arr == NULL) {
            fprintf(stderr, "Erro ao realocar memória para ocorrências\n");
            return;
        }
        ht->table[index].occurrences = new_arr;
        ht->table[index].max_occurrences = new_size;
    }

    // Insere mantendo ordem
    int i = ht->table[index].num_occurrences;
    while (i > 0 && ht->table[index].occurrences[i-1] > position) {
        ht->table[index].occurrences[i] = ht->table[index].occurrences[i-1];
        i--;
    }
    ht->table[index].occurrences[i] = position;
    ht->table[index].num_occurrences++;
}

// Busca uma palavra na tabela hash
int* hash_search(HashTable* ht, const char* word, int* num_occurrences) {
    if (!ht || !word || !num_occurrences) {
        if (num_occurrences) *num_occurrences = 0;
        return NULL;
    }

    unsigned int index = hash_function(word, ht->size);
    unsigned int initial_index = index;

    while (ht->table[index].word != NULL) {
        if (strcasecmp(ht->table[index].word, word) == 0) {
            *num_occurrences = ht->table[index].num_occurrences;
            return ht->table[index].occurrences;
        }
        index = (index + 1) % ht->size;
        if (index == initial_index) break;
    }

    *num_occurrences = 0;
    return NULL;
}

// Cria o índice remissivo usando hash de forma otimizada
int criar_indice_hash(HashTable** ht, char* palavras[], int* posicoes[], int num_palavras, 
                     char keywords[][MAX_WORD_SIZE], int num_keywords) {
    if (*ht != NULL) hash_destroy(*ht);
    
    // Cria tabela com tamanho inicial baseado no número de keywords
    int initial_size = num_keywords * 2;
    if (initial_size < INITIAL_HASH_SIZE) initial_size = INITIAL_HASH_SIZE;
    *ht = hash_create(initial_size);
    
    // Cria hash table auxiliar para keywords para busca O(1)
    HashTable* keyword_ht = hash_create(num_keywords * 2);
    for (int i = 0; i < num_keywords; i++) {
        if (keywords[i][0] != '\0') {  // Verifica se a keyword é válida
            hash_insert(keyword_ht, keywords[i], -1);
        }
    }
    
    // Processa palavras
    for (int i = 0; i < num_palavras; i++) {
        if (palavras[i] && palavras[i][0] != '\0') {  // Verifica se a palavra é válida
            int dummy;
            if (hash_search(keyword_ht, palavras[i], &dummy) != NULL) {
                for (int j = 1; j <= posicoes[i][0]; j++) {
                    hash_insert(*ht, palavras[i], posicoes[i][j]);
                }
            }
        }
    }
    
    hash_destroy(keyword_ht);
    return 1;
}

// Estrutura auxiliar para ordenação
typedef struct {
    char* word;
    int* positions;
    int count;
} WordEntry;

// Função de comparação para qsort
static int compare_entries(const void* a, const void* b) {
    return strcasecmp(((WordEntry*)a)->word, ((WordEntry*)b)->word);
}

// Imprime o índice em ordem alfabética
void imprimir_indice_hash(HashTable* ht) {
    if (ht == NULL) {
        printf("Índice hash não criado.\n");
        return;
    }

    // Obtém as palavras-chave que foram utilizadas para criar o índice
    char (*keywords)[MAX_WORD_SIZE] = get_keywords_hash();
    int num_keywords = get_num_keywords_hash();
    
    // Marca as palavras-chave que forem encontradas
    int* found_keywords = (int*)calloc(num_keywords, sizeof(int));
    if (!found_keywords) {
        printf("Erro ao alocar memória para verificação de palavras-chave.\n");
        return;
    }

    // Aloca array de entradas
    WordEntry* entries = (WordEntry*)malloc(ht->entries * sizeof(WordEntry));
    if (!entries) {
        free(found_keywords);
        return;
    }
    
    // Coleta entradas não nulas
    int idx = 0;
    for (int i = 0; i < ht->size; i++) {
        if (ht->table[i].word != NULL) {
            entries[idx].word = ht->table[i].word;
            entries[idx].positions = ht->table[i].occurrences;
            entries[idx].count = ht->table[i].num_occurrences;
            
            // Marca a palavra-chave como encontrada
            for (int j = 0; j < num_keywords; j++) {
                if (strcasecmp(keywords[j], entries[idx].word) == 0) {
                    found_keywords[j] = 1;
                    break;
                }
            }
            
            idx++;
        }
    }

    // Ordena usando qsort
    qsort(entries, ht->entries, sizeof(WordEntry), compare_entries);

    // Imprime resultado
    printf("\n=== Índice Hash ===\n");
    for (int i = 0; i < idx; i++) {
        printf("%s: ", entries[i].word);
        for (int j = 0; j < entries[i].count; j++) {
            printf("%d", entries[i].positions[j]);
            if (j < entries[i].count - 1) printf(", ");
        }
        printf("\n");
    }
    
    // Imprime as palavras-chave que não foram encontradas
    for (int i = 0; i < num_keywords; i++) {
        if (!found_keywords[i] && keywords[i][0] != '\0') {
            printf("%s: Não foi encontrada no texto.\n", keywords[i]);
        }
    }

    free(entries);
    free(found_keywords);
}

// Função para visualizar a estrutura da tabela hash em formato de árvore
void imprimir_estrutura_hash(HashTable* ht) {
    if (ht == NULL) {
        printf("Tabela hash não criada.\n");
        return;
    }
    
    printf("\n=== Estrutura da Tabela Hash (tamanho: %d) ===\n", ht->size);
    printf("Raiz [%d entradas]\n", ht->entries);
    
    // Contar slots vazios e preenchidos
    int vazios = 0;
    for (int i = 0; i < ht->size; i++) {
        if (ht->table[i].word == NULL) {
            vazios++;
        }
    }
    
    printf("├── Slots preenchidos: %d (%.1f%%)\n", ht->entries, (float)ht->entries / ht->size * 100);
    printf("└── Slots vazios: %d (%.1f%%)\n", vazios, (float)vazios / ht->size * 100);
    
    // Mostrar distribuição de colisões
    int max_colisoes = 0;
    int* colisoes = (int*)calloc(ht->size, sizeof(int));
    
    if (colisoes == NULL) {
        printf("Erro ao alocar memória para análise de colisões.\n");
        return;
    }
    
    // Calcular colisões
    for (int i = 0; i < ht->size; i++) {
        if (ht->table[i].word != NULL) {
            unsigned int pos_ideal = hash_function(ht->table[i].word, ht->size);
            int dist = ((unsigned int)i >= pos_ideal) ? (i - (int)pos_ideal) : (ht->size + i - (int)pos_ideal);
            colisoes[dist]++;
            if (dist > max_colisoes) max_colisoes = dist;
        }
    }
    
    printf("\n=== Análise de Colisões ===\n");
    printf("Posição ideal: %d palavras (%.1f%%)\n", colisoes[0], (float)colisoes[0] / ht->entries * 100);
    
    int total_colisoes = 0;
    for (int i = 1; i <= max_colisoes; i++) {
        if (colisoes[i] > 0) {
            total_colisoes += colisoes[i];
            printf("Deslocamento %d: %d palavras (%.1f%%)\n", 
                   i, colisoes[i], (float)colisoes[i] / ht->entries * 100);
        }
    }
    
    printf("\nTotal de colisões: %d (%.1f%% das entradas)\n", 
           total_colisoes, (float)total_colisoes / ht->entries * 100);
    printf("Fator de carga: %.2f\n", (float)ht->entries / ht->size);
    
    free(colisoes);
    
    // Mostrar amostra da tabela (primeiros 20 slots não vazios)
    printf("\n=== Amostra da Tabela Hash ===\n");
    int mostrados = 0;
    for (int i = 0; i < ht->size && mostrados < 20; i++) {
        if (ht->table[i].word != NULL) {
            unsigned int pos_ideal = hash_function(ht->table[i].word, ht->size);
            int deslocamento = ((unsigned int)i >= pos_ideal) ? (i - (int)pos_ideal) : (ht->size + i - (int)pos_ideal);
            
            printf("[%d] -> %s (hash ideal: %d, deslocamento: %d, ocorrências: %d)\n", 
                   i, ht->table[i].word, pos_ideal, deslocamento, ht->table[i].num_occurrences);
            mostrados++;
        }
    }
    
    if (mostrados == 0) {
        printf("Nenhuma entrada na tabela hash.\n");
    }
}

// Função para visualizar a estrutura da tabela hash como uma árvore
void imprimir_hash_arvore(HashTable* ht) {
    if (ht == NULL) {
        printf("Tabela hash não criada.\n");
        return;
    }
    
    printf("\n=== Representação em Árvore da Tabela Hash ===\n");
    printf("Raiz [%d entradas em %d slots]\n", ht->entries, ht->size);
    
    // Organizar palavras em ordem alfabética para melhor visualização
    WordEntry* entries = (WordEntry*)malloc(ht->entries * sizeof(WordEntry));
    if (!entries) {
        printf("Erro de alocação de memória para visualização em árvore.\n");
        return;
    }
    
    int idx = 0;
    for (int i = 0; i < ht->size; i++) {
        if (ht->table[i].word != NULL) {
            entries[idx].word = ht->table[i].word;
            entries[idx].positions = ht->table[i].occurrences;
            entries[idx].count = ht->table[i].num_occurrences;
            idx++;
        }
    }
    
    // Ordena
    qsort(entries, idx, sizeof(WordEntry), compare_entries);
    
    // Imprime em formato de árvore
    for (int i = 0; i < idx; i++) {
        if (i == idx - 1) {
            printf("└── %s (%d ocorrências)\n", entries[i].word, entries[i].count);
            for (int j = 0; j < entries[i].count; j++) {
                if (j == entries[i].count - 1) {
                    printf("    └── Posição: %d\n", entries[i].positions[j]);
                } else {
                    printf("    ├── Posição: %d\n", entries[i].positions[j]);
                }
            }
        } else {
            printf("├── %s (%d ocorrências)\n", entries[i].word, entries[i].count);
            for (int j = 0; j < entries[i].count; j++) {
                if (j == entries[i].count - 1) {
                    printf("│   └── Posição: %d\n", entries[i].positions[j]);
                } else {
                    printf("│   ├── Posição: %d\n", entries[i].positions[j]);
                }
            }
        }
    }
    
    free(entries);
}

// Destrói a tabela hash
void hash_destroy(HashTable* ht) {
    if (ht == NULL) return;

    for (int i = 0; i < ht->size; i++) {
        if (ht->table[i].word != NULL) {
            free(ht->table[i].word);
            free(ht->table[i].occurrences);
        }
    }

    free(ht->table);
    free(ht);
}