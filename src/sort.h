#ifndef SORT_H
#define SORT_H

#include <stddef.h>

/**
 * @brief Ordena um array usando o algoritmo Merge Sort.
 * 
 * @param base Ponteiro para o início do array.
 * @param nmemb Número de elementos no array.
 * @param size Tamanho de cada elemento em bytes.
 * @param compar Função de comparação.
 */
void merge_sort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));

/**
 * @brief Ordena um array usando o algoritmo Insertion Sort.
 * 
 * @param base Ponteiro para o início do array.
 * @param nmemb Número de elementos no array.
 * @param size Tamanho de cada elemento em bytes.
 * @param compar Função de comparação.
 */
void insertion_sort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));

/**
 * @brief Wrapper para a função qsort da biblioteca padrão.
 * 
 * @param base Ponteiro para o início do array.
 * @param nmemb Número de elementos no array.
 * @param size Tamanho de cada elemento em bytes.
 * @param compar Função de comparação.
 */
void quick_sort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));

#endif
