/**
 * @file main.c
 * @brief Implementação principal do programa de índice remissivo
 *
 * Este programa gerencia a criação e manipulação de índices remissivos utilizando
 * duas estruturas de dados diferentes: tabela hash e árvore trie. Permite carregar
 * textos e palavras-chave, criar índices, visualizar e excluir as estruturas.
 *
 * @note Utiliza variáveis globais compartilhadas para gerenciar os dados comuns
 * entre as estruturas.
 *
 * Funcionalidades principais:
 * - Carregamento de texto fonte
 * - Carregamento de palavras-chave
 * - Criação de índices (hash e/ou trie)
 * - Impressão dos índices
 * - Visualização da representação em árvore
 * - Exclusão dos índices
 *
 * Estruturas de dados suportadas:
 * - Tabela Hash
 * - Árvore Trie (árvore de pesquisa digital)
 *
 * @see indice_remissivo.h
 * @see trie.h
 * @see hash.h
 *
 * Limitações:
 * - Tamanho máximo do texto definido por MAX_TEXT_SIZE
 * - Número máximo de palavras-chave definido por MAX_KEYWORDS
 * - Tamanho máximo de palavra definido por MAX_WORD_SIZE
 *
 * @author Eduardo Brito, Eric Cesconetti, Gabriel Vargas e Paulo Albuquerque
 * @date 25/02/2025
 */

#include "indice_remissivo.h"
#include "trie.h"
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Declarações de funções externas
extern void set_texto_hash(char*);
extern void set_texto_trie(char*);
extern char* get_texto_hash(void);
extern char* get_texto_trie(void);
extern void set_trie_root(TrieNode*);
extern TrieNode* get_trie_root(void);
extern void set_hash_table(HashTable*);
extern HashTable* get_hash_table(void);
extern char** get_palavras_hash(void);
extern char** get_palavras_trie(void);
extern int** get_posicoes_hash(void);
extern int** get_posicoes_trie(void);
extern int get_num_palavras_hash(void);
extern int get_num_palavras_trie(void);
extern void set_num_palavras_hash(int);
extern void set_num_palavras_trie(int);
extern char (*get_keywords_hash(void))[MAX_WORD_SIZE];
extern char (*get_keywords_trie(void))[MAX_WORD_SIZE];
extern int get_num_keywords_hash(void);
extern int get_num_keywords_trie(void);
extern void set_num_keywords_hash(int);
extern void set_num_keywords_trie(int);
extern void limpar_recursos(void);
extern void limpar_recursos_hash(void);
extern void limpar_recursos_trie(void);

// Protótipos locais
static void exibir_menu(void);
static void carregar_texto(void);
static void carregar_lista_keywords(void);
static void criar_indice_menu(void);
static void imprimir_indice_menu(void);
static void imprimir_representacao_arvore_menu(void);
static void excluir_indice_menu(void);
static void limpar_recursos_comuns(void);

// Variáveis globais compartilhadas (static para escopo de arquivo)
static char texto_comum[MAX_TEXT_SIZE];
static char keywords_comum[MAX_KEYWORDS][MAX_WORD_SIZE];
static int num_keywords_comum;
static char** palavras_comum;
static int** posicoes_comum;
static int num_palavras_comum;
static int texto_carregado;
static int keywords_carregadas;

int main() {
    palavras_comum = calloc(MAX_TEXT_SIZE, sizeof(char*));
    posicoes_comum = calloc(MAX_TEXT_SIZE, sizeof(int*));
    
    if (!palavras_comum || !posicoes_comum) {
        fprintf(stderr, "Erro: falha na alocação de memória\n");
        free(palavras_comum);
        free(posicoes_comum);
        return EXIT_FAILURE;
    }
    
    for (int opcao = -1; opcao != 0;) {
        exibir_menu();
        scanf("%d%*c", &opcao);

        switch (opcao) {
            case 0: printf("Saindo...\n"); break;
            case 1: carregar_texto(); break;
            case 2: carregar_lista_keywords(); break;
            case 3: criar_indice_menu(); break;
            case 4: imprimir_indice_menu(); break;
            case 5: imprimir_representacao_arvore_menu(); break;
            case 6: excluir_indice_menu(); break;
            default: printf("Opção inválida!\n");
        }
    }
    
    limpar_recursos();
    
    // Libera memória dos arrays comuns
    for (int i = 0; i < num_palavras_comum; i++) {
        free(palavras_comum[i]);
        free(posicoes_comum[i]);
    }
    free(palavras_comum);
    free(posicoes_comum);
    
    return EXIT_SUCCESS;
}

