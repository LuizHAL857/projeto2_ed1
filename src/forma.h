#ifndef FORMA_H
#define FORMA_H

#include <stdio.h>
#include <stdbool.h>
#include "poligono.h"
/**
 * @file forma.h
 * @brief Módulo genérico para gerenciamento de formas geométricas.
 *
 * Este módulo fornece uma interface unificada para trabalhar com diferentes
 * tipos de formas geométricas (círculos, retângulos, linhas, textos e estilos de texto).
 * Ele encapsula os detalhes de cada tipo de forma e fornece funções genéricas
 * para criação, acesso, renderização SVG e desalocação.
 */

/**
 * @enum tipo_forma
 * @brief Enumeração dos tipos de formas suportados.
 */
typedef enum {
    CIRCLE,      ///< Círculo
    RECTANGLE,   ///< Retângulo
    LINE,        ///< Linha
    TEXT,        ///< Texto
    TEXT_STYLE,  ///< Estilo de texto
    ANTEPARO     ///< Anteparo
} tipo_forma;

/**
 * @typedef Forma
 * @brief Tipo opaco que representa uma forma genérica.
 * 
 * Este tipo encapsula qualquer tipo de forma geométrica junto com
 * informações sobre seu tipo específico.
 */
typedef void* Forma;

/**
 * @brief Cria uma nova forma genérica.
 * 
 * Esta função encapsula um ponteiro de dados de forma específica
 * (CIRCULO, RETANGULO, LINHA, TEXTO ou TEXTSTYLE) em uma estrutura
 * genérica que mantém o tipo da forma.
 * 
 * @param tipo O tipo da forma (CIRCLE, RECTANGLE, LINE, TEXT, TEXT_STYLE, ANTEPARO).
 * @param data Ponteiro para os dados da forma específica (já alocados).
 * @return Forma Ponteiro para a forma genérica criada, ou NULL em caso de erro.
 * 
 * @note O ponteiro 'data' deve ser um ponteiro válido para uma forma já criada
 *       usando as funções específicas (criaCirculo, criaRetangulo, etc.).
 * @note A propriedade dos dados é transferida para a Forma. Quando a Forma
 *       for desalocada, os dados também serão liberados.
 */
Forma criaForma(tipo_forma tipo, void* data);

/**
 * @brief Retorna o tipo da forma.
 * 
 * @param f Ponteiro para a forma.
 * @return tipo_forma O tipo da forma (CIRCLE, RECTANGLE, LINE, TEXT, TEXT_STYLE, ANTEPARO).
 * 
 * @note Se f for NULL, o comportamento é indefinido.
 */
tipo_forma getTipoForma(Forma f);

/**
 * @brief Retorna o ponteiro para os dados internos da forma.
 * 
 * Esta função permite acessar o ponteiro void* que contém os dados
 * específicos da forma (CIRCULO, RETANGULO, LINHA, TEXTO ou TEXTSTYLE).
 * 
 * @param f Ponteiro para a forma.
 * @return void* Ponteiro para os dados da forma específica.
 * 
 * @note O ponteiro retornado deve ser convertido para o tipo apropriado
 *       com base no tipo retornado por getTipoForma().
 * @note Não libere este ponteiro diretamente; use desalocaForma() para
 *       liberar toda a forma.
 */
void* getDataForma(Forma f);

/**
 * @brief Escreve a representação SVG da forma em um arquivo.
 * 
 * Esta função renderiza a forma no formato SVG apropriado de acordo
 * com seu tipo e escreve no arquivo fornecido.
 * 
 * @param f Ponteiro para a forma.
 * @param arquivo Ponteiro para o arquivo onde o SVG será escrito.
 * 
 * @note Se f ou arquivo forem NULL, a função não faz nada.
 * @note O arquivo deve estar aberto para escrita antes de chamar esta função.
 */
void escreveFormaSVG(Forma f, FILE* arquivo);

/**
 * @brief Desaloca completamente uma forma e seus dados.
 * 
 * Esta função libera toda a memória associada à forma, incluindo
 * os dados específicos da forma (chamando a função de desalocação
 * apropriada para cada tipo).
 * 
 * @param f Ponteiro para a forma a ser desalocada.
 * 
 * @note Após chamar esta função, o ponteiro f não deve mais ser usado.
 * @note Se f for NULL, a função não faz nada.
 */
void desalocaForma(Forma f);



/**
 * @brief Calcula o Bounding Box de uma forma.
 * 
 * @param f Ponteiro para a forma.
 * @return BoundingBox da forma.
 */
BoundingBox getBBForma(Forma f);

/**
 * @brief Verifica se uma forma está totalmente dentro de um polígono.
 * 
 * @param p Ponteiro para o polígono.
 * @param f Ponteiro para a forma.
 * @return true se a forma está dentro, false caso contrário.
 */
bool formaEstaDentro(Poligono p, Forma f);

/**
 * @brief Clona uma forma com novo ID e deslocamento.
 * 
 * @param f Ponteiro para a forma a ser clonada.
 * @param novo_id Novo ID para o clone.
 * @param dx Deslocamento em x.
 * @param dy Deslocamento em y.
 * @return Ponteiro para a forma clonada, ou NULL em caso de erro.
 */
Forma clonaForma(Forma f, int novo_id, float dx, float dy);

/**
 * @brief Define a cor de preenchimento de uma forma.
 * 
 * @param f Ponteiro para a forma.
 * @param cor String com a cor de preenchimento.
 */
void setCorPForma(Forma f, const char* cor);

/**
 * @brief Define a cor de borda de uma forma.
 * 
 * @param f Ponteiro para a forma.
 * @param cor String com a cor de borda.
 */
void setCorBForma(Forma f, const char* cor);

#endif
