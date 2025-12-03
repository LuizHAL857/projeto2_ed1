#ifndef VISIBILIDADE_H
#define VISIBILIDADE_H

#include <stdbool.h>
#include "lista.h"
#include "forma.h"

// O contexto é opaco — não expomos a struct interna
typedef void* ContextoVisibilidade;

/**
 * Cria o contexto de visibilidade para um observador
 * na posição (bx, by), usando a lista de formas fornecida.
 *
 * tipo_sort e threshold existem apenas para manter compatibilidade,
 * mas não são usados nesta implementação (ordenamos sempre com quick_sort).
 */
ContextoVisibilidade criaContextoVisibilidade(
    float bx,
    float by,
    Lista formas,
    char tipo_sort,
    int threshold
);

/**
 * Retorna true se o ponto (px, py) for visível a partir do contexto.
 */
bool pontoVisivel(
    ContextoVisibilidade C,
    float px,
    float py
);

/**
 * Verifica se uma forma é visível (qualquer parte dela).
 */
bool formaVisivel(
    ContextoVisibilidade C,
    Forma f
);

/**
 * Retorna uma lista contendo todas as formas visíveis.
 * A lista retornada deve ser liberada pelo chamador,
 * mas não as formas contidas nela.
 */
Lista getFormasVisiveis(
    ContextoVisibilidade C
);

/**
 * Libera completamente o contexto e toda memória interna alocada.
 */
void liberaContextoVisibilidade(
    ContextoVisibilidade C
);

#endif