static void exibir_menu(void) {
    printf("\n =============== Menu ===============\n"
           "|1. Carregar texto                   |\n"
           "|2. Carregar palavras-chave          |\n"
           "|3. Criar índice                     |\n"
           "|4. Imprimir índices                 |\n"
           "|5. Imprimir representação em árvore |\n"
           "|6. Excluir índices                  |\n"
           "|0. Sair                             |\n"
           " ====================================\n\n"
           "Escolha: ");
    fflush(stdout);
}

// Função auxiliar para limpar os recursos comuns
static void limpar_recursos_comuns(void) {
    for (int i = 0; i < num_palavras_comum; i++) {
        free(palavras_comum[i]);
        free(posicoes_comum[i]);
        palavras_comum[i] = NULL;
        posicoes_comum[i] = NULL;
    }
    num_palavras_comum = texto_carregado = 0;
}

static void carregar_texto(void) {
    char filename[256];
    
    if (get_hash_table() || get_trie_root()) {
        printf("Aviso: Já existem índices carregados. Deseja excluí-los? (s/n): ");
        if (getchar() == 's') {
            getchar(); // Limpa o \n
            excluir_indice_menu();
        } else {
            getchar(); // Limpa o \n
        }
    }
    
    limpar_recursos_comuns();

    do {
        printf("Nome do arquivo de texto: ");
        if (!fgets(filename, sizeof(filename), stdin)) continue;
        filename[strcspn(filename, "\n")] = 0;
        
        if (!carregar_arquivo_texto(filename, texto_comum)) {
            printf("Erro ao carregar arquivo!\n");
            continue;
        }

        processar_texto(texto_comum, palavras_comum, posicoes_comum, &num_palavras_comum);
        
        if (!num_palavras_comum) {
            printf("Arquivo vazio ou sem conteúdo válido.\n");
            continue;
        }
        
        texto_carregado = 1;
        printf("Texto carregado com sucesso (%d palavras)\n", num_palavras_comum);
        break;
    } while (1);
}

/* Função para carregar a lista de palavras-chave */
void carregar_lista_keywords(void) {
    char filename[256];
    char buffer[1024];  // Buffer para ler uma linha inteira
    
    do {
        printf("Digite o nome do arquivo de palavras-chave: ");
        fflush(stdout);
        
        fgets(filename, sizeof(filename), stdin);
        filename[strcspn(filename, "\n")] = '\0';
        
        FILE *file = fopen(filename, "r");
        if (!file) {
            printf("Falha ao abrir o arquivo de palavras-chave.\n");
            continue;
        }
        
        num_keywords_comum = 0;
        int has_non_whitespace = 0;  // Flag para verificar se há conteúdo não-branco
        
        while (fgets(buffer, sizeof(buffer), file) && num_keywords_comum < MAX_KEYWORDS) {
            buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline
            
            // Verifica se a linha tem algum caractere não-branco
            for (int i = 0; buffer[i]; i++) {
                if (buffer[i] != ' ' && buffer[i] != '\t') {
                    has_non_whitespace = 1;
                    break;
                }
            }
            
            if (!has_non_whitespace) continue;  // Pula linhas que só têm espaços
            
            char *token = strtok(buffer, ",");  // Divide a linha por vírgulas
            while (token && num_keywords_comum < MAX_KEYWORDS) {
                // Remove espaços no início e fim da palavra
                while (*token == ' ') token++;
                int len = strlen(token);
                while (len > 0 && token[len-1] == ' ') token[--len] = '\0';
                
                if (len > 0) {
                    strncpy(keywords_comum[num_keywords_comum], token, MAX_WORD_SIZE - 1);
                    keywords_comum[num_keywords_comum][MAX_WORD_SIZE - 1] = '\0';
                    num_keywords_comum++;
                }
                token = strtok(NULL, ",");
            }
        }
        
        fclose(file);
        
        if (num_keywords_comum == 0 || !has_non_whitespace) {
            printf("O arquivo está vazio ou contém apenas espaços em branco. Por favor, forneça um arquivo com palavras-chave.\n");
            continue;
        }
        
        keywords_carregadas = 1;
        printf("Lista de palavras-chave carregada com sucesso.\n");
        printf("Total de palavras-chave: %d\n", num_keywords_comum);
        break;
        
    } while (1);
}

