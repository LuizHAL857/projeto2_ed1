#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "anteparo.h"
#include "circulo.h"
#include "retangulo.h"
#include "linha.h"
#include "texto.h"
#include "forma.h"


typedef struct{

     int id;
     float x1, x2, y1, y2;
     char* cor;


}Anteparo_T;




typedef struct 
{
    tipo_forma tipo;
    void* data;
}Forma_t;


static Anteparo transforma_circulo(Forma_t* circulo_original, char h_ou_v, int maior_id);
static Anteparo transforma_retangulo(Forma_t* retangulo_original, int *maior_id, Anteparo *anteparos);
static Anteparo transforma_linha(Forma_t* linha_original, int maior_id);
static Anteparo transforma_texto(Forma_t* texto_original, int maior_id);


Anteparo transforma_em_anteparo(void* forma_original,  char h_ou_v, int maior_id){

    Forma_t* forma = (Forma_t*)forma_original;
    
    switch (forma->tipo)
    {
    case CIRCLE:
        return transforma_circulo(forma, h_ou_v, maior_id);
        
    case RECTANGLE:
        // Retângulos geram 4 anteparos, mas retornamos apenas o primeiro
        // Os outros serão adicionados separadamente
        return NULL; 
        
    case LINE:
        return transforma_linha(forma, maior_id);
        
    case TEXT:
        return transforma_texto(forma, maior_id);
    
    default:
        return NULL;
    }
}


static Anteparo transforma_circulo(Forma_t* circulo_original, char h_ou_v, int maior_id){


    Anteparo_T* a =(Anteparo_T*)malloc(sizeof(Anteparo_T)); 

    if(a == NULL){

        printf("erro de alocação para o anteparo");
        exit(1);
    }
        
    
    a->id = maior_id;
      
      if(h_ou_v == 'h'){

        a->x1 = getXCirculo(circulo_original->data) - getRaioCirculo(circulo_original->data);
        a->x2 = getXCirculo(circulo_original->data) + getRaioCirculo(circulo_original->data);   
        a->y1 = getYCirculo(circulo_original->data);
        a->y2 = a->y1;


      }else {


        a->y1 = getYCirculo(circulo_original->data) - getRaioCirculo(circulo_original->data);
        a->y2 = getYCirculo(circulo_original->data) + getRaioCirculo(circulo_original->data);
        a->x1 = getXCirculo(circulo_original->data);
        a->x2 = a->x1; 
      }

      a->cor =(char*)malloc(sizeof(char)*(strlen(getCorBCirculo(circulo_original->data)) + 1));
      if(a->cor == NULL){

        printf("erro de alocação para a cor do anteparo");
        exit(1);
    }
      strcpy(a->cor, getCorBCirculo(circulo_original->data));
      return a;
}

static Anteparo transforma_retangulo(Forma_t* retangulo_original, int *maior_id, Anteparo *anteparos){
  
  RETANGULO r = (RETANGULO)retangulo_original->data;
  float x = getXRetangulo(r);
  float y = getYRetangulo(r);
  float largura = getLarguraRetangulo(r);
  float altura = getAlturaRetangulo(r);
  char* cor = getCorBRetangulo(r);
  
  // Cria 4 anteparos (um para cada lado do retângulo)
  for(int i = 0; i < 4; i++){
    Anteparo_T* a = (Anteparo_T*)malloc(sizeof(Anteparo_T));
    
    if(a == NULL){
      printf("erro de alocação para o anteparo\n");
      exit(1);
    }
    
    a->id = ++(*maior_id);
    
    // Aloca e copia a cor
    a->cor = (char*)malloc(sizeof(char)*(strlen(cor) + 1));
    if(a->cor == NULL){
      printf("erro de alocação para a cor do anteparo\n");
      exit(1);
    }
    strcpy(a->cor, cor);
    
    // Define as coordenadas de cada lado
    switch(i){
      case 0: // Lado superior
        a->x1 = x;
        a->y1 = y;
        a->x2 = x + largura;
        a->y2 = y;
        break;
      case 1: // Lado direito
        a->x1 = x + largura;
        a->y1 = y;
        a->x2 = x + largura;
        a->y2 = y + altura;
        break;
      case 2: // Lado inferior
        a->x1 = x + largura;
        a->y1 = y + altura;
        a->x2 = x;
        a->y2 = y + altura;
        break;
      case 3: // Lado esquerdo
        a->x1 = x;
        a->y1 = y + altura;
        a->x2 = x;
        a->y2 = y;
        break;
    }
    
    anteparos[i] = a;
  }
  
  return anteparos[0];
}


static Anteparo transforma_linha(Forma_t* linha_original, int maior_id){

  Anteparo_T* a = (Anteparo_T*)malloc(sizeof(Anteparo_T));

  if(a == NULL){

    printf("erro de alocação para o anteparo");
    exit(1);
  }

  a->id = maior_id;
  a->x1 = getX1Linha(linha_original->data);
  a->x2 = getX2Linha(linha_original->data);
  a->y1 = getY1Linha(linha_original->data);
  a->y2 = getY2Linha(linha_original->data);

  a->cor =(char*)malloc(sizeof(char)*(strlen(getCorLinha(linha_original->data)) + 1));

  if(a->cor == NULL){

    printf("erro de alocação da cor do anteparo");
    exit(1);
  }

  strcpy(a->cor, getCorLinha(linha_original->data));

  return a;
}



