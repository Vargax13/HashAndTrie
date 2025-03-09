/**
* @file trie.h
* @brief Implementação de uma estrutura de dados Trie para indexação de palavras
*
* @authors Eduardo Brito, Eric Cesconetti, Gabriel Vargas e Paulo Albuquerque
* @date 09/03/2025
* 
* Este arquivo contém as definições e protótipos para uma implementação de árvore Trie
* utilizada para indexação de palavras e suas ocorrências em um texto. A estrutura
* suporta inserção, busca e recuperação de palavras com suas respectivas posições.
*
* @struct TrieNode
* @brief Estrutura do nó da árvore Trie
* @var TrieNode::children
*    Array de ponteiros para os nós filhos (26 letras + hífen)
* @var TrieNode::is_end_of_word
*    Flag que indica se o nó representa o fim de uma palavra
* @var TrieNode::occurrences
*    Array com as posições onde a palavra ocorre
* @var TrieNode::num_occurrences
*    Número atual de ocorrências armazenadas
* @var TrieNode::max_occurrences
*    Capacidade máxima do array de ocorrências
* @var TrieNode::original_word
*    Armazena a palavra original sem modificações
*
* @fn TrieNode* trie_create_node()
* @brief Cria um novo nó da Trie
*
* @fn void trie_insert(TrieNode* root, const char* word, int position)
* @brief Insere uma palavra e sua posição na Trie
*
* @fn int* trie_search(TrieNode* root, const char* word, int* num_occurrences)
* @brief Busca uma palavra na Trie e retorna suas ocorrências
*
* @fn void trie_get_all_words(TrieNode* root, char* prefix, char*** words, int*** positions, int** num_positions, int* num_words, int* max_words)
* @brief Recupera todas as palavras armazenadas na Trie
*
* @fn void trie_destroy(TrieNode* root)
* @brief Libera a memória alocada para a Trie
*
* @fn int criar_indice_trie(TrieNode** root, char* palavras[], int* posicoes[], int num_palavras, char keywords[][MAX_WORD_SIZE], int num_keywords)
* @brief Cria um índice remissivo utilizando a estrutura Trie
*
* @fn void imprimir_indice_trie(TrieNode* root)
* @brief Imprime o índice remissivo armazenado na Trie
*
* @fn void imprimir_trie_arvore(TrieNode* root)
* @brief Imprime a estrutura da árvore Trie
*
* @fn int binary_search_word(char* palavras[], int num_palavras, const char* palavra)
* @brief Realiza busca binária em um array ordenado de palavras
*/

#ifndef TRIE_H
#define TRIE_H
 
#define MAX_WORD_SIZE 100
#include "indice_remissivo.h"
 
//Definição da estrutura do nó da Trie
typedef struct TrieNode {
    struct TrieNode* children[27]; // Alterado para 27 (26 letras + hífen)
    int is_end_of_word;
    int* occurrences;
    int num_occurrences;
    int max_occurrences;
    char* original_word; // ARMAZENA A PALAVRA ORIGINAL, NÃO ALTERE
    char stored_char[10]; // Caractere armazenado no nó 
    unsigned char* stored_utf8; // Caractere UTF-8 armazenado no nó
} TrieNode;
 
//Protótipos das funções da Trie
TrieNode* trie_create_node();
void trie_insert(TrieNode* root, const char* word, int position);
int* trie_search(TrieNode* root, const char* word, int* num_occurrences);
void trie_get_all_words(TrieNode* root, char* prefix, char*** words, int*** positions, 
                        int** num_positions, int* num_words, int* max_words);
void trie_destroy(TrieNode* root);
int criar_indice_trie(TrieNode** root, char* palavras[], int* posicoes[], int num_palavras, 
                     char keywords[][MAX_WORD_SIZE], int num_keywords);
void imprimir_indice_trie(TrieNode* root);
void imprimir_trie_arvore(TrieNode* root);
 
//Função de busca binária para encontrar palavras em array ordenado
int binary_search_word(char* palavras[], int num_palavras, const char* palavra);
 
#endif /* TRIE_H */