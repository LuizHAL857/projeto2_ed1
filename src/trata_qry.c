#include "trata_qry.h"
#include "lista.h"
#include "leitor_arquivos.h"
#include "forma.h"
#include "circulo.h"
#include "retangulo.h"
#include "linha.h"
#include "texto.h"
#include "anteparo.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief Estrutura interna que representa o contexto de processamento do .qry
 */
typedef struct {
    Cidade cidade;
    Lista comandos_executados;
    int maior_id_atual;
    char* caminho_output;
    FILE* txt_file;
    char tipo_sort;
    int threshold;
} Qry_t;

// Declarações forward das funções estáticas
static void executa_comando_anteparo(Qry_t *qry, char *linha);
static void executa_comando_destruicao(Qry_t *qry, char *linha);
static void executa_comando_pintura(Qry_t *qry, char *linha);
static void executa_comando_clonagem(Qry_t *qry, char *linha);
static void cria_svg_qry(Qry_t *qry, DadosDoArquivo fileData);


/**
 * @brief Função principal que processa o arquivo .qry
 */
Qry executa_comando_qry(DadosDoArquivo fileData, Cidade cidade, 
                        char *caminho_output, int maior_id_inicial, char tipo_sort, int threshold) {
    
    Qry_t *qry = malloc(sizeof(Qry_t));
    
    if (qry == NULL) {
        printf("Erro de alocação para Qry\n");
        exit(1);
    }
    
    qry->cidade = cidade;
    qry->comandos_executados = criaLista();
    qry->maior_id_atual = maior_id_inicial;
    qry->tipo_sort = tipo_sort;
    qry->threshold = threshold;
    
    // Copia o caminho de output
    qry->caminho_output = malloc(strlen(caminho_output) + 1);
    if (qry->caminho_output == NULL) {
        printf("Erro de alocação para caminho_output\n");
        exit(1);
    }
    strcpy(qry->caminho_output, caminho_output);
    
    // Abre arquivo TXT de saída
    char *nome_orig = obter_nome_arquivo(fileData);
    char *nome_base = strrchr(nome_orig, '/');
    if(nome_base) nome_base++; else nome_base = nome_orig;
    
    char *nome_arq = malloc(strlen(nome_base) + 5);
    if(nome_arq) {
        strcpy(nome_arq, nome_base);
        char *dot = strrchr(nome_arq, '.');
        if(dot) *dot = '\0';
        strcat(nome_arq, ".txt");
        
        char *path = malloc(strlen(caminho_output) + strlen(nome_arq) + 2);
        if(path) {
            sprintf(path, "%s/%s", caminho_output, nome_arq);
            qry->txt_file = fopen(path, "w");
            if (qry->txt_file == NULL) {
                printf("Erro ao criar arquivo TXT: %s\n", path);
            }
            free(path);
        }
        free(nome_arq);
    } else {
        qry->txt_file = NULL;
    }
    
    // Processa cada linha do arquivo .qry
    while (!listaVazia(obter_lista_linhas(fileData))) {
        char *linha = (char*)removeInicioLista(obter_lista_linhas(fileData));
        char *linha_copia = malloc(strlen(linha) + 1);
        
        if (linha_copia == NULL) {
            printf("Erro de alocação para linha_copia\n");
            exit(1);
        }
        strcpy(linha_copia, linha);
        
        char *comando = strtok(linha, " ");
        
        if (comando == NULL) {
            free(linha_copia);
            continue;
        }
        
        printf("Processando comando: %s\n", comando);
        
        if (strcmp(comando, "a") == 0) {
            executa_comando_anteparo(qry, linha_copia);
            
        } else if (strcmp(comando, "d") == 0) {
            executa_comando_destruicao(qry, linha_copia);
            
        } else if (strcmp(comando, "p") == 0) {
            executa_comando_pintura(qry, linha_copia);
            
        } else if (strcmp(comando, "cln") == 0) {
            executa_comando_clonagem(qry, linha_copia);
            
        } else {
            printf("Comando inválido: %s\n", comando);
        }
        
        free(linha_copia);
    }
    
    // Generate SVG output showing final city state
    cria_svg_qry(qry, fileData);
    
    return qry;
}


