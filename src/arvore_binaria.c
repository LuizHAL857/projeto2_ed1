#include "arvore_binaria.h"
#include <stdio.h>
#include <stdlib.h>


typedef struct NoImpl {
    void* dado;
    struct NoImpl* esq;
    struct NoImpl* dir;
    struct NoImpl* pai;
} NoImpl;

typedef struct {
    NoImpl* raiz;
    FuncaoComparacao comparar;
    void* contexto;
    int tamanho;
} ArvoreImpl;



static NoImpl* criar_no(void* dado) {
    NoImpl* no = malloc(sizeof(NoImpl));
    if (no == NULL) {
        return NULL;
    }

    no->dado = dado;
    no->esq = NULL;
    no->dir = NULL;
    no->pai = NULL;

    return no;
}

static NoImpl* encontrar_minimo_interno(NoImpl* no) {
    if (no == NULL) {
        return NULL;
    }

    while (no->esq != NULL) {
        no = no->esq;
    }

    return no;
}


static NoImpl* encontrar_maximo_interno(NoImpl* no) {
    if (no == NULL) {
        return NULL;
    }

    while (no->dir != NULL) {
        no = no->dir;
    }

    return no;
}


static void transplantar(ArvoreImpl* arvore, NoImpl* u, NoImpl* v) {
    if (u->pai == NULL) {
        arvore->raiz = v;
    } else if (u == u->pai->esq) {
        u->pai->esq = v;
    } else {
        u->pai->dir = v;
    }

    if (v != NULL) {
        v->pai = u->pai;
    }
}

static void limpar_subarvore(NoImpl* no, FuncaoDesalocacao desalocar) {
    if (no == NULL) {
        return;
    }

    limpar_subarvore(no->esq, desalocar);
    limpar_subarvore(no->dir, desalocar);

    if (desalocar != NULL && no->dado != NULL) {
        desalocar(no->dado);
    }

    free(no);
}



ArvoreBinaria criaArvoreBinaria(FuncaoComparacao comparar, void* contexto) {
    if (comparar == NULL) {
        return NULL;
    }

    ArvoreImpl* arvore = malloc(sizeof(ArvoreImpl));
    if (arvore == NULL) {
        return NULL;
    }

    arvore->raiz = NULL;
    arvore->comparar = comparar;
    arvore->contexto = contexto;
    arvore->tamanho = 0;

    return (ArvoreBinaria)arvore;
}

NoArvore insereArvoreBinaria(ArvoreBinaria arvore, void* dado) {
    if (arvore == NULL || dado == NULL) {
        return NULL;
    }

    ArvoreImpl* impl = (ArvoreImpl*)arvore;
    NoImpl* novo_no = criar_no(dado);
    if (novo_no == NULL) {
        return NULL;
    }

   
    if (impl->raiz == NULL) {
        impl->raiz = novo_no;
        impl->tamanho++;
        return (NoArvore)novo_no;
    }

   
    NoImpl* atual = impl->raiz;
    NoImpl* pai = NULL;

    while (atual != NULL) {
        pai = atual;
        int cmp = impl->comparar(dado, atual->dado, impl->contexto);

        if (cmp < 0) {
            atual = atual->esq;
        } else {
            atual = atual->dir;
        }
    }

    
    novo_no->pai = pai;
    int cmp = impl->comparar(dado, pai->dado, impl->contexto);

    if (cmp < 0) {
        pai->esq = novo_no;
    } else {
        pai->dir = novo_no;
    }

    impl->tamanho++;
    return (NoArvore)novo_no;
}

void removeNoArvore(ArvoreBinaria arvore, NoArvore no) {
    if (arvore == NULL || no == NULL) {
        return;
    }

    ArvoreImpl* impl = (ArvoreImpl*)arvore;
    NoImpl* z = (NoImpl*)no;

    
    if (z->esq == NULL) {
        transplantar(impl, z, z->dir);
    }
    
    else if (z->dir == NULL) {
        transplantar(impl, z, z->esq);
    }
   
    else {
        // Encontra sucessor (mínimo na subárvore direita)
        NoImpl* y = encontrar_minimo_interno(z->dir);

        // Se sucessor não é filho imediato
        if (y->pai != z) {
            transplantar(impl, y, y->dir);
            y->dir = z->dir;
            y->dir->pai = y;
        }

        transplantar(impl, z, y);
        y->esq = z->esq;
        y->esq->pai = y;
    }

    free(z);
    impl->tamanho--;
}

