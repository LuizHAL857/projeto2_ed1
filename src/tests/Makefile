# Makefile para Testes Unitários
# Compilador
CC = gcc

# Flags de compilação
CFLAGS = -std=c99 -Wall -Wextra -g -I../src

# Diretório de código-fonte
SRC_DIR = ../src

# Bibliotecas
LIBS = -lm

# Arquivos de código-fonte necessários (excluindo main.c)
SRC_FILES = $(SRC_DIR)/lista.c \
            $(SRC_DIR)/arvore_binaria.c \
            $(SRC_DIR)/circulo.c \
            $(SRC_DIR)/retangulo.c \
            $(SRC_DIR)/linha.c \
            $(SRC_DIR)/texto.c \
            $(SRC_DIR)/anteparo.c \
            $(SRC_DIR)/sort.c \
            $(SRC_DIR)/forma.c \
            $(SRC_DIR)/poligono.c

# Arquivos de teste
TESTS = test_lista test_arvore_binaria test_circulo test_retangulo \
        test_linha test_texto test_anteparo test_sort

# Alvo padrão: compilar todos os testes
all: $(TESTS)

# Regra para test_lista
test_lista: test_lista.c $(SRC_DIR)/lista.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Regra para test_arvore_binaria
test_arvore_binaria: test_arvore_binaria.c $(SRC_DIR)/arvore_binaria.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Regra para test_circulo
test_circulo: test_circulo.c $(SRC_DIR)/circulo.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Regra para test_retangulo
test_retangulo: test_retangulo.c $(SRC_DIR)/retangulo.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Regra para test_linha
test_linha: test_linha.c $(SRC_DIR)/linha.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Regra para test_texto
test_texto: test_texto.c $(SRC_DIR)/texto.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Regra para test_anteparo
test_anteparo: test_anteparo.c $(SRC_DIR)/anteparo.c $(SRC_DIR)/forma.c \
                  $(SRC_DIR)/circulo.c $(SRC_DIR)/retangulo.c $(SRC_DIR)/linha.c \
                  $(SRC_DIR)/texto.c $(SRC_DIR)/lista.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Regra para test_sort
test_sort: test_sort.c $(SRC_DIR)/sort.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Executar todos os testes
test: $(TESTS)
	@echo ""
	@echo "═══════════════════════════════════════════════════════"
	@echo "           EXECUTANDO TODOS OS TESTES"
	@echo "═══════════════════════════════════════════════════════"
	@echo ""
	@for test in $(TESTS); do \
		echo "Executando $$test..."; \
		./$$test || exit 1; \
		echo ""; \
	done
	@echo "═══════════════════════════════════════════════════════"
	@echo "           TODOS OS TESTES CONCLUÍDOS"
	@echo "═══════════════════════════════════════════════════════"

# Executar testes individuais
run_lista: test_lista
	./test_lista

run_arvore: test_arvore_binaria
	./test_arvore_binaria

run_circulo: test_circulo
	./test_circulo

run_retangulo: test_retangulo
	./test_retangulo

run_linha: test_linha
	./test_linha

run_texto: test_texto
	./test_texto

run_anteparo: test_anteparo
	./test_anteparo

run_sort: test_sort
	./test_sort

# Limpar arquivos compilados
clean:
	rm -f $(TESTS) *.o

# Limpar e recompilar
rebuild: clean all

# Alvos falsos
.PHONY: all test clean rebuild run_lista run_arvore run_circulo run_retangulo \
        run_linha run_texto run_anteparo run_sort
