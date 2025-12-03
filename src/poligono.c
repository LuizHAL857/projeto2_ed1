#include "poligono.h"
#include <stdlib.h>
#include <math.h>

#define EPSILON 1e-9

typedef struct {
    Lista vertices;   // Lista de Ponto*
    Lista segmentos;  // Lista de Segmento*
} stPoligono;

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
    
    // Libera todos os vértices
    while (!listaVazia(p->vertices)) {
        Ponto* ponto = removeInicioLista(p->vertices);
        free(ponto);
    }
    liberaLista(p->vertices);
    
    // Libera todos os segmentos
    while (!listaVazia(p->segmentos)) {
        Segmento* seg = removeInicioLista(p->segmentos);
        free(seg);
    }
    liberaLista(p->segmentos);
    
    free(p);
}

/* ================= Inserção ================= */

void insereVertice(Poligono pol, Ponto ponto) {
    if (!pol) return;
    
    stPoligono* p = (stPoligono*)pol;
    
    Ponto* novo = malloc(sizeof(Ponto));
    if (!novo) return;
    
    novo->x = ponto.x;
    novo->y = ponto.y;
    
    insereFinalLista(p->vertices, novo);
}

void insereSegmento(Poligono pol, Segmento seg) {
    if (!pol) return;
    
    stPoligono* p = (stPoligono*)pol;
    
    Segmento* novo = malloc(sizeof(Segmento));
    if (!novo) return;
    
    novo->p1 = seg.p1;
    novo->p2 = seg.p2;
    
    insereFinalLista(p->segmentos, novo);
}

/* ================= Consultas ================= */

BoundingBox getBoundingBox(Poligono pol) {
    BoundingBox bbox = {INFINITY, INFINITY, -INFINITY, -INFINITY};
    
    if (!pol) return bbox;
    
    stPoligono* p = (stPoligono*)pol;
    
    if (listaVazia(p->vertices)) return bbox;
    
    for (Celula c = getInicioLista(p->vertices); c; c = getProxCelula(c)) {
        Ponto* ponto = (Ponto*)getConteudoCelula(c);
        
        if (ponto->x < bbox.min_x) bbox.min_x = ponto->x;
        if (ponto->x > bbox.max_x) bbox.max_x = ponto->x;
        if (ponto->y < bbox.min_y) bbox.min_y = ponto->y;
        if (ponto->y > bbox.max_y) bbox.max_y = ponto->y;
    }
    
    return bbox;
}

bool isInside(Poligono pol, Ponto ponto) {
    if (!pol) return false;
    
    stPoligono* p = (stPoligono*)pol;
    
    if (listaVazia(p->vertices)) return false;
    
    // Algoritmo de ray casting
    // Lança um raio horizontal para a direita e conta quantas vezes cruza as arestas
    int num_vertices = getTamanhoLista(p->vertices);
    if (num_vertices < 3) return false;
    
    int interseccoes = 0;
    
    Celula c1 = getInicioLista(p->vertices);
    Celula c2 = getProxCelula(c1);
    
    while (c2) {
        Ponto* p1 = (Ponto*)getConteudoCelula(c1);
        Ponto* p2 = (Ponto*)getConteudoCelula(c2);
        
        // Verifica se o raio horizontal cruza esta aresta
        if (((p1->y > ponto.y) != (p2->y > ponto.y)) &&
            (ponto.x < (p2->x - p1->x) * (ponto.y - p1->y) / (p2->y - p1->y) + p1->x)) {
            interseccoes++;
        }
        
        c1 = c2;
        c2 = getProxCelula(c2);
    }
    
    // Fecha o polígono: última aresta conecta último vértice ao primeiro
    Ponto* p_ultimo = (Ponto*)getConteudoCelula(c1);
    Ponto* p_primeiro = (Ponto*)getConteudoCelula(getInicioLista(p->vertices));
    
    if (((p_ultimo->y > ponto.y) != (p_primeiro->y > ponto.y)) &&
        (ponto.x < (p_primeiro->x - p_ultimo->x) * (ponto.y - p_ultimo->y) / 
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
    // Verifica se NÃO há intersecção e inverte o resultado
    // Se um retângulo está totalmente à esquerda, direita, acima ou abaixo do outro
    if (a.max_x < b.min_x || a.min_x > b.max_x ||
        a.max_y < b.min_y || a.min_y > b.max_y) {
        return false;
    }
    return true;
}
