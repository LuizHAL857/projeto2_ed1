#include "trata_geo.h"
#include "lista.h"
#include "leitor_arquivos.h"
#include "circulo.h"
#include "linha.h"
#include "retangulo.h"
#include "texto.h"
#include "text_style.h"
#include "anteparo.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "forma.h"



typedef struct 
{
    Lista lista_formas;
    Lista lista_para_free;
    Lista lista_svg;
    int maior_id;  // Armazena o maior ID encontrado durante o processamento

}Cidade_t;

typedef struct 
{

    tipo_forma tipo;
    void* data;


}Forma_t;

static void executa_comando_retangulo(Cidade_t *cidade);
static void executa_comando_circulo(Cidade_t *cidade);
static void executa_comando_linha(Cidade_t *cidade);
static void executa_comando_texto(Cidade_t *cidade);
static void executa_comando_textstyle(Cidade_t *cidade);
static void cria_lista_svg(Cidade_t *cidade,  char* caminho_output, DadosDoArquivo fileData,  char *sufixo_comando);

Cidade executa_comando_geo(DadosDoArquivo fileData,  char *caminho_output,  char *sufixo_comando){


    Cidade_t *cidade = malloc(sizeof(Cidade_t));

    if (cidade == NULL){

        printf("Erro de alocação");
        exit(1);

    } 

    cidade->lista_formas = criaLista();
    cidade->lista_para_free = criaLista();
    cidade->lista_svg = criaLista();
    cidade->maior_id = 0;  // Inicializa o maior ID como 0

    while(!listaVazia(obter_lista_linhas(fileData))){

        char *linha = (char*)removeInicioLista(obter_lista_linhas(fileData));
        char* comando = strtok(linha, " ");

        if(strcmp(comando, "c") == 0) {

            executa_comando_circulo(cidade);

        }else if(strcmp(comando, "r") == 0) {

            executa_comando_retangulo(cidade);

        }else if(strcmp(comando, "l") == 0) {

            executa_comando_linha(cidade);
        }else if(strcmp(comando, "t") == 0) {

            executa_comando_texto(cidade);

        }else if(strcmp(comando, "ts") == 0) {

            executa_comando_textstyle(cidade);

        }else {

            printf("comando inválido");

        }

    }

    cria_lista_svg(cidade, caminho_output, fileData, sufixo_comando);
    return cidade;

}

Lista get_lista_cidade(Cidade cidade){

    Cidade_t *chao_t = (Cidade_t *)cidade;

    return chao_t->lista_formas;
}

Lista get_lista_svg_cidade(Cidade cidade){
    Cidade_t *chao_t = (Cidade_t *)cidade;
    return chao_t->lista_svg;
}



void desaloca_geo(Cidade cidade){

    Cidade_t* chao_t = (Cidade_t *)cidade;
    liberaLista(chao_t->lista_formas);
    liberaLista(chao_t->lista_svg);

    while(!listaVazia(chao_t->lista_para_free)){
      
        Forma_t  *forma = removeInicioLista(chao_t->lista_para_free);

        
        switch(forma->tipo){
       
            case CIRCLE:   desalocaCirculo(forma->data);
            break;
            
            case RECTANGLE:  desalocarRetangulo(forma->data);
            break;

            case LINE: desalocaLinha(forma->data);
            break;

            case TEXT: desalocaTexto(forma->data);
            break;

            case TEXT_STYLE: desalocaTextStyle(forma->data);
            break;

            case ANTEPARO: desalocaAnteparo(forma->data);
            break;

            default:
            break;

          }
          free(forma);

    }
    liberaLista(chao_t->lista_para_free);
    free(cidade);

}

Lista obtem_lista_para_desalocar(Cidade cidade) {


  Cidade_t *chao_t = (Cidade_t *)cidade;


  return chao_t->lista_para_free;


}

