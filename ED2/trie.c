/**
 * @file trie.c
 * @brief Implementation of a Trie data structure for text indexing with UTF-8 support
 *
 * This implementation provides a Trie-based text indexing system that:
 * - Supports UTF-8 encoded text
 * - Handles accented characters (normalizing them to their base form)
 * - Maintains word positions in the original text
 * - Supports hyphens in words
 * - Preserves original word forms while using normalized forms for searching
 *
 * Key Features:
 * - Character normalization for accent-insensitive searching
 * - UTF-8 character handling
 * - Position tracking for word occurrences
 * - Tree visualization capabilities
 * - Case-insensitive word matching
 * - Support for special characters (like hyphens)
 *
 * Main Functions:
 * - trie_create_node(): Creates a new Trie node
 * - trie_insert(): Inserts a word with its position
 * - trie_search(): Searches for a word and returns its positions
 * - criar_indice_trie(): Creates an index from text and keywords
 * - imprimir_trie_arvore(): Visualizes the Trie structure
 * - imprimir_indice_trie(): Displays the index contents
 *
 * Memory Management:
 * - Dynamic allocation for nodes and position arrays
 * - Automatic expansion of position arrays when needed
 * - Proper memory cleanup in trie_destroy()
 *
 * Performance Characteristics:
 * - Search: O(m) where m is the length of the word
 * - Insertion: O(m) where m is the length of the word
 * - Space: O(ALPHABET_SIZE * n) where n is the number of nodes
 *
 * @note This implementation assumes valid UTF-8 input text
 * @note The maximum word size is defined by MAX_WORD_SIZE
 */
#include "trie.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static char normalize_utf8_char(const char* utf8_char) {
    if (!utf8_char || !*utf8_char) return '\0';

    // Verifica caracteres multi-byte (UTF-8)
    unsigned char c0 = (unsigned char)utf8_char[0];
    unsigned char c1 = (unsigned char)utf8_char[1];

    // Tabela de mapeamento para caracteres especiais
    if (c0 == 0xC3) { // Caracteres latinos com diacríticos (2 bytes)
        switch (c1) {
            case 0x87: return 'c'; // Ç
            case 0xA7: return 'c'; // ç
            case 0x83: return 'a'; // Ã
            case 0xA3: return 'a'; // ã
            case 0x95: return 'o'; // Õ
            case 0xB5: return 'o'; // õ
            case 0x81: return 'a'; // Á
            case 0xA1: return 'a'; // á
            case 0x89: return 'e'; // É
            case 0xA9: return 'e'; // é
            case 0x8D: return 'i'; // Í
            case 0xAD: return 'i'; // í
            case 0x93: return 'o'; // Ó
            case 0xB3: return 'o'; // ó
            case 0x9A: return 'u'; // Ú
            case 0xBA: return 'u'; // ú
            case 0x9C: return 'u'; // Ü
            case 0xBC: return 'u'; // ü
        }
    }

    // Caracteres especiais (hífen mantido)
    if (c0 == '-') return '-';

    // Caracteres ASCII
    return tolower(c0);
}

int get_next_utf8_char(const char* str, char* buf, int max_len) {
    if (!str || !buf || max_len <= 0) return 0;
    
    unsigned char first_byte = (unsigned char)str[0];
    int char_len = 1;
    
    // Determina o comprimento do caractere UTF-8
    if ((first_byte & 0x80) == 0) {
        // ASCII (0xxxxxxx)
        char_len = 1;
    } else if ((first_byte & 0xE0) == 0xC0) {
        // 2 bytes (110xxxxx 10xxxxxx)
        char_len = 2;
    } else if ((first_byte & 0xF0) == 0xE0) {
        // 3 bytes (1110xxxx 10xxxxxx 10xxxxxx)
        char_len = 3;
    } else if ((first_byte & 0xF8) == 0xF0) {
        // 4 bytes (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
        char_len = 4;
    }
    
    // Verifica se o buffer é grande o suficiente
    if (char_len > max_len) char_len = max_len;
    
    // Copia o caractere completo
    memcpy(buf, str, char_len);
    buf[char_len] = '\0';
    
    return char_len;
}

TrieNode* trie_create_node() {
    TrieNode* node = malloc(sizeof(TrieNode));
    if (!node) exit(EXIT_FAILURE);    
    
    for (int i = 0; i < 27; i++) node->children[i] = NULL;
    node->is_end_of_word = 0;
    node->occurrences = malloc(10 * sizeof(int));
    if (!node->occurrences) {
        free(node);
        exit(EXIT_FAILURE);
    }
    node->num_occurrences = 0;
    node->max_occurrences = 10;
    node->original_word = NULL;
    node->stored_utf8 = NULL;  // Inicializa como NULL
    return node;
}