static void executa_comando_anteparo(Qry_t *qry, char *linha) {
    char *comando = strtok(linha, " ");
    char *id_inicio = strtok(NULL, " ");
    char *id_fim = strtok(NULL, " ");
    char *orientacao = strtok(NULL, " \n");
    
    if (id_inicio == NULL || id_fim == NULL || orientacao == NULL) {
        printf("Erro: comando 'a' com parâmetros inválidos\n");
        return;
    }
    
    int i = atoi(id_inicio);
    int j = atoi(id_fim);
    char h_ou_v = orientacao[0];
    
    
    
    Lista lista_formas = get_lista_cidade(qry->cidade);
    Lista lista_svg = get_lista_svg_cidade(qry->cidade);
    Lista lista_free = obtem_lista_para_desalocar(qry->cidade);
    
    Lista to_remove = criaLista();
    Lista to_add = criaLista();
    
    Celula aux = getInicioLista(lista_formas);
    while(aux != NULL) {
        Forma f = getConteudoCelula(aux);
        tipo_forma tipo = getTipoForma(f);
        void* data = getDataForma(f);
        int id = -1;
        
        switch(tipo) {
            case CIRCLE: id = getIDCirculo(data); break;
            case RECTANGLE: id = getIDRetangulo(data); break;
            case LINE: id = getIDLinha(data); break;
            case TEXT: id = getIDTexto(data); break;
            default: break;
        }
        
        if (id >= i && id <= j) {
          
            if (tipo == RECTANGLE) {
                Anteparo anteparos[4];
                transforma_retangulo_em_anteparos(f, &qry->maior_id_atual, anteparos);
                
                if (qry->txt_file) fprintf(qry->txt_file, "Retangulo ID %d transformado em anteparos:\n", id);
                
                for(int k=0; k<4; k++) {
                    if (anteparos[k] != NULL) {
                        Forma novo = criaForma(ANTEPARO, anteparos[k]);
                        insereFinalLista(to_add, novo);
                        
                        if (qry->txt_file) {
                            fprintf(qry->txt_file, "  Anteparo ID %d: (%.2f, %.2f) -> (%.2f, %.2f)\n", 
                                getIDAnteparo(anteparos[k]), 
                                getX1Anteparo(anteparos[k]), getY1Anteparo(anteparos[k]),
                                getX2Anteparo(anteparos[k]), getY2Anteparo(anteparos[k]));
                        }
                    }
                }
            } else {
                Anteparo a = transforma_em_anteparo(f, h_ou_v, ++qry->maior_id_atual);
                if (a != NULL) {
                    Forma novo = criaForma(ANTEPARO, a);
                    insereFinalLista(to_add, novo);
                    
                    if (qry->txt_file) {
                        char *tipo_str = (tipo==CIRCLE?"Circulo":(tipo==LINE?"Linha":"Texto"));
                        fprintf(qry->txt_file, "%s ID %d transformado em anteparo ID %d: (%.2f, %.2f) -> (%.2f, %.2f)\n", 
                            tipo_str, id,
                            getIDAnteparo(a), getX1Anteparo(a), getY1Anteparo(a), 
                            getX2Anteparo(a), getY2Anteparo(a));
                    }
                }
            }
            insereFinalLista(to_remove, f);
        }
        aux = getProxCelula(aux);
    }
    
    
    while(!listaVazia(to_remove)) {
        Forma f = removeInicioLista(to_remove);
        removeElementoLista(lista_formas, f);
        removeElementoLista(lista_svg, f);
        removeElementoLista(lista_free, f);
        desalocaForma(f);
    }
    liberaLista(to_remove);
    
   
    while(!listaVazia(to_add)) {
        Forma f = removeInicioLista(to_add);
        insereFinalLista(lista_formas, f);
        insereFinalLista(lista_svg, f);
        insereFinalLista(lista_free, f);
    }
    liberaLista(to_add);
}


