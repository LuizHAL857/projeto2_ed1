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
    Segmento* anteparos_temp[4];  
    bool tem_anteparos_temp;
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
    
    
    
    float seg_dx = x2 - x1;
    float seg_dy = y2 - y1;
    
    // Determinante para verificar se raio e segmento são paralelos
    float det = seg_dx * dy - seg_dy * dx;
    if (fabs(det) < EPSILON) {
        s->dist = INFINITY;
        return;
    }
    
    
    
    // Parâmetro do segmento (0 <= s <= 1)
    float s_val = (dx * y1 - dy * x1) / det;
    
    // Parâmetro do raio (t >= 0, representa a distância)
    float t = (seg_dx * y1 - seg_dy * x1) / det;
    
    // Verifica se a interseção está dentro dos limites válidos
    if (s_val >= 0.0 && s_val <= 1.0 && t >= 0.0) {
        s->dist = t;
    } else {
        s->dist = INFINITY;
    }
}



// ------------------------ Comparadores ------------------------

static int cmpSeg(const void* a, const void* b, void* ctx) {
    (void)ctx; 
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
    ArvoreBinaria ativos = criaArvoreBinaria(cmpSeg, NULL);

    // Processa todos os eventos até o ângulo especificado (inclusive)
    for (int i = 0; i < ctx->n_eventos; i++) {
        Evento* ev = &ctx->eventos[i];
        
        // Processa eventos até o ângulo alvo
        if (ev->angulo > ang + EPSILON) {
            break; // Paramos quando passamos do ângulo alvo
        }
        
        if (ev->tipo == EVT_INICIO) {
            insereArvoreBinaria(ativos, ev->seg);
        } else {
            // Só remove se o segmento estiver na árvore
            // (pode não estar se o INICIO foi antes do primeiro evento processado)
            if (buscaArvoreBinaria(ativos, ev->seg) != NULL) {
                removeArvoreBinaria(ativos, ev->seg);
            }
        }
    }
    
    return ativos;
}


static bool criaAnteparosTemporarios(Lista lista_formas, float margem, Segmento** anteparos_temp) {
    if (!lista_formas || listaVazia(lista_formas)) return false;
    
    // Inicializa os limites do bounding box
    float min_x = INFINITY, max_x = -INFINITY;
    float min_y = INFINITY, max_y = -INFINITY;
    bool encontrou_forma = false;
    
    // Percorre todas as formas para calcular o bounding box
    for (Celula c = getInicioLista(lista_formas); c; c = getProxCelula(c)) {
        Forma f = getConteudoCelula(c);
        tipo_forma tipo = getTipoForma(f);
        
        switch (tipo) {
            case CIRCLE: {
                CIRCULO circ = getDataForma(f);
                float cx = getXCirculo(circ);
                float cy = getYCirculo(circ);
                float r = getRaioCirculo(circ);
                
                if (cx - r < min_x) min_x = cx - r;
                if (cx + r > max_x) max_x = cx + r;
                if (cy - r < min_y) min_y = cy - r;
                if (cy + r > max_y) max_y = cy + r;
                encontrou_forma = true;
                break;
            }
            
            case RECTANGLE: {
                RETANGULO ret = getDataForma(f);
                float rx = getXRetangulo(ret);
                float ry = getYRetangulo(ret);
                float w = getLarguraRetangulo(ret);
                float h = getAlturaRetangulo(ret);
                
                if (rx < min_x) min_x = rx;
                if (rx + w > max_x) max_x = rx + w;
                if (ry < min_y) min_y = ry;
                if (ry + h > max_y) max_y = ry + h;
                encontrou_forma = true;
                break;
            }
            
            case LINE: {
                LINHA linha = getDataForma(f);
                float x1 = getX1Linha(linha);
                float y1 = getY1Linha(linha);
                float x2 = getX2Linha(linha);
                float y2 = getY2Linha(linha);
                
                if (x1 < min_x) min_x = x1;
                if (x2 < min_x) min_x = x2;
                if (x1 > max_x) max_x = x1;
                if (x2 > max_x) max_x = x2;
                if (y1 < min_y) min_y = y1;
                if (y2 < min_y) min_y = y2;
                if (y1 > max_y) max_y = y1;
                if (y2 > max_y) max_y = y2;
                encontrou_forma = true;
                break;
            }
            
            case TEXT: {
                TEXTO txt = getDataForma(f);
                float tx = getXTexto(txt);
                float ty = getYTexto(txt);
                char anc = getAncoraTexto(txt);
                char* texto = getTxtTexto(txt);
                int len = strlen(texto);
                float L = 10.0f * len;
                
                float x1, x2;
                if (anc == 'i' || anc == 'I') {
                    x1 = tx;
                    x2 = tx + L;
                } else if (anc == 'f' || anc == 'F') {
                    x1 = tx - L;
                    x2 = tx;
                } else {
                    x1 = tx - L/2;
                    x2 = tx + L/2;
                }
                
                if (x1 < min_x) min_x = x1;
                if (x2 > max_x) max_x = x2;
                if (ty < min_y) min_y = ty;
                if (ty > max_y) max_y = ty;
                encontrou_forma = true;
                break;
            }
            
            case ANTEPARO: {
                Anteparo ant = getDataForma(f);
                float x1 = getX1Anteparo(ant);
                float y1 = getY1Anteparo(ant);
                float x2 = getX2Anteparo(ant);
                float y2 = getY2Anteparo(ant);
                
                if (x1 < min_x) min_x = x1;
                if (x2 < min_x) min_x = x2;
                if (x1 > max_x) max_x = x1;
                if (x2 > max_x) max_x = x2;
                if (y1 < min_y) min_y = y1;
                if (y2 < min_y) min_y = y2;
                if (y1 > max_y) max_y = y1;
                if (y2 > max_y) max_y = y2;
                encontrou_forma = true;
                break;
            }
            
            default:
                break;
        }
    }
    
    if (!encontrou_forma) return false;
    
    // Adiciona a margem ao bounding box
    min_x -= margem;
    max_x += margem;
    min_y -= margem;
    max_y += margem;
    
    // Cria os 4 anteparos temporários
    // Anteparo superior (horizontal, y = min_y)
    anteparos_temp[0] = malloc(sizeof(Segmento));
    anteparos_temp[0]->x1 = min_x;
    anteparos_temp[0]->y1 = min_y;
    anteparos_temp[0]->x2 = max_x;
    anteparos_temp[0]->y2 = min_y;
    anteparos_temp[0]->source = NULL;
    anteparos_temp[0]->dist = INFINITY;
    anteparos_temp[0]->distKey = INFINITY;
    
    // Anteparo direito (vertical, x = max_x)
    anteparos_temp[1] = malloc(sizeof(Segmento));
    anteparos_temp[1]->x1 = max_x;
    anteparos_temp[1]->y1 = min_y;
    anteparos_temp[1]->x2 = max_x;
    anteparos_temp[1]->y2 = max_y;
    anteparos_temp[1]->source = NULL;
    anteparos_temp[1]->dist = INFINITY;
    anteparos_temp[1]->distKey = INFINITY;
    
    // Anteparo inferior (horizontal, y = max_y)
    anteparos_temp[2] = malloc(sizeof(Segmento));
    anteparos_temp[2]->x1 = max_x;
    anteparos_temp[2]->y1 = max_y;
    anteparos_temp[2]->x2 = min_x;
    anteparos_temp[2]->y2 = max_y;
    anteparos_temp[2]->source = NULL;
    anteparos_temp[2]->dist = INFINITY;
    anteparos_temp[2]->distKey = INFINITY;
    
    // Anteparo esquerdo (vertical, x = min_x)
    anteparos_temp[3] = malloc(sizeof(Segmento));
    anteparos_temp[3]->x1 = min_x;
    anteparos_temp[3]->y1 = max_y;
    anteparos_temp[3]->x2 = min_x;
    anteparos_temp[3]->y2 = min_y;
    anteparos_temp[3]->source = NULL;
    anteparos_temp[3]->dist = INFINITY;
    anteparos_temp[3]->distKey = INFINITY;
    
    return true;
}



