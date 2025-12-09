#ifndef VISIBILIDADE_H
#define VISIBILIDADE_H

/**
 * @file visibilidade.h
 * @brief Interface para o módulo de cálculo de visibilidade 2D.
 *
 * Este módulo implementa um algoritmo de varredura angular (angular sweep) para
 * calcular a região de visibilidade a partir de um ponto observador em um ambiente
 * 2D com obstáculos representados por segmentos de linha (anteparos).
 *
 * O algoritmo segue a abordagem clássica de plane sweep:
 * - Ordena vértices por ângulo em torno do observador
 * - Mantém conjunto de segmentos ativos em árvore binária balanceada
 * - Constrói incrementalmente a região de visibilidade
 */

#include <stdbool.h>
#include "lista.h"
#include "forma.h"

/**
 * @typedef ContextoVisibilidade
 * @brief Tipo opaco que representa o contexto de visibilidade.
 *
 * Encapsula todas as informações necessárias para cálculos de visibilidade,
 * incluindo a posição do observador, os segmentos de obstáculos (anteparos),
 * estruturas auxiliares para o algoritmo de varredura angular, e a região
 * de visibilidade calculada.
 */
typedef void* ContextoVisibilidade;

/**
 * @typedef SegmentoVisibilidade
 * @brief Tipo opaco que representa um segmento da região de visibilidade.
 *
 * Cada segmento delimita parte da fronteira da área visível a partir do
 * ponto observador.
 */
typedef void* SegmentoVisibilidade;

/**
 * @brief Cria um contexto de visibilidade para um observador.
 *
 * Inicializa todas as estruturas de dados necessárias para realizar cálculos
 * de visibilidade a partir de um ponto observador (bx, by). Extrai os anteparos
 * da lista de formas fornecida e executa o algoritmo de varredura angular para
 * calcular a região de visibilidade.
 *
 * O algoritmo:
 * 1. Cria retângulo envolvente
 * 2. Traça raio inicial e divide segmentos interceptados
 * 3. Ordena vértices por ângulo, distância e tipo
 * 4. Executa varredura angular mantendo segmentos ativos
 * 5. Constrói região de visibilidade V(x)
 *
 * @param bx Coordenada X do ponto observador (bomba).
 * @param by Coordenada Y do ponto observador (bomba).
 * @param formas Lista de formas geométricas contendo os anteparos (obstáculos).
 * @param tipo_sort Tipo de ordenação ('q' para quicksort, 'm' para mergesort).
 *                  Usado para ordenar vértices por ângulo.
 * @param threshold Limiar para uso de insertion sort em sub-arrays pequenos.
 *                  Usado em conjunto com tipo_sort para otimização.
 *
 * @return ContextoVisibilidade Contexto inicializado, ou NULL em caso de erro.
 *
 * @note O contexto retornado deve ser liberado com liberaContextoVisibilidade()
 *       após o uso para evitar vazamentos de memória.
 * @note Apenas formas do tipo ANTEPARO são consideradas como obstáculos.
 *
 * 
 */
ContextoVisibilidade criaContextoVisibilidade(
    float bx,
    float by,
    Lista formas,
    char tipo_sort,
    int threshold
);

/**
 * @brief Retorna a região de visibilidade calculada.
 *
 * Retorna uma lista de segmentos que delimitam a área visível a partir do
 * ponto observador. Cada segmento é do tipo SegmentoVisibilidade.
 *
 * @param C Contexto de visibilidade previamente criado.
 *
 * @return Lista de SegmentoVisibilidade* que formam a região de visibilidade,
 *         ou NULL se o contexto for inválido.
 *
 * @note A lista retornada pertence ao contexto e será liberada automaticamente
 *       quando liberaContextoVisibilidade() for chamado. Não libere a lista
 *       manualmente.
 * @note Os segmentos na lista estão ordenados conforme foram gerados durante
 *       a varredura angular.
 *
 * @see criaContextoVisibilidade()
 */
Lista getRegiaoVisibilidade(ContextoVisibilidade C);

/**
 * @brief Obtém as coordenadas de um segmento de visibilidade.
 *
 * @param seg Segmento de visibilidade.
 * @param x1 Ponteiro para armazenar coordenada X do ponto inicial.
 * @param y1 Ponteiro para armazenar coordenada Y do ponto inicial.
 * @param x2 Ponteiro para armazenar coordenada X do ponto final.
 * @param y2 Ponteiro para armazenar coordenada Y do ponto final.
 *
 * @note Todos os ponteiros devem ser não-nulos.
 */
void getCoordenadasSegmentoVis(
    SegmentoVisibilidade seg,
    float* x1, float* y1,
    float* x2, float* y2
);

/**
 * @brief Verifica se um ponto é visível a partir do observador.
 *
 * Determina se existe uma linha de visão desobstruída entre o observador
 * (definido no contexto) e o ponto (px, py). Utiliza a região de visibilidade
 * previamente calculada para fazer a verificação.
 *
 * @param C Contexto de visibilidade previamente criado.
 * @param px Coordenada X do ponto a ser testado.
 * @param py Coordenada Y do ponto a ser testado.
 *
 * @return true se o ponto é visível (não obstruído), false caso contrário.
 *
 * @note Um ponto coincidente com o observador é sempre considerado visível.
 * @note A verificação usa ray casting na região de visibilidade calculada.
 *
 * @see criaContextoVisibilidade()
 */
bool pontoVisivel(
    ContextoVisibilidade C,
    float px,
    float py
);

/**
 * @brief Libera toda a memória alocada para o contexto de visibilidade.
 *
 * Desaloca todas as estruturas internas do contexto, incluindo:
 * - Array de segmentos
 * - Array de vértices ordenados
 * - Árvore binária de segmentos ativos
 * - Lista de segmentos da região de visibilidade
 * - Outras estruturas de dados temporárias
 *
 * @param C Contexto de visibilidade a ser liberado.
 *
 * @note Após chamar esta função, o contexto C torna-se inválido e não deve
 *       ser usado novamente.
 * @note É seguro passar NULL como argumento (nenhuma operação é realizada).
 * @note Esta função NÃO libera as formas originais, apenas as estruturas
 *       internas do contexto.
 *
 * @see criaContextoVisibilidade()
 */
void liberaContextoVisibilidade(
    ContextoVisibilidade C
);

#endif