static void executa_comando_destruicao(Qry_t *qry, char *linha) {
    char *comando = strtok(linha, " ");
    char *x_str = strtok(NULL, " ");
    char *y_str = strtok(NULL, " ");
    char *sufixo = strtok(NULL, " \n");
    
    if (x_str == NULL || y_str == NULL || sufixo == NULL) {
        printf("Erro: comando 'd' com parâmetros inválidos\n");
        return;
    }
    
    float x = atof(x_str);
    float y = atof(y_str);
    
    printf("  Comando DESTRUIÇÃO: x=%.2f, y=%.2f, sufixo=%s\n", x, y, sufixo);
    
    
    (void)qry;
}


static void executa_comando_pintura(Qry_t *qry, char *linha) {
    char *comando = strtok(linha, " ");
    char *x_str = strtok(NULL, " ");
    char *y_str = strtok(NULL, " ");
    char *cor = strtok(NULL, " ");
    char *sufixo = strtok(NULL, " \n");
    
    if (x_str == NULL || y_str == NULL || cor == NULL || sufixo == NULL) {
        printf("Erro: comando 'p' com parâmetros inválidos\n");
        return;
    }
    
    float x = atof(x_str);
    float y = atof(y_str);
    
    printf("  Comando PINTURA: x=%.2f, y=%.2f, cor=%s, sufixo=%s\n", x, y, cor, sufixo);
    
    
    (void)qry;
}


static void executa_comando_clonagem(Qry_t *qry, char *linha) {
    char *comando = strtok(linha, " ");
    char *x_str = strtok(NULL, " ");
    char *y_str = strtok(NULL, " ");
    char *dx_str = strtok(NULL, " ");
    char *dy_str = strtok(NULL, " ");
    char *sufixo = strtok(NULL, " \n");
    
    if (x_str == NULL || y_str == NULL || dx_str == NULL || 
        dy_str == NULL || sufixo == NULL) {
        printf("Erro: comando 'cln' com parâmetros inválidos\n");
        return;
    }
    
    float x = atof(x_str);
    float y = atof(y_str);
    float dx = atof(dx_str);
    float dy = atof(dy_str);
    
    printf("  Comando CLONAGEM: x=%.2f, y=%.2f, dx=%.2f, dy=%.2f, sufixo=%s\n", 
           x, y, dx, dy, sufixo);
    
  
    (void)qry;
}


/**
 * @brief Cria o arquivo SVG com o estado final da cidade após processamento do .qry
 */
