#include "visibilidade.h"
#include "lista.h"
#include "arvore_binaria.h"
#include "forma.h"
#include "anteparo.h"
#include "sort.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define EPSILON 1e-9



typedef struct {
    float x, y;
} Ponto2D;


typedef struct {
    Ponto2D pto_ini, pto_fim;
    Anteparo source;
} SegmentoInterno;

typedef enum { INICIO, FIM } TipoVertice;
typedef enum { ORIG, ARTF, RE } CodigoVertice;

typedef struct {
    TipoVertice tipo;
    SegmentoInterno* pSeg;
    Ponto2D ponto;
    CodigoVertice codigo;
} Vertice;

typedef struct {
    Ponto2D x;  // Ponto observador
    SegmentoInterno* segmentos;
    int n_segmentos;
    Vertice* vertices;
    int n_vertices;
    ArvoreBinaria SegsAtvs;
    Ponto2D biombo;
    Lista regiao_visibilidade;
    char tipo_sort;
    int threshold;
} CtxVis;


//função que calcula o determinante
static float area2(Ponto2D x, Ponto2D y, Ponto2D z) {
    return (y.x - x.x) * (z.y - x.y) - (y.y - x.y) * (z.x - x.x);
}

static bool viradaDireita(Ponto2D x, Ponto2D y, Ponto2D z) {
    return area2(x, y, z) < -EPSILON;
}



static float distancia(Ponto2D a, Ponto2D b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return sqrt(dx*dx + dy*dy);
}

static float angulo(Ponto2D origem, Ponto2D p) {
    return atan2(p.y - origem.y, p.x - origem.x);
}


static Ponto2D interseccao(Ponto2D x, Ponto2D v, SegmentoInterno* s) {
    float dx = v.x - x.x;
    float dy = v.y - x.y;
    
    float x1 = s->pto_ini.x - x.x;
    float y1 = s->pto_ini.y - x.y;
    float x2 = s->pto_fim.x - x.x;
    float y2 = s->pto_fim.y - x.y;
    
    float seg_dx = x2 - x1;
    float seg_dy = y2 - y1;
    
    float det = seg_dx * dy - seg_dy * dx;
    if (fabs(det) < EPSILON) {
        return (Ponto2D){INFINITY, INFINITY};
    }
    
    float s_val = (dx * y1 - dy * x1) / det;
    float t = (seg_dx * y1 - seg_dy * x1) / det;
    
    if (s_val >= 0.0 && s_val <= 1.0 && t >= 0.0) {
        return (Ponto2D){x.x + t * dx, x.y + t * dy};
    }
    
    return (Ponto2D){INFINITY, INFINITY};
}



// Contexto global temporário para ordenação (necessário porque qsort não aceita contexto)
static Ponto2D g_origem_ordenacao;

static int cmpVertices(const void* a, const void* b) {
    const Vertice* va = *(Vertice**)a;
    const Vertice* vb = *(Vertice**)b;
    
    //Ângulo
    float ang_a = angulo(g_origem_ordenacao, va->ponto);
    float ang_b = angulo(g_origem_ordenacao, vb->ponto);
    if (ang_a < ang_b - EPSILON) return -1;
    if (ang_a > ang_b + EPSILON) return 1;
    
    //Distância (maior primeiro = mais próximo)
    float dist_a = distancia(g_origem_ordenacao, va->ponto);
    float dist_b = distancia(g_origem_ordenacao, vb->ponto);
    if (dist_a > dist_b + EPSILON) return -1;
    if (dist_a < dist_b - EPSILON) return 1;
    
    //Tipo (INICIO antes de FIM)
    if (va->tipo == INICIO && vb->tipo == FIM) return -1;
    if (va->tipo == FIM && vb->tipo == INICIO) return 1;
    
    return 0;
}

static int cmpSegmentos(const void* a, const void* b, void* ctx) {
    (void)ctx;
    const SegmentoInterno* sa = a;
    const SegmentoInterno* sb = b;
    
    
    return (sa < sb) ? -1 : 1;
}

// ALGORITMO PRINCIPAL