static Anteparo transforma_texto(Forma_t* texto_original, int maior_id){

    Anteparo_T* a = (Anteparo_T*)malloc(sizeof(Anteparo_T));

    if(a == NULL){

    printf("erro de alocação para o anteparo");
    exit(1);
  }

  a->id = maior_id;

  
  char ancora = getAncoraTexto(texto_original->data);


  if(ancora == 'i'){

    a->x1 = getXTexto(texto_original->data);
    a->x2 = a->x1 + 10*strlen(getTxtTexto(texto_original->data));
    a->y1 = getYTexto(texto_original->data);
    a->y2 = a->y1;
  }else if(ancora == 'f'){


    a->x2 = getXTexto(texto_original->data);
    a->y2 = getYTexto(texto_original->data);
    a->y1 = a->y2;
    a->x1 = a->x2 -10*strlen(getTxtTexto(texto_original->data));
  }else if(ancora == 'm'){

    a->x1 = getXTexto(texto_original->data) - 5*strlen(getTxtTexto(texto_original->data));
    a->x2 = getXTexto(texto_original->data) + 5*strlen(getTxtTexto(texto_original->data));
    a->y1 = getYTexto(texto_original->data);
    a->y2 = a->y1;

  }
  
  a->cor = (char*)malloc(sizeof(char)*(strlen(getCorBTexto(texto_original->data)) + 1));
  if(a->cor == NULL){
    printf("erro de alocação da cor do anteparo");
    exit(1);
  }
  strcpy(a->cor, getCorBTexto(texto_original->data));
  
  return a;
}

float getX1Anteparo(Anteparo a){

  Anteparo_T* aTemp = ((Anteparo_T*)a);

  return aTemp->x1;
}

float getX2Anteparo(Anteparo a){

  Anteparo_T* aTemp = ((Anteparo_T*)a);

  return aTemp->x2;
}

float getY1Anteparo(Anteparo a){

  Anteparo_T* aTemp = ((Anteparo_T*)a);

  return aTemp->y1;
}

float getY2Anteparo(Anteparo a){

  Anteparo_T* aTemp = ((Anteparo_T*)a);

  return aTemp->y2;
}

int getIDAnteparo(Anteparo a){

  Anteparo_T* aTemp = ((Anteparo_T*)a);

  return aTemp->id;
}



char* getCorAnteparo(Anteparo a){
  Anteparo_T* aTemp = ((Anteparo_T*)a);
  return aTemp->cor;
}

void desalocaAnteparo(Anteparo a){
  if(a == NULL) return;
  
  Anteparo_T* aTemp = ((Anteparo_T*)a);
  
  if(aTemp->cor != NULL){
    free(aTemp->cor);
  }
  
  free(aTemp);
}

Anteparo transforma_retangulo_em_anteparos(void* forma_original, int *maior_id, Anteparo *anteparos){
  Forma_t* forma = (Forma_t*)forma_original;
  return transforma_retangulo(forma, maior_id, anteparos);
}

void setCorAnteparo(Anteparo a, char* nova_cor){
  if(a == NULL || nova_cor == NULL) return;
  
  Anteparo_T* aTemp = ((Anteparo_T*)a);
  
  // Libera a cor antiga
  if(aTemp->cor != NULL){
    free(aTemp->cor);
  }
  
  // Aloca e copia a nova cor
  aTemp->cor = (char*)malloc(sizeof(char)*(strlen(nova_cor) + 1));
  if(aTemp->cor == NULL){
    printf("erro de alocação para a cor do anteparo\n");
    exit(1);
  }
  strcpy(aTemp->cor, nova_cor);
}

Anteparo clonaAnteparo(Anteparo a, int novo_id, float dx, float dy){
  if(a == NULL) return NULL;
  
  Anteparo_T* aTemp = ((Anteparo_T*)a);
  Anteparo_T* clone = (Anteparo_T*)malloc(sizeof(Anteparo_T));
  
  if(clone == NULL){
    printf("erro de alocação para o clone do anteparo\n");
    exit(1);
  }
  
  clone->id = novo_id;
  clone->x1 = aTemp->x1 + dx;
  clone->x2 = aTemp->x2 + dx;
  clone->y1 = aTemp->y1 + dy;
  clone->y2 = aTemp->y2 + dy;
  
  // Aloca e copia a cor
  clone->cor = (char*)malloc(sizeof(char)*(strlen(aTemp->cor) + 1));
  if(clone->cor == NULL){
    printf("erro de alocação para a cor do clone do anteparo\n");
    exit(1);
  }
  strcpy(clone->cor, aTemp->cor);
  
  return clone;
}