// Modificar a verificação de índice na inserção para incluir hífen
void trie_insert(TrieNode* root, const char* word, int position) {
    if (!root || !word || position < 0) return;

    TrieNode* current = root;
    char original_word[MAX_WORD_SIZE];
    strncpy(original_word, word, MAX_WORD_SIZE - 1);
    original_word[MAX_WORD_SIZE - 1] = '\0';

    const char* ptr = word;
    
    while (*ptr) {
        char utf8_char[5] = {0};
        int char_len = get_next_utf8_char(ptr, utf8_char, 4);
        
        if (char_len == 0) break;

        // Normalização mantendo mapeamento para indexação
        char normalized = normalize_utf8_char(utf8_char);
        int index;

        // Lógica de indexação corrigida
        if (normalized == '-') {
            index = 26; // Slot especial para hífen
        } else if (normalized >= 'a' && normalized <= 'z') {
            index = normalized - 'a';
        } else {
            ptr += char_len; // Ignora caracteres inválidos
            continue;
        }

        // Cria nó se necessário
        if (!current->children[index]) {
            current->children[index] = trie_create_node();
            
            // Armazena UTF-8 original com cast explícito
            current->children[index]->stored_utf8 = (unsigned char*)strdup(utf8_char);
            
            if (!current->children[index]->stored_utf8) {
                fprintf(stderr, "Falha ao alocar memória para caractere UTF-8\n");
                return;
            }
        }

        current = current->children[index];
        ptr += char_len; // Avança pelo tamanho real do caractere UTF-8
    }

    // Atualiza final de palavra
    if (current != root) {
        current->is_end_of_word = 1;

        // Armazena palavra original se necessário
        if (!current->original_word) {
            current->original_word = strdup(original_word);
            if (!current->original_word) {
                fprintf(stderr, "Erro ao copiar palavra original\n");
                return;
            }
        }

        // Expande array de ocorrências se necessário
        if (current->num_occurrences >= current->max_occurrences) {
            current->max_occurrences *= 2;
            int* new_occurrences = realloc(current->occurrences, 
                                        current->max_occurrences * sizeof(int));
            if (!new_occurrences) {
                fprintf(stderr, "Erro ao expandir ocorrências\n");
                return;
            }
            current->occurrences = new_occurrences;
        }

        current->occurrences[current->num_occurrences++] = position;
    }
}

int* trie_search(TrieNode* root, const char* word, int* num_occurrences) {
    if (!root || !word || !num_occurrences) {
        *num_occurrences = 0;
        return NULL;
    }

    TrieNode* current = root;
    const char* ptr = word;

    while (*ptr) {
        char utf8_char[5] = {0};
        int char_len = get_next_utf8_char(ptr, utf8_char, 4);
        
        if (char_len == 0) break;

        // Normalização correta usando o caractere UTF-8 completo
        char normalized = normalize_utf8_char(utf8_char);
        int index;

        if (normalized == '-') {
            index = 26;
        } else if (normalized >= 'a' && normalized <= 'z') {
            index = normalized - 'a';
        } else {
            ptr += char_len; // Ignora caracteres inválidos
            continue;
        }

        if (index < 0 || index >= 27 || !current->children[index]) {
            *num_occurrences = 0;
            return NULL;
        }

        current = current->children[index];
        ptr += char_len; // Avança pelo tamanho do caractere UTF-8
    }

    if (current && current->is_end_of_word) {
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
    
    for (int i = 0; i < 27; i++) {
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
    
    for (int i = 0; i < 27; i++) {
        if (root->children[i]) {
            trie_destroy(root->children[i]);
        }
    }
    
    free(root->occurrences);
    free(root->original_word);
    free(root->stored_utf8); // Libera a memória do caractere UTF-8
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
void imprimir_trie_arvore_recursivo(TrieNode* node, char* prefix, int is_last, 
                                        unsigned char* path, int level) {
    if (!node) return;

    printf("%s", prefix);
    printf(is_last ? "└── " : "├── ");

    if (node->stored_utf8) {
        printf("%s", node->stored_utf8);
    } else if (level > 0) {
        printf("%c", 'a' + path[level-1]);
    }

    if (node->is_end_of_word) {
        printf(" -> %s", node->original_word);
        printf(" (%d ocorrências)", node->num_occurrences);
    }

    printf("\n");

    char new_prefix[256];
    snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, is_last ? "    " : "│   ");
    int child_count = 0;
    for (int i = 0; i < 27; i++) {
        if (node->children[i]) child_count++;
    }

    int current_child = 0;
    for (int i = 0; i < 27; i++) {
        if (node->children[i]) {
        unsigned char new_path[MAX_WORD_SIZE] = {0};
        if (level > 0) memcpy(new_path, path, level);
        new_path[level] = i;

        imprimir_trie_arvore_recursivo(node->children[i], new_prefix, ++current_child == child_count, new_path, level + 1);
        }
    }
}

void imprimir_trie_arvore(TrieNode* root) {
    if (!root) {
        printf("Árvore Trie vazia!\n");
        return;
    }

    printf("\n=== Estrutura da Árvore Trie ===\n");
    unsigned char path[MAX_WORD_SIZE] = {0};
    imprimir_trie_arvore_recursivo(root, "", 1, path, 0);
    printf("===============================\n\n");
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