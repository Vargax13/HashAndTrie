# Makefile para compilação do programa 'indice_remissivo'
#
# Variáveis:
# CC      - Compilador a ser utilizado (gcc)
# CFLAGS  - Flags de compilação
#           -std=c11: Define o padrão C11
#           -D_POSIX_C_SOURCE=200809L: Define funcionalidades POSIX
#           -Wall -Wextra -pedantic: Ativa warnings
#           -g: Inclui informações de debug
# TARGET  - Nome do executável final
# SRC     - Arquivos fonte .c
# OBJ     - Arquivos objeto gerados
# HEADERS - Arquivos de cabeçalho
#
# Regras:
# all        - Compila o programa completo (regra padrão)
# $(TARGET)  - Gera o executável final
# %.o        - Compila arquivos fonte em objetos
# clean      - Remove arquivos gerados pela compilação
#
# Uso:
# make       - Compila o programa
# make clean - Limpa arquivos gerados
# make valgrind - Executa o programa com Valgrind (NÃO RECOMENDADO REALIZAR TESTES DE ARQUIVOS GRANDES EM COMPUTADORES COM MENOS DE 8GB DE RAM, PODE TRAVAR O COMPUTADOR)
#
# Autor: Gabriel Vargas de Melo - UFES - 2025
# Última modificação: 26/02/2025

# Configurações do compilador
CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -pedantic -g
TARGET = indice_remissivo
SRC = main.c util.c hash.c trie.c
OBJ = $(SRC:.c=.o)
HEADERS = indice_remissivo.h trie.h hash.h

# Regra padrão (compila tudo)
all: $(TARGET)

# Compila o programa principal
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Gera os objetos
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpa os arquivos gerados
clean:
	rm -f $(OBJ) $(TARGET) valgrind.log

# Make Valgrind
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind.log ./$(TARGET)

.PHONY: all clean valgrind

# Fim do Makefile