//Funções Privadas
static void executa_comando_circulo(Cidade_t *cidade){


    char *id = strtok(NULL, " ");
    char *X = strtok(NULL, " ");
    char *Y = strtok(NULL, " ");
    char *raio = strtok(NULL, " ");
    char *corB = strtok(NULL, " ");
    char *corP = strtok(NULL, " ");
  
    int id_num = atoi(id);
    CIRCULO c = criaCirculo(id_num, atof(X), atof(Y),
                                  atof(raio), corP, corB);
  
    // Atualiza o maior ID se necessário
    if (id_num > cidade->maior_id) {
        cidade->maior_id = id_num;
    }
  
    Forma_t *forma = malloc(sizeof(Forma_t));
    if (forma == NULL) {
      printf("Erro de alocação\n");
      exit(1);
    }
    forma->tipo = CIRCLE;
    forma->data = c;
    insereFinalLista(cidade->lista_formas, forma);
    insereFinalLista(cidade->lista_para_free, forma);
    insereFinalLista(cidade->lista_svg, forma);
}


static void executa_comando_retangulo(Cidade_t *cidade) {
    char *id = strtok(NULL, " ");
    char *X = strtok(NULL, " ");
    char *Y = strtok(NULL, " ");
    char *largura = strtok(NULL, " ");
    char *altura = strtok(NULL, " ");
    char *corB = strtok(NULL, " ");
    char *corP = strtok(NULL, " ");
  
    int id_num = atoi(id);
    RETANGULO r =
        criaRetangulo(id_num, atof(X), atof(Y), atof(altura),
                         atof(largura), corB, corP);
  
    // Atualiza o maior ID se necessário
    if (id_num > cidade->maior_id) {
        cidade->maior_id = id_num;
    }
  
    Forma_t *forma = malloc(sizeof(Forma_t));
    if (forma == NULL) {
      printf("Erro de alocação\n");
      exit(1);
    }
    forma->tipo = RECTANGLE;
    forma->data = r;
    insereFinalLista(cidade->lista_formas, forma);
    insereFinalLista(cidade->lista_para_free, forma);
    insereFinalLista(cidade->lista_svg, forma);
  }

  static void executa_comando_linha(Cidade_t *cidade) {
    char *id = strtok(NULL, " ");
    char *x1 = strtok(NULL, " ");
    char *y1 = strtok(NULL, " ");
    char *x2 = strtok(NULL, " ");
    char *y2 = strtok(NULL, " ");
    char *cor = strtok(NULL, " ");
  
    int id_num = atoi(id);
    LINHA l = criaLinha(id_num, atof(x1), atof(y1), atof(x2),
                            atof(y2), cor);
  
    // Atualiza o maior ID se necessário
    if (id_num > cidade->maior_id) {
        cidade->maior_id = id_num;
    }
  
    Forma_t *forma = malloc(sizeof(Forma_t));
    if (forma == NULL) {
      printf("Erro de alocação\n");
      exit(1);
    }
    forma->tipo = LINE;
    forma->data = l;
    insereFinalLista(cidade->lista_formas, forma);
    insereFinalLista(cidade->lista_para_free, forma);
    insereFinalLista(cidade->lista_svg, forma);
  }

  static void executa_comando_texto(Cidade_t *cidade) {
    char *id = strtok(NULL, " ");
    char *X = strtok(NULL, " ");
    char *Y = strtok(NULL, " ");
    char *corB = strtok(NULL, " ");
    char *corP = strtok(NULL, " ");
    char *ancora = strtok(NULL, " ");
    char *txt = strtok(NULL, "");
  
    int id_num = atoi(id);
    TEXTO t = criaTexto(id_num, atof(X), atof(Y),
                                corB, corP, *ancora, txt);
  
    // Atualiza o maior ID se necessário
    if (id_num > cidade->maior_id) {
        cidade->maior_id = id_num;
    }
  
    Forma_t *forma = malloc(sizeof(Forma_t));
    if (forma == NULL) {
      printf("Erro de alocação\n");
      exit(1);
    }
    forma->tipo = TEXT;
    forma->data = t;
    insereFinalLista(cidade->lista_formas, forma);
    insereFinalLista(cidade->lista_para_free, forma);
    insereFinalLista(cidade->lista_svg, forma);
  }



  static void executa_comando_textstyle(Cidade_t *cidade) {
    char *ff = strtok(NULL, " ");
    char *fw = strtok(NULL, " ");
    char *fs = strtok(NULL, " ");
  
    TEXTSTYLE ts =
        criaTextStyle(ff, fw, atoi(fs));
  
    Forma_t *forma = malloc(sizeof(Forma_t));
    if (forma == NULL) {
      printf("Erro de alocação\n");
      exit(1);
    }
    forma->tipo = TEXT_STYLE;
    forma->data = ts;
    insereFinalLista(cidade->lista_formas, forma);
    insereFinalLista(cidade->lista_para_free, forma);
    insereFinalLista(cidade->lista_svg, forma);

  }



