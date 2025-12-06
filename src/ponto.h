#ifndef PONTO_H
#define PONTO_H

/**
 * @brief Tipo opaco para representar um ponto 2D.
 */
typedef void* Ponto;

/**
 * @brief Cria um novo ponto.
 * 
 * @param x Coordenada x.
 * @param y Coordenada y.
 * @return Ponteiro para o ponto criado.
 */
Ponto criaPonto(float x, float y);

/**
 * @brief Libera a memória de um ponto.
 * 
 * @param p Ponteiro para o ponto.
 */
void liberaPonto(Ponto p);

/**
 * @brief Obtém a coordenada x de um ponto.
 * 
 * @param p Ponteiro para o ponto.
 * @return Coordenada x.
 */
float getPontoX(Ponto p);

/**
 * @brief Obtém a coordenada y de um ponto.
 * 
 * @param p Ponteiro para o ponto.
 * @return Coordenada y.
 */
float getPontoY(Ponto p);

/**
 * @brief Define a coordenada x de um ponto.
 * 
 * @param p Ponteiro para o ponto.
 * @param x Nova coordenada x.
 */
void setPontoX(Ponto p, float x);

/**
 * @brief Define a coordenada y de um ponto.
 * 
 * @param p Ponteiro para o ponto.
 * @param y Nova coordenada y.
 */
void setPontoY(Ponto p, float y);

#endif
