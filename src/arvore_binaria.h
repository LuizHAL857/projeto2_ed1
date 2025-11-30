#ifndef ARVORE_BINARIA_H
#define ARVORE_BINARIA_H

#include <stdbool.h>

/**
 * @file arvore_binaria.h
 * @brief Módulo de árvore binária de busca para armazenar anteparos
 * 
 * Este módulo implementa uma árvore binária de busca genérica que pode
 * armazenar qualquer tipo de dado através de ponteiros void*.
 * É especialmente útil para ordenar e buscar anteparos durante o
 * cálculo de visibilidade das bombas.
 */

typedef void* ArvoreBinaria;
typedef void* NoArvore;

/**
 * @brief Tipo de função de comparação para ordenar elementos na árvore
 * 
 * @param a Primeiro elemento a comparar
 * @param b Segundo elemento a comparar
 * @return int < 0 se a < b, 0 se a == b, > 0 se a > b
 */
typedef int (*FuncaoComparacao)(const void* a, const void* b);

/**
 * @brief Tipo de função para liberar memória de um elemento
 * 
 * @param elemento Elemento a ser desalocado
 */
typedef void (*FuncaoDesalocacao)(void* elemento);

/**
 * @brief Tipo de função para processar cada elemento durante travessia
 * 
 * @param elemento Elemento sendo visitado
 * @param contexto Contexto adicional (pode ser NULL)
 */
typedef void (*FuncaoVisita)(void* elemento, void* contexto);

/**
 * @brief Cria uma nova árvore binária vazia
 * 
 * @param comparar Função de comparação para ordenar elementos
 * @return ArvoreBinaria Nova árvore criada
 */
ArvoreBinaria criaArvoreBinaria(FuncaoComparacao comparar);

/**
 * @brief Insere um elemento na árvore
 * 
 * @param arvore Árvore onde inserir
 * @param elemento Elemento a ser inserido
 * @return bool true se inserido com sucesso, false caso contrário
 */
bool insereArvoreBinaria(ArvoreBinaria arvore, void* elemento);

/**
 * @brief Busca um elemento na árvore
 * 
 * @param arvore Árvore onde buscar
 * @param elemento Elemento a buscar (usado para comparação)
 * @return void* Elemento encontrado ou NULL se não encontrado
 */
void* buscaArvoreBinaria(ArvoreBinaria arvore, const void* elemento);

/**
 * @brief Remove um elemento da árvore
 * 
 * @param arvore Árvore de onde remover
 * @param elemento Elemento a remover
 * @return void* Elemento removido ou NULL se não encontrado
 */
void* removeArvoreBinaria(ArvoreBinaria arvore, const void* elemento);

/**
 * @brief Verifica se a árvore está vazia
 * 
 * @param arvore Árvore a verificar
 * @return bool true se vazia, false caso contrário
 */
bool arvoreVazia(ArvoreBinaria arvore);

/**
 * @brief Retorna o número de elementos na árvore
 * 
 * @param arvore Árvore a contar
 * @return int Número de elementos
 */
int tamanhoArvore(ArvoreBinaria arvore);

/**
 * @brief Retorna a altura da árvore
 * 
 * @param arvore Árvore a medir
 * @return int Altura da árvore (0 para árvore vazia)
 */
int alturaArvore(ArvoreBinaria arvore);

/**
 * @brief Percorre a árvore em ordem 
 * 
 * Visita os elementos em ordem crescente segundo a função de comparação
 * 
 * @param arvore Árvore a percorrer
 * @param visita Função chamada para cada elemento
 * @param contexto Contexto adicional passado para a função de visita
 */
void percorreEmOrdem(ArvoreBinaria arvore, FuncaoVisita visita, void* contexto);

/**
 * @brief Percorre a árvore em pré-ordem 
 * 
 * @param arvore Árvore a percorrer
 * @param visita Função chamada para cada elemento
 * @param contexto Contexto adicional passado para a função de visita
 */
void percorrePreOrdem(ArvoreBinaria arvore, FuncaoVisita visita, void* contexto);

/**
 * @brief Percorre a árvore em pós-ordem 
 * 
 * @param arvore Árvore a percorrer
 * @param visita Função chamada para cada elemento
 * @param contexto Contexto adicional passado para a função de visita
 */
void percorrePosOrdem(ArvoreBinaria arvore, FuncaoVisita visita, void* contexto);

/**
 * @brief Converte a árvore em um array ordenado
 * 
 * Útil para usar com qsort ou outros algoritmos de ordenação
 * O array retornado deve ser liberado pelo chamador
 * 
 * @param arvore Árvore a converter
 * @param tamanho Ponteiro para armazenar o tamanho do array
 * @return void** Array de ponteiros para os elementos (ordenado)
 */
void** arvoreParaArray(ArvoreBinaria arvore, int* tamanho);

/**
 * @brief Cria uma árvore a partir de um array
 * 
 * @param elementos Array de elementos
 * @param tamanho Tamanho do array
 * @param comparar Função de comparação
 * @return ArvoreBinaria Nova árvore criada
 */
ArvoreBinaria arrayParaArvore(void** elementos, int tamanho, FuncaoComparacao comparar);

/**
 * @brief Limpa todos os elementos da árvore
 * 
 * @param arvore Árvore a limpar
 * @param desalocar Função para desalocar cada elemento (pode ser NULL)
 */
void limpaArvore(ArvoreBinaria arvore, FuncaoDesalocacao desalocar);

/**
 * @brief Libera toda a memória da árvore
 * 
 * @param arvore Árvore a desalocar
 * @param desalocar Função para desalocar cada elemento (pode ser NULL)
 */
void liberaArvoreBinaria(ArvoreBinaria arvore, FuncaoDesalocacao desalocar);

/**
 * @brief Retorna o menor elemento da árvore
 * 
 * @param arvore Árvore a buscar
 * @return void* Menor elemento ou NULL se árvore vazia
 */
void* getMenorElemento(ArvoreBinaria arvore);

/**
 * @brief Retorna o maior elemento da árvore
 * 
 * @param arvore Árvore a buscar
 * @return void* Maior elemento ou NULL se árvore vazia
 */
void* getMaiorElemento(ArvoreBinaria arvore);

#endif
