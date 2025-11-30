#include "sort.h"
#include <stdlib.h>
#include <string.h>

static void merge(char *base, size_t l, size_t m, size_t r, size_t size, int (*compar)(const void *, const void *)) {
    size_t n1 = m - l + 1;
    size_t n2 = r - m;

    char *L = malloc(n1 * size);
    char *R = malloc(n2 * size);

    if (!L || !R) {
        if (L) free(L);
        if (R) free(R);
        return;
    }

    memcpy(L, base + l * size, n1 * size);
    memcpy(R, base + (m + 1) * size, n2 * size);

    size_t i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (compar(L + i * size, R + j * size) <= 0) {
            memcpy(base + k * size, L + i * size, size);
            i++;
        } else {
            memcpy(base + k * size, R + j * size, size);
            j++;
        }
        k++;
    }

    while (i < n1) {
        memcpy(base + k * size, L + i * size, size);
        i++;
        k++;
    }

    while (j < n2) {
        memcpy(base + k * size, R + j * size, size);
        j++;
        k++;
    }

    free(L);
    free(R);
}

static void merge_sort_recursive(char *base, size_t l, size_t r, size_t size, int (*compar)(const void *, const void *)) {
    if (l < r) {
        size_t m = l + (r - l) / 2;
        merge_sort_recursive(base, l, m, size, compar);
        merge_sort_recursive(base, m + 1, r, size, compar);
        merge(base, l, m, r, size, compar);
    }
}

void merge_sort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
    if (nmemb < 2) return;
    merge_sort_recursive((char *)base, 0, nmemb - 1, size, compar);
}

void insertion_sort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
    char *arr = (char *)base;
    char *key = malloc(size);
    if (!key) return;

    for (size_t i = 1; i < nmemb; i++) {
        memcpy(key, arr + i * size, size);
        size_t j = i;
        
        while (j > 0 && compar(arr + (j - 1) * size, key) > 0) {
            memcpy(arr + j * size, arr + (j - 1) * size, size);
            j--;
        }
        memcpy(arr + j * size, key, size);
    }
    free(key);
}

void quick_sort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
    qsort(base, nmemb, size, compar);
}
