/**
 * @file util.c
 * @brief Funções utilitárias para processamento de texto e gerenciamento de estruturas de dados
 *
 * Este arquivo contém funções utilitárias para gerenciar processamento de texto, carregamento
 * de palavras-chave e gerenciamento de recursos para implementações de índice remissivo
 * usando tanto Trie quanto Tabela Hash.
 *
 * Variáveis globais:
 * @var texto_hash - Armazenamento para texto a ser processado pela implementação hash
 * @var texto_trie - Armazenamento para texto a ser processado pela implementação trie
 * @var palavras_hash - Array de palavras extraídas do texto para implementação hash
 * @var palavras_trie - Array de palavras extraídas do texto para implementação trie
 * @var posicoes_hash - Array de posições para cada palavra na implementação hash
 * @var posicoes_trie - Array de posições para cada palavra na implementação trie
 * @var num_palavras_hash - Contador para número de palavras na implementação hash
 * @var num_palavras_trie - Contador para número de palavras na implementação trie
 * @var keywords_hash - Array de palavras-chave para implementação hash
 * @var keywords_trie - Array de palavras-chave para implementação trie
 * @var num_keywords_hash - Contador para número de palavras-chave na implementação hash
 * @var num_keywords_trie - Contador para número de palavras-chave na implementação trie
 * @var trie_root - Nó raiz da estrutura trie
 * @var hash_table - Ponteiro para a estrutura da tabela hash
 *
 * Funções:
 * @fn carregar_arquivo_texto - Carrega texto do arquivo para memória
 * @fn carregar_keywords - Carrega palavras-chave do arquivo para memória
 * @fn processar_texto - Processa texto para extrair palavras e suas posições
 * @fn limpar_recursos_hash - Limpa recursos usados pela implementação hash
 * @fn limpar_recursos_trie - Limpa recursos usados pela implementação trie
 * @fn limpar_recursos - Limpa todos os recursos
 * 
 * Getters e setters são fornecidos para todas as variáveis globais para garantir
 * encapsulamento adequado e acesso aos dados.
 * 
 * @note Tamanhos máximos são definidos nos arquivos de cabeçalho
 * @note Todo processamento de texto converte para minúsculas para consistência
 * @warning Gerenciamento de memória é crítico - assegure limpeza adequada usando as funções fornecidas
 * @author Eduardo Brito, Eric Cesconetti, Gabriel Vargas e Paulo Albuquerque
 * @date 25/02/2025
 */
#include "indice_remissivo.h"
#include "trie.h"
#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

// Variáveis globais modificadas para suportar múltiplos textos e keywords
static char texto_hash[MAX_TEXT_SIZE];
static char texto_trie[MAX_TEXT_SIZE];
static char* palavras_hash[MAX_TEXT_SIZE];
static char* palavras_trie[MAX_TEXT_SIZE];
static int* posicoes_hash[MAX_TEXT_SIZE];
static int* posicoes_trie[MAX_TEXT_SIZE];
static int num_palavras_hash = 0;
static int num_palavras_trie = 0;
static char keywords_hash[MAX_KEYWORDS][MAX_WORD_SIZE];
static char keywords_trie[MAX_KEYWORDS][MAX_WORD_SIZE];
static int num_keywords_hash = 0;
static int num_keywords_trie = 0;
static TrieNode* trie_root = NULL;
static HashTable* hash_table = NULL;

// Função para carregar o texto
int carregar_arquivo_texto(const char* filename, char* texto) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Erro ao abrir o arquivo: %s\n", filename);
        return 0;
    }
    size_t len = fread(texto, 1, MAX_TEXT_SIZE - 1, file);
    texto[len] = '\0';
    fclose(file);
    return 1;
}

// Função para carregar palavras-chave
int carregar_keywords(const char* filename, char keywords[][MAX_WORD_SIZE], int* num_keywords) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Erro ao abrir o arquivo: %s\n", filename);
        return 0;
    }
    *num_keywords = 0;
    char buffer[MAX_WORD_SIZE];
    while (fgets(buffer, MAX_WORD_SIZE, file) && *num_keywords < MAX_KEYWORDS) {
        buffer[strcspn(buffer, "\n")] = '\0';
        for (int i = 0; buffer[i]; i++) {
            buffer[i] = tolower(buffer[i]);
        }
        strncpy(keywords[(*num_keywords)++], buffer, MAX_WORD_SIZE - 1);
        keywords[*num_keywords - 1][MAX_WORD_SIZE - 1] = '\0'; // Garante terminação nula
    }
    fclose(file);
    return 1;
}

