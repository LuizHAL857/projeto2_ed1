#include "visibilidade.h"
#include "lista.h"
#include "arvore_binaria.h"
#include "forma.h"
#include "anteparo.h"
#include "circulo.h"
#include "retangulo.h"
#include "linha.h"
#include "texto.h"
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
    float x1, y1, x2, y2;
    Anteparo source;
    float ang1, ang2;
    float dist;      
    float distKey;  
} Segmento;

typedef enum { EVT_INICIO, EVT_FIM } TipoEv;

typedef struct {
    float angulo;
    TipoEv tipo;
    Segmento* seg;
} Evento;

typedef struct {
    float bomb_x, bomb_y;
    Lista formas;
    Lista segmentos;
    Evento* eventos;
    int n_eventos;
} CtxVis;

// ------------------------ Geometria ------------------------

static float normalizaAngulo(float a) {
    while (a >  M_PI) a -= 2*M_PI;
    while (a < -M_PI) a += 2*M_PI;
    return a;
}

static float anguloPonto(float bx, float by, float px, float py) {
    return normalizaAngulo(atan2(py - by, px - bx));
}

static void atualizarDist(Segmento* s, float bx, float by, float dx, float dy) {
    // Translada segmento para origem da bomba
    float x1 = s->x1 - bx;
    float y1 = s->y1 - by;
    float x2 = s->x2 - bx;
    float y2 = s->y2 - by;
    
    float det = (x2 - x1)*dy - (y2 - y1)*dx;
    if (fabs(det) < EPSILON) {
        s->dist = INFINITY;
        return;
    }
    
    float t = (x1*dy - y1*dx) / det;
    float s_val = (x1*(y2 - y1) - y1*(x2 - x1)) / det;
    
    if (t >= 0.0 && t <= 1.0 && s_val >= 0.0) {
        s->dist = s_val;
    } else {
        s->dist = INFINITY;
    }
}

// ------------------------ Comparadores ------------------------

static int cmpSeg(const void* a, const void* b) {
    const Segmento* sa = a;
    const Segmento* sb = b;
    if (sa->distKey < sb->distKey) return -1;
    if (sa->distKey > sb->distKey) return 1;
    return (sa < sb) ? -1 : 1;
}

static int cmpEvt(const void* a, const void* b) {
    const Evento* ea = *(Evento**)a;
    const Evento* eb = *(Evento**)b;
    if (ea->angulo < eb->angulo) return -1;
    if (ea->angulo > eb->angulo) return 1;
    return (ea->tipo == EVT_INICIO ? -1 : 1);
}

// ------------------------ Sweep  ------------------------

static ArvoreBinaria construirAtivos(CtxVis* ctx, float ang) {
    ArvoreBinaria ativos = criaArvoreBinaria(cmpSeg);

    for (int i = 0; i < ctx->n_eventos; i++) {
        Evento* ev = &ctx->eventos[i];
        if (ev->angulo > ang) break;

        // Calcula direção do raio
        float dx = cos(ev->angulo);
        float dy = sin(ev->angulo);

        // Atualiza distância com coordenadas da bomba
        atualizarDist(ev->seg, ctx->bomb_x, ctx->bomb_y, dx, dy);

        if (ev->tipo == EVT_INICIO) {
            ev->seg->distKey = ev->seg->dist;  // fixa a chave
            insereArvoreBinaria(ativos, ev->seg);
        } else {
            removeArvoreBinaria(ativos, ev->seg);
        }
    }
    return ativos;
}

// ------------------------ Criação do contexto ------------------------

ContextoVisibilidade criaContextoVisibilidade(float x, float y, Lista lista_formas,
                                              char tipo_sort, int threshold) {
    if (!lista_formas) return NULL;

    CtxVis* ctx = malloc(sizeof(CtxVis));
    ctx->bomb_x = x;
    ctx->bomb_y = y;
    ctx->formas = lista_formas;
    ctx->segmentos = criaLista();
    ctx->eventos = NULL;
    ctx->n_eventos = 0;

    // Coleta anteparos
    for (Celula c = getInicioLista(lista_formas); c; c = getProxCelula(c)) {
        Forma f = getConteudoCelula(c);
        if (getTipoForma(f) != ANTEPARO) continue;

        Anteparo a = getDataForma(f);
        Segmento* s = malloc(sizeof(Segmento));
        s->x1 = getX1Anteparo(a); s->y1 = getY1Anteparo(a);
        s->x2 = getX2Anteparo(a); s->y2 = getY2Anteparo(a);
        s->source = a;
        s->ang1 = anguloPonto(x,y, s->x1, s->y1);
        s->ang2 = anguloPonto(x,y, s->x2, s->y2);
        s->dist = INFINITY;
        s->distKey = INFINITY;

        insereFinalLista(ctx->segmentos, s);
    }

    int nSeg = getTamanhoLista(ctx->segmentos);
    ctx->n_eventos = 2 * nSeg;
    ctx->eventos = malloc(ctx->n_eventos * sizeof(Evento));

    int idx = 0;
    for (Celula c = getInicioLista(ctx->segmentos); c; c = getProxCelula(c)) {
        Segmento* s = getConteudoCelula(c);
        float a1 = fmin(s->ang1, s->ang2);
        float a2 = fmax(s->ang1, s->ang2);
        ctx->eventos[idx++] = (Evento){ a1, EVT_INICIO, s };
        ctx->eventos[idx++] = (Evento){ a2, EVT_FIM,    s };
    }

    Evento** arr = malloc(sizeof(Evento*) * ctx->n_eventos);
    for (int i = 0; i < ctx->n_eventos; i++)
        arr[i] = &ctx->eventos[i];

    if (ctx->n_eventos <= threshold)
        insertion_sort(arr, ctx->n_eventos, sizeof(void*), cmpEvt);
    else if (tipo_sort == 'm')
        merge_sort(arr, ctx->n_eventos, sizeof(void*), cmpEvt);
    else
        quick_sort(arr, ctx->n_eventos, sizeof(void*), cmpEvt);

    Evento* ordenados = malloc(ctx->n_eventos * sizeof(Evento));
    for (int i = 0; i < ctx->n_eventos; i++)
        ordenados[i] = *arr[i];

    free(arr);
    free(ctx->eventos);
    ctx->eventos = ordenados;

    return ctx;
}

