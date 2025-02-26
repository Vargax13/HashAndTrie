/**
 * @file hash.h
 * @brief Arquivo de cabeçalho contendo implementação de tabela hash para indexação de ocorrências de palavras
 *
 * Este arquivo contém as declarações de estruturas de dados e funções necessárias para implementar
 * uma tabela hash que armazena palavras e suas posições em um texto.
 *
 * @struct HashEntry
 * @brief Estrutura representando uma entrada na tabela hash
 * @var HashEntry::word
 * String contendo a palavra armazenada
 * @var HashEntry::occurrences
 * Array de inteiros contendo as posições onde a palavra aparece
 * @var HashEntry::num_occurrences
 * Número de ocorrências atualmente armazenadas
 * @var HashEntry::max_occurrences
 * Número máximo de ocorrências que podem ser armazenadas
 *
 * @struct HashTable
 * @brief Estrutura representando a tabela hash
 * @var HashTable::table
 * Array de estruturas HashEntry
 * @var HashTable::size
 * Tamanho da tabela hash
 * @var HashTable::entries
 * Número de entradas atualmente armazenadas na tabela
 *
 * @fn HashTable* hash_create(int size)
 * Cria uma nova tabela hash com o tamanho especificado
 *
 * @fn unsigned int hash_function(const char* word, int size)
 * Calcula o valor hash para uma palavra fornecida
 *
 * @fn void hash_insert(HashTable* ht, const char* word, int position)
 * Insere uma palavra e sua posição na tabela hash
 *
 * @fn int* hash_search(HashTable* ht, const char* word, int* num_occurrences)
 * Procura uma palavra na tabela hash e retorna suas ocorrências
 *
 * @fn void hash_destroy(HashTable* ht)
 * Libera toda a memória alocada para a tabela hash
 *
 * @fn int hash_resize(HashTable* ht)
 * Redimensiona a tabela hash quando ela fica muito cheia
 *
 * @fn int criar_indice_hash(HashTable** ht, char* palavras[], int* posicoes[], int num_palavras, char keywords[][MAX_WORD_SIZE], int num_keywords)
 * Cria um índice usando a tabela hash para as palavras fornecidas e suas posições
 *
 * @fn void imprimir_indice_hash(HashTable* ht)
 * Imprime o conteúdo da tabela hash
 *
 * @fn void imprimir_hash_arvore(HashTable* root)
 * Imprime o conteúdo da tabela hash em formato de árvore
 * @author Eduardo Brito, Eric Cesconetti, Gabriel Vargas e Paulo Albuquerque
 * @date 25/02/2025
 */
#ifndef HASH_H
#define HASH_H

#include "indice_remissivo.h"

/* Definição da estrutura de entrada da tabela hash */
typedef struct {
    char* word;
    int* occurrences;
    int num_occurrences;
    int max_occurrences;
} HashEntry;

/* Definição da estrutura da tabela hash */
typedef struct {
    HashEntry* table;
    int size;
    int entries;
} HashTable;

/* Protótipos das funções da tabela hash */
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