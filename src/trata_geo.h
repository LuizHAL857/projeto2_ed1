#ifndef TRATA_GEO_H
#define TRATA_GEO_H

#include "lista.h"
#include "leitor_arquivos.h" 

/**
  Módulo responsável por interpretar e executar comandos do arquivo `.geo`, 
 *        armazenando as formas e gerando a saída em SVG.
 * 
 * Esse módulo processa o conteúdo lido de arquivos `.geo`, cria objetos geométricos 
 * correspondentes (círculo, retângulo, linha), e os armazena 
 * em estruturas auxiliares. Ele também é responsável por gerar a saída gráfica em formato SVG.
 */

/**
 * @typedef Cidade
 * @brief Representa um contexto interno onde todas as formas geométricas e estilos são armazenados.
 * 
 */
typedef void* Cidade;

/**
 * @brief Processa os comandos presentes no arquivo `.geo`.
 * 
 * Essa função percorre cada linha do arquivo `.geo`, interpreta os comandos (como "c", "r", "l", "t", "ts"),
 * e armazena as formas correspondentes nas filas e pilhas internas. Ao final, gera um arquivo `.svg`
 * com os elementos gráficos resultantes.
 * 
 * @param fileData Estrutura contendo os dados lidos do arquivo `.geo`, incluindo suas linhas.
 * @param caminho_output Caminho para o diretório onde o arquivo SVG de saída será criado.
 * @param sufixo_comando Sufixo a ser adicionado ao nome do arquivo de saída SVG, antes da extensão.
 * 
 * @return Um ponteiro opaco para o contexto (Cidade) contendo todas as formas e estruturas alocadas.
 *         Esse ponteiro deve ser usado para operações posteriores e precisa ser desalocado com `desaloca_geo`.
 */
Cidade executa_comando_geo(DadosDoArquivo fileData, char *caminho_output,  char *sufixo_comando);

/**
 * @brief Retorna a fila contendo todas as formas geométricas criadas no contexto `Cidade`.
 * 
 * Essa fila pode ser usada para processamento adicional ou visualização das formas
 * fora do módulo `trata_geo`.
 * 
 * @param cidade Contexto de execução retornado por `executa_comando_geo`.
 * @return Fila com os elementos gráficos processados.
 */
Lista get_lista_cidade(Cidade cidade);

/**
 * @brief Retorna a lista de formas para SVG
 */
Lista get_lista_svg_cidade(Cidade cidade);

/**
 * @brief Libera toda a memória alocada para o contexto `Cidade`.
 * 
 * Essa função desaloca todas as formas geométricas, estilos de texto, bem como
 * as estruturas auxiliares (fila, pilha) utilizadas durante o processamento.
 * 
 * @param cidade Contexto a ser desalocado, retornado por `executa_comando_geo`.
 */
void desaloca_geo(Cidade cidade);

/** *


    Obtem a lista para liberar formas da cidade

    @param Cidade cidade- A cidade do qual se vai obter a lista para liberar formas

    @return A pilha para liberar formas


*/
Lista obtem_lista_para_desalocar(Cidade cidade); 

/**
 * @brief Retorna o maior ID de forma processado durante a leitura do arquivo `.geo`.
 * 
 * Essa função permite obter o maior identificador numérico encontrado entre todas
 * as formas geométricas (círculos, retângulos, linhas, textos) processadas.
 * Útil para análises posteriores ou para garantir IDs únicos em operações futuras.
 * 
 * @param cidade Contexto de execução retornado por `executa_comando_geo`.
 * @return O maior ID encontrado, ou 0 se nenhuma forma com ID foi processada.
 */
int get_maior_id_geo(Cidade cidade);

#endif 