#include "arvore_binaria.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Estrutura interna de um nó da árvore
 */
typedef struct No {
    void* elemento;
    struct No* esquerda;
    struct No* direita;
} No_t;

/**
 * @brief Estrutura interna da árvore binária
 */
typedef struct {
    No_t* raiz;
    FuncaoComparacao comparar;
    int tamanho;
} ArvoreBinaria_t;

// ==================== Funções auxiliares internas ====================

/**
 * @brief Cria um novo nó
 */
static No_t* criaNo(void* elemento) {
    No_t* no = (No_t*)malloc(sizeof(No_t));
    if (no == NULL) {
        printf("Erro de alocação para nó da árvore\n");
        exit(1);
    }
    no->elemento = elemento;
    no->esquerda = NULL;
    no->direita = NULL;
    return no;
}

/**
 * @brief Insere recursivamente em um nó
 */
static No_t* insereNo(No_t* no, void* elemento, FuncaoComparacao comparar) {
    if (no == NULL) {
        return criaNo(elemento);
    }
    
    int cmp = comparar(elemento, no->elemento);
    
    if (cmp < 0) {
        no->esquerda = insereNo(no->esquerda, elemento, comparar);
    } else if (cmp > 0) {
        no->direita = insereNo(no->direita, elemento, comparar);
    }
    
    
    return no;
}

/**
 * @brief Busca recursivamente em um nó
 */
static void* buscaNo(No_t* no, const void* elemento, FuncaoComparacao comparar) {
    if (no == NULL) {
        return NULL;
    }
    
    int cmp = comparar(elemento, no->elemento);
    
    if (cmp < 0) {
        return buscaNo(no->esquerda, elemento, comparar);
    } else if (cmp > 0) {
        return buscaNo(no->direita, elemento, comparar);
    } else {
        return no->elemento;
    }
}

/**
 * @brief Encontra o menor nó a partir de um nó dado
 */
static No_t* encontraMenorNo(No_t* no) {
    while (no != NULL && no->esquerda != NULL) {
        no = no->esquerda;
    }
    return no;
}

/**
 * @brief Encontra o maior nó a partir de um nó dado
 */
static No_t* encontraMaiorNo(No_t* no) {
    while (no != NULL && no->direita != NULL) {
        no = no->direita;
    }
    return no;
}

/**
 * @brief Remove recursivamente de um nó
 */
static No_t* removeNo(No_t* no, const void* elemento, FuncaoComparacao comparar, void** removido) {
    if (no == NULL) {
        *removido = NULL;
        return NULL;
    }
    
    int cmp = comparar(elemento, no->elemento);
    
    if (cmp < 0) {
        no->esquerda = removeNo(no->esquerda, elemento, comparar, removido);
    } else if (cmp > 0) {
        no->direita = removeNo(no->direita, elemento, comparar, removido);
    } else {
        
        *removido = no->elemento;
        
        
        if (no->esquerda == NULL) {
            No_t* temp = no->direita;
            free(no);
            return temp;
        } else if (no->direita == NULL) {
            No_t* temp = no->esquerda;
            free(no);
            return temp;
        }
        
        
        No_t* sucessor = encontraMenorNo(no->direita);
        
        
        no->elemento = sucessor->elemento;
        
        
        void* temp_removido;
        no->direita = removeNo(no->direita, sucessor->elemento, comparar, &temp_removido);
        *removido = temp_removido;
    }
    
    return no;
}


static int alturaNo(No_t* no) {
    if (no == NULL) {
        return 0;
    }
    
    int alturaEsq = alturaNo(no->esquerda);
    int alturaDir = alturaNo(no->direita);
    
    return 1 + (alturaEsq > alturaDir ? alturaEsq : alturaDir);
}


static void percorreEmOrdemNo(No_t* no, FuncaoVisita visita, void* contexto) {
    if (no == NULL) {
        return;
    }
    
    percorreEmOrdemNo(no->esquerda, visita, contexto);
    visita(no->elemento, contexto);
    percorreEmOrdemNo(no->direita, visita, contexto);
}

