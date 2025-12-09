#include "trata_geo.h"
#include "trata_qry.h"

#include "leitor_arquivos.h"
#include "circulo.h"
#include "linha.h"
#include "retangulo.h"
#include "texto.h"
#include "text_style.h"

#include "trata_argumentos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {

    // Verifica se há argumentos demais
    if (argc > 14) { 
        printf("Erro, muitos argumentos!\n");
        exit(1);
    }

    // Obtém os caminhos e opções passadas na linha de comando
    char *caminho_output  = obter_valor_opcao(argc, argv, "o");
    char *caminho_geo     = obter_valor_opcao(argc, argv, "f");
    char *caminho_prefixo = obter_valor_opcao(argc, argv, "e");
    char *caminho_qry     = obter_valor_opcao(argc, argv, "q");
    char *comando_sufixo  = obter_sufixo_comando(argc, argv);

   
    char *caminho_completo_geo    = NULL;
    char *caminho_completo_qry    = NULL;

    // Caso o prefixo tenha sido especificado (-e)
    if (caminho_prefixo != NULL) {

        size_t tamanho_prefixo = strlen(caminho_prefixo);
        int falta_barra = (tamanho_prefixo > 0 && caminho_prefixo[tamanho_prefixo - 1] != '/');

        // Monta o caminho completo do arquivo GEO
        if (caminho_geo != NULL) {
            size_t geo_tamanho = strlen(caminho_geo);
            caminho_completo_geo = (char *) malloc(tamanho_prefixo + geo_tamanho + 2);

            if (falta_barra)
                sprintf(caminho_completo_geo, "%s/%s", caminho_prefixo, caminho_geo);
            else
                sprintf(caminho_completo_geo, "%s%s", caminho_prefixo, caminho_geo);

            caminho_geo = caminho_completo_geo;
        }

        

        // Monta o caminho completo do arquivo QRY
        if (caminho_qry != NULL) {
            size_t qry_tamanho = strlen(caminho_qry);
            caminho_completo_qry = (char *) malloc(tamanho_prefixo + qry_tamanho + 2);

            if (falta_barra)
                sprintf(caminho_completo_qry, "%s/%s", caminho_prefixo, caminho_qry);
            else
                sprintf(caminho_completo_qry, "%s%s", caminho_prefixo, caminho_qry);

            caminho_qry = caminho_completo_qry;
        }
    }

   
    if (caminho_geo == NULL || caminho_output == NULL) {
        printf("Erro: -o e -f são obrigatórios\n");
        exit(1);
    }

    
    DadosDoArquivo arqGeo = criar_dados_arquivo(caminho_geo);
    if (arqGeo == NULL) {
        printf("Erro na criação de dados do arquivo GEO\n");
        exit(1);
    }

    
    Cidade cidade = executa_comando_geo(arqGeo, caminho_output, comando_sufixo);

   
    int maior_id_geo = get_maior_id_geo(cidade);

    
    char *tipo_sort_str = obter_valor_opcao(argc, argv, "to");
    char *threshold_str = obter_valor_opcao(argc, argv, "i");
    
    char tipo_sort = 'q'; 
    if (tipo_sort_str != NULL && (strcmp(tipo_sort_str, "m") == 0 || strcmp(tipo_sort_str, "q") == 0)) {
        tipo_sort = tipo_sort_str[0];
    }
    
    int threshold = 10; 
    if (threshold_str != NULL) {
        threshold = atoi(threshold_str);
    }

    
    if (caminho_qry != NULL) {
        DadosDoArquivo arqQry = criar_dados_arquivo(caminho_qry);

        if (arqQry == NULL) {
            printf("Erro na criação de dados do arquivo QRY\n");
            desaloca_geo(cidade);
            exit(1);
        }

        printf("\n=== Processando arquivo QRY ===\n");
        
        Qry qry = executa_comando_qry(arqQry, cidade, caminho_output, maior_id_geo, tipo_sort, threshold);
        printf("=== Processamento QRY concluído ===\n\n");

        destruir_dados_arquivo(arqQry);
        desaloca_qry(qry);
    }

    
    destruir_dados_arquivo(arqGeo);
    desaloca_geo(cidade);

   
    if (caminho_completo_geo != NULL)
        free(caminho_completo_geo);

    

    if (caminho_completo_qry != NULL)
        free(caminho_completo_qry);

    return 0;
}