static SegmentoInterno* segAtivoMaisProx(CtxVis* ctx, Ponto2D v) {
    if (arvoreVazia(ctx->SegsAtvs)) return NULL;
    
    int tamanho;
    void** segs = arvoreParaArray(ctx->SegsAtvs, &tamanho);
    
    SegmentoInterno* mais_proximo = NULL;
    float menor_dist = INFINITY;
    
    for (int i = 0; i < tamanho; i++) {
        SegmentoInterno* s = (SegmentoInterno*)segs[i];
        Ponto2D inter = interseccao(ctx->x, v, s);
        
        if (inter.x != INFINITY) {
            float d = distancia(ctx->x, inter);
            if (d < menor_dist) {
                menor_dist = d;
                mais_proximo = s;
            }
        }
    }
    
    free(segs);
    return mais_proximo;
}

static bool encoberto(CtxVis* ctx, Vertice* v) {
    SegmentoInterno* s = segAtivoMaisProx(ctx, v->ponto);
    if (s == NULL) return false;
    
    Ponto2D inter = interseccao(ctx->x, v->ponto, s);
    if (inter.x == INFINITY) return false;
    
    float dist_inter = distancia(ctx->x, inter);
    float dist_v = distancia(ctx->x, v->ponto);
    
    return dist_inter < dist_v - EPSILON;
}