static void percorrePreOrdemNo(No_t* no, FuncaoVisita visita, void* contexto) {
    if (no == NULL) {
        return;
    }
    
    visita(no->elemento, contexto);
    percorrePreOrdemNo(no->esquerda, visita, contexto);
    percorrePreOrdemNo(no->direita, visita, contexto);
}


static void percorrePosOrdemNo(No_t* no, FuncaoVisita visita, void* contexto) {
    if (no == NULL) {
        return;
    }
    
    percorrePosOrdemNo(no->esquerda, visita, contexto);
    percorrePosOrdemNo(no->direita, visita, contexto);
    visita(no->elemento, contexto);
}


static void liberaNo(No_t* no, FuncaoDesalocacao desalocar) {
    if (no == NULL) {
        return;
    }
    
    liberaNo(no->esquerda, desalocar);
    liberaNo(no->direita, desalocar);
    
    if (desalocar != NULL && no->elemento != NULL) {
        desalocar(no->elemento);
    }
    
    free(no);
}

/**
 * Contexto para conversão de árvore para array
 */
typedef struct {
    void** array;
    int indice;
} ContextoArray;

/**
 * Função de visita para adicionar elementos ao array
 */
static void adicionaAoArray(void* elemento, void* contexto) {
    ContextoArray* ctx = (ContextoArray*)contexto;
    ctx->array[ctx->indice++] = elemento;
}

//funções publicas

ArvoreBinaria criaArvoreBinaria(FuncaoComparacao comparar) {
    if (comparar == NULL) {
        printf("Erro: função de comparação não pode ser NULL\n");
        return NULL;
    }
    
    ArvoreBinaria_t* arvore = (ArvoreBinaria_t*)malloc(sizeof(ArvoreBinaria_t));
    if (arvore == NULL) {
        printf("Erro de alocação para árvore binária\n");
        exit(1);
    }
    
    arvore->raiz = NULL;
    arvore->comparar = comparar;
    arvore->tamanho = 0;
    
    return arvore;
}

bool insereArvoreBinaria(ArvoreBinaria arvore, void* elemento) {
    if (arvore == NULL || elemento == NULL) {
        return false;
    }
    
    ArvoreBinaria_t* arv = (ArvoreBinaria_t*)arvore;
    int tamanho_anterior = arv->tamanho;
    
    arv->raiz = insereNo(arv->raiz, elemento, arv->comparar);
    
    // Verifica se realmente inseriu (não era duplicata)
    if (buscaNo(arv->raiz, elemento, arv->comparar) != NULL) {
        if (tamanho_anterior == arv->tamanho) {
            arv->tamanho++;
        }
        return true;
    }
    
    return false;
}

void* buscaArvoreBinaria(ArvoreBinaria arvore, const void* elemento) {
    if (arvore == NULL || elemento == NULL) {
        return NULL;
    }
    
    ArvoreBinaria_t* arv = (ArvoreBinaria_t*)arvore;
    return buscaNo(arv->raiz, elemento, arv->comparar);
}

void* removeArvoreBinaria(ArvoreBinaria arvore, const void* elemento) {
    if (arvore == NULL || elemento == NULL) {
        return NULL;
    }
    
    ArvoreBinaria_t* arv = (ArvoreBinaria_t*)arvore;
    void* removido = NULL;
    
    arv->raiz = removeNo(arv->raiz, elemento, arv->comparar, &removido);
    
    if (removido != NULL) {
        arv->tamanho--;
    }
    
    return removido;
}

bool arvoreVazia(ArvoreBinaria arvore) {
    if (arvore == NULL) {
        return true;
    }
    
    ArvoreBinaria_t* arv = (ArvoreBinaria_t*)arvore;
    return arv->raiz == NULL;
}

int tamanhoArvore(ArvoreBinaria arvore) {
    if (arvore == NULL) {
        return 0;
    }
    
    ArvoreBinaria_t* arv = (ArvoreBinaria_t*)arvore;
    return arv->tamanho;
}

