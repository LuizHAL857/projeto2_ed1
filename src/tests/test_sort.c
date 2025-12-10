#include "test_framework.h"
#include "../src/sort.h"
#include <stdlib.h>
#include <string.h>

/* Função de comparação para inteiros */
int comparar_ints(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

/* Auxiliar: Verificar se array está ordenado */
int esta_ordenado(int* arr, int tamanho) {
    for (int i = 1; i < tamanho; i++) {
        if (arr[i] < arr[i-1]) {
            return 0;
        }
    }
    return 1;
}

/* Auxiliar: Copiar array */
void copiar_array(int* dest, int* src, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        dest[i] = src[i];
    }
}

/* Teste: Merge sort com array aleatório */
void teste_merge_sort_aleatorio() {
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 88, 45, 50};
    int tamanho = 10;
    
    merge_sort(arr, tamanho, sizeof(int), comparar_ints);
    
    ASSERT_TRUE(esta_ordenado(arr, tamanho), "Array deve estar ordenado após merge sort");
    ASSERT_EQUAL(11, arr[0], "Primeiro elemento deve ser 11");
    ASSERT_EQUAL(90, arr[9], "Último elemento deve ser 90");
}

/* Teste: Quick sort com array aleatório */
void teste_quick_sort_aleatorio() {
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 88, 45, 50};
    int tamanho = 10;
    
    quick_sort(arr, tamanho, sizeof(int), comparar_ints);
    
    ASSERT_TRUE(esta_ordenado(arr, tamanho), "Array deve estar ordenado após quick sort");
    ASSERT_EQUAL(11, arr[0], "Primeiro elemento deve ser 11");
    ASSERT_EQUAL(90, arr[9], "Último elemento deve ser 90");
}

/* Teste: Insertion sort com array aleatório */
void teste_insertion_sort_aleatorio() {
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 88, 45, 50};
    int tamanho = 10;
    
    insertion_sort(arr, tamanho, sizeof(int), comparar_ints);
    
    ASSERT_TRUE(esta_ordenado(arr, tamanho), "Array deve estar ordenado após insertion sort");
    ASSERT_EQUAL(11, arr[0], "Primeiro elemento deve ser 11");
    ASSERT_EQUAL(90, arr[9], "Último elemento deve ser 90");
}

/* Teste: Ordenar array já ordenado */
void teste_ordenar_ja_ordenado() {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int tamanho = 10;
    int copia[10];
    
    copiar_array(copia, arr, tamanho);
    merge_sort(copia, tamanho, sizeof(int), comparar_ints);
    ASSERT_TRUE(esta_ordenado(copia, tamanho), "Merge sort deve lidar com array já ordenado");
    
    copiar_array(copia, arr, tamanho);
    quick_sort(copia, tamanho, sizeof(int), comparar_ints);
    ASSERT_TRUE(esta_ordenado(copia, tamanho), "Quick sort deve lidar com array já ordenado");
    
    copiar_array(copia, arr, tamanho);
    insertion_sort(copia, tamanho, sizeof(int), comparar_ints);
    ASSERT_TRUE(esta_ordenado(copia, tamanho), "Insertion sort deve lidar com array já ordenado");
}

/* Teste: Ordenar array em ordem reversa */
void teste_ordenar_ordem_reversa() {
    int arr[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    int tamanho = 10;
    int copia[10];
    
    copiar_array(copia, arr, tamanho);
    merge_sort(copia, tamanho, sizeof(int), comparar_ints);
    ASSERT_TRUE(esta_ordenado(copia, tamanho), "Merge sort deve lidar com array em ordem reversa");
    
    copiar_array(copia, arr, tamanho);
    quick_sort(copia, tamanho, sizeof(int), comparar_ints);
    ASSERT_TRUE(esta_ordenado(copia, tamanho), "Quick sort deve lidar com array em ordem reversa");
    
    copiar_array(copia, arr, tamanho);
    insertion_sort(copia, tamanho, sizeof(int), comparar_ints);
    ASSERT_TRUE(esta_ordenado(copia, tamanho), "Insertion sort deve lidar com array em ordem reversa");
}

/* Teste: Ordenar array com duplicatas */
void teste_ordenar_com_duplicatas() {
    int arr[] = {5, 2, 8, 2, 9, 1, 5, 5, 2, 8};
    int tamanho = 10;
    int copia[10];
    
    copiar_array(copia, arr, tamanho);
    merge_sort(copia, tamanho, sizeof(int), comparar_ints);
    ASSERT_TRUE(esta_ordenado(copia, tamanho), "Merge sort deve lidar com duplicatas");
    
    copiar_array(copia, arr, tamanho);
    quick_sort(copia, tamanho, sizeof(int), comparar_ints);
    ASSERT_TRUE(esta_ordenado(copia, tamanho), "Quick sort deve lidar com duplicatas");
    
    copiar_array(copia, arr, tamanho);
    insertion_sort(copia, tamanho, sizeof(int), comparar_ints);
    ASSERT_TRUE(esta_ordenado(copia, tamanho), "Insertion sort deve lidar com duplicatas");
}

/* Teste: Ordenar array com um elemento */
void teste_ordenar_um_elemento() {
    int arr[] = {42};
    int tamanho = 1;
    int copia[1];
    
    copiar_array(copia, arr, tamanho);
    merge_sort(copia, tamanho, sizeof(int), comparar_ints);
    ASSERT_EQUAL(42, copia[0], "Merge sort deve lidar com um elemento");
    
    copiar_array(copia, arr, tamanho);
    quick_sort(copia, tamanho, sizeof(int), comparar_ints);
    ASSERT_EQUAL(42, copia[0], "Quick sort deve lidar com um elemento");
    
    copiar_array(copia, arr, tamanho);
    insertion_sort(copia, tamanho, sizeof(int), comparar_ints);
    ASSERT_EQUAL(42, copia[0], "Insertion sort deve lidar com um elemento");
}

/* Teste: Ordenar array vazio */
void teste_ordenar_array_vazio() {
    int* arr = NULL;
    int tamanho = 0;
    
    /* Não deve causar crash */
    merge_sort(arr, tamanho, sizeof(int), comparar_ints);
    quick_sort(arr, tamanho, sizeof(int), comparar_ints);
    insertion_sort(arr, tamanho, sizeof(int), comparar_ints);
    
    ASSERT_TRUE(1, "Ordenar array vazio não deve causar crash");
}

int main() {
    RESETAR_ESTATISTICAS();
    
    EXECUTAR_TESTE(teste_merge_sort_aleatorio);
    EXECUTAR_TESTE(teste_quick_sort_aleatorio);
    EXECUTAR_TESTE(teste_insertion_sort_aleatorio);
    EXECUTAR_TESTE(teste_ordenar_ja_ordenado);
    EXECUTAR_TESTE(teste_ordenar_ordem_reversa);
    EXECUTAR_TESTE(teste_ordenar_com_duplicatas);
    EXECUTAR_TESTE(teste_ordenar_um_elemento);
    EXECUTAR_TESTE(teste_ordenar_array_vazio);
    
    IMPRIMIR_RESUMO_TESTES("Módulo Sort");
    
    return CODIGO_SAIDA_TESTE();
}