ContextoVisibilidade criaContextoVisibilidade(float x, float y, Lista lista_formas,
                                              char tipo_sort, int threshold) {
    if (!lista_formas) return NULL;

    CtxVis* ctx = malloc(sizeof(CtxVis));
    ctx->bomb_x = x;
    ctx->bomb_y = y;
    ctx->formas = lista_formas;
    ctx->tem_anteparos_temp = false;
    ctx->segmentos = criaLista();
    ctx->eventos = NULL;
    ctx->n_eventos = 0;
    
    // Cria anteparos temporários para delimitar a região de visibilidade
    
    if (criaAnteparosTemporarios(lista_formas, 10.0f, ctx->anteparos_temp)) {
        ctx->tem_anteparos_temp = true;
        // Adiciona os anteparos temporários à lista de segmentos
        for (int i = 0; i < 4; i++) {
            Segmento* s = ctx->anteparos_temp[i];
            s->ang1 = anguloPonto(x, y, s->x1, s->y1);
            s->ang2 = anguloPonto(x, y, s->x2, s->y2);
            insereFinalLista(ctx->segmentos, s);
        }
    }

    // Coleta anteparos das formas
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

    // Conta quantos segmentos cruzam o corte -PI/+PI para alocar eventos suficientes
    int nSeg = getTamanhoLista(ctx->segmentos);
    // int nEventosMax = 2 * nSeg; // Base - removido para evitar warning
    
    // Verifica cruzamentos (estimativa pessimista ou realocação seria melhor, mas vamos iterar)
    // Para simplificar, vamos alocar o dobro do necessário no pior caso (4 eventos por segmento)
    ctx->eventos = malloc(4 * nSeg * sizeof(Evento));
    
    int idx = 0;
    for (Celula c = getInicioLista(ctx->segmentos); c; c = getProxCelula(c)) {
        Segmento* s = getConteudoCelula(c);
        float a1 = s->ang1;
        float a2 = s->ang2;
        
        if (fabs(a1 - a2) > M_PI) {
            // Cruza o corte. O segmento cobre o arco que passa por PI/-PI.
            // Intervalos: [max(a1,a2), PI] e [-PI, min(a1,a2)]
            float menor = fmin(a1, a2);
            float maior = fmax(a1, a2);
            
            // Intervalo 1: [maior, PI]
            ctx->eventos[idx++] = (Evento){ maior, EVT_INICIO, s };
            ctx->eventos[idx++] = (Evento){ M_PI,  EVT_FIM,    s };
            
            // Intervalo 2: [-PI, menor]
            ctx->eventos[idx++] = (Evento){ -M_PI, EVT_INICIO, s };
            ctx->eventos[idx++] = (Evento){ menor, EVT_FIM,    s };
        } else {
            // Caso normal: [min, max]
            float menor = fmin(a1, a2);
            float maior = fmax(a1, a2);
            ctx->eventos[idx++] = (Evento){ menor, EVT_INICIO, s };
            ctx->eventos[idx++] = (Evento){ maior, EVT_FIM,    s };
        }
    }
    
    ctx->n_eventos = idx;

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


static bool encoberto(CtxVis* ctx, float px, float py, ArvoreBinaria ativos) {
    if (arvoreVazia(ativos)) {
        return false;
    }
    
    float dx = px - ctx->bomb_x;
    float dy = py - ctx->bomb_y;
    float distP = sqrt(dx*dx + dy*dy);
    
    if (distP < EPSILON) {
        return false; // Ponto coincide com a bomba
    }
    
    // Normaliza direção
    float ndx = dx / distP;
    float ndy = dy / distP;
    
    // Percorre todos os segmentos ativos e verifica se algum bloqueia o ponto
    int tamanho;
    void** segmentos = arvoreParaArray(ativos, &tamanho);
    
    bool bloqueado = false;
    for (int i = 0; i < tamanho; i++) {
        Segmento* s = (Segmento*)segmentos[i];
        
        // Verifica se o ponto está sobre ou muito próximo deste segmento
        // Se estiver, ignora este segmento (evita auto-bloqueio)
        float dx1 = px - s->x1;
        float dy1 = py - s->y1;
        float dx2 = px - s->x2;
        float dy2 = py - s->y2;
        float dist1 = sqrt(dx1*dx1 + dy1*dy1);
        float dist2 = sqrt(dx2*dx2 + dy2*dy2);
        
        // Se o ponto é um dos vértices do segmento, ignora
        if (dist1 < EPSILON || dist2 < EPSILON) {
            continue;
        }
        
        // Verifica se o ponto está sobre o segmento (distância perpendicular muito pequena)
        float seg_len = sqrt((s->x2 - s->x1)*(s->x2 - s->x1) + (s->y2 - s->y1)*(s->y2 - s->y1));
        if (seg_len > EPSILON) {
            // Produto vetorial para calcular distância perpendicular
            float cross = fabs((s->x2 - s->x1) * (s->y1 - py) - (s->x1 - px) * (s->y2 - s->y1));
            float perp_dist = cross / seg_len;
            
            if (perp_dist < EPSILON) {
                // Ponto está sobre a reta do segmento, verifica se está entre os extremos
                float dot = (px - s->x1) * (s->x2 - s->x1) + (py - s->y1) * (s->y2 - s->y1);
                float t = dot / (seg_len * seg_len);
                if (t >= -EPSILON && t <= 1.0 + EPSILON) {
                    // Ponto está sobre o segmento, ignora
                    continue;
                }
            }
        }
        
        atualizarDist(s, ctx->bomb_x, ctx->bomb_y, ndx, ndy);
        
        // Se o segmento está mais próximo que o ponto, ele bloqueia
        if (s->dist < distP - EPSILON) {
            bloqueado = true;
            break;
        }
    }
    
    free(segmentos);
    return bloqueado;
}

bool pontoVisivel(ContextoVisibilidade C, float px, float py) {
    CtxVis* ctx = (CtxVis*)C;
    float ang = anguloPonto(ctx->bomb_x, ctx->bomb_y, px, py);
    
    // Constrói conjunto de segmentos ativos para este ângulo
    ArvoreBinaria ativos = construirAtivos(ctx, ang);
    
    // Verifica se o ponto está encoberto por algum segmento ativo
    bool visivel = !encoberto(ctx, px, py, ativos);
    
    liberaArvoreBinaria(ativos, NULL);
    return visivel;
}

// ------------------------ Visibilidade de forma ------------------------

bool formaVisivel(ContextoVisibilidade C, Forma f) {
    if (!C || !f) return false;

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

        case ANTEPARO: {
            Anteparo a=getDataForma(f);
            float x1=getX1Anteparo(a), y1=getY1Anteparo(a), x2=getX2Anteparo(a), y2=getY2Anteparo(a);
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

    // Libera os segmentos
    
    while(!listaVazia(ctx->segmentos)){
        Segmento* s=removeInicioLista(ctx->segmentos);
        free(s);
    }
    liberaLista(ctx->segmentos);
    free(ctx->eventos);
    free(ctx);
}