/* Função para criar o índice remissivo */
void criar_indice_menu(void) {
    char opcao[10];
    
    if (!texto_carregado) {
        printf("Nenhum texto foi carregado. Carregue um texto primeiro.\n");
        return;
    }
    
    if (!keywords_carregadas) {
        printf("Nenhuma palavra-chave foi carregada. Carregue palavras-chave primeiro.\n");
        return;
    }
    
    printf("Criar índice em qual estrutura? (hash/trie/ambas): ");
    fgets(opcao, sizeof(opcao), stdin);
    opcao[strcspn(opcao, "\n")] = '\0';
    
    if (strcmp(opcao, "hash") == 0 || strcmp(opcao, "ambas") == 0) {
        // Copiar dados para a estrutura hash
        limpar_recursos_hash();
        set_texto_hash(texto_comum);
        
        // Copiar palavras e posições para hash
        for (int i = 0; i < num_palavras_comum; i++) {
            get_palavras_hash()[i] = strdup(palavras_comum[i]);
            
            // Copiar posições
            int* pos_original = posicoes_comum[i];
            int count = pos_original[0] + 1; // +1 para o contador no início
            get_posicoes_hash()[i] = (int*)malloc(count * sizeof(int));
            memcpy(get_posicoes_hash()[i], pos_original, count * sizeof(int));
        }
        set_num_palavras_hash(num_palavras_comum);
        
        // Copiar keywords para hash
        memcpy(get_keywords_hash(), keywords_comum, num_keywords_comum * MAX_WORD_SIZE);
        set_num_keywords_hash(num_keywords_comum);
        
        // Criar o índice hash
        HashTable* ht = get_hash_table();
        if (criar_indice_hash(&ht, get_palavras_hash(), get_posicoes_hash(),
                            get_num_palavras_hash(), get_keywords_hash(),
                            get_num_keywords_hash())) {
            set_hash_table(ht);
            printf("Índice remissivo usando tabela hash criado com sucesso.\n");
        } else {
            printf("Falha ao criar o índice remissivo usando tabela hash.\n");
        }
    }
    
    if (strcmp(opcao, "trie") == 0 || strcmp(opcao, "ambas") == 0) {
        // Copiar dados para a estrutura trie
        limpar_recursos_trie();
        set_texto_trie(texto_comum);
        
        // Copiar palavras e posições para trie
        for (int i = 0; i < num_palavras_comum; i++) {
            get_palavras_trie()[i] = strdup(palavras_comum[i]);
            
            // Copiar posições
            int* pos_original = posicoes_comum[i];
            int count = pos_original[0] + 1; // +1 para o contador no início
            get_posicoes_trie()[i] = (int*)malloc(count * sizeof(int));
            memcpy(get_posicoes_trie()[i], pos_original, count * sizeof(int));
        }
        set_num_palavras_trie(num_palavras_comum);
        
        // Copiar keywords para trie
        memcpy(get_keywords_trie(), keywords_comum, num_keywords_comum * MAX_WORD_SIZE);
        set_num_keywords_trie(num_keywords_comum);
        
        // Criar o índice trie
        TrieNode* root = get_trie_root();
        if (criar_indice_trie(&root, get_palavras_trie(), get_posicoes_trie(),
                           get_num_palavras_trie(), get_keywords_trie(),
                           get_num_keywords_trie())) {
            set_trie_root(root);
            printf("Índice remissivo usando árvore de pesquisa digital criado com sucesso.\n");
        } else {
            printf("Falha ao criar o índice remissivo usando árvore de pesquisa digital.\n");
        }
    }
    
    if (strcmp(opcao, "hash") != 0 && strcmp(opcao, "trie") != 0 && strcmp(opcao, "ambas") != 0) {
        printf("Opção inválida. Use 'hash', 'trie' ou 'ambas'.\n");
    }
}

