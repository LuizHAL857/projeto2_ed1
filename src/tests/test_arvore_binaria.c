#include "test_framework.h"
#include "../src/arvore_binaria.h"
#include <stdlib.h>

/* Função de comparação para inteiros */
int comparar_ints(const void* a, const void* b, void* contexto) {
    int val_a = *(int*)a;
    int val_b = *(int*)b;
    return val_a - val_b;
}

/* Função auxiliar: Criar ponteiro de inteiro */
int* criar_int(int valor) {
    int* p = malloc(sizeof(int));
    *p = valor;
    return p;
}

/* Teste: Criar e destruir árvore vazia */
void teste_criar_destruir_arvore() {
    ArvoreBinaria arvore = criaArvoreBinaria(comparar_ints, NULL);
    ASSERT_NOT_NULL(arvore, "Árvore deve ser criada");
    ASSERT_TRUE(arvoreVazia(arvore), "Nova árvore deve estar vazia");
    ASSERT_EQUAL(0, tamanhoArvore(arvore), "Tamanho da árvore vazia deve ser 0");
    ASSERT_EQUAL(0, alturaArvore(arvore), "Altura da árvore vazia deve ser 0");
    liberaArvoreBinaria(arvore, free);
}

/* Teste: Inserir elementos */
void teste_inserir_elementos() {
    ArvoreBinaria arvore = criaArvoreBinaria(comparar_ints, NULL);
    
    int* val1 = criar_int(50);
    int* val2 = criar_int(30);
    int* val3 = criar_int(70);
    
    NoArvore n1 = insereArvoreBinaria(arvore, val1);
    ASSERT_NOT_NULL(n1, "Primeira inserção deve ter sucesso");
    ASSERT_EQUAL(1, tamanhoArvore(arvore), "Tamanho da árvore deve ser 1");
    
    NoArvore n2 = insereArvoreBinaria(arvore, val2);
    ASSERT_NOT_NULL(n2, "Segunda inserção deve ter sucesso");
    ASSERT_EQUAL(2, tamanhoArvore(arvore), "Tamanho da árvore deve ser 2");
    
    NoArvore n3 = insereArvoreBinaria(arvore, val3);
    ASSERT_NOT_NULL(n3, "Terceira inserção deve ter sucesso");
    ASSERT_EQUAL(3, tamanhoArvore(arvore), "Tamanho da árvore deve ser 3");
    
    liberaArvoreBinaria(arvore, free);
}

/* Teste: Buscar elementos */
void teste_buscar_elementos() {
    ArvoreBinaria arvore = criaArvoreBinaria(comparar_ints, NULL);
    
    int* val1 = criar_int(50);
    int* val2 = criar_int(30);
    int* val3 = criar_int(70);
    
    insereArvoreBinaria(arvore, val1);
    insereArvoreBinaria(arvore, val2);
    insereArvoreBinaria(arvore, val3);
    
    int valor_busca = 30;
    int* encontrado = (int*)buscaArvoreBinaria(arvore, &valor_busca);
    ASSERT_NOT_NULL(encontrado, "Deve encontrar valor 30");
    ASSERT_EQUAL(30, *encontrado, "Valor encontrado deve ser 30");
    
    valor_busca = 100;
    encontrado = (int*)buscaArvoreBinaria(arvore, &valor_busca);
    ASSERT_NULL(encontrado, "Não deve encontrar valor 100");
    
    liberaArvoreBinaria(arvore, free);
}

/* Teste: Obter mínimo e máximo */
void teste_min_max() {
    ArvoreBinaria arvore = criaArvoreBinaria(comparar_ints, NULL);
    
    int* val1 = criar_int(50);
    int* val2 = criar_int(30);
    int* val3 = criar_int(70);
    int* val4 = criar_int(20);
    int* val5 = criar_int(80);
    
    insereArvoreBinaria(arvore, val1);
    insereArvoreBinaria(arvore, val2);
    insereArvoreBinaria(arvore, val3);
    insereArvoreBinaria(arvore, val4);
    insereArvoreBinaria(arvore, val5);
    
    int* min = (int*)getMenorElemento(arvore);
    ASSERT_NOT_NULL(min, "Deve encontrar mínimo");
    ASSERT_EQUAL(20, *min, "Mínimo deve ser 20");
    
    int* max = (int*)getMaiorElemento(arvore);
    ASSERT_NOT_NULL(max, "Deve encontrar máximo");
    ASSERT_EQUAL(80, *max, "Máximo deve ser 80");
    
    liberaArvoreBinaria(arvore, free);
}

