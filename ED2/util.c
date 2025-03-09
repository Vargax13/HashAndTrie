/**
 * @file util.c
 * @brief Implementação de funções utilitárias para o índice remissivo
 *
 * @author Eduardo Brito, Eric Cesconetti, Gabriel Vargas e Paulo Albuquerque
 * @date 28/02/2025
 * 
 * Este arquivo contém a implementação de funções utilitárias para manipulação
 * de texto, palavras-chave e estruturas de dados (Trie e Hash) usadas no
 * índice remissivo.
 *
 * @note Utiliza variáveis globais para manter estado das estruturas de dados
 * 
 * Variáveis globais:
 * - texto_hash, texto_trie: Armazenam os textos para processamento
 * - palavras_hash, palavras_trie: Arrays de palavras extraídas
 * - posicoes_hash, posicoes_trie: Arrays com posições das palavras
 * - num_palavras_hash, num_palavras_trie: Contadores de palavras
 * - keywords_hash, keywords_trie: Arrays de palavras-chave
 * - num_keywords_hash, num_keywords_trie: Contadores de palavras-chave
 * - trie_root: Raiz da árvore Trie
 * - hash_table: Tabela hash
 *
 * Funções principais:
 * - carregar_arquivo_texto(): Carrega texto de arquivo
 * - carregar_keywords(): Carrega palavras-chave de arquivo
 * - processar_texto(): Extrai e processa palavras do texto
 * - limpar_recursos_hash(): Libera recursos da tabela hash
 * - limpar_recursos_trie(): Libera recursos da árvore Trie
 * - limpar_recursos(): Libera todos os recursos alocados
 *
 * Além de várias funções getters e setters para acesso às variáveis globais
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
    char* token = strtok_r(texto_copia, " \t\n\r\f\v.,;:!?\"'()[]{}", &saveptr);
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
        token = strtok_r(NULL, " \t\n\r\f\v.,;:!?\"'()[]{}", &saveptr);
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