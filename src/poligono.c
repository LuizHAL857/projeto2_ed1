#include "poligono.h"
#include "ponto.h"
#include "forma.h"
#include "circulo.h"
#include "retangulo.h"
#include "linha.h"
#include "texto.h"
#include "anteparo.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define EPSILON 1e-9

/* ================= Definições das estruturas ================= */

typedef struct {
    float x;
    float y;
} stPonto;

typedef struct {
    stPonto p1;
    stPonto p2;
} stSegmento;

typedef struct {
    float min_x;
    float min_y;
    float max_x;
    float max_y;
} stBoundingBox;

typedef struct {
    Lista vertices;   // Lista de stPonto*
    Lista segmentos;  // Lista de stSegmento*
} stPoligono;

/* ================= Segmento - Implementação ================= */


Segmento criaSegmento(Ponto p1, Ponto p2) {
    if (!p1 || !p2) return NULL;
    
    stSegmento* seg = malloc(sizeof(stSegmento));
    if (!seg) return NULL;
    
    stPonto* pt1 = (stPonto*)p1;
    stPonto* pt2 = (stPonto*)p2;
    
    seg->p1.x = pt1->x;
    seg->p1.y = pt1->y;
    seg->p2.x = pt2->x;
    seg->p2.y = pt2->y;
    
    return seg;
}

void liberaSegmento(Segmento seg) {
    free(seg);
}

Ponto getSegmentoP1(Segmento seg) {
    if (!seg) return NULL;
    stSegmento* s = (stSegmento*)seg;
    return criaPonto(s->p1.x, s->p1.y);
}

Ponto getSegmentoP2(Segmento seg) {
    if (!seg) return NULL;
    stSegmento* s = (stSegmento*)seg;
    return criaPonto(s->p2.x, s->p2.y);
}

/* ================= BoundingBox - Implementação ================= */

BoundingBox criaBoundingBox(float min_x, float min_y, float max_x, float max_y) {
    stBoundingBox* bb = malloc(sizeof(stBoundingBox));
    if (!bb) return NULL;
    bb->min_x = min_x;
    bb->min_y = min_y;
    bb->max_x = max_x;
    bb->max_y = max_y;
    return bb;
}

void liberaBoundingBox(BoundingBox bb) {
    free(bb);
}

float getBBMinX(BoundingBox bb) {
    if (!bb) return INFINITY;
    stBoundingBox* b = (stBoundingBox*)bb;
    return b->min_x;
}

float getBBMinY(BoundingBox bb) {
    if (!bb) return INFINITY;
    stBoundingBox* b = (stBoundingBox*)bb;
    return b->min_y;
}

float getBBMaxX(BoundingBox bb) {
    if (!bb) return -INFINITY;
    stBoundingBox* b = (stBoundingBox*)bb;
    return b->max_x;
}

float getBBMaxY(BoundingBox bb) {
    if (!bb) return -INFINITY;
    stBoundingBox* b = (stBoundingBox*)bb;
    return b->max_y;
}

/* ================= Polígono - Implementação ================= */


/* ================= Criação e liberação ================= */

Poligono criaPoligono() {
    stPoligono* p = malloc(sizeof(stPoligono));
    if (!p) return NULL;
    
    p->vertices = criaLista();
    p->segmentos = criaLista();
    
    return p;
}

void liberaPoligono(Poligono pol) {
    if (!pol) return;
    
    stPoligono* p = (stPoligono*)pol;
    
    while (!listaVazia(p->vertices)) {
        Ponto* ponto = removeInicioLista(p->vertices);
        free(ponto);
    }
    liberaLista(p->vertices);
    
    
    while (!listaVazia(p->segmentos)) {
        Segmento* seg = removeInicioLista(p->segmentos);
        free(seg);
    }
    liberaLista(p->segmentos);
    
    free(p);
}





void insereVertice(Poligono pol, Ponto ponto) {
    if (!pol || !ponto) return;
    
    stPoligono* p = (stPoligono*)pol;
    stPonto* pt = (stPonto*)ponto;
    
    stPonto* novo = malloc(sizeof(stPonto));
    if (!novo) return;
    
    novo->x = pt->x;
    novo->y = pt->y;
    
    insereFinalLista(p->vertices, novo);
}

