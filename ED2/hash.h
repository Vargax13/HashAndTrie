/**
 * @file hash.h
 * @brief Implementação de uma tabela hash para indexação de palavras e suas ocorrências
 *
 * Este arquivo contém as definições e protótipos de funções para uma tabela hash
 * que armazena palavras e suas posições de ocorrência em um texto.
 *
 * @struct HashEntry
 * @brief Estrutura que representa uma entrada na tabela hash
 * @var HashEntry::word
 * String contendo a palavra armazenada
 * @var HashEntry::occurrences
 * Array com as posições onde a palavra ocorre
 * @var HashEntry::num_occurrences
 * Número atual de ocorrências armazenadas
 * @var HashEntry::max_occurrences
 * Capacidade máxima do array de ocorrências
 *
 * @struct HashTable
 * @brief Estrutura principal da tabela hash
 * @var HashTable::table
 * Array de entradas HashEntry
 * @var HashTable::size
 * Tamanho total da tabela
 * @var HashTable::entries
 * Número atual de entradas ocupadas
 *
 * @fn HashTable* hash_create(int size)
 * @brief Cria uma nova tabela hash
 * @param size Tamanho inicial da tabela
 * @return Ponteiro para a tabela hash criada
 *
 * @fn unsigned int hash_function(const char* word, int size)
 * @brief Calcula o índice hash para uma palavra
 * @param word Palavra a ser calculada
 * @param size Tamanho da tabela
 * @return Índice calculado
 *
 * @fn void hash_insert(HashTable* ht, const char* word, int position)
 * @brief Insere uma palavra e sua posição na tabela
 * @param ht Ponteiro para a tabela hash
 * @param word Palavra a ser inserida
 * @param position Posição da palavra no texto
 *
 * @fn int* hash_search(HashTable* ht, const char* word, int* num_occurrences)
 * @brief Busca uma palavra na tabela hash
 * @param ht Ponteiro para a tabela hash
 * @param word Palavra a ser buscada
 * @param num_occurrences Ponteiro para armazenar número de ocorrências
 * @return Array com as posições da palavra
 *
 * @fn void hash_destroy(HashTable* ht)
 * @brief Libera a memória alocada pela tabela hash
 * @param ht Ponteiro para a tabela hash
 *
 * @fn int hash_resize(HashTable* ht)
 * @brief Redimensiona a tabela hash quando necessário
 * @param ht Ponteiro para a tabela hash
 * @return 1 se sucesso, 0 se falha
 *
 * @fn int criar_indice_hash(HashTable** ht, char* palavras[], int* posicoes[], int num_palavras, char keywords[][MAX_WORD_SIZE], int num_keywords)
 * @brief Cria um índice remissivo usando tabela hash
 * @param ht Ponteiro para ponteiro da tabela hash
 * @param palavras Array de palavras do texto
 * @param posicoes Array com posições das palavras
 * @param num_palavras Número total de palavras
 * @param keywords Array de palavras-chave
 * @param num_keywords Número de palavras-chave
 * @return 1 se sucesso, 0 se falha
 *
 * @fn void imprimir_indice_hash(HashTable* ht)
 * @brief Imprime o índice remissivo da tabela hash
 * @param ht Ponteiro para a tabela hash
 *
 * @fn void imprimir_hash_arvore(HashTable* ht)
 * @brief Imprime a tabela hash em formato de árvore
 * @param ht Ponteiro para a tabela hash
 */

#ifndef HASH_H
#define HASH_H

#include "indice_remissivo.h"

//Definição da estrutura de entrada da tabela hash
typedef struct {
    char* word;
    int* occurrences;
    int num_occurrences;
    int max_occurrences;
} HashEntry;

//Definição da estrutura da tabela hash
typedef struct {
    HashEntry* table;
    int size;
    int entries;
} HashTable;

//Protótipos das funções da tabela hash
HashTable* hash_create(int size);
unsigned int hash_function(const char* word, int size);
void hash_insert(HashTable* ht, const char* word, int position);
int* hash_search(HashTable* ht, const char* word, int* num_occurrences);
void hash_destroy(HashTable* ht);
int hash_resize(HashTable* ht);
int criar_indice_hash(HashTable** ht, char* palavras[], int* posicoes[], int num_palavras, 
                     char keywords[][MAX_WORD_SIZE], int num_keywords);
void imprimir_indice_hash(HashTable* ht);
void imprimir_hash_arvore(HashTable* ht);

#endif /* HASH_H */