// ------------------------ Visibilidade de ponto ------------------------

bool pontoVisivel(ContextoVisibilidade C, float px, float py) {
    CtxVis* ctx = (CtxVis*)C;
    float ang = anguloPonto(ctx->bomb_x, ctx->bomb_y, px, py);
    float dx = px - ctx->bomb_x, dy = py - ctx->bomb_y;
    float distP = sqrt(dx*dx + dy*dy);

    ArvoreBinaria ativos = construirAtivos(ctx, ang);
    Segmento* primeiro = (Segmento*)getMenorElemento(ativos);
    bool visivel = true;

    if (primeiro) {
        // Normaliza direção para o cálculo correto
        float ndx = dx / distP;
        float ndy = dy / distP;
        atualizarDist(primeiro, ctx->bomb_x, ctx->bomb_y, ndx, ndy);
        if (primeiro->dist < distP - EPSILON)
            visivel = false;
    }

    liberaArvoreBinaria(ativos, NULL);
    return visivel;
}

// ------------------------ Visibilidade de forma ------------------------

bool formaVisivel(ContextoVisibilidade C, Forma f) {
    if (!C || !f) return false;
    if (getTipoForma(f) == ANTEPARO) return false;

    float xs[16], ys[16]; int n = 0;

    switch (getTipoForma(f)) {
        case CIRCLE: {
            CIRCULO c = getDataForma(f);
            float cx = getXCirculo(c), cy = getYCirculo(c), r = getRaioCirculo(c);
            xs[n]=cx; ys[n]=cy; n++;
            for(int i=0;i<8;i++){ float ang=i*M_PI/4; xs[n]=cx+r*cos(ang); ys[n]=cy+r*sin(ang); n++; }
        } break;

        case RECTANGLE: {
            RETANGULO r = getDataForma(f);
            float x=getXRetangulo(r), y=getYRetangulo(r), w=getLarguraRetangulo(r), h=getAlturaRetangulo(r);
            xs[n]=x; ys[n]=y; n++; xs[n]=x+w; ys[n]=y; n++; xs[n]=x; ys[n]=y+h; n++; xs[n]=x+w; ys[n]=y+h; n++;
            xs[n]=x+w/2; ys[n]=y+h/2; n++;
        } break;

        case LINE: {
            LINHA l=getDataForma(f);
            float x1=getX1Linha(l), y1=getY1Linha(l), x2=getX2Linha(l), y2=getY2Linha(l);
            xs[n]=x1; ys[n]=y1; n++; xs[n]=x2; ys[n]=y2; n++; xs[n]=(x1+x2)/2; ys[n]=(y1+y2)/2; n++;
        } break;

        case TEXT: {
            TEXTO t=getDataForma(f); float xt=getXTexto(t), yt=getYTexto(t); char anc=getAncoraTexto(t); char* txt=getTxtTexto(t);
            int len=strlen(txt); float L=10.0f*len; float x1,y1,x2,y2;
            if(anc=='i'||anc=='I'){x1=xt;y1=yt;x2=xt+L;y2=yt;}
            else if(anc=='f'||anc=='F'){x1=xt-L;y1=yt;x2=xt;y2=yt;}
            else{x1=xt-L/2;y1=yt;x2=xt+L/2;y2=yt;}
            xs[n]=x1; ys[n]=y1; n++; xs[n]=x2; ys[n]=y2; n++; xs[n]=(x1+x2)/2; ys[n]=(y1+y2)/2; n++;
        } break;

        default: break;
    }

    for(int i=0;i<n;i++)
        if(pontoVisivel(C, xs[i], ys[i]))
            return true;

    return false;
}

// ------------------------ Lista de formas visíveis ------------------------

Lista getFormasVisiveis(ContextoVisibilidade C) {
    if(!C) return NULL;
    CtxVis* ctx=(CtxVis*)C;
    Lista L=criaLista();

    for(Celula c=getInicioLista(ctx->formas); c; c=getProxCelula(c)){
        Forma f=getConteudoCelula(c);
        if(formaVisivel(C,f)) insereFinalLista(L,f);
    }

    return L;
}

// ------------------------ Liberação ------------------------

void liberaContextoVisibilidade(ContextoVisibilidade C) {
    if(!C) return;
    CtxVis* ctx=(CtxVis*)C;

    while(!listaVazia(ctx->segmentos)){
        Segmento* s=removeInicioLista(ctx->segmentos);
        free(s);
    }
    liberaLista(ctx->segmentos);
    free(ctx->eventos);
    free(ctx);
}
