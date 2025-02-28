/**
 * @file trie.h
 * @brief Arquivo de cabeçalho contendo implementação da estrutura de dados Trie para indexação de palavras
 *
 * Este arquivo contém a definição da estrutura e protótipos de funções para uma estrutura
 * de dados Trie usada para criar e gerenciar um índice de palavras-chave. A Trie permite
 * armazenamento e recuperação eficiente de palavras junto com suas posições em um texto.
 *
 * @struct TrieNode
 * @brief Estrutura representando um nó na Trie
 * @var TrieNode::children Vetor de ponteiros para nós filhos (26 para letras minúsculas)
 * @var TrieNode::is_end_of_word Flag indicando se o nó representa fim de uma palavra
 * @var TrieNode::occurrences Vetor armazenando posições onde a palavra aparece
 * @var TrieNode::num_occurrences Número de ocorrências da palavra
 * @var TrieNode::max_occurrences Capacidade máxima do vetor de ocorrências
 * @var TrieNode::original_word Armazenamento para a palavra original completa
 *
 * Protótipos das funções:
 * @fn TrieNode* trie_create_node() Cria um novo nó da Trie
 * @fn void trie_insert() Insere uma palavra e sua posição na Trie
 * @fn int* trie_search() Busca uma palavra e retorna suas posições
 * @fn void trie_get_all_words() Recupera todas as palavras e suas posições da Trie
 * @fn void trie_destroy() Desaloca toda a estrutura da Trie
 * @fn int criar_indice_trie() Cria um índice de palavras-chave usando a Trie
 * @fn void imprimir_indice_trie() Imprime o índice de palavras-chave
 * @fn void imprimir_trie_arvore() Imprime a estrutura da Trie como uma árvore
 * @fn int binary_search_word() Realiza busca binária em um array ordenado de palavras
 * @author Eduardo Brito, Eric Cesconetti, Gabriel Vargas e Paulo Albuquerque
 * @date 25/02/2025
 */

 #ifndef TRIE_H
 #define TRIE_H
 
 #define MAX_WORD_SIZE 100
 #include "indice_remissivo.h"
 
 /* Definição da estrutura do nó da Trie */
 typedef struct TrieNode {
     struct TrieNode* children[27]; // Alterado para 27 (26 letras + hífen)
     int is_end_of_word;
     int* occurrences;
     int num_occurrences;
     int max_occurrences;
     char* original_word; // ARMAZENA A PALAVRA ORIGINAL, NÃO ALTERE
 } TrieNode;
 
 /* Protótipos das funções da Trie */
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
 
 /* Função de busca binária para encontrar palavras em array ordenado */
 int binary_search_word(char* palavras[], int num_palavras, const char* palavra);
 
 #endif /* TRIE_H */