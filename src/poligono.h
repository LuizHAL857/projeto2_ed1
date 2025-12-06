#ifndef POLIGONO_H
#define POLIGONO_H

#include <stdbool.h>
#include "lista.h"
#include "ponto.h"

/**
 * @brief Tipo opaco para representar um polígono.
 */
typedef void* Poligono;

/**
 * @brief Tipo opaco para representar um segmento de reta.
 */
typedef void* Segmento;

/**
 * @brief Tipo opaco para representar um bounding box.
 */
typedef void* BoundingBox;

/**
 * @brief Forward declaration para Forma (definido em forma.h).
 */
typedef void* Forma;

/* ================= Segmento - Criação e acesso ================= */

/**
 * @brief Cria um novo segmento.
 * 
 * @param p1 Primeiro ponto do segmento.
 * @param p2 Segundo ponto do segmento.
 * @return Ponteiro para o segmento criado.
 */
Segmento criaSegmento(Ponto p1, Ponto p2);

/**
 * @brief Libera a memória de um segmento.
 * 
 * @param s Ponteiro para o segmento.
 */
void liberaSegmento(Segmento s);

/**
 * @brief Obtém o primeiro ponto do segmento.
 * 
 * @param s Ponteiro para o segmento.
 * @return Primeiro ponto.
 */
Ponto getSegmentoP1(Segmento s);

/**
 * @brief Obtém o segundo ponto do segmento.
 * 
 * @param s Ponteiro para o segmento.
 * @return Segundo ponto.
 */
Ponto getSegmentoP2(Segmento s);

/* ================= BoundingBox - Criação e acesso ================= */

/**
 * @brief Cria um novo bounding box.
 * 
 * @param min_x Coordenada x mínima.
 * @param min_y Coordenada y mínima.
 * @param max_x Coordenada x máxima.
 * @param max_y Coordenada y máxima.
 * @return Ponteiro para o bounding box criado.
 */
BoundingBox criaBoundingBox(float min_x, float min_y, float max_x, float max_y);

/**
 * @brief Libera a memória de um bounding box.
 * 
 * @param bb Ponteiro para o bounding box.
 */
void liberaBoundingBox(BoundingBox bb);

/**
 * @brief Obtém a coordenada x mínima do bounding box.
 * 
 * @param bb Ponteiro para o bounding box.
 * @return Coordenada x mínima.
 */
float getBBMinX(BoundingBox bb);

/**
 * @brief Obtém a coordenada y mínima do bounding box.
 * 
 * @param bb Ponteiro para o bounding box.
 * @return Coordenada y mínima.
 */
float getBBMinY(BoundingBox bb);

/**
 * @brief Obtém a coordenada x máxima do bounding box.
 * 
 * @param bb Ponteiro para o bounding box.
 * @return Coordenada x máxima.
 */
float getBBMaxX(BoundingBox bb);

/**
 * @brief Obtém a coordenada y máxima do bounding box.
 * 
 * @param bb Ponteiro para o bounding box.
 * @return Coordenada y máxima.
 */
float getBBMaxY(BoundingBox bb);


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

/**
 * @brief Verifica se uma forma intersecta ou está dentro de um polígono.
 * 
 * Esta função realiza verificações geométricas precisas para determinar se
 * há qualquer interseção entre a forma e o polígono, incluindo:
 * - Teste de bounding box
 * - Vértices da forma dentro do polígono
 * - Vértices do polígono dentro da forma
 * - Interseção de arestas
 * 
 * @param p Ponteiro para o polígono.
 * @param f Ponteiro para a forma.
 * @return true se há interseção ou contenção, false caso contrário.
 */
bool formaIntersectaPoligono(Poligono p, Forma f);

#endif
