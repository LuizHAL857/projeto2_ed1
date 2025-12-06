#include "forma.h"
#include "circulo.h"
#include "retangulo.h"
#include "linha.h"
#include "texto.h"
#include "text_style.h"
#include "anteparo.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


typedef struct {
    tipo_forma tipo;  
    void* data;       
} Forma_t;

Forma criaForma(tipo_forma tipo, void* data) {
    if (data == NULL) {
        return NULL;
    }

    Forma_t* forma = (Forma_t*)malloc(sizeof(Forma_t));
    if (forma == NULL) {
        printf("Erro de alocação em criaForma\n");
        return NULL;
    }

    forma->tipo = tipo;
    forma->data = data;

    return (Forma)forma;
}

tipo_forma getTipoForma(Forma f) {
    if (f == NULL) {
        return CIRCLE; // Valor padrão em caso de erro
    }

    Forma_t* forma = (Forma_t*)f;
    return forma->tipo;
}

void* getDataForma(Forma f) {
    if (f == NULL) {
        return NULL;
    }

    Forma_t* forma = (Forma_t*)f;
    return forma->data;
}

void escreveFormaSVG(Forma f, FILE* arquivo) {
    if (f == NULL || arquivo == NULL) {
        return;
    }

    Forma_t* forma = (Forma_t*)f;

    switch (forma->tipo) {
        case CIRCLE: {
            CIRCULO c = (CIRCULO)forma->data;
            fprintf(arquivo,
                    "<circle cx='%.2f' cy='%.2f' r='%.2f' fill='%s' stroke='%s'/>\n",
                    getXCirculo(c), getYCirculo(c),
                    getRaioCirculo(c), getCorPCirculo(c),
                    getCorBCirculo(c));
            break;
        }

        case RECTANGLE: {
            RETANGULO r = (RETANGULO)forma->data;
            fprintf(arquivo,
                    "<rect x='%.2f' y='%.2f' width='%.2f' height='%.2f' fill='%s' "
                    "stroke='%s'/>\n",
                    getXRetangulo(r), getYRetangulo(r),
                    getLarguraRetangulo(r), getAlturaRetangulo(r),
                    getCorPRetangulo(r),
                    getCorBRetangulo(r));
            break;
        }

        case LINE: {
            LINHA l = (LINHA)forma->data;
            fprintf(arquivo,
                    "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='%s'/>\n",
                    getX1Linha(l), getY1Linha(l), getX2Linha(l),
                    getY2Linha(l), getCorLinha(l));
            break;
        }

        case TEXT: {
            TEXTO t = (TEXTO)forma->data;
            char ancora = getAncoraTexto(t);
            char* texto_ancora = "start";

            if (ancora == 'm' || ancora == 'M') {
                texto_ancora = "middle";
            } else if (ancora == 'f' || ancora == 'F') {
                texto_ancora = "end";
            } else if (ancora == 'i' || ancora == 'I') {
                texto_ancora = "start";
            }

            fprintf(arquivo,
                    "<text x='%.2f' y='%.2f' fill='%s' stroke='%s' "
                    "text-anchor='%s'>%s</text>\n",
                    getXTexto(t), getYTexto(t), getCorPTexto(t),
                    getCorBTexto(t), texto_ancora, getTxtTexto(t));
            break;
        }

        case TEXT_STYLE:
            
            break;

        case ANTEPARO: {
            Anteparo a = (Anteparo)forma->data;
            fprintf(arquivo,
                    "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='%s'/>\n",
                    getX1Anteparo(a), getY1Anteparo(a), getX2Anteparo(a),
                    getY2Anteparo(a), getCorAnteparo(a));
            break;
        }

        default:
            break;
    }
}

void desalocaForma(Forma f) {
    if (f == NULL) {
        return;
    }

    Forma_t* forma = (Forma_t*)f;

    // Desaloca os dados específicos da forma
    switch (forma->tipo) {
        case CIRCLE:
            desalocaCirculo(forma->data);
            break;

        case RECTANGLE:
            desalocarRetangulo(forma->data);
            break;

        case LINE:
            desalocaLinha(forma->data);
            break;

        case TEXT:
            desalocaTexto(forma->data);
            break;

        case TEXT_STYLE:
            desalocaTextStyle(forma->data);
            break;

        case ANTEPARO:
            desalocaAnteparo(forma->data);
            break;

        default:
            break;
    }

    // Desaloca a estrutura da forma
    free(forma);
}

