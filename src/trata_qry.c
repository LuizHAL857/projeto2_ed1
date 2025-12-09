#include "trata_qry.h"
#include "lista.h"
#include "leitor_arquivos.h"
#include "forma.h"
#include "circulo.h"
#include "retangulo.h"
#include "linha.h"
#include "texto.h"
#include "anteparo.h"
#include "visibilidade.h"
#include "poligono.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    Cidade cidade;
    DadosDoArquivo fileData;
    Lista comandos_executados;
    int maior_id_atual;
    char* caminho_output;
    FILE* txt_file;
    char tipo_sort;
    int threshold;
    Lista visibility_polygons; 
} Qry_t;

static void executa_comando_anteparo(Qry_t *qry, char *linha);
static void executa_comando_destruicao(Qry_t *qry, char *linha);
static void executa_comando_pintura(Qry_t *qry, char *linha);
static void executa_comando_clonagem(Qry_t *qry, char *linha);
static void cria_svg_qry(Qry_t *qry, DadosDoArquivo fileData);

Qry executa_comando_qry(DadosDoArquivo fileData, Cidade cidade, 
                        char *caminho_output, int maior_id_inicial, char tipo_sort, int threshold) {
    
    Qry_t *qry = malloc(sizeof(Qry_t));
    
    if (qry == NULL) {
        printf("Erro de alocação para Qry\n");
        exit(1);
    }
    
    qry->cidade = cidade;
    qry->fileData = fileData;
    qry->comandos_executados = criaLista();
    qry->maior_id_atual = maior_id_inicial;
    qry->tipo_sort = tipo_sort;
    qry->threshold = threshold;
    qry->visibility_polygons = criaLista(); // Inicializa lista de polígonos de visibilidade
    
   
    qry->caminho_output = malloc(strlen(caminho_output) + 1);
    if (qry->caminho_output == NULL) {
        printf("Erro de alocação para caminho_output\n");
        exit(1);
    }
    strcpy(qry->caminho_output, caminho_output);
    
   
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
    
    
    cria_svg_qry(qry, fileData);
    
    return qry;
}