void insereSegmento(Poligono pol, Segmento seg) {
    if (!pol || !seg) return;
    
    stPoligono* p = (stPoligono*)pol;
    stSegmento* s = (stSegmento*)seg;
    
    stSegmento* novo = malloc(sizeof(stSegmento));
    if (!novo) return;
    
    novo->p1 = s->p1;
    novo->p2 = s->p2;
    
    insereFinalLista(p->segmentos, novo);
}


/* ================= Consultas ================= */

BoundingBox getBoundingBox(Poligono pol) {
    if (!pol) return criaBoundingBox(INFINITY, INFINITY, -INFINITY, -INFINITY);
    
    stPoligono* p = (stPoligono*)pol;
    
    if (listaVazia(p->vertices)) {
        return criaBoundingBox(INFINITY, INFINITY, -INFINITY, -INFINITY);
    }
    
    float min_x = INFINITY, min_y = INFINITY;
    float max_x = -INFINITY, max_y = -INFINITY;
    
    for (Celula c = getInicioLista(p->vertices); c; c = getProxCelula(c)) {
        stPonto* ponto = (stPonto*)getConteudoCelula(c);
        
        if (ponto->x < min_x) min_x = ponto->x;
        if (ponto->x > max_x) max_x = ponto->x;
        if (ponto->y < min_y) min_y = ponto->y;
        if (ponto->y > max_y) max_y = ponto->y;
    }
    
    return criaBoundingBox(min_x, min_y, max_x, max_y);
}

bool isInside(Poligono pol, Ponto ponto) {
    if (!pol || !ponto) return false;
    
    stPoligono* p = (stPoligono*)pol;
    stPonto* pt = (stPonto*)ponto;
    
    if (listaVazia(p->vertices)) return false;
    
    // Algoritmo de ray casting
    // Lança um raio horizontal para a direita e conta quantas vezes cruza as arestas
    int num_vertices = getTamanhoLista(p->vertices);
    if (num_vertices < 3) return false;
    
    int interseccoes = 0;
    
    Celula c1 = getInicioLista(p->vertices);
    Celula c2 = getProxCelula(c1);
    
    while (c2) {
        stPonto* p1 = (stPonto*)getConteudoCelula(c1);
        stPonto* p2 = (stPonto*)getConteudoCelula(c2);
        
        // Verifica se o raio horizontal cruza esta aresta
        if (((p1->y > pt->y) != (p2->y > pt->y)) &&
            (pt->x < (p2->x - p1->x) * (pt->y - p1->y) / (p2->y - p1->y) + p1->x)) {
            interseccoes++;
        }
        
        c1 = c2;
        c2 = getProxCelula(c2);
    }
    
    // Fecha o polígono: última aresta conecta último vértice ao primeiro
    stPonto* p_ultimo = (stPonto*)getConteudoCelula(c1);
    stPonto* p_primeiro = (stPonto*)getConteudoCelula(getInicioLista(p->vertices));
    
    if (((p_ultimo->y > pt->y) != (p_primeiro->y > pt->y)) &&
        (pt->x < (p_primeiro->x - p_ultimo->x) * (pt->y - p_ultimo->y) / 
                   (p_primeiro->y - p_ultimo->y) + p_ultimo->x)) {
        interseccoes++;
    }
    
    // Se número de interseções é ímpar, o ponto está dentro
    return (interseccoes % 2) == 1;
}

Lista getVertices(Poligono pol) {

    if (!pol) return NULL;
    
    stPoligono* p = (stPoligono*)pol;
    return p->vertices;
}

Lista getSegmentos(Poligono pol) {
    if (!pol) return NULL;
    
    stPoligono* p = (stPoligono*)pol;
    return p->segmentos;
}

int getNumVertices(Poligono pol) {
    if (!pol) return 0;
    
    stPoligono* p = (stPoligono*)pol;
    return getTamanhoLista(p->vertices);
}