BoundingBox getBBForma(Forma f) {
    tipo_forma tipo = getTipoForma(f);
    void* data = getDataForma(f);
    
    float min_x = INFINITY, min_y = INFINITY;
    float max_x = -INFINITY, max_y = -INFINITY;
    
    switch(tipo) {
        case CIRCLE: {
            float x = getXCirculo(data);
            float y = getYCirculo(data);
            float r = getRaioCirculo(data);
            min_x = x - r; max_x = x + r;
            min_y = y - r; max_y = y + r;
            break;
        }
        case RECTANGLE: {
            float x = getXRetangulo(data);
            float y = getYRetangulo(data);
            float w = getLarguraRetangulo(data);
            float h = getAlturaRetangulo(data);
            min_x = x; max_x = x + w;
            min_y = y; max_y = y + h;
            break;
        }
        case LINE: {
            float x1 = getX1Linha(data);
            float y1 = getY1Linha(data);
            float x2 = getX2Linha(data);
            float y2 = getY2Linha(data);
            min_x = fmin(x1, x2); max_x = fmax(x1, x2);
            min_y = fmin(y1, y2); max_y = fmax(y1, y2);
            break;
        }
        case TEXT: {
            float x = getXTexto(data);
            float y = getYTexto(data);
            min_x = x; max_x = x + 10; 
            min_y = y; max_y = y + 10;
            break;
        }
        case ANTEPARO: {
            float x1 = getX1Anteparo(data);
            float y1 = getY1Anteparo(data);
            float x2 = getX2Anteparo(data);
            float y2 = getY2Anteparo(data);
            min_x = fmin(x1, x2); max_x = fmax(x1, x2);
            min_y = fmin(y1, y2); max_y = fmax(y1, y2);
            break;
        }
        default: break;
    }
    return criaBoundingBox(min_x, min_y, max_x, max_y);
}

