#include "test_framework.h"
#include "../src/linha.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Teste: Criar e destruir linha */
void teste_criar_destruir_linha() {
    LINHA l = criaLinha(1, 0.0, 0.0, 100.0, 100.0, "red");
    ASSERT_NOT_NULL(l, "Linha deve ser criada");
    desalocaLinha(l);
}

/* Teste: Getters */
void teste_linha_getters() {
    LINHA l = criaLinha(42, 10.5, 20.5, 30.5, 40.5, "blue");
    
    ASSERT_EQUAL(42, getIDLinha(l), "ID deve ser 42");
    ASSERT_FLOAT_EQUAL(10.5, getX1Linha(l), 0.001, "X1 deve ser 10.5");
    ASSERT_FLOAT_EQUAL(20.5, getY1Linha(l), 0.001, "Y1 deve ser 20.5");
    ASSERT_FLOAT_EQUAL(30.5, getX2Linha(l), 0.001, "X2 deve ser 30.5");
    ASSERT_FLOAT_EQUAL(40.5, getY2Linha(l), 0.001, "Y2 deve ser 40.5");
    ASSERT_STR_EQUAL("blue", getCorLinha(l), "Cor deve ser blue");
    
    desalocaLinha(l);
}

/* Teste: Setters */
void teste_linha_setters() {
    LINHA l = criaLinha(1, 0.0, 0.0, 100.0, 100.0, "red");
    
    setIDLinha(l, 99);
    ASSERT_EQUAL(99, getIDLinha(l), "ID deve ser atualizado para 99");
    
    setX1Linha(l, 50.0);
    ASSERT_FLOAT_EQUAL(50.0, getX1Linha(l), 0.001, "X1 deve ser atualizado para 50.0");
    
    setY1Linha(l, 60.0);
    ASSERT_FLOAT_EQUAL(60.0, getY1Linha(l), 0.001, "Y1 deve ser atualizado para 60.0");
    
    setX2Linha(l, 150.0);
    ASSERT_FLOAT_EQUAL(150.0, getX2Linha(l), 0.001, "X2 deve ser atualizado para 150.0");
    
    setY2Linha(l, 160.0);
    ASSERT_FLOAT_EQUAL(160.0, getY2Linha(l), 0.001, "Y2 deve ser atualizado para 160.0");
    
    setCorLinha(l, "green");
    ASSERT_STR_EQUAL("green", getCorLinha(l), "Cor deve ser atualizada para green");
    
    desalocaLinha(l);
}

/* Teste: Cálculo de área */
void teste_area_linha() {
    LINHA l = criaLinha(1, 0.0, 0.0, 3.0, 4.0, "red");
    
    float area = calculoAreaLinha(l);
    /* Comprimento = sqrt(3^2 + 4^2) = 5, Área = 2 * 5 = 10 */
    ASSERT_FLOAT_EQUAL(10.0, area, 0.001, "Área deve ser 2 * comprimento");
    
    desalocaLinha(l);
}

/* Teste: Clonar linha */
void teste_clonar_linha() {
    LINHA l1 = criaLinha(1, 10.0, 20.0, 30.0, 40.0, "red");
    LINHA l2 = clonaLinha(l1, 2, 5.0, 10.0);
    
    ASSERT_NOT_NULL(l2, "Linha clonada deve ser criada");
    ASSERT_EQUAL(2, getIDLinha(l2), "Linha clonada deve ter novo ID");
    ASSERT_FLOAT_EQUAL(15.0, getX1Linha(l2), 0.001, "X1 clonado deve ser deslocado por dx");
    ASSERT_FLOAT_EQUAL(30.0, getY1Linha(l2), 0.001, "Y1 clonado deve ser deslocado por dy");
    ASSERT_FLOAT_EQUAL(35.0, getX2Linha(l2), 0.001, "X2 clonado deve ser deslocado por dx");
    ASSERT_FLOAT_EQUAL(50.0, getY2Linha(l2), 0.001, "Y2 clonado deve ser deslocado por dy");
    ASSERT_STR_EQUAL("red", getCorLinha(l2), "Cor clonada deve ser igual");
    
    desalocaLinha(l1);
    desalocaLinha(l2);
}

/* Teste: Linha com comprimento zero */
void teste_linha_comprimento_zero() {
    LINHA l = criaLinha(1, 100.0, 200.0, 100.0, 200.0, "red");
    
    float area = calculoAreaLinha(l);
    ASSERT_FLOAT_EQUAL(0.0, area, 0.001, "Área de linha com comprimento zero deve ser 0");
    
    desalocaLinha(l);
}

int main() {
    RESETAR_ESTATISTICAS();
    
    EXECUTAR_TESTE(teste_criar_destruir_linha);
    EXECUTAR_TESTE(teste_linha_getters);
    EXECUTAR_TESTE(teste_linha_setters);
    EXECUTAR_TESTE(teste_area_linha);
    EXECUTAR_TESTE(teste_clonar_linha);
    EXECUTAR_TESTE(teste_linha_comprimento_zero);
    
    IMPRIMIR_RESUMO_TESTES("Módulo Linha");
    
    return CODIGO_SAIDA_TESTE();
}