ContextoVisibilidade criaContextoVisibilidade(float x, float y, Lista formas,
                                              char tipo_sort, int threshold) {
    if (!formas) return NULL;
    
    CtxVis* ctx = malloc(sizeof(CtxVis));
    ctx->x.x = x;
    ctx->x.y = y;
    ctx->tipo_sort = tipo_sort;
    ctx->threshold = threshold;
    ctx->regiao_visibilidade = criaLista();
    ctx->SegsAtvs = criaArvoreBinaria(cmpSegmentos, NULL);
    
    // Cria retângulo envolvente considerando TODAS as formas
    // Isso garante que a região de visibilidade sempre cubra todas as formas potencialmente visíveis
    float min_x = x, max_x = x, min_y = y, max_y = y;
    
    for (Celula c = getInicioLista(formas); c; c = getProxCelula(c)) {
        Forma f = getConteudoCelula(c);
        BoundingBox bb = getBBForma(f);
        if (bb) {
            float bb_min_x = getBBMinX(bb);
            float bb_min_y = getBBMinY(bb);
            float bb_max_x = getBBMaxX(bb);
            float bb_max_y = getBBMaxY(bb);
            
            if (bb_min_x < min_x) min_x = bb_min_x;
            if (bb_max_x > max_x) max_x = bb_max_x;
            if (bb_min_y < min_y) min_y = bb_min_y;
            if (bb_max_y > max_y) max_y = bb_max_y;
            
            liberaBoundingBox(bb);
        }
    }
    
    float margem = 50.0f;
    min_x -= margem;
    max_x += margem;
    min_y -= margem;
    max_y += margem;
    
    // Conta anteparos
    int n_ant = 0;
    for (Celula c = getInicioLista(formas); c; c = getProxCelula(c)) {
        if (getTipoForma(getConteudoCelula(c)) == ANTEPARO) n_ant++;
    }
    
    ctx->n_segmentos = n_ant + 4;
    ctx->segmentos = malloc(ctx->n_segmentos * sizeof(SegmentoInterno));
    
    // Marcador especial para segmentos do retângulo envolvente
    Anteparo MARCADOR_RETANGULO = (Anteparo)0x1;
    
    // Adiciona retângulo envolvente
    int idx = 0;
    ctx->segmentos[idx].pto_ini = (Ponto2D){min_x, min_y};
    ctx->segmentos[idx].pto_fim = (Ponto2D){max_x, min_y};
    ctx->segmentos[idx].source = MARCADOR_RETANGULO;
    idx++;
    
    ctx->segmentos[idx].pto_ini = (Ponto2D){max_x, min_y};
    ctx->segmentos[idx].pto_fim = (Ponto2D){max_x, max_y};
    ctx->segmentos[idx].source = MARCADOR_RETANGULO;
    idx++;
    
    ctx->segmentos[idx].pto_ini = (Ponto2D){max_x, max_y};
    ctx->segmentos[idx].pto_fim = (Ponto2D){min_x, max_y};
    ctx->segmentos[idx].source = MARCADOR_RETANGULO;
    idx++;
    
    ctx->segmentos[idx].pto_ini = (Ponto2D){min_x, max_y};
    ctx->segmentos[idx].pto_fim = (Ponto2D){min_x, min_y};
    ctx->segmentos[idx].source = MARCADOR_RETANGULO;
    idx++;
    
    //Adiciona anteparos
    for (Celula c = getInicioLista(formas); c; c = getProxCelula(c)) {
        Forma f = getConteudoCelula(c);
        if (getTipoForma(f) != ANTEPARO) continue;
        
        Anteparo a = getDataForma(f);
        ctx->segmentos[idx].pto_ini.x = getX1Anteparo(a);
        ctx->segmentos[idx].pto_ini.y = getY1Anteparo(a);
        ctx->segmentos[idx].pto_fim.x = getX2Anteparo(a);
        ctx->segmentos[idx].pto_fim.y = getY2Anteparo(a);
        ctx->segmentos[idx].source = a;
        idx++;
    }
    
    //Orienta segmentos 
    for (int i = 0; i < ctx->n_segmentos; i++) {
        if (viradaDireita(ctx->x, ctx->segmentos[i].pto_ini, ctx->segmentos[i].pto_fim)) {
            Ponto2D temp = ctx->segmentos[i].pto_ini;
            ctx->segmentos[i].pto_ini = ctx->segmentos[i].pto_fim;
            ctx->segmentos[i].pto_fim = temp;
        }
    }
    
    // Cria vértices
    ctx->n_vertices = ctx->n_segmentos * 2;
    ctx->vertices = malloc(ctx->n_vertices * sizeof(Vertice));
    
    for (int i = 0; i < ctx->n_segmentos; i++) {
        CodigoVertice cod = (ctx->segmentos[i].source == NULL) ? RE : ORIG;
        
        ctx->vertices[2*i].tipo = INICIO;
        ctx->vertices[2*i].pSeg = &ctx->segmentos[i];
        ctx->vertices[2*i].ponto = ctx->segmentos[i].pto_ini;
        ctx->vertices[2*i].codigo = cod;
        
        ctx->vertices[2*i+1].tipo = FIM;
        ctx->vertices[2*i+1].pSeg = &ctx->segmentos[i];
        ctx->vertices[2*i+1].ponto = ctx->segmentos[i].pto_fim;
        ctx->vertices[2*i+1].codigo = cod;
    }
    
    // Ordena vértices
    g_origem_ordenacao = ctx->x;
    
    Vertice** arr = malloc(ctx->n_vertices * sizeof(Vertice*));
    for (int i = 0; i < ctx->n_vertices; i++) {
        arr[i] = &ctx->vertices[i];
    }
    
    if (ctx->n_vertices <= ctx->threshold) {
        insertion_sort(arr, ctx->n_vertices, sizeof(Vertice*), cmpVertices);
    } else if (ctx->tipo_sort == 'm') {
        merge_sort(arr, ctx->n_vertices, sizeof(Vertice*), cmpVertices);
    } else {
        quick_sort(arr, ctx->n_vertices, sizeof(Vertice*), cmpVertices);
    }
    
    Vertice* ordenados = malloc(ctx->n_vertices * sizeof(Vertice));
    for (int i = 0; i < ctx->n_vertices; i++) {
        ordenados[i] = *arr[i];
    }
    free(arr);
    free(ctx->vertices);
    ctx->vertices = ordenados;
    
    //Inicializa biombo
    if (ctx->n_vertices > 0) {
        ctx->biombo = ctx->vertices[0].ponto;
    }
    
    //Executa varredura angular
    for (int i = 0; i < ctx->n_vertices; i++) {
        Vertice* v = &ctx->vertices[i];
        
        if (v->tipo == INICIO) {
            // Vértice de início
            if (!encoberto(ctx, v)) {
                // v está na frente
                SegmentoInterno* s = segAtivoMaisProx(ctx, v->ponto);
                if (s != NULL) {
                    Ponto2D y = interseccao(ctx->x, v->ponto, s);
                    if (y.x != INFINITY && distancia(ctx->biombo, y) > EPSILON) {
                        SegmentoInterno* seg1 = malloc(sizeof(SegmentoInterno));
                        seg1->pto_ini = ctx->biombo;
                        seg1->pto_fim = y;
                        seg1->source = NULL;
                        insereFinalLista(ctx->regiao_visibilidade, seg1);
                        
                        if (distancia(y, v->ponto) > EPSILON) {
                            SegmentoInterno* seg2 = malloc(sizeof(SegmentoInterno));
                            seg2->pto_ini = y;
                            seg2->pto_fim = v->ponto;
                            seg2->source = NULL;
                            insereFinalLista(ctx->regiao_visibilidade, seg2);
                        }
                    }
                }
                ctx->biombo = v->ponto;
            }
            insereArvoreBinaria(ctx->SegsAtvs, v->pSeg);
            
        } else {
            // Vértice de fim
            if (!encoberto(ctx, v)) {
                // v está na frente
                if (distancia(ctx->biombo, v->ponto) > EPSILON) {
                    SegmentoInterno* seg1 = malloc(sizeof(SegmentoInterno));
                    seg1->pto_ini = ctx->biombo;
                    seg1->pto_fim = v->ponto;
                    seg1->source = NULL;
                    insereFinalLista(ctx->regiao_visibilidade, seg1);
                }
                
                removeArvoreBinaria(ctx->SegsAtvs, v->pSeg);
                SegmentoInterno* sy = segAtivoMaisProx(ctx, v->ponto);
                
                if (sy != NULL) {
                    Ponto2D y = interseccao(ctx->x, v->ponto, sy);
                    if (y.x != INFINITY && distancia(v->ponto, y) > EPSILON) {
                        SegmentoInterno* seg2 = malloc(sizeof(SegmentoInterno));
                        seg2->pto_ini = v->ponto;
                        seg2->pto_fim = y;
                        seg2->source = NULL;
                        insereFinalLista(ctx->regiao_visibilidade, seg2);
                        ctx->biombo = y;
                    } else {
                        ctx->biombo = v->ponto;
                    }
                } else {
                    ctx->biombo = v->ponto;
                }
            } else {
                removeArvoreBinaria(ctx->SegsAtvs, v->pSeg);
            }
        }
    }
    
    // Fecha o polígono: adiciona segmento do biombo atual de volta ao primeiro vértice
    if (ctx->n_vertices > 0 && !listaVazia(ctx->regiao_visibilidade)) {
        Ponto2D primeiro = ctx->vertices[0].ponto;
        if (distancia(ctx->biombo, primeiro) > EPSILON) {
            SegmentoInterno* seg_final = malloc(sizeof(SegmentoInterno));
            seg_final->pto_ini = ctx->biombo;
            seg_final->pto_fim = primeiro;
            seg_final->source = NULL;
            insereFinalLista(ctx->regiao_visibilidade, seg_final);
        }
    }
    
    return ctx;
}