/* Teste: Remover elementos */
void teste_remover_elementos() {
    ArvoreBinaria arvore = criaArvoreBinaria(comparar_ints, NULL);
    
    int* val1 = criar_int(50);
    int* val2 = criar_int(30);
    int* val3 = criar_int(70);
    
    insereArvoreBinaria(arvore, val1);
    insereArvoreBinaria(arvore, val2);
    insereArvoreBinaria(arvore, val3);
    
    ASSERT_EQUAL(3, tamanhoArvore(arvore), "Árvore deve ter 3 elementos");
    
    int valor_remover = 30;
    bool removido = removeArvoreBinaria(arvore, &valor_remover);
    ASSERT_TRUE(removido, "Deve remover elemento com sucesso");
    ASSERT_EQUAL(2, tamanhoArvore(arvore), "Árvore deve ter 2 elementos após remoção");
    
    int* encontrado = (int*)buscaArvoreBinaria(arvore, &valor_remover);
    ASSERT_NULL(encontrado, "Elemento removido não deve ser encontrado");
    
    liberaArvoreBinaria(arvore, free);
}

/* Teste: Converter árvore para array */
void teste_arvore_para_array() {
    ArvoreBinaria arvore = criaArvoreBinaria(comparar_ints, NULL);
    
    int* val1 = criar_int(50);
    int* val2 = criar_int(30);
    int* val3 = criar_int(70);
    int* val4 = criar_int(20);
    int* val5 = criar_int(40);
    
    insereArvoreBinaria(arvore, val1);
    insereArvoreBinaria(arvore, val2);
    insereArvoreBinaria(arvore, val3);
    insereArvoreBinaria(arvore, val4);
    insereArvoreBinaria(arvore, val5);
    
    int tamanho;
    void** array = arvoreParaArray(arvore, &tamanho);
    
    ASSERT_NOT_NULL(array, "Array deve ser criado");
    ASSERT_EQUAL(5, tamanho, "Tamanho do array deve ser 5");
    
    /* Verificar se array está ordenado */
    ASSERT_EQUAL(20, *(int*)array[0], "Primeiro elemento deve ser 20");
    ASSERT_EQUAL(30, *(int*)array[1], "Segundo elemento deve ser 30");
    ASSERT_EQUAL(40, *(int*)array[2], "Terceiro elemento deve ser 40");
    ASSERT_EQUAL(50, *(int*)array[3], "Quarto elemento deve ser 50");
    ASSERT_EQUAL(70, *(int*)array[4], "Quinto elemento deve ser 70");
    
    free(array);
    liberaArvoreBinaria(arvore, free);
}

/* Teste: Operações em árvore vazia */
void teste_operacoes_arvore_vazia() {
    ArvoreBinaria arvore = criaArvoreBinaria(comparar_ints, NULL);
    
    ASSERT_NULL(getMenorElemento(arvore), "Mínimo de árvore vazia deve ser NULL");
    ASSERT_NULL(getMaiorElemento(arvore), "Máximo de árvore vazia deve ser NULL");
    
    int valor_busca = 10;
    ASSERT_NULL(buscaArvoreBinaria(arvore, &valor_busca), "Busca em árvore vazia deve retornar NULL");
    
    int tamanho;
    void** array = arvoreParaArray(arvore, &tamanho);
    ASSERT_EQUAL(0, tamanho, "Array de árvore vazia deve ter tamanho 0");
    if (array) free(array);
    
    liberaArvoreBinaria(arvore, free);
}

/* Teste: Altura da árvore */
void teste_altura_arvore() {
    ArvoreBinaria arvore = criaArvoreBinaria(comparar_ints, NULL);
    
    ASSERT_EQUAL(0, alturaArvore(arvore), "Altura da árvore vazia deve ser 0");
    
    int* val1 = criar_int(50);
    insereArvoreBinaria(arvore, val1);
    int altura1 = alturaArvore(arvore);
    ASSERT_TRUE(altura1 >= 0, "Árvore com um elemento deve ter altura >= 0");
    
    int* val2 = criar_int(30);
    int* val3 = criar_int(70);
    insereArvoreBinaria(arvore, val2);
    insereArvoreBinaria(arvore, val3);
    int altura3 = alturaArvore(arvore);
    ASSERT_TRUE(altura3 >= 0, "Árvore com três elementos deve ter altura >= 0");
    
    liberaArvoreBinaria(arvore, free);
}

int main() {
    RESETAR_ESTATISTICAS();
    
    EXECUTAR_TESTE(teste_criar_destruir_arvore);
    EXECUTAR_TESTE(teste_inserir_elementos);
    EXECUTAR_TESTE(teste_buscar_elementos);
    EXECUTAR_TESTE(teste_min_max);
    EXECUTAR_TESTE(teste_remover_elementos);
    EXECUTAR_TESTE(teste_arvore_para_array);
    EXECUTAR_TESTE(teste_operacoes_arvore_vazia);
    EXECUTAR_TESTE(teste_altura_arvore);
    
    IMPRIMIR_RESUMO_TESTES("Módulo Árvore Binária");
    
    return CODIGO_SAIDA_TESTE();
}