int getNumSegmentos(Poligono pol) {
    if (!pol) return 0;
    
    stPoligono* p = (stPoligono*)pol;
    return getTamanhoLista(p->segmentos);
}

bool haInterseccaoBB(BoundingBox a, BoundingBox b) {
    if (!a || !b) return false;
    
    stBoundingBox* bb_a = (stBoundingBox*)a;
    stBoundingBox* bb_b = (stBoundingBox*)b;
    
    // Verifica se NÃO há intersecção e inverte o resultado
    // Se um retângulo está totalmente à esquerda, direita, acima ou abaixo do outro
    if (bb_a->max_x < bb_b->min_x || bb_a->min_x > bb_b->max_x ||
        bb_a->max_y < bb_b->min_y || bb_a->min_y > bb_b->max_y) {
        return false;
    }
    return true;
}
// Função auxiliar: verifica se dois segmentos se intersectam
static bool segmentosIntersectam(float x1, float y1, float x2, float y2,
                                  float x3, float y3, float x4, float y4) {
    float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (fabs(d) < 1e-9) return false; // Paralelos
    
    float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / d;
    float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / d;
    
    return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
}

// Função auxiliar: distância de ponto a segmento
static float distanciaPontoSegmento(float px, float py, float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len2 = dx*dx + dy*dy;
    
    if (len2 < 1e-9) {
        // Segmento degenerado (ponto)
        dx = px - x1;
        dy = py - y1;
        return sqrt(dx*dx + dy*dy);
    }
    
    float t = ((px - x1) * dx + (py - y1) * dy) / len2;
    t = fmax(0, fmin(1, t));
    
    float projX = x1 + t * dx;
    float projY = y1 + t * dy;
    
    dx = px - projX;
    dy = py - projY;
    return sqrt(dx*dx + dy*dy);
}