// Processar texto (extrair palavras e posições)
void processar_texto(char* texto, char* palavras[], int* posicoes[], int* num_palavras) {
    if (!texto || !palavras || !posicoes || !num_palavras) {
        return;
    }
    
    *num_palavras = 0;
    
    // Garantir inicialização segura
    for (int i = 0; i < MAX_TEXT_SIZE; i++) {
        palavras[i] = NULL;
        posicoes[i] = NULL;
    }
    
    char* texto_copia = strdup(texto);
    if (!texto_copia) {
        fprintf(stderr, "Falha ao copiar texto\n");
        return;
    }

    // Tokenização
    char* saveptr;
    char* token = strtok_r(texto_copia, " \t\n\r\f\v.,;:!?\"'()[]{}-", &saveptr);
    while (token && *num_palavras < MAX_TEXT_SIZE) {
        palavras[*num_palavras] = strdup(token);
        if (!palavras[*num_palavras]) {
            fprintf(stderr, "Falha ao alocar memória para palavra\n");
            free(texto_copia);
            return;
        }
        
        // Converter para minúsculas
        for (int i = 0; palavras[*num_palavras][i]; i++) {
            palavras[*num_palavras][i] = tolower(palavras[*num_palavras][i]);
        }

        // Alocar espaço para posições (primeiro elemento será o contador)
        posicoes[*num_palavras] = calloc(MAX_TEXT_SIZE, sizeof(int));
        if (!posicoes[*num_palavras]) {
            fprintf(stderr, "Falha ao alocar memória para posições\n");
            free(palavras[*num_palavras]);
            palavras[*num_palavras] = NULL;
            free(texto_copia);
            return;
        }
        posicoes[*num_palavras][0] = 0; // Inicializa o contador
        (*num_palavras)++;
        token = strtok_r(NULL, " \t\n\r\f\v.,;:!?\"'()[]{}-", &saveptr);
    }

    // Contar ocorrências
    for (int i = 0; i < *num_palavras; i++) {
        for (int j = 0; j < *num_palavras; j++) {
            // Comparação case-insensitive usando strcmp após converter para minúsculas
            if (strcmp(palavras[i], palavras[j]) == 0) {
                if (posicoes[i][0] < MAX_TEXT_SIZE - 1) {
                    posicoes[i][++posicoes[i][0]] = j;
                }
            }
        }
    }
    free(texto_copia);
}

// Limpar recursos - modificada para limpar recursos específicos
void limpar_recursos_hash(void) {
    for (int i = 0; i < num_palavras_hash; i++) {
        if (palavras_hash[i]) {
            free(palavras_hash[i]);
            palavras_hash[i] = NULL;
        }
        if (posicoes_hash[i]) {
            free(posicoes_hash[i]);
            posicoes_hash[i] = NULL;
        }
    }
    if (hash_table) {
        hash_destroy(hash_table);
        hash_table = NULL;
    }
    num_palavras_hash = 0;
    // Não limpa num_keywords_hash pois as palavras-chave são carregadas separadamente
}

void limpar_recursos_trie(void) {
    for (int i = 0; i < num_palavras_trie; i++) {
        if (palavras_trie[i]) {
            free(palavras_trie[i]);
            palavras_trie[i] = NULL;
        }
        if (posicoes_trie[i]) {
            free(posicoes_trie[i]);
            posicoes_trie[i] = NULL;
        }
    }
    if (trie_root) {
        trie_destroy(trie_root);
        trie_root = NULL;
    }
    num_palavras_trie = 0;
    // Não limpa num_keywords_trie pois as palavras-chave são carregadas separadamente
}

// Função para limpar todos os recursos
void limpar_recursos(void) {
    limpar_recursos_hash();
    limpar_recursos_trie();
    
    // Limpar os arrays de keywords também ao finalizar o programa
    num_keywords_hash = 0;
    num_keywords_trie = 0;
}

// Funções de acesso às variáveis globais (setters e getters) - modificadas
void set_texto_hash(char* texto) {
    strncpy(texto_hash, texto, MAX_TEXT_SIZE - 1);
    texto_hash[MAX_TEXT_SIZE - 1] = '\0';
}

void set_texto_trie(char* texto) {
    strncpy(texto_trie, texto, MAX_TEXT_SIZE - 1);
    texto_trie[MAX_TEXT_SIZE - 1] = '\0';
}

char* get_texto_hash(void) { return texto_hash; }
char* get_texto_trie(void) { return texto_trie; }
void set_trie_root(TrieNode* root) { trie_root = root; }
TrieNode* get_trie_root(void) { return trie_root; }
void set_hash_table(HashTable* ht) { hash_table = ht; }
HashTable* get_hash_table(void) { return hash_table; }
char** get_palavras_hash(void) { return palavras_hash; }
char** get_palavras_trie(void) { return palavras_trie; }
int** get_posicoes_hash(void) { return posicoes_hash; }
int** get_posicoes_trie(void) { return posicoes_trie; }
int get_num_palavras_hash(void) { return num_palavras_hash; }
int get_num_palavras_trie(void) { return num_palavras_trie; }
void set_num_palavras_hash(int num) { num_palavras_hash = num; }
void set_num_palavras_trie(int num) { num_palavras_trie = num; }
char (*get_keywords_hash(void))[MAX_WORD_SIZE] { return keywords_hash; }
char (*get_keywords_trie(void))[MAX_WORD_SIZE] { return keywords_trie; }
int get_num_keywords_hash(void) { return num_keywords_hash; }
int get_num_keywords_trie(void) { return num_keywords_trie; }
void set_num_keywords_hash(int num) { num_keywords_hash = num; }
void set_num_keywords_trie(int num) { num_keywords_trie = num; }