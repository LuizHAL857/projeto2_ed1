#define _USE_MATH_DEFINES
#include "test_framework.h"
#include "../src/circulo.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14
#endif

/* Teste: Criar e destruir círculo */
void teste_criar_destruir_circulo() {
    CIRCULO c = criaCirculo(1, 100.0, 200.0, 50.0, "red", "blue");
    ASSERT_NOT_NULL(c, "Círculo deve ser criado");
    desalocaCirculo(c);
}

/* Teste: Getters */
void teste_circulo_getters() {
    CIRCULO c = criaCirculo(42, 150.5, 250.5, 75.0, "green", "yellow");
    
    ASSERT_EQUAL(42, getIDCirculo(c), "ID deve ser 42");
    ASSERT_FLOAT_EQUAL(150.5, getXCirculo(c), 0.001, "X deve ser 150.5");
    ASSERT_FLOAT_EQUAL(250.5, getYCirculo(c), 0.001, "Y deve ser 250.5");
    ASSERT_FLOAT_EQUAL(75.0, getRaioCirculo(c), 0.001, "Raio deve ser 75.0");
    ASSERT_STR_EQUAL("green", getCorPCirculo(c), "Cor de preenchimento deve ser green");
    ASSERT_STR_EQUAL("yellow", getCorBCirculo(c), "Cor da borda deve ser yellow");
    
    desalocaCirculo(c);
}

/* Teste: Setters */
void teste_circulo_setters() {
    CIRCULO c = criaCirculo(1, 100.0, 200.0, 50.0, "red", "blue");
    
    setIDCirculo(c, 99);
    ASSERT_EQUAL(99, getIDCirculo(c), "ID deve ser atualizado para 99");
    
    setXCirculo(c, 300.0);
    ASSERT_FLOAT_EQUAL(300.0, getXCirculo(c), 0.001, "X deve ser atualizado para 300.0");
    
    setYCirculo(c, 400.0);
    ASSERT_FLOAT_EQUAL(400.0, getYCirculo(c), 0.001, "Y deve ser atualizado para 400.0");
    
    setRaioCirculo(c, 100.0);
    ASSERT_FLOAT_EQUAL(100.0, getRaioCirculo(c), 0.001, "Raio deve ser atualizado para 100.0");
    
    setCorPCirculo(c, "purple");
    ASSERT_STR_EQUAL("purple", getCorPCirculo(c), "Cor de preenchimento deve ser atualizada para purple");
    
    setCorBCirculo(c, "orange");
    ASSERT_STR_EQUAL("orange", getCorBCirculo(c), "Cor da borda deve ser atualizada para orange");
    
    desalocaCirculo(c);
}

/* Teste: Cálculo de área */
void teste_area_circulo() {
    CIRCULO c = criaCirculo(1, 0.0, 0.0, 10.0, "red", "blue");
    
    float area = calculoAreaCirculo(c);
    float esperado = M_PI * 10.0 * 10.0;
    ASSERT_FLOAT_EQUAL(esperado, area, 0.001, "Área deve ser π * r²");
    
    desalocaCirculo(c);
}

/* Teste: Clonar círculo */
void teste_clonar_circulo() {
    CIRCULO c1 = criaCirculo(1, 100.0, 200.0, 50.0, "red", "blue");
    CIRCULO c2 = clonaCirculo(c1, 2, 10.0, 20.0);
    
    ASSERT_NOT_NULL(c2, "Círculo clonado deve ser criado");
    ASSERT_EQUAL(2, getIDCirculo(c2), "Círculo clonado deve ter novo ID");
    ASSERT_FLOAT_EQUAL(110.0, getXCirculo(c2), 0.001, "X clonado deve ser deslocado por dx");
    ASSERT_FLOAT_EQUAL(220.0, getYCirculo(c2), 0.001, "Y clonado deve ser deslocado por dy");
    ASSERT_FLOAT_EQUAL(50.0, getRaioCirculo(c2), 0.001, "Raio clonado deve ser igual");
    ASSERT_STR_EQUAL("red", getCorPCirculo(c2), "Cor de preenchimento clonada deve ser igual");
    ASSERT_STR_EQUAL("blue", getCorBCirculo(c2), "Cor da borda clonada deve ser igual");
    
    desalocaCirculo(c1);
    desalocaCirculo(c2);
}

/* Teste: Círculo com raio zero */
void teste_circulo_raio_zero() {
    CIRCULO c = criaCirculo(1, 100.0, 200.0, 0.0, "red", "blue");
    
    ASSERT_FLOAT_EQUAL(0.0, getRaioCirculo(c), 0.001, "Raio deve ser 0");
    ASSERT_FLOAT_EQUAL(0.0, calculoAreaCirculo(c), 0.001, "Área de círculo com raio zero deve ser 0");
    
    desalocaCirculo(c);
}

int main() {
    RESETAR_ESTATISTICAS();
    
    EXECUTAR_TESTE(teste_criar_destruir_circulo);
    EXECUTAR_TESTE(teste_circulo_getters);
    EXECUTAR_TESTE(teste_circulo_setters);
    EXECUTAR_TESTE(teste_area_circulo);
    EXECUTAR_TESTE(teste_clonar_circulo);
    EXECUTAR_TESTE(teste_circulo_raio_zero);
    
    IMPRIMIR_RESUMO_TESTES("Módulo Círculo");
    
    return CODIGO_SAIDA_TESTE();
}