bool formaIntersectaPoligono(Poligono p, Forma f) {
    if (!p || !f) return false;
    
    tipo_forma tipo = getTipoForma(f);
    void* data = getDataForma(f);
    
    // 1. Teste de bounding box
    BoundingBox bb_forma = getBBForma(f);
    BoundingBox bb_poly = getBoundingBox(p);
    
    if (!haInterseccaoBB(bb_forma, bb_poly)) {
        liberaBoundingBox(bb_forma);
        liberaBoundingBox(bb_poly);
        return false;
    }
    
    liberaBoundingBox(bb_forma);
    liberaBoundingBox(bb_poly);
    
    // Obtém vértices do polígono
    Lista vertices_poly = getVertices(p);
    
    switch(tipo) {
        case LINE: {
            LINHA l = (LINHA)data;
            float x1 = getX1Linha(l);
            float y1 = getY1Linha(l);
            float x2 = getX2Linha(l);
            float y2 = getY2Linha(l);
            
            // Verifica se extremidades estão dentro do polígono
            Ponto p1 = criaPonto(x1, y1);
            Ponto p2 = criaPonto(x2, y2);
            
            if (isInside(p, p1) || isInside(p, p2)) {
                liberaPonto(p1);
                liberaPonto(p2);
                return true;
            }
            liberaPonto(p1);
            liberaPonto(p2);
            
            // Verifica interseção com arestas do polígono
            Celula c = getInicioLista(vertices_poly);
            if (!c) return false;
            
            Ponto v_first = (Ponto)getConteudoCelula(c);
            Ponto v_prev = v_first;
            
            for (c = getProxCelula(c); c; c = getProxCelula(c)) {
                Ponto v_curr = (Ponto)getConteudoCelula(c);
                
                if (segmentosIntersectam(x1, y1, x2, y2,
                                        getPontoX(v_prev), getPontoY(v_prev),
                                        getPontoX(v_curr), getPontoY(v_curr))) {
                    return true;
                }
                v_prev = v_curr;
            }
            
            // Fecha o polígono
            if (segmentosIntersectam(x1, y1, x2, y2,
                                    getPontoX(v_prev), getPontoY(v_prev),
                                    getPontoX(v_first), getPontoY(v_first))) {
                return true;
            }
            
            return false;
        }
        
        case RECTANGLE: {
            RETANGULO r = (RETANGULO)data;
            float rx = getXRetangulo(r);
            float ry = getYRetangulo(r);
            float w = getLarguraRetangulo(r);
            float h = getAlturaRetangulo(r);
            
            // Vértices do retângulo
            float vx[4] = {rx, rx+w, rx+w, rx};
            float vy[4] = {ry, ry, ry+h, ry+h};
            
            // Verifica se algum vértice do retângulo está dentro do polígono
            for (int i = 0; i < 4; i++) {
                Ponto pt = criaPonto(vx[i], vy[i]);
                if (isInside(p, pt)) {
                    liberaPonto(pt);
                    return true;
                }
                liberaPonto(pt);
            }
            
            // Verifica se algum vértice do polígono está dentro do retângulo
            for (Celula c = getInicioLista(vertices_poly); c; c = getProxCelula(c)) {
                Ponto v = (Ponto)getConteudoCelula(c);
                float vx_p = getPontoX(v);
                float vy_p = getPontoY(v);
                
                if (vx_p >= rx && vx_p <= rx+w && vy_p >= ry && vy_p <= ry+h) {
                    return true;
                }
            }
            
            // Verifica interseção de arestas
            Celula c = getInicioLista(vertices_poly);
            if (!c) return false;
            
            Ponto v_first = (Ponto)getConteudoCelula(c);
            Ponto v_prev = v_first;
            
            for (c = getProxCelula(c); c; c = getProxCelula(c)) {
                Ponto v_curr = (Ponto)getConteudoCelula(c);
                float px1 = getPontoX(v_prev);
                float py1 = getPontoY(v_prev);
                float px2 = getPontoX(v_curr);
                float py2 = getPontoY(v_curr);
                
                // Testa interseção com cada aresta do retângulo
                for (int i = 0; i < 4; i++) {
                    int j = (i + 1) % 4;
                    if (segmentosIntersectam(px1, py1, px2, py2, vx[i], vy[i], vx[j], vy[j])) {
                        return true;
                    }
                }
                v_prev = v_curr;
            }
            
            // Fecha o polígono
            float px1 = getPontoX(v_prev);
            float py1 = getPontoY(v_prev);
            float px2 = getPontoX(v_first);
            float py2 = getPontoY(v_first);
            
            for (int i = 0; i < 4; i++) {
                int j = (i + 1) % 4;
                if (segmentosIntersectam(px1, py1, px2, py2, vx[i], vy[i], vx[j], vy[j])) {
                    return true;
                }
            }
            
            return false;
        }
        
        case CIRCLE: {
            CIRCULO circ = (CIRCULO)data;
            float cx = getXCirculo(circ);
            float cy = getYCirculo(circ);
            float r = getRaioCirculo(circ);
            
            // Verifica se o centro está dentro do polígono
            Ponto centro = criaPonto(cx, cy);
            if (isInside(p, centro)) {
                liberaPonto(centro);
                return true;
            }
            liberaPonto(centro);
            
            // Verifica se algum vértice do polígono está dentro do círculo
            for (Celula c = getInicioLista(vertices_poly); c; c = getProxCelula(c)) {
                Ponto v = (Ponto)getConteudoCelula(c);
                float dx = getPontoX(v) - cx;
                float dy = getPontoY(v) - cy;
                
                if (sqrt(dx*dx + dy*dy) <= r) {
                    return true;
                }
            }
            
            // Verifica distância de arestas do polígono ao centro do círculo
            Celula c = getInicioLista(vertices_poly);
            if (!c) return false;
            
            Ponto v_first = (Ponto)getConteudoCelula(c);
            Ponto v_prev = v_first;
            
            for (c = getProxCelula(c); c; c = getProxCelula(c)) {
                Ponto v_curr = (Ponto)getConteudoCelula(c);
                
                float dist = distanciaPontoSegmento(cx, cy,
                                                   getPontoX(v_prev), getPontoY(v_prev),
                                                   getPontoX(v_curr), getPontoY(v_curr));
                if (dist <= r) {
                    return true;
                }
                v_prev = v_curr;
            }
            
            // Fecha o polígono
            float dist = distanciaPontoSegmento(cx, cy,
                                               getPontoX(v_prev), getPontoY(v_prev),
                                               getPontoX(v_first), getPontoY(v_first));
            if (dist <= r) {
                return true;
            }
            
            return false;
        }
        
        case TEXT: {
            TEXTO t = (TEXTO)data;
            float tx = getXTexto(t);
            float ty = getYTexto(t);
            char anc = getAncoraTexto(t);
            char* texto = getTxtTexto(t);
            int len = strlen(texto);
            float L = 10.0f * len;
            
            // Calcula extremidades do segmento de texto
            float x1, y1, x2, y2;
            if (anc == 'i' || anc == 'I') {
                x1 = tx;
                y1 = ty;
                x2 = tx + L;
                y2 = ty;
            } else if (anc == 'f' || anc == 'F') {
                x1 = tx - L;
                y1 = ty;
                x2 = tx;
                y2 = ty;
            } else {
                x1 = tx - L/2;
                y1 = ty;
                x2 = tx + L/2;
                y2 = ty;
            }
            
            // Verifica se extremidades estão dentro do polígono
            Ponto p1 = criaPonto(x1, y1);
            Ponto p2 = criaPonto(x2, y2);
            
            if (isInside(p, p1) || isInside(p, p2)) {
                liberaPonto(p1);
                liberaPonto(p2);
                return true;
            }
            liberaPonto(p1);
            liberaPonto(p2);
            
            // Verifica interseção com arestas do polígono
            Celula c = getInicioLista(vertices_poly);
            if (!c) return false;
            
            Ponto v_first = (Ponto)getConteudoCelula(c);
            Ponto v_prev = v_first;
            
            for (c = getProxCelula(c); c; c = getProxCelula(c)) {
                Ponto v_curr = (Ponto)getConteudoCelula(c);
                
                if (segmentosIntersectam(x1, y1, x2, y2,
                                        getPontoX(v_prev), getPontoY(v_prev),
                                        getPontoX(v_curr), getPontoY(v_curr))) {
                    return true;
                }
                v_prev = v_curr;
            }
            
            // Fecha o polígono
            if (segmentosIntersectam(x1, y1, x2, y2,
                                    getPontoX(v_prev), getPontoY(v_prev),
                                    getPontoX(v_first), getPontoY(v_first))) {
                return true;
            }
            
            return false;
        }
        
        case ANTEPARO: {
            Anteparo a = (Anteparo)data;
            float x1 = getX1Anteparo(a);
            float y1 = getY1Anteparo(a);
            float x2 = getX2Anteparo(a);
            float y2 = getY2Anteparo(a);
            
            // Verifica se extremidades estão dentro do polígono
            Ponto p1 = criaPonto(x1, y1);
            Ponto p2 = criaPonto(x2, y2);
            
            if (isInside(p, p1) || isInside(p, p2)) {
                liberaPonto(p1);
                liberaPonto(p2);
                return true;
            }
            liberaPonto(p1);
            liberaPonto(p2);
            
            // Verifica interseção com arestas do polígono
            Celula c = getInicioLista(vertices_poly);
            if (!c) return false;
            
            Ponto v_first = (Ponto)getConteudoCelula(c);
            Ponto v_prev = v_first;
            
            for (c = getProxCelula(c); c; c = getProxCelula(c)) {
                Ponto v_curr = (Ponto)getConteudoCelula(c);
                
                if (segmentosIntersectam(x1, y1, x2, y2,
                                        getPontoX(v_prev), getPontoY(v_prev),
                                        getPontoX(v_curr), getPontoY(v_curr))) {
                    return true;
                }
                v_prev = v_curr;
            }
            
            // Fecha o polígono
            if (segmentosIntersectam(x1, y1, x2, y2,
                                    getPontoX(v_prev), getPontoY(v_prev),
                                    getPontoX(v_first), getPontoY(v_first))) {
                return true;
            }
            
            return false;
        }
            
        default:
            return false;
    }
}