bool removeArvoreBinaria(ArvoreBinaria arvore, void* dado) {
    if (arvore == NULL || dado == NULL) {
        return false;
    }

    ArvoreImpl* impl = (ArvoreImpl*)arvore;
    NoImpl* atual = impl->raiz;

    // Busca pelo nó
    while (atual != NULL) {
        int cmp = impl->comparar(dado, atual->dado, impl->contexto);

        if (cmp == 0) {
            
            removeNoArvore(arvore, (NoArvore)atual);
            return true;
        } else if (cmp < 0) {
            atual = atual->esq;
        } else {
            atual = atual->dir;
        }
    }

    return false;
}

void* buscaArvoreBinaria(ArvoreBinaria arvore, const void* dado) {
    if (arvore == NULL || dado == NULL) {
        return NULL;
    }

    ArvoreImpl* impl = (ArvoreImpl*)arvore;
    NoImpl* atual = impl->raiz;

    while (atual != NULL) {
        int cmp = impl->comparar(dado, atual->dado, impl->contexto);

        if (cmp == 0) {
            return atual->dado;
        } else if (cmp < 0) {
            atual = atual->esq;
        } else {
            atual = atual->dir;
        }
    }

    return NULL;
}

void* getMenorElemento(ArvoreBinaria arvore) {
    NoArvore no = getMenorNo(arvore);
    return getDadoNo(no);
}

NoArvore getMenorNo(ArvoreBinaria arvore) {
    if (arvore == NULL) {
        return NULL;
    }

    ArvoreImpl* impl = (ArvoreImpl*)arvore;
    NoImpl* min_no = encontrar_minimo_interno(impl->raiz);

    return (NoArvore)min_no;
}

void* getMaiorElemento(ArvoreBinaria arvore) {
    if (arvore == NULL) return NULL;
    ArvoreImpl* impl = (ArvoreImpl*)arvore;
    NoImpl* max_no = encontrar_maximo_interno(impl->raiz);
    return max_no ? max_no->dado : NULL;
}

void* getDadoNo(NoArvore no) {
    if (no == NULL) {
        return NULL;
    }

    NoImpl* impl = (NoImpl*)no;
    return impl->dado;
}

bool arvoreVazia(ArvoreBinaria arvore) {
    if (arvore == NULL) {
        return true;
    }

    ArvoreImpl* impl = (ArvoreImpl*)arvore;
    return impl->raiz == NULL;
}

int tamanhoArvore(ArvoreBinaria arvore) {
    if (arvore == NULL) {
        return 0;
    }

    ArvoreImpl* impl = (ArvoreImpl*)arvore;
    return impl->tamanho;
}


void limpaArvoreBinaria(ArvoreBinaria arvore, FuncaoDesalocacao desalocar) {
    if (arvore == NULL) {
        return;
    }

    ArvoreImpl* impl = (ArvoreImpl*)arvore;
    limpar_subarvore(impl->raiz, desalocar);
    impl->raiz = NULL;
    impl->tamanho = 0;
}

void liberaArvoreBinaria(ArvoreBinaria arvore, FuncaoDesalocacao desalocar) {
    if (arvore == NULL) {
        return;
    }

    limpaArvoreBinaria(arvore, desalocar);
    free(arvore);
}



static void percorre_em_ordem_rec(NoImpl* no, void (*visita)(void*, void*), void* contexto) {
    if (no == NULL) return;
    percorre_em_ordem_rec(no->esq, visita, contexto);
    visita(no->dado, contexto);
    percorre_em_ordem_rec(no->dir, visita, contexto);
}

void** arvoreParaArray(ArvoreBinaria arvore, int* tamanho) {
    if (arvore == NULL || tamanho == NULL) {
        if (tamanho) *tamanho = 0;
        return NULL;
    }

    ArvoreImpl* impl = (ArvoreImpl*)arvore;
    *tamanho = impl->tamanho;

    if (impl->tamanho == 0) return NULL;

    void** array = malloc(sizeof(void*) * impl->tamanho);
    if (!array) return NULL;

    // Estrutura auxiliar para preencher array
    struct {
        void** arr;
        int idx;
    } ctx = { array, 0 };

   
    void preencher(void* dado, void* c) {
        struct { void** arr; int idx; } *contexto = c;
        contexto->arr[contexto->idx++] = dado;
    }

    percorre_em_ordem_rec(impl->raiz, preencher, &ctx);

    return array;
}
