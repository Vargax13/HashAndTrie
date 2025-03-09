/**
 * @file indice_remissivo.h
 * @brief Sistema de índice remissivo utilizando estruturas Hash e Trie
 * 
 * @author Eduardo Brito, Eric Cesconetti, Gabriel Vargas e Paulo Albuquerque
 * @date 25/02/2025
 * 
 * Este arquivo de cabeçalho define as estruturas e funções necessárias para
 * criar e manipular um índice remissivo de palavras-chave em um texto.
 * O sistema suporta duas estruturas de dados diferentes: Hash e Trie.
 * 
 * @note Definições importantes:
 * - MAX_WORD_SIZE: Tamanho máximo para palavras (100 caracteres)
 * - INITIAL_HASH_SIZE: Tamanho inicial da tabela hash (1023 posições)
 * - MAX_TEXT_SIZE: Tamanho máximo do texto a ser processado (100000 caracteres)
 * - MAX_KEYWORDS: Número máximo de palavras-chave suportadas (1000 palavras)
 * 
 * @section funcionalidades Principais Funcionalidades
 * - Carregamento de arquivo de texto
 * - Carregamento de palavras-chave
 * - Processamento de texto para identificação de palavras-chave
 * - Gerenciamento de recursos para ambas estruturas (Hash e Trie)
 * 
 * @section estruturas Estruturas Suportadas
 * - ESTRUTURA_HASH (1): Utiliza apenas tabela hash
 * - ESTRUTURA_TRIE (2): Utiliza apenas árvore trie
 * - ESTRUTURA_AMBAS (3): Utiliza ambas as estruturas
 */
#ifndef INDICE_REMISSIVO_H
#define INDICE_REMISSIVO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

/* Definições e constantes */
#define MAX_WORD_SIZE 100
#define INITIAL_HASH_SIZE 1023
#define MAX_TEXT_SIZE 100000
#define MAX_KEYWORDS 1000

/* Tipos de estrutura de dados */
typedef enum {
    ESTRUTURA_HASH = 1,
    ESTRUTURA_TRIE = 2,
    ESTRUTURA_AMBAS = 3
} TipoEstrutura;

/* Protótipos de funções para manipulação do sistema */
int carregar_arquivo_texto(const char* filename, char* texto);
int carregar_keywords(const char* filename, char keywords[][MAX_WORD_SIZE], int* num_keywords);
void processar_texto(char* texto, char* palavras[], int* posicoes[], int* num_palavras);
void limpar_recursos(void);
void limpar_recursos_hash(void);
void limpar_recursos_trie(void);

/* Funções de acesso específicas para hash e trie */
char* get_texto_hash(void);
char* get_texto_trie(void);
void set_texto_hash(char* texto);
void set_texto_trie(char* texto);
char** get_palavras_hash(void);
char** get_palavras_trie(void);
int** get_posicoes_hash(void);
int** get_posicoes_trie(void);
int get_num_palavras_hash(void);
int get_num_palavras_trie(void);
void set_num_palavras_hash(int num);
void set_num_palavras_trie(int num);
char (*get_keywords_hash(void))[MAX_WORD_SIZE];
char (*get_keywords_trie(void))[MAX_WORD_SIZE];
int get_num_keywords_hash(void);
int get_num_keywords_trie(void);
int get_num_keywords_hash(void);
int get_num_keywords_trie(void);
void set_num_keywords_hash(int num);
void set_num_keywords_trie(int num);

#endif /* INDICE_REMISSIVO_H */