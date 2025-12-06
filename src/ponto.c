#include "ponto.h"
#include <stdlib.h>

typedef struct {
    float x;
    float y;
} stPonto;

Ponto criaPonto(float x, float y) {
    stPonto* p = malloc(sizeof(stPonto));
    if (!p) return NULL;
    p->x = x;
    p->y = y;
    return p;
}

void liberaPonto(Ponto ponto) {
    free(ponto);
}

float getPontoX(Ponto ponto) {
    if (!ponto) return 0.0f;
    stPonto* p = (stPonto*)ponto;
    return p->x;
}

float getPontoY(Ponto ponto) {
    if (!ponto) return 0.0f;
    stPonto* p = (stPonto*)ponto;
    return p->y;
}

void setPontoX(Ponto ponto, float x) {
    if (!ponto) return;
    stPonto* p = (stPonto*)ponto;
    p->x = x;
}

void setPontoY(Ponto ponto, float y) {
    if (!ponto) return;
    stPonto* p = (stPonto*)ponto;
    p->y = y;
}
