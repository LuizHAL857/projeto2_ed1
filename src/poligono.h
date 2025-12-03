#ifndef POLIGONO_H
#define POLIGONO_H

#include <stdbool.h>
#include "lista.h"

/**
 * @brief Tipo opaco para representar um polígono.
 */
typedef void* Poligono;

/**
 * @brief Estrutura para representar um ponto 2D.
 */
typedef struct {
    float x;
    float y;
} Ponto;

/**
 * @brief Estrutura para representar um segmento de reta.
 */
typedef struct {
    Ponto p1;
    Ponto p2;
} Segmento;

/**
 * @brief Estrutura para representar um bounding box.
 */
typedef struct {
    float min_x;
    float min_y;
    float max_x;
    float max_y;
} BoundingBox;

/* ================= Criação e liberação ================= */

/**
 * @brief Cria um novo polígono vazio.
 * 
 * @return Ponteiro para o polígono criado.
 */
Poligono criaPoligono();

/**
 * @brief Libera a memória alocada para o polígono.
 * 
 * @param p Ponteiro para o polígono a ser liberado.
 */
void liberaPoligono(Poligono p);

/* ================= Inserção ================= */

/**
 * @brief Insere um vértice no polígono.
 * 
 * @param p Ponteiro para o polígono.
 * @param ponto Ponto a ser inserido.
 */
void insereVertice(Poligono p, Ponto ponto);

/**
 * @brief Insere um segmento no polígono.
 * 
 * @param p Ponteiro para o polígono.
 * @param seg Segmento a ser inserido.
 */
void insereSegmento(Poligono p, Segmento seg);

/* ================= Consultas ================= */

/**
 * @brief Calcula e retorna o bounding box do polígono.
 * 
 * @param p Ponteiro para o polígono.
 * @return BoundingBox contendo os limites do polígono.
 */
BoundingBox getBoundingBox(Poligono p);

/**
 * @brief Verifica se um ponto está dentro do polígono.
 * 
 * Usa o algoritmo de ray casting para determinar se o ponto
 * está dentro do polígono.
 * 
 * @param p Ponteiro para o polígono.
 * @param ponto Ponto a ser testado.
 * @return true se o ponto está dentro, false caso contrário.
 */
bool isInside(Poligono p, Ponto ponto);

/**
 * @brief Retorna a lista de vértices do polígono.
 * 
 * A lista retornada contém ponteiros para Ponto.
 * O chamador não deve liberar os pontos, apenas a lista.
 * 
 * @param p Ponteiro para o polígono.
 * @return Lista de vértices (Ponto*).
 */
Lista getVertices(Poligono p);

/**
 * @brief Retorna a lista de segmentos do polígono.
 * 
 * A lista retornada contém ponteiros para Segmento.
 * O chamador não deve liberar os segmentos, apenas a lista.
 * 
 * @param p Ponteiro para o polígono.
 * @return Lista de segmentos (Segmento*).
 */
Lista getSegmentos(Poligono p);

/**
 * @brief Retorna o número de vértices do polígono.
 * 
 * @param p Ponteiro para o polígono.
 * @return Número de vértices.
 */
int getNumVertices(Poligono p);

/**
 * @brief Retorna o número de segmentos do polígono.
 * 
 * @param p Ponteiro para o polígono.
 * @return Número de segmentos.
 */
int getNumSegmentos(Poligono p);

/**
 * @brief Verifica se dois bounding boxes se intersectam.
 * 
 * @param a Primeiro bounding box.
 * @param b Segundo bounding box.
 * @return true se intersectam, false caso contrário.
 */
bool haInterseccaoBB(BoundingBox a, BoundingBox b);

#endif
