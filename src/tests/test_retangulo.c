#include "test_framework.h"
#include "../src/retangulo.h"
#include <stdlib.h>
#include <string.h>

/* Teste: Criar e destruir retângulo */
void teste_criar_destruir_retangulo() {
    RETANGULO r = criaRetangulo(1, 100.0, 200.0, 50.0, 80.0, "red", "blue");
    ASSERT_NOT_NULL(r, "Retângulo deve ser criado");
    desalocarRetangulo(r);
}

/* Teste: Getters */
void teste_retangulo_getters() {
    RETANGULO r = criaRetangulo(42, 150.5, 250.5, 75.0, 100.0, "green", "yellow");
    
    ASSERT_EQUAL(42, getIDRetangulo(r), "ID deve ser 42");
    ASSERT_FLOAT_EQUAL(150.5, getXRetangulo(r), 0.001, "X deve ser 150.5");
    ASSERT_FLOAT_EQUAL(250.5, getYRetangulo(r), 0.001, "Y deve ser 250.5");
    ASSERT_FLOAT_EQUAL(75.0, getAlturaRetangulo(r), 0.001, "Altura deve ser 75.0");
    ASSERT_FLOAT_EQUAL(100.0, getLarguraRetangulo(r), 0.001, "Largura deve ser 100.0");
    ASSERT_STR_EQUAL("green", getCorBRetangulo(r), "Cor da borda deve ser green");
    ASSERT_STR_EQUAL("yellow", getCorPRetangulo(r), "Cor de preenchimento deve ser yellow");
    
    desalocarRetangulo(r);
}

/* Teste: Setters */
void teste_retangulo_setters() {
    RETANGULO r = criaRetangulo(1, 100.0, 200.0, 50.0, 80.0, "red", "blue");
    
    setIDRetangulo(r, 99);
    ASSERT_EQUAL(99, getIDRetangulo(r), "ID deve ser atualizado para 99");
    
    setXRetangulo(r, 300.0);
    ASSERT_FLOAT_EQUAL(300.0, getXRetangulo(r), 0.001, "X deve ser atualizado para 300.0");
    
    setYRetangulo(r, 400.0);
    ASSERT_FLOAT_EQUAL(400.0, getYRetangulo(r), 0.001, "Y deve ser atualizado para 400.0");
    
    setCorBRetangulo(r, "purple");
    ASSERT_STR_EQUAL("purple", getCorBRetangulo(r), "Cor da borda deve ser atualizada para purple");
    
    setCorPRetangulo(r, "orange");
    ASSERT_STR_EQUAL("orange", getCorPRetangulo(r), "Cor de preenchimento deve ser atualizada para orange");
    
    desalocarRetangulo(r);
}

/* Teste: Cálculo de área */
void teste_area_retangulo() {
    RETANGULO r = criaRetangulo(1, 0.0, 0.0, 10.0, 20.0, "red", "blue");
    
    float area = calculoAreaRetangulo(r);
    ASSERT_FLOAT_EQUAL(200.0, area, 0.001, "Área deve ser largura * altura");
    
    desalocarRetangulo(r);
}

/* Teste: Clonar retângulo */
void teste_clonar_retangulo() {
    RETANGULO r1 = criaRetangulo(1, 100.0, 200.0, 50.0, 80.0, "red", "blue");
    RETANGULO r2 = clonaRetangulo(r1, 2, 10.0, 20.0);
    
    ASSERT_NOT_NULL(r2, "Retângulo clonado deve ser criado");
    ASSERT_EQUAL(2, getIDRetangulo(r2), "Retângulo clonado deve ter novo ID");
    ASSERT_FLOAT_EQUAL(110.0, getXRetangulo(r2), 0.001, "X clonado deve ser deslocado por dx");
    ASSERT_FLOAT_EQUAL(220.0, getYRetangulo(r2), 0.001, "Y clonado deve ser deslocado por dy");
    ASSERT_FLOAT_EQUAL(50.0, getAlturaRetangulo(r2), 0.001, "Altura clonada deve ser igual");
    ASSERT_FLOAT_EQUAL(80.0, getLarguraRetangulo(r2), 0.001, "Largura clonada deve ser igual");
    ASSERT_STR_EQUAL("red", getCorBRetangulo(r2), "Cor da borda clonada deve ser igual");
    ASSERT_STR_EQUAL("blue", getCorPRetangulo(r2), "Cor de preenchimento clonada deve ser igual");
    
    desalocarRetangulo(r1);
    desalocarRetangulo(r2);
}

/* Teste: Retângulo com dimensões zero */
void teste_retangulo_dimensoes_zero() {
    RETANGULO r = criaRetangulo(1, 100.0, 200.0, 0.0, 0.0, "red", "blue");
    
    ASSERT_FLOAT_EQUAL(0.0, getAlturaRetangulo(r), 0.001, "Altura deve ser 0");
    ASSERT_FLOAT_EQUAL(0.0, getLarguraRetangulo(r), 0.001, "Largura deve ser 0");
    ASSERT_FLOAT_EQUAL(0.0, calculoAreaRetangulo(r), 0.001, "Área deve ser 0");
    
    desalocarRetangulo(r);
}

int main() {
    RESETAR_ESTATISTICAS();
    
    EXECUTAR_TESTE(teste_criar_destruir_retangulo);
    EXECUTAR_TESTE(teste_retangulo_getters);
    EXECUTAR_TESTE(teste_retangulo_setters);
    EXECUTAR_TESTE(teste_area_retangulo);
    EXECUTAR_TESTE(teste_clonar_retangulo);
    EXECUTAR_TESTE(teste_retangulo_dimensoes_zero);
    
    IMPRIMIR_RESUMO_TESTES("Módulo Retângulo");
    
    return CODIGO_SAIDA_TESTE();
}