static void executa_comando_anteparo(Qry_t *qry, char *linha) {
    strtok(linha, " ");
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

static Poligono calculaPoligonoVisibilidade(ContextoVisibilidade ctx, float x, float y) {
    // Obtém a região de visibilidade como lista de segmentos
    Lista segmentos_vis = getRegiaoVisibilidade(ctx);
    
    if (!segmentos_vis || listaVazia(segmentos_vis)) {
        return NULL;
    }
    
    // Cria polígono a partir dos segmentos
    
    Poligono regiao_visibilidade = criaPoligono();
    if (!regiao_visibilidade) return NULL;
    
    // Adiciona todos os pontos iniciais dos segmentos
    // Isso garante que toda a região visível seja coberta
    for (Celula c = getInicioLista(segmentos_vis); c; c = getProxCelula(c)) {
        SegmentoVisibilidade seg = getConteudoCelula(c);
        float x1, y1, x2, y2;
        getCoordenadasSegmentoVis(seg, &x1, &y1, &x2, &y2);
        
        Ponto p1 = criaPonto(x1, y1);
        insereVertice(regiao_visibilidade, p1);
        liberaPonto(p1);
    }
    
    return regiao_visibilidade;
}


static void destroiFormasEmColisao(Lista lista_formas, Poligono regiao_visibilidade, Qry_t *qry) {
    Lista lista_svg = get_lista_svg_cidade(qry->cidade);
    Lista lista_free = obtem_lista_para_desalocar(qry->cidade);
    BoundingBox bb_poly_orig = getBoundingBox(regiao_visibilidade);
    
    // Expande BB do polígono com uma margem de tolerância para garantir 
    // que as paredes que delimitam a visibilidade sejam capturadas
    float margem = 1.0f;
    BoundingBox bb_poly = criaBoundingBox(
        getBBMinX(bb_poly_orig) - margem,
        getBBMinY(bb_poly_orig) - margem,
        getBBMaxX(bb_poly_orig) + margem,
        getBBMaxY(bb_poly_orig) + margem
    );
    liberaBoundingBox(bb_poly_orig);
    
    
    Lista formas_para_destruir = criaLista();
    
    // Coleta formas a destruir
    for (Celula c = getInicioLista(lista_formas); c; c = getProxCelula(c)) {
        Forma f = getConteudoCelula(c);
        
        BoundingBox bb_forma = getBBForma(f);
        
        // Teste rápido: Bounding Box
        if (haInterseccaoBB(bb_poly, bb_forma)) {
            // Teste preciso: verifica se a forma intersecta o polígono
            if (formaIntersectaPoligono(regiao_visibilidade, f)) {
                insereFinalLista(formas_para_destruir, f);
            }
        }
        liberaBoundingBox(bb_forma);
    }


    liberaBoundingBox(bb_poly);
    
    // Destrói as formas coletadas
    int count = 0;
    while (!listaVazia(formas_para_destruir)) {
        Forma f = removeInicioLista(formas_para_destruir);
        tipo_forma tipo = getTipoForma(f);
        void* data = getDataForma(f);
        
        int id = -1;
        char *tipo_str = "Desconhecido";
        
        switch(tipo) {
            case CIRCLE: id = getIDCirculo(data); tipo_str = "Circulo"; break;
            case RECTANGLE: id = getIDRetangulo(data); tipo_str = "Retangulo"; break;
            case LINE: id = getIDLinha(data); tipo_str = "Linha"; break;
            case TEXT: id = getIDTexto(data); tipo_str = "Texto"; break;
            case ANTEPARO: id = getIDAnteparo(data); tipo_str = "Anteparo"; break;
            default: break;
        }
        
        if (qry->txt_file && id != -1) {
            fprintf(qry->txt_file, "  Destruído: %s ID %d\n", tipo_str, id);
        }
        
        removeElementoLista(lista_formas, f);
        removeElementoLista(lista_svg, f);
        removeElementoLista(lista_free, f);
        desalocaForma(f);
        count++;
    }
    
    liberaLista(formas_para_destruir);
    
    if (qry->txt_file) {
        fprintf(qry->txt_file, "Total de formas destruídas: %d\n", count);
    }
}

static void geraSVGVisibilidade(Poligono regiao_visibilidade, float x, float y, char* sufixo, Qry_t *qry) {
    if (strcmp(sufixo, "-") == 0) {
        // Adiciona o polígono de visibilidade à lista para renderizar no SVG principal 
        typedef struct {
            Poligono poligono;
            float bomb_x;
            float bomb_y;
        } VisibilityData;
        
        VisibilityData* vis_data = malloc(sizeof(VisibilityData));
        if (vis_data) {
            vis_data->poligono = regiao_visibilidade;
            vis_data->bomb_x = x;
            vis_data->bomb_y = y;
            insereFinalLista(qry->visibility_polygons, vis_data);
        }
        return;
    }

    char *nome_orig = obter_nome_arquivo(qry->fileData);
    char *nome_base = strrchr(nome_orig, '/');
    if(nome_base) nome_base++; else nome_base = nome_orig;
    
    char *nome_arq = malloc(strlen(nome_base) + strlen(sufixo) + 10);
    if (!nome_arq) return;

    strcpy(nome_arq, nome_base);
    char *dot = strrchr(nome_arq, '.');
    if(dot) *dot = '\0';
    
    strcat(nome_arq, "-");
    strcat(nome_arq, sufixo);
    strcat(nome_arq, ".svg");
    
    char *path = malloc(strlen(qry->caminho_output) + strlen(nome_arq) + 2);
    if (path) {
        sprintf(path, "%s/%s", qry->caminho_output, nome_arq);
        
        FILE *svg_file = fopen(path, "w");
        if (svg_file) {
            // Calcula o Bounding Box do polígono para definir o viewBox
            BoundingBox bb = getBoundingBox(regiao_visibilidade);
            
            // Adiciona margem ao viewBox
            float margem = 50.0f;
            float vb_x = getBBMinX(bb) - margem;
            float vb_y = getBBMinY(bb) - margem;
            float vb_w = (getBBMaxX(bb) - getBBMinX(bb)) + 2*margem;
            float vb_h = (getBBMaxY(bb) - getBBMinY(bb)) + 2*margem;
            liberaBoundingBox(bb);
            
            fprintf(svg_file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
            fprintf(svg_file, "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"%.2f %.2f %.2f %.2f\">\n",
                    vb_x, vb_y, vb_w, vb_h);
            
            fprintf(svg_file, "<polygon points=\"");
            Lista vertices = getVertices(regiao_visibilidade);
            for (Celula c = getInicioLista(vertices); c; c = getProxCelula(c)) {
                Ponto p = (Ponto)getConteudoCelula(c);
                fprintf(svg_file, "%.2f,%.2f ", getPontoX(p), getPontoY(p));
            }
            fprintf(svg_file, "\" fill=\"rgba(255,200,0,0.3)\" stroke=\"orange\" stroke-width=\"2\"/>\n");
            fprintf(svg_file, "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"5\" fill=\"red\"/>\n", x, y);
            fprintf(svg_file, "</svg>\n");
            fclose(svg_file);
        }
        free(path);
    }
    free(nome_arq);
}

static void executa_comando_destruicao(Qry_t *qry, char *linha) {
    strtok(linha, " ");
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
    
    Lista lista_formas = get_lista_cidade(qry->cidade);
    ContextoVisibilidade ctx = criaContextoVisibilidade(x, y, lista_formas, 
                                                   qry->tipo_sort, qry->threshold); 
    if (!ctx) {
        printf("Erro ao criar contexto de visibilidade\n");
        return;
    }
    
    Poligono regiao_visibilidade = calculaPoligonoVisibilidade(ctx, x, y);
    
    if (regiao_visibilidade) {

        if (qry->txt_file) {
            fprintf(qry->txt_file, "\nBomba de destruição em (%.2f, %.2f):\n", x, y);
        }
        
        destroiFormasEmColisao(lista_formas, regiao_visibilidade, qry);
        
        geraSVGVisibilidade(regiao_visibilidade, x, y, sufixo, qry);
        
        if (strcmp(sufixo, "-") != 0) {
            liberaPoligono(regiao_visibilidade);
        }
    }
    
    liberaContextoVisibilidade(ctx);
}

static void executa_comando_pintura(Qry_t *qry, char *linha) {
    strtok(linha, " ");
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
    

    Lista lista_formas = get_lista_cidade(qry->cidade);
    ContextoVisibilidade ctx = criaContextoVisibilidade(x, y, lista_formas, 
                                                        qry->tipo_sort, qry->threshold);
    
    if (!ctx) {
        printf("Erro ao criar contexto de visibilidade\n");
        return;
    }
    Poligono regiao_visibilidade = calculaPoligonoVisibilidade(ctx, x, y);
    
    if (regiao_visibilidade) {
        if (qry->txt_file) {
            fprintf(qry->txt_file, "\nBomba de pintura em (%.2f, %.2f) com cor %s:\n", x, y, cor);
        }
        
        BoundingBox bb_poly_orig = getBoundingBox(regiao_visibilidade);
        
        float margem = 1.0f;
        BoundingBox bb_poly = criaBoundingBox(
            getBBMinX(bb_poly_orig) - margem,
            getBBMinY(bb_poly_orig) - margem,
            getBBMaxX(bb_poly_orig) + margem,
            getBBMaxY(bb_poly_orig) + margem
        );
        liberaBoundingBox(bb_poly_orig);
        
        int count = 0;
        for (Celula c = getInicioLista(lista_formas); c; c = getProxCelula(c)) {
            Forma f = getConteudoCelula(c);
            BoundingBox bb_forma = getBBForma(f);
            
            if (haInterseccaoBB(bb_poly, bb_forma)) {

                if (formaIntersectaPoligono(regiao_visibilidade, f)) {
                    tipo_forma tipo = getTipoForma(f);
                    void* data = getDataForma(f);
                    int id = -1;
                    char *tipo_str = "Desconhecido";
                    
                   
                    setCorPForma(f, cor);
                    setCorBForma(f, cor);
                    
                    
                    switch(tipo) {
                        case CIRCLE:
                            id = getIDCirculo(data);
                            tipo_str = "Circulo";
                            break;
                        case RECTANGLE:
                            id = getIDRetangulo(data);
                            tipo_str = "Retangulo";
                            break;
                        case LINE:
                            id = getIDLinha(data);
                            tipo_str = "Linha";
                            break;
                        case TEXT:
                            id = getIDTexto(data);
                            tipo_str = "Texto";
                            break;
                        case ANTEPARO:
                            id = getIDAnteparo(data);
                            tipo_str = "Anteparo";
                            break;
                        default:
                            break;
                    }
                    
                    if (qry->txt_file && id != -1) {
                        fprintf(qry->txt_file, "  Pintado: %s ID %d\n", tipo_str, id);
                    }
                    count++;
                }
            }
            liberaBoundingBox(bb_forma);
        }
        liberaBoundingBox(bb_poly);
        
        if (qry->txt_file) {
            fprintf(qry->txt_file, "Total de formas pintadas: %d\n", count);
        }
        
        geraSVGVisibilidade(regiao_visibilidade, x, y, sufixo, qry);
        
        if (strcmp(sufixo, "-") != 0) {
            liberaPoligono(regiao_visibilidade);
        }
    }
    liberaContextoVisibilidade(ctx);
}

static void executa_comando_clonagem(Qry_t *qry, char *linha) {
    strtok(linha, " ");
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
    
    // Criar contexto de visibilidade
    Lista lista_formas = get_lista_cidade(qry->cidade);
    Lista lista_svg = get_lista_svg_cidade(qry->cidade);
    Lista lista_free = obtem_lista_para_desalocar(qry->cidade);
    
    ContextoVisibilidade ctx = criaContextoVisibilidade(x, y, lista_formas, 
                                                        qry->tipo_sort, qry->threshold);
    
    if (!ctx) {
        printf("Erro ao criar contexto de visibilidade\n");
        return;
    }
    
    // Calcular polígono de visibilidade
    Poligono regiao_visibilidade = calculaPoligonoVisibilidade(ctx, x, y);
    
    if (regiao_visibilidade) {
        //Identificar formas visíveis e cloná-las
        if (qry->txt_file) {
            fprintf(qry->txt_file, "\nBomba de clonagem em (%.2f, %.2f) com deslocamento (%.2f, %.2f):\n", 
                    x, y, dx, dy);
        }
        
        BoundingBox bb_poly_orig = getBoundingBox(regiao_visibilidade);
        
        float margem = 1.0f;
        BoundingBox bb_poly = criaBoundingBox(
            getBBMinX(bb_poly_orig) - margem,
            getBBMinY(bb_poly_orig) - margem,
            getBBMaxX(bb_poly_orig) + margem,
            getBBMaxY(bb_poly_orig) + margem
        );
        liberaBoundingBox(bb_poly_orig);
        
        Lista clones = criaLista();
        int count = 0;
        
        for (Celula c = getInicioLista(lista_formas); c; c = getProxCelula(c)) {
            Forma f = getConteudoCelula(c);
            BoundingBox bb_forma = getBBForma(f);
            
            // Teste rápido: Bounding Box
            if (haInterseccaoBB(bb_poly, bb_forma)) {
                // Teste preciso: verifica se a forma está dentro do polígono
                if (formaIntersectaPoligono(regiao_visibilidade, f)) {
                    tipo_forma tipo = getTipoForma(f);
                    void* data = getDataForma(f);
                    int id_original = -1;
                    int id_clone = ++qry->maior_id_atual;
                    char *tipo_str = "Desconhecido";

                   
                    Forma clone_forma = clonaForma(f, id_clone, dx, dy);
                    
                    if (clone_forma != NULL) {
                        // Obtém ID original e nome do tipo
                        switch(tipo) {
                            case CIRCLE:
                                id_original = getIDCirculo(data);
                                tipo_str = "Circulo";
                                break;
                            case RECTANGLE:
                                id_original = getIDRetangulo(data);
                                tipo_str = "Retangulo";
                                break;
                            case LINE:
                                id_original = getIDLinha(data);
                                tipo_str = "Linha";
                                break;
                            case TEXT:
                                id_original = getIDTexto(data);
                                tipo_str = "Texto";
                                break;
                            case ANTEPARO:
                                id_original = getIDAnteparo(data);
                                tipo_str = "Anteparo";
                                break;
                            default:
                                break;
                        }
                        
                        insereFinalLista(clones, clone_forma);
                        
                        if (qry->txt_file && id_original != -1) {
                            fprintf(qry->txt_file, "  Clonado: %s ID %d -> Clone ID %d\n", 
                                    tipo_str, id_original, id_clone);
                        }
                        count++;
                    }
                }
            }
            liberaBoundingBox(bb_forma);
        }
        liberaBoundingBox(bb_poly);
        
        while (!listaVazia(clones)) {
            Forma clone = removeInicioLista(clones);
            insereFinalLista(lista_formas, clone);
            insereFinalLista(lista_svg, clone);
            insereFinalLista(lista_free, clone);
        }
        liberaLista(clones);
        
        if (qry->txt_file) {
            fprintf(qry->txt_file, "Total de formas clonadas: %d\n", count);
        }
        
        //Gerar SVG da região de visibilidade
        geraSVGVisibilidade(regiao_visibilidade, x, y, sufixo, qry);
        
        // Só libera o polígono se não foi armazenado para renderização posterior
        if (strcmp(sufixo, "-") != 0) {
            liberaPoligono(regiao_visibilidade);
        }
    }
    
    liberaContextoVisibilidade(ctx);
}

static void cria_svg_qry(Qry_t *qry, DadosDoArquivo fileData) {
    char *nome_orig = obter_nome_arquivo(fileData);
    char *nome_base = strrchr(nome_orig, '/');
    if(nome_base) nome_base++; else nome_base = nome_orig;
    
    size_t name_len = strlen(nome_base);
    char *nome_arquivo = malloc(name_len + 1);
    if (nome_arquivo == NULL) {
        printf("Erro de alocação\n");
        return;
    }
    
    strcpy(nome_arquivo, nome_base);
    char *dot = strrchr(nome_arquivo, '.');
    if (dot) *dot = '\0';
    
    size_t caminho_len = strlen(qry->caminho_output);
    size_t nome_len_final = strlen(nome_arquivo);
    size_t total_len = caminho_len + 1 + nome_len_final + 5; 
    
    char *caminho_output_arquivo = malloc(total_len);
    if (caminho_output_arquivo == NULL) {
        printf("Erro de alocação\n");
        free(nome_arquivo);
        return;
    }
    
    snprintf(caminho_output_arquivo, total_len, "%s/%s.svg", qry->caminho_output, nome_arquivo);
    
    
    FILE *file = fopen(caminho_output_arquivo, "w");
    if (file == NULL) {
        printf("Erro ao criar arquivo SVG: %s\n", caminho_output_arquivo);
        free(caminho_output_arquivo);
        free(nome_arquivo);
        return;
    }
    
    // Calcula o bounding box de todas as formas para definir o viewBox
    Lista lista_formas = get_lista_cidade(qry->cidade);
    float min_x = INFINITY, min_y = INFINITY;
    float max_x = -INFINITY, max_y = -INFINITY;
    bool has_shapes = false;
    
    Celula aux = getInicioLista(lista_formas);
    while (aux != NULL) {
        Forma f = getConteudoCelula(aux);
        BoundingBox bb = getBBForma(f);
        if (bb != NULL) {
            has_shapes = true;
            float bb_min_x = getBBMinX(bb);
            float bb_min_y = getBBMinY(bb);
            float bb_max_x = getBBMaxX(bb);
            float bb_max_y = getBBMaxY(bb);
            
            if (bb_min_x < min_x) min_x = bb_min_x;
            if (bb_min_y < min_y) min_y = bb_min_y;
            if (bb_max_x > max_x) max_x = bb_max_x;
            if (bb_max_y > max_y) max_y = bb_max_y;
            
            liberaBoundingBox(bb);
        }
        aux = getProxCelula(aux);
    }
    
    // Também considera os polígonos de visibilidade
    if (!listaVazia(qry->visibility_polygons)) {
        typedef struct {
            Poligono poligono;
            float bomb_x;
            float bomb_y;
        } VisibilityData;
        
        for (Celula c = getInicioLista(qry->visibility_polygons); c; c = getProxCelula(c)) {
            VisibilityData* vis_data = (VisibilityData*)getConteudoCelula(c);
            BoundingBox bb = getBoundingBox(vis_data->poligono);
            if (bb != NULL) {
                has_shapes = true;
                float bb_min_x = getBBMinX(bb);
                float bb_min_y = getBBMinY(bb);
                float bb_max_x = getBBMaxX(bb);
                float bb_max_y = getBBMaxY(bb);
                
                if (bb_min_x < min_x) min_x = bb_min_x;
                if (bb_min_y < min_y) min_y = bb_min_y;
                if (bb_max_x > max_x) max_x = bb_max_x;
                if (bb_max_y > max_y) max_y = bb_max_y;
                
                liberaBoundingBox(bb);
            }
        }
    }
    
    // Define valores padrão se não houver formas
    if (!has_shapes) {
        min_x = 0;
        min_y = 0;
        max_x = 1000;
        max_y = 1000;
    }
    
    // Adiciona margem ao viewBox
    float margem = 50.0f;
    float vb_x = min_x - margem;
    float vb_y = min_y - margem;
    float vb_w = (max_x - min_x) + 2 * margem;
    float vb_h = (max_y - min_y) + 2 * margem;
   
    fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(file, "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"%.2f %.2f %.2f %.2f\">\n",
            vb_x, vb_y, vb_w, vb_h);
    
   
    Lista lista_svg_temp = criaLista();
    
   
    // Reutiliza lista_formas e aux já declarados anteriormente
    aux = getInicioLista(lista_formas);
    while (aux != NULL) {
        Forma f = getConteudoCelula(aux);
        insereFinalLista(lista_svg_temp, f);
        aux = getProxCelula(aux);
    }
    
    
    while (!listaVazia(lista_svg_temp)) {
        Forma forma = removeFinalLista(lista_svg_temp);
        if (forma != NULL) {
            escreveFormaSVG(forma, file);
        }
    }
    
    if (!listaVazia(qry->visibility_polygons)) {
        typedef struct {
            Poligono poligono;
            float bomb_x;
            float bomb_y;
        } VisibilityData;
        
        for (Celula c = getInicioLista(qry->visibility_polygons); c; c = getProxCelula(c)) {
            VisibilityData* vis_data = (VisibilityData*)getConteudoCelula(c);
            
            fprintf(file, "<!-- Região de visibilidade da bomba em (%.2f, %.2f) -->\n", 
                    vis_data->bomb_x, vis_data->bomb_y);
            fprintf(file, "<g id=\"visibility-region\" opacity=\"0.5\">\n");
            fprintf(file, "  <polygon points=\"");
            
            Lista vertices = getVertices(vis_data->poligono);
            for (Celula v = getInicioLista(vertices); v; v = getProxCelula(v)) {
                Ponto p = (Ponto)getConteudoCelula(v);
                fprintf(file, "%.2f,%.2f ", getPontoX(p), getPontoY(p));
            }
            
            fprintf(file, "\" fill=\"rgba(255,200,0,0.3)\" stroke=\"orange\" stroke-width=\"2\"/>\n");
            fprintf(file, "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"5\" fill=\"red\"/>\n", 
                    vis_data->bomb_x, vis_data->bomb_y);
            fprintf(file, "</g>\n");
        }
    }
    
    fprintf(file, "</svg>\n");
    fclose(file);
    
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
    
    // Libera lista de polígonos de visibilidade e seus conteúdos
    if (qry_t->visibility_polygons != NULL) {
        // Libera cada polígono armazenado
        while (!listaVazia(qry_t->visibility_polygons)) {
            typedef struct {
                Poligono poligono;
                float bomb_x;
                float bomb_y;
            } VisibilityData;
            
            VisibilityData* vis_data = (VisibilityData*)removeInicioLista(qry_t->visibility_polygons);
            if (vis_data != NULL) {
                if (vis_data->poligono != NULL) {
                    liberaPoligono(vis_data->poligono);
                }
                free(vis_data);
            }
        }
        liberaLista(qry_t->visibility_polygons);
    }
    
    if (qry_t->caminho_output != NULL) {
        free(qry_t->caminho_output);
    }
    
    if (qry_t->txt_file != NULL) {
        fclose(qry_t->txt_file);
    }
    
    free(qry_t);
}