Lista getRegiaoVisibilidade(ContextoVisibilidade C) {
    if (!C) return NULL;
    return ((CtxVis*)C)->regiao_visibilidade;
}

void getCoordenadasSegmentoVis(SegmentoVisibilidade seg,
                               float* x1, float* y1,
                               float* x2, float* y2) {
    if (!seg) return;
    SegmentoInterno* s = (SegmentoInterno*)seg;
    if (x1) *x1 = s->pto_ini.x;
    if (y1) *y1 = s->pto_ini.y;
    if (x2) *x2 = s->pto_fim.x;
    if (y2) *y2 = s->pto_fim.y;
}

bool pontoVisivel(ContextoVisibilidade C, float px, float py) {
    if (!C) return false;
    CtxVis* ctx = (CtxVis*)C;
    
    if (fabs(px - ctx->x.x) < EPSILON && fabs(py - ctx->x.y) < EPSILON) {
        return true;
    }
    
    int interseccoes = 0;
    for (Celula c = getInicioLista(ctx->regiao_visibilidade); c; c = getProxCelula(c)) {
        SegmentoInterno* s = (SegmentoInterno*)getConteudoCelula(c);
        
        float y1 = s->pto_ini.y;
        float y2 = s->pto_fim.y;
        
        if ((y1 <= py && y2 > py) || (y2 <= py && y1 > py)) {
            float x_inter = s->pto_ini.x + (py - s->pto_ini.y) * (s->pto_fim.x - s->pto_ini.x) / (s->pto_fim.y - s->pto_ini.y);
            if (x_inter > px) {
                interseccoes++;
            }
        }
    }
    
    return (interseccoes % 2) == 1;
}

void liberaContextoVisibilidade(ContextoVisibilidade C) {
    if (!C) return;
    CtxVis* ctx = (CtxVis*)C;
    
    free(ctx->segmentos);
    free(ctx->vertices);
    liberaArvoreBinaria(ctx->SegsAtvs, NULL);
    
    while (!listaVazia(ctx->regiao_visibilidade)) {
        SegmentoInterno* s = removeInicioLista(ctx->regiao_visibilidade);
        free(s);
    }
    liberaLista(ctx->regiao_visibilidade);
    
    free(ctx);
}
