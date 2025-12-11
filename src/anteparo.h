#ifndef ANTEPARO_H
#define ANTEPARO_H

#include "forma.h"

typedef void* Anteparo;

/**
 * @brief Transforma uma forma em anteparo
 * @param forma_original Forma a ser transformada (deve ser Forma_t*)
 * @param h_ou_v Orientação para círculos ('h' ou 'v')
 * @param maior_id ID a ser usado para o anteparo
 * @return Anteparo criado
 */
Anteparo transforma_em_anteparo(void* forma_original, char h_ou_v, int maior_id);

/**
 * @brief Transforma um retângulo em 4 anteparos (um para cada lado)
 * @param forma_original Forma retângulo a ser transformada
 * @param maior_id Ponteiro para o maior ID (será incrementado)
 * @param anteparos Array para armazenar os 4 anteparos criados
 * @return Primeiro anteparo criado
 */
Anteparo transforma_retangulo_em_anteparos(void* forma_original, int *maior_id, Anteparo *anteparos);

/**
 * @brief Obtém o ID do anteparo
 */
int getIDAnteparo(Anteparo a);

/**
 * @brief Obtém coordenada x1 do anteparo
 */
float getX1Anteparo(Anteparo a);

/**
 * @brief Obtém coordenada x2 do anteparo
 */
float getX2Anteparo(Anteparo a);

/**
 * @brief Obtém coordenada y1 do anteparo
 */
float getY1Anteparo(Anteparo a);

/**
 * @brief Obtém coordenada y2 do anteparo
 */
float getY2Anteparo(Anteparo a);

/**
 * @brief Obtém a cor do anteparo
 */
char* getCorAnteparo(Anteparo a);

/**
 * @brief Define a cor do anteparo
 */
void setCorAnteparo(Anteparo a, const char* nova_cor);

/**
 * @brief Cria um clone do anteparo com novo ID e deslocamento.
 */
Anteparo clonaAnteparo(Anteparo a, int novo_id, float dx, float dy);

/**
 * @brief Desaloca um anteparo
 */
void desalocaAnteparo(Anteparo a);

#endif