#ifndef VISIBILIDADE_H
#define VISIBILIDADE_H

#include "lista.h"
#include "arvore_binaria.h"
#include "forma.h"  
#include "anteparo.h" 

typedef void* ContextoVisibilidade;

/**
 * @brief Cria o contexto de visibilidade baseado na posição da bomba e na lista de formas.
 *
 * @param x coordenada X da bomba
 * @param y coordenada Y da bomba
 * @param lista_formas lista de formas (círculos, retângulos, linhas, textos, anteparos)
 * @param tipo_sort   'm' = mergesort, 'q' = quicksort, 'i' = insertion
 * @param threshold   limite para usar insertion sort
 */
ContextoVisibilidade criaContextoVisibilidade( float x, float y,Lista lista_formas, char tipo_sort, int threshold);

/**
 * @brief Testa se um ponto é visível a partir da bomba (O(log n)).
 */
bool pontoVisivel(ContextoVisibilidade ctx, float px, float py);

/**
 * @brief Testa se uma forma é visível (amostra pontos, custo O(log n)).
 */
bool formaVisivel(ContextoVisibilidade ctx, Forma forma);

/**
 * @brief Retorna lista com todas as formas visíveis.
 */
Lista getFormasVisiveis(ContextoVisibilidade ctx);

/**
 * @brief Libera todo o contexto.
 */
void liberaContextoVisibilidade(ContextoVisibilidade ctx);



#endif