static void cria_svg_qry(Qry_t *qry, DadosDoArquivo fileData) {
    // Extrai o nome base do arquivo
    char *nome_orig = obter_nome_arquivo(fileData);
    char *nome_base = strrchr(nome_orig, '/');
    if(nome_base) nome_base++; else nome_base = nome_orig;
    
    // Cria nome do arquivo SVG
    size_t name_len = strlen(nome_base);
    char *nome_arquivo = malloc(name_len + 1);
    if (nome_arquivo == NULL) {
        printf("Erro de alocação\n");
        return;
    }
    
    strcpy(nome_arquivo, nome_base);
    char *dot = strrchr(nome_arquivo, '.');
    if (dot) *dot = '\0';
    
    // Monta o caminho completo do arquivo SVG
    size_t caminho_len = strlen(qry->caminho_output);
    size_t nome_len_final = strlen(nome_arquivo);
    size_t total_len = caminho_len + 1 + nome_len_final + 5; // +5 para ".svg\0"
    
    char *caminho_output_arquivo = malloc(total_len);
    if (caminho_output_arquivo == NULL) {
        printf("Erro de alocação\n");
        free(nome_arquivo);
        return;
    }
    
    snprintf(caminho_output_arquivo, total_len, "%s/%s.svg", qry->caminho_output, nome_arquivo);
    
    // Abre o arquivo SVG para escrita
    FILE *file = fopen(caminho_output_arquivo, "w");
    if (file == NULL) {
        printf("Erro ao criar arquivo SVG: %s\n", caminho_output_arquivo);
        free(caminho_output_arquivo);
        free(nome_arquivo);
        return;
    }
    
    // Escreve o cabeçalho SVG
    fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(file, "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 1000 1000\">\n");
    
    // Cria uma lista temporária para renderização
    Lista lista_svg_temp = criaLista();
    
    // Copia todas as formas da lista_formas para lista_svg_temp
    Lista lista_formas = get_lista_cidade(qry->cidade);
    Celula aux = getInicioLista(lista_formas);
    while (aux != NULL) {
        Forma f = getConteudoCelula(aux);
        insereFinalLista(lista_svg_temp, f);
        aux = getProxCelula(aux);
    }
    
    // Renderiza as formas em ordem reversa (usando removeFinalLista)
    while (!listaVazia(lista_svg_temp)) {
        Forma forma = removeFinalLista(lista_svg_temp);
        if (forma != NULL) {
            tipo_forma tipo = getTipoForma(forma);
            void* data = getDataForma(forma);
            
            if (tipo == CIRCLE) {
                CIRCULO c = (CIRCULO)data;
                fprintf(file, "<circle cx='%.2f' cy='%.2f' r='%.2f' fill='%s' stroke='%s'/>\n",
                    getXCirculo(c), getYCirculo(c), getRaioCirculo(c), 
                    getCorPCirculo(c), getCorBCirculo(c));
                    
            } else if (tipo == RECTANGLE) {
                RETANGULO r = (RETANGULO)data;
                fprintf(file, "<rect x='%.2f' y='%.2f' width='%.2f' height='%.2f' fill='%s' stroke='%s'/>\n",
                    getXRetangulo(r), getYRetangulo(r), getLarguraRetangulo(r), 
                    getAlturaRetangulo(r), getCorPRetangulo(r), getCorBRetangulo(r));
                    
            } else if (tipo == LINE) {
                LINHA l = (LINHA)data;
                fprintf(file, "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='%s'/>\n",
                    getX1Linha(l), getY1Linha(l), getX2Linha(l), getY2Linha(l), getCorLinha(l));
                    
            } else if (tipo == TEXT) {
                TEXTO t = (TEXTO)data;
                char ancora = getAncoraTexto(t);
                char *texto_ancora = "start";
                
                if (ancora == 'm' || ancora == 'M') {
                    texto_ancora = "middle";
                } else if (ancora == 'f' || ancora == 'F') {
                    texto_ancora = "end";
                } else if (ancora == 'i' || ancora == 'I') {
                    texto_ancora = "start";
                }
                
                fprintf(file, "<text x='%.2f' y='%.2f' fill='%s' stroke='%s' text-anchor='%s'>%s</text>\n",
                    getXTexto(t), getYTexto(t), getCorPTexto(t), 
                    getCorBTexto(t), texto_ancora, getTxtTexto(t));
                    
            } else if (tipo == ANTEPARO) {
                Anteparo a = (Anteparo)data;
                fprintf(file, "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='%s'/>\n",
                    getX1Anteparo(a), getY1Anteparo(a), getX2Anteparo(a), 
                    getY2Anteparo(a), getCorAnteparo(a));
            }
        }
    }
    
    // Fecha o SVG
    fprintf(file, "</svg>\n");
    fclose(file);
    
    // Libera memória
    liberaLista(lista_svg_temp);
    free(caminho_output_arquivo);
    free(nome_arquivo);
}


void desaloca_qry(Qry qry) {
    if (qry == NULL) {
        return;
    }
    
    Qry_t *qry_t = (Qry_t *)qry;
    
    if (qry_t->comandos_executados != NULL) {
        liberaLista(qry_t->comandos_executados);
    }
    
    if (qry_t->caminho_output != NULL) {
        free(qry_t->caminho_output);
    }
    
    if (qry_t->txt_file != NULL) {
        fclose(qry_t->txt_file);
    }
    
    free(qry_t);
}
