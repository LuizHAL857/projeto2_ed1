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
 * O algoritmo utiliza uma árvore binária balanceada para manter os segmentos ativos
 * ordenados por distância ao observador, permitindo determinar eficientemente quais
 * pontos são visíveis e quais estão obstruídos.
 
 */

#include <stdbool.h>
#include "lista.h"
#include "forma.h"

/**
 * @typedef ContextoVisibilidade
 * @brief Tipo opaco que representa o contexto de visibilidade.
 *
 * Encapsula todas as informações necessárias para cálculos de visibilidade,
 * incluindo a posição do observador, os segmentos de obstáculos (anteparos)
 * e estruturas auxiliares para o algoritmo de varredura angular.
 */
typedef void* ContextoVisibilidade;

/**
 * @brief Cria um contexto de visibilidade para um observador.
 *
 * Inicializa todas as estruturas de dados necessárias para realizar cálculos
 * de visibilidade a partir de um ponto observador (bx, by). Extrai os anteparos
 * da lista de formas fornecida e prepara os eventos angulares para o algoritmo
 * de varredura.
 *
 * @param bx Coordenada X do ponto observador (bomba).
 * @param by Coordenada Y do ponto observador (bomba).
 * @param formas Lista de formas geométricas contendo os anteparos (obstáculos).
 * @param tipo_sort Tipo de ordenação ('q' para quicksort, 'm' para mergesort).
 *                  Usado para ordenar eventos angulares.
 * @param threshold Limiar para uso de insertion sort em sub-arrays pequenos.
 *                  Usado em conjunto com tipo_sort para otimização.
 *
 * @return ContextoVisibilidade Contexto inicializado, ou NULL em caso de erro.
 *
 * @note O contexto retornado deve ser liberado com liberaContextoVisibilidade()
 *       após o uso para evitar vazamentos de memória.
 * @note Apenas formas do tipo ANTEPARO são consideradas como obstáculos.
 *
 * @see liberaContextoVisibilidade()
 */
ContextoVisibilidade criaContextoVisibilidade(
    float bx,
    float by,
    Lista formas,
    char tipo_sort,
    int threshold
);

/**
 * @brief Verifica se um ponto é visível a partir do observador.
 *
 * Determina se existe uma linha de visão desobstruída entre o observador
 * (definido no contexto) e o ponto (px, py). Utiliza o algoritmo de varredura
 * angular para verificar se algum segmento ativo bloqueia a visão.
 *
 * @param C Contexto de visibilidade previamente criado.
 * @param px Coordenada X do ponto a ser testado.
 * @param py Coordenada Y do ponto a ser testado.
 *
 * @return true se o ponto é visível (não obstruído), false caso contrário.
 *
 * @note Um ponto coincidente com o observador é sempre considerado visível.
 * @note Pontos sobre segmentos de obstáculos são tratados como visíveis.
 *
 * @see formaVisivel()
 */
bool pontoVisivel(
    ContextoVisibilidade C,
    float px,
    float py
);

/**
 * @brief Verifica se uma forma geométrica é visível.
 *
 * Determina se pelo menos uma parte da forma é visível a partir do observador.
 * Para diferentes tipos de formas, verifica pontos característicos:
 * - Círculos: centro
 * - Retângulos: centro
 * - Linhas: ponto médio
 * - Textos: ponto de âncora
 * - Anteparos: extremidades e ponto médio
 *
 * @param C Contexto de visibilidade previamente criado.
 * @param f Forma geométrica a ser testada.
 *
 * @return true se a forma (ou parte dela) é visível, false caso contrário.
 *
 * @note Para anteparos, verifica se qualquer dos três pontos (p1, pmid, p2)
 *       está visível, garantindo detecção de anteparos parcialmente visíveis.
 *
 * @see pontoVisivel()
 * @see getFormasVisiveis()
 */
bool formaVisivel(
    ContextoVisibilidade C,
    Forma f
);

/**
 * @brief Retorna uma lista com todas as formas visíveis.
 *
 * Percorre a lista de formas original (fornecida em criaContextoVisibilidade)
 * e retorna uma nova lista contendo apenas as formas que são visíveis a partir
 * do observador.
 *
 * @param C Contexto de visibilidade previamente criado.
 *
 * @return Lista contendo ponteiros para as formas visíveis.
 *         Retorna uma lista vazia se nenhuma forma for visível.
 *         Retorna NULL em caso de erro.
 *
 * @note A lista retornada deve ser liberada pelo chamador usando liberaLista().
 * @note As formas contidas na lista NÃO devem ser desalocadas, pois são
 *       referências às formas originais.
 * @note A lista retornada é uma nova lista, independente da lista original.
 *
 * @see formaVisivel()
 * @see liberaLista()
 */
Lista getFormasVisiveis(
    ContextoVisibilidade C
);

/**
 * @brief Libera toda a memória alocada para o contexto de visibilidade.
 *
 * Desaloca todas as estruturas internas do contexto, incluindo:
 * - Array de segmentos
 * - Array de eventos angulares
 * - Árvores binárias auxiliares
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