static void cria_lista_svg(Cidade_t *cidade, char* caminho_output, DadosDoArquivo fileData,  char *sufixo_comando){
     char *nome_arquivo_original = obter_nome_arquivo(fileData);
    size_t name_len = strlen(nome_arquivo_original);
    char *nome_arquivo = malloc(name_len + 1);
    
    
    if (nome_arquivo == NULL) {
        printf("Erro de alocação\n");
        return;
    }

    strcpy(nome_arquivo, nome_arquivo_original);
    strtok(nome_arquivo, ".");

    if (sufixo_comando != NULL) {
    strcat(nome_arquivo, "-");
    strcat(nome_arquivo, sufixo_comando);
    }


   
    size_t caminho_len = strlen(caminho_output);
    size_t nome_processado_len = strlen(nome_arquivo);
    size_t total_len = caminho_len + 1 + nome_processado_len + 4 +
    1; 

 
    char *caminho_output_arquivo = malloc(total_len);
    if (caminho_output_arquivo == NULL) {
    printf("Erro de alocaçõa\n");
    return;
    }

   
    int result = snprintf(caminho_output_arquivo, total_len, "%s/%s.svg",
    caminho_output, nome_arquivo);
    if (result < 0 || (size_t)result >= total_len) {
    printf("Erro no caminho\n");
    free(caminho_output_arquivo);
    return;
    }

    FILE *file = fopen(caminho_output_arquivo, "w");
    if (file == NULL) {
    printf("Error: Failed to open file: %s\n", caminho_output_arquivo);
    free(caminho_output_arquivo);
    return;
    }
    fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(
    file,
    "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 1000 1000\">\n");
    while (!listaVazia(cidade->lista_svg)) {
    Forma_t *forma = removeInicioLista(cidade->lista_svg);
    if (forma != NULL) {
      if (forma->tipo == CIRCLE) {
        CIRCULO c = (CIRCULO)forma->data;


    fprintf(
    file,
    "<circle cx='%.2f' cy='%.2f' r='%.2f' fill='%s' stroke='%s'/>\n",
    getXCirculo(c), getYCirculo(c),
    getRaioCirculo(c), getCorPCirculo(c),
    getCorBCirculo(c));


    } else if (forma->tipo == RECTANGLE) {
    RETANGULO r = (RETANGULO)forma->data;
    fprintf(file,
    "<rect x='%.2f' y='%.2f' width='%.2f' height='%.2f' fill='%s' "
    "stroke='%s'/>\n",
    getXRetangulo(r), getYRetangulo(r),
    getLarguraRetangulo(r), getAlturaRetangulo(r),
    getCorPRetangulo(r),
    getCorBRetangulo(r));


    } else if (forma->tipo == LINE) {
    LINHA l = (LINHA)forma->data;
    fprintf(file,
    "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='%s'/>\n",
    getX1Linha(l), getY1Linha(l), getX2Linha(l),
    getY2Linha(l), getCorLinha(l));


    } else if (forma->tipo == TEXT) {
    TEXTO t = (TEXTO)forma->data;
    char ancora = getAncoraTexto(t);
     char *texto_ancora = "start"; 

  
    if (ancora == 'm' || ancora == 'M') {
    texto_ancora = "middle";

    } else if (ancora == 'f' || ancora == 'F') {
    texto_ancora = "end";


    } else if (ancora == 'i' || ancora == 'I') {
    texto_ancora = "start";
    }

    fprintf(file,
    "<text x='%.2f' y='%.2f' fill='%s' stroke='%s' "
    "text-anchor='%s'>%s</text>\n",
    getXTexto(t), getYTexto(t), getCorPTexto(t),
    getCorBTexto(t), texto_ancora, getTxtTexto(t));
    }
    }
    }
    fprintf(file, "</svg>\n");
    fclose(file);
    free(caminho_output_arquivo);
    free(nome_arquivo);
}


int get_maior_id_geo(Cidade cidade) {
    Cidade_t *chao_t = (Cidade_t *)cidade;
    return chao_t->maior_id;
}