bool formaEstaDentro(Poligono p, Forma f) {
    tipo_forma tipo = getTipoForma(f);
    void* data = getDataForma(f);
    
    switch(tipo) {
        case CIRCLE: {
            // Para círculos, testa o centro e pontos ao redor da circunferência
            float cx = getXCirculo(data);
            float cy = getYCirculo(data);
            float r = getRaioCirculo(data);
            
            // Testa o centro
            Ponto pt = criaPonto(cx, cy);
            bool resultado = isInside(p, pt);
            liberaPonto(pt);
            if (resultado) return true;
            
            // Testa 8 pontos ao redor da circunferência
            for (int i = 0; i < 8; i++) {
                float angulo = (2.0 * M_PI * i) / 8.0;
                pt = criaPonto(cx + r * cos(angulo), cy + r * sin(angulo));
                resultado = isInside(p, pt);
                liberaPonto(pt);
                if (resultado) return true;
            }
            return false;
        }
            
        case RECTANGLE: {
            // Para retângulos, testa os 4 cantos e o centro
            float x = getXRetangulo(data);
            float y = getYRetangulo(data);
            float w = getLarguraRetangulo(data);
            float h = getAlturaRetangulo(data);
            
            // Testa os 4 cantos
            Ponto cantos[4] = {
                criaPonto(x, y),
                criaPonto(x + w, y),
                criaPonto(x, y + h),
                criaPonto(x + w, y + h)
            };
            
            for (int i = 0; i < 4; i++) {
                if (isInside(p, cantos[i])) {
                    for (int j = 0; j < 4; j++) liberaPonto(cantos[j]);
                    return true;
                }
            }
            for (int i = 0; i < 4; i++) liberaPonto(cantos[i]);
            
            // Testa o centro
            Ponto pt = criaPonto(x + w/2, y + h/2);
            bool resultado = isInside(p, pt);
            liberaPonto(pt);
            return resultado;
        }
            
        case LINE: {
            // Para linhas, testa vários pontos ao longo do segmento
            float x1 = getX1Linha(data);
            float y1 = getY1Linha(data);
            float x2 = getX2Linha(data);
            float y2 = getY2Linha(data);
            
            // Testa 5 pontos ao longo da linha
            for (int i = 0; i <= 4; i++) {
                float t = i / 4.0f;
                Ponto pt = criaPonto(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
                bool resultado = isInside(p, pt);
                liberaPonto(pt);
                if (resultado) return true;
            }
            return false;
        }
            
        case TEXT: {
            // Para texto, testa o ponto de âncora
            Ponto pt = criaPonto(getXTexto(data), getYTexto(data));
            bool resultado = isInside(p, pt);
            liberaPonto(pt);
            return resultado;
        }
            
        case ANTEPARO: {
            // Para anteparos, verifica múltiplos pontos ao longo do segmento
            float p1_x = getX1Anteparo(data);
            float p1_y = getY1Anteparo(data);
            float p2_x = getX2Anteparo(data);
            float p2_y = getY2Anteparo(data);
            
            // Testa vários pontos ao longo do segmento
            for (int i = 0; i <= 4; i++) {
                float t = i / 4.0f;
                float pt_x = p1_x + t * (p2_x - p1_x);
                float pt_y = p1_y + t * (p2_y - p1_y);
                
                Ponto pt = criaPonto(pt_x, pt_y);
                if (isInside(p, pt)) {
                    liberaPonto(pt);
                    return true;
                }
                liberaPonto(pt);
                
                // Testa pontos deslocados para ambos os lados do segmento
                float dx = p2_y - p1_y;  // perpendicular ao segmento
                float dy = -(p2_x - p1_x);
                float len = sqrt(dx*dx + dy*dy);
                if (len > 0.001f) {
                    dx /= len;
                    dy /= len;
                    
                    // Testa deslocamento de 1.0 unidade para um lado
                    Ponto pt_offset1 = criaPonto(pt_x + dx * 1.0f, pt_y + dy * 1.0f);
                    if (isInside(p, pt_offset1)) {
                        liberaPonto(pt_offset1);
                        return true;
                    }
                    liberaPonto(pt_offset1);
                    
                    // Testa deslocamento para o outro lado
                    Ponto pt_offset2 = criaPonto(pt_x - dx * 1.0f, pt_y - dy * 1.0f);
                    if (isInside(p, pt_offset2)) {
                        liberaPonto(pt_offset2);
                        return true;
                    }
                    liberaPonto(pt_offset2);
                }
            }
            
            return false;
        }
            
            return false;
    }
}

Forma clonaForma(Forma f, int novo_id, float dx, float dy) {
    if (!f) return NULL;
    
    tipo_forma tipo = getTipoForma(f);
    void* data = getDataForma(f);
    void* clone_data = NULL;
    
    switch(tipo) {
        case CIRCLE:
            clone_data = clonaCirculo(data, novo_id, dx, dy);
            break;
        case RECTANGLE:
            clone_data = clonaRetangulo(data, novo_id, dx, dy);
            break;
        case LINE:
            clone_data = clonaLinha(data, novo_id, dx, dy);
            break;
        case TEXT:
            clone_data = clonaTexto(data, novo_id, dx, dy);
            break;
        case ANTEPARO:
            clone_data = clonaAnteparo(data, novo_id, dx, dy);
            break;
        default:
            return NULL;
    }
    
    if (clone_data == NULL) return NULL;
    
    return criaForma(tipo, clone_data);
}

void setCorPForma(Forma f, const char* cor) {
    if (!f || !cor) return;
    
    tipo_forma tipo = getTipoForma(f);
    void* data = getDataForma(f);
    
    switch(tipo) {
        case CIRCLE:
            setCorPCirculo(data, cor);
            break;
        case RECTANGLE:
            setCorPRetangulo(data, cor);
            break;
        case TEXT:
            setCorPTexto(data, cor);
            break;
        default:
            break;
    }
}

void setCorBForma(Forma f, const char* cor) {
    if (!f || !cor) return;
    
    tipo_forma tipo = getTipoForma(f);
    void* data = getDataForma(f);
    
    switch(tipo) {
        case CIRCLE:
            setCorBCirculo(data, cor);
            break;
        case RECTANGLE:
            setCorBRetangulo(data, cor);
            break;
        case LINE:
            setCorLinha(data, cor);
            break;
        case TEXT:
            setCorBTexto(data, cor);
            break;
        case ANTEPARO:
            setCorAnteparo(data, cor);
            break;
        default:
            break;
    }
}

