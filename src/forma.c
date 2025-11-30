#include "forma.h"
#include "circulo.h"
#include "retangulo.h"
#include "linha.h"
#include "texto.h"
#include "text_style.h"
#include "anteparo.h"
#include <stdlib.h>
#include <stdio.h>


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