int alturaArvore(ArvoreBinaria arvore) {
    if (arvore == NULL) {
        return 0;
    }
    
    ArvoreBinaria_t* arv = (ArvoreBinaria_t*)arvore;
    return alturaNo(arv->raiz);
}

void percorreEmOrdem(ArvoreBinaria arvore, FuncaoVisita visita, void* contexto) {
    if (arvore == NULL || visita == NULL) {
        return;
    }
    
    ArvoreBinaria_t* arv = (ArvoreBinaria_t*)arvore;
    percorreEmOrdemNo(arv->raiz, visita, contexto);
}

void percorrePreOrdem(ArvoreBinaria arvore, FuncaoVisita visita, void* contexto) {
    if (arvore == NULL || visita == NULL) {
        return;
    }
    
    ArvoreBinaria_t* arv = (ArvoreBinaria_t*)arvore;
    percorrePreOrdemNo(arv->raiz, visita, contexto);
}

void percorrePosOrdem(ArvoreBinaria arvore, FuncaoVisita visita, void* contexto) {
    if (arvore == NULL || visita == NULL) {
        return;
    }
    
    ArvoreBinaria_t* arv = (ArvoreBinaria_t*)arvore;
    percorrePosOrdemNo(arv->raiz, visita, contexto);
}

void** arvoreParaArray(ArvoreBinaria arvore, int* tamanho) {
    if (arvore == NULL || tamanho == NULL) {
        if (tamanho != NULL) *tamanho = 0;
        return NULL;
    }
    
    ArvoreBinaria_t* arv = (ArvoreBinaria_t*)arvore;
    *tamanho = arv->tamanho;
    
    if (arv->tamanho == 0) {
        return NULL;
    }
    
    void** array = (void**)malloc(sizeof(void*) * arv->tamanho);
    if (array == NULL) {
        printf("Erro de alocação para array\n");
        exit(1);
    }
    
    ContextoArray ctx;
    ctx.array = array;
    ctx.indice = 0;
    
    percorreEmOrdemNo(arv->raiz, adicionaAoArray, &ctx);
    
    return array;
}

ArvoreBinaria arrayParaArvore(void** elementos, int tamanho, FuncaoComparacao comparar) {
    if (elementos == NULL || tamanho <= 0 || comparar == NULL) {
        return NULL;
    }
    
    ArvoreBinaria arvore = criaArvoreBinaria(comparar);
    
    for (int i = 0; i < tamanho; i++) {
        insereArvoreBinaria(arvore, elementos[i]);
    }
    
    return arvore;
}

void limpaArvore(ArvoreBinaria arvore, FuncaoDesalocacao desalocar) {
    if (arvore == NULL) {
        return;
    }
    
    ArvoreBinaria_t* arv = (ArvoreBinaria_t*)arvore;
    liberaNo(arv->raiz, desalocar);
    arv->raiz = NULL;
    arv->tamanho = 0;
}

void liberaArvoreBinaria(ArvoreBinaria arvore, FuncaoDesalocacao desalocar) {
    if (arvore == NULL) {
        return;
    }
    
    ArvoreBinaria_t* arv = (ArvoreBinaria_t*)arvore;
    liberaNo(arv->raiz, desalocar);
    free(arv);
}

void* getMenorElemento(ArvoreBinaria arvore) {
    if (arvore == NULL) {
        return NULL;
    }
    
    ArvoreBinaria_t* arv = (ArvoreBinaria_t*)arvore;
    No_t* menor = encontraMenorNo(arv->raiz);
    
    return menor != NULL ? menor->elemento : NULL;
}

void* getMaiorElemento(ArvoreBinaria arvore) {
    if (arvore == NULL) {
        return NULL;
    }
    
    ArvoreBinaria_t* arv = (ArvoreBinaria_t*)arvore;
    No_t* maior = encontraMaiorNo(arv->raiz);
    
    return maior != NULL ? maior->elemento : NULL;
}