/* Função para imprimir o índice remissivo */
void imprimir_indice_menu(void) {
    char opcao[10];
    printf("Qual estrutura deseja imprimir (hash/trie/ambas): ");
    fflush(stdout);
    
    fgets(opcao, sizeof(opcao), stdin);
    opcao[strcspn(opcao, "\n")] = '\0';
    printf("=== ÍNDICES CRIADOS ===\n\n");
    
    if (strcmp(opcao, "hash") == 0 || strcmp(opcao, "ambas") == 0) {
        if (get_hash_table() != NULL) {
            imprimir_indice_hash(get_hash_table());
        } else {
            printf("=================================\n");
            printf("Índice hash não foi criado ainda.\n");
        }
    }
    
    if (strcmp(opcao, "trie") == 0 || strcmp(opcao, "ambas") == 0) {
        if (get_trie_root() != NULL) {
            imprimir_indice_trie(get_trie_root());
        } else {
            printf("=================================\n");
            printf("Índice trie não foi criado ainda.\n");
        }
    }
    
    if (strcmp(opcao, "hash") != 0 && strcmp(opcao, "trie") != 0 && strcmp(opcao, "ambas") != 0) {
        printf("Opção inválida. Use 'hash', 'trie' ou 'ambas'.\n");
    }
}

/* Função para excluir o índice remissivo */
void excluir_indice_menu(void) {
    char opcao[10];
    printf("Qual estrutura deseja excluir (hash/trie/ambas): ");
    fflush(stdout);
    
    fgets(opcao, sizeof(opcao), stdin);
    opcao[strcspn(opcao, "\n")] = '\0';
    
    if (strcmp(opcao, "hash") == 0 || strcmp(opcao, "ambas") == 0) {
        HashTable* ht = get_hash_table();
        if (ht != NULL) {
            hash_destroy(ht);
            set_hash_table(NULL);
            printf("Índice hash excluído.\n");
        } else {
            printf("Índice hash não existe.\n");
        }
        
        if (strcmp(opcao, "hash") == 0) {
            limpar_recursos_hash();
        }
    }
    
    if (strcmp(opcao, "trie") == 0 || strcmp(opcao, "ambas") == 0) {
        TrieNode* root = get_trie_root();
        if (root != NULL) {
            trie_destroy(root);
            set_trie_root(NULL);
            printf("Índice trie excluído.\n");
        } else {
            printf("Índice trie não existe.\n");
        }
        
        if (strcmp(opcao, "trie") == 0) {
            limpar_recursos_trie();
        }
    }
    
    if (strcmp(opcao, "ambas") == 0) {
        limpar_recursos();
    }
    
    if (strcmp(opcao, "hash") != 0 && strcmp(opcao, "trie") != 0 && 
        strcmp(opcao, "ambas") != 0) {
        printf("Opção inválida. Use 'hash', 'trie' ou 'ambas'.\n");
    }
}

/* Nova função para imprimir a representação em árvore */
void imprimir_representacao_arvore_menu(void) {
    char opcao[10];
    printf("Qual estrutura deseja visualizar como árvore (hash/trie/ambas): ");
    fflush(stdout);
    
    fgets(opcao, sizeof(opcao), stdin);
    opcao[strcspn(opcao, "\n")] = '\0';
    
    if (strcmp(opcao, "hash") == 0 || strcmp(opcao, "ambas") == 0) {
        if (get_hash_table() != NULL) {
            imprimir_hash_arvore(get_hash_table());
        } else {
            printf("=================================\n");
            printf("A arvore hash não foi criada ainda.\n");
        }
    }
    
    if (strcmp(opcao, "trie") == 0 || strcmp(opcao, "ambas") == 0) {
        if (get_trie_root() != NULL) {
            imprimir_trie_arvore(get_trie_root());
        } else {
            printf("=================================\n");
            printf("A arvore trie não foi criada ainda.\n");
        }
    }
    
    if (strcmp(opcao, "hash") != 0 && strcmp(opcao, "trie") != 0 && strcmp(opcao, "ambas") != 0) {
        printf("Opção inválida. Use 'hash', 'trie' ou 'ambas'.\n");
    }
}