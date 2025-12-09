#ifndef ARVORE_BINARIA_H
#define ARVORE_BINARIA_H

#include <stdbool.h>

/**
 * @brief Tipo opaco para representar uma árvore binária de busca.
 */
typedef void* ArvoreBinaria;

/**
 * @brief Tipo opaco para representar um nó da árvore.
 */
typedef void* NoArvore;



/**
 * @brief Tipo de função para liberar memória de um elemento.
 * 
 * @param elemento Elemento a ser desalocado
 */
typedef void (*FuncaoDesalocacao)(void* elemento);

/**
 * @brief Tipo de função de comparação para ordenar elementos na árvore.
 * 
 * @param a Primeiro elemento a comparar
 * @param b Segundo elemento a comparar
 * @param contexto Contexto adicional para comparação (pode ser NULL)
 * @return int < 0 se a < b, 0 se a == b, > 0 se a > b
 */
typedef int (*FuncaoComparacao)(const void* a, const void* b, void* contexto);



/**
 * @brief Cria uma nova árvore binária de busca vazia.
 * 
 * @param comparar Função de comparação para ordenar elementos
 * @param contexto Contexto adicional passado para a função de comparação
 * @return Ponteiro para a árvore criada ou NULL em caso de erro
 */
ArvoreBinaria criaArvoreBinaria(FuncaoComparacao comparar, void* contexto);

/**
 * @brief Libera toda a memória da árvore.
 * 
 * @param arvore Ponteiro para a árvore a ser liberada
 * @param desalocar Função para desalocar cada elemento (pode ser NULL)
 */
void liberaArvoreBinaria(ArvoreBinaria arvore, FuncaoDesalocacao desalocar);

/**
 * @brief Limpa todos os elementos da árvore sem liberar a estrutura.
 * 
 * @param arvore Ponteiro para a árvore
 * @param desalocar Função para desalocar cada elemento (pode ser NULL)
 */
void limpaArvoreBinaria(ArvoreBinaria arvore, FuncaoDesalocacao desalocar);



/**
 * @brief Insere um elemento na árvore.
 * 
 * @param arvore Ponteiro para a árvore
 * @param dado Ponteiro para o dado a ser inserido
 * @return NoArvore representando o nó inserido ou NULL em caso de erro
 */
NoArvore insereArvoreBinaria(ArvoreBinaria arvore, void* dado);

/**
 * @brief Remove um nó específico da árvore.
 * 
 * @param arvore Ponteiro para a árvore
 * @param no Nó a ser removido
 */
void removeNoArvore(ArvoreBinaria arvore, NoArvore no);

/**
 * @brief Remove um elemento da árvore por valor.
 * 
 * @param arvore Ponteiro para a árvore
 * @param dado Dado a ser removido
 * @return true se removido com sucesso, false caso contrário
 */
bool removeArvoreBinaria(ArvoreBinaria arvore, void* dado);



/**
 * @brief Busca um elemento na árvore.
 * 
 * @param arvore Ponteiro para a árvore
 * @param dado Dado a buscar (usado para comparação)
 * @return Ponteiro para o elemento encontrado ou NULL se não encontrado
 */
void* buscaArvoreBinaria(ArvoreBinaria arvore, const void* dado);

/**
 * @brief Retorna o menor elemento da árvore.
 * 
 * @param arvore Ponteiro para a árvore
 * @return Ponteiro para o menor elemento ou NULL se árvore vazia
 */
void* getMenorElemento(ArvoreBinaria arvore);

/**
 * @brief Retorna o nó com o menor elemento da árvore.
 * 
 * @param arvore Ponteiro para a árvore
 * @return NoArvore com menor elemento ou NULL se árvore vazia
 */
NoArvore getMenorNo(ArvoreBinaria arvore);

/**
 * @brief Retorna o maior elemento da árvore.
 * 
 * @param arvore Ponteiro para a árvore
 * @return Ponteiro para o maior elemento ou NULL se árvore vazia
 */
void* getMaiorElemento(ArvoreBinaria arvore);

/**
 * @brief Retorna o dado armazenado em um nó.
 * 
 * @param no Ponteiro para o nó
 * @return Ponteiro para o dado ou NULL se nó for NULL
 */
void* getDadoNo(NoArvore no);



/**
 * @brief Verifica se a árvore está vazia.
 * 
 * @param arvore Ponteiro para a árvore
 * @return true se vazia ou NULL, false caso contrário
 */
bool arvoreVazia(ArvoreBinaria arvore);

/**
 * @brief Retorna o número de elementos na árvore.
 * 
 * @param arvore Ponteiro para a árvore
 * @return Número de elementos
 */
int tamanhoArvore(ArvoreBinaria arvore);







/**
 * @brief Converte a árvore em um array ordenado.
 * 
 * O array retornado deve ser liberado pelo chamador.
 * 
 * @param arvore Ponteiro para a árvore
 * @param tamanho Ponteiro para armazenar o tamanho do array
 * @return Array de ponteiros para os elementos (ordenado) ou NULL em caso de erro
 */
void** arvoreParaArray(ArvoreBinaria arvore, int* tamanho);

#endif
