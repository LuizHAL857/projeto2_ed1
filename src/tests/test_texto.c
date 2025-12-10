#include "test_framework.h"
#include "../src/texto.h"
#include <stdlib.h>
#include <string.h>

/* Teste: Criar e destruir texto */
void teste_criar_destruir_texto() {
    TEXTO t = criaTexto(1, 100.0, 200.0, "red", "blue", 'i', "Olá Mundo");
    ASSERT_NOT_NULL(t, "Texto deve ser criado");
    desalocaTexto(t);
}

/* Teste: Getters */
void teste_texto_getters() {
    TEXTO t = criaTexto(42, 150.5, 250.5, "green", "yellow", 'm', "Texto Teste");
    
    ASSERT_EQUAL(42, getIDTexto(t), "ID deve ser 42");
    ASSERT_FLOAT_EQUAL(150.5, getXTexto(t), 0.001, "X deve ser 150.5");
    ASSERT_FLOAT_EQUAL(250.5, getYTexto(t), 0.001, "Y deve ser 250.5");
    ASSERT_EQUAL('m', getAncoraTexto(t), "Âncora deve ser 'm'");
    ASSERT_STR_EQUAL("green", getCorBTexto(t), "Cor da borda deve ser green");
    ASSERT_STR_EQUAL("yellow", getCorPTexto(t), "Cor de preenchimento deve ser yellow");
    ASSERT_STR_EQUAL("Texto Teste", getTxtTexto(t), "Conteúdo do texto deve ser 'Texto Teste'");
    
    desalocaTexto(t);
}

/* Teste: Setters */
void teste_texto_setters() {
    TEXTO t = criaTexto(1, 100.0, 200.0, "red", "blue", 'i', "Olá");
    
    setIDTexto(t, 99);
    ASSERT_EQUAL(99, getIDTexto(t), "ID deve ser atualizado para 99");
    
    setXTexto(t, 300.0);
    ASSERT_FLOAT_EQUAL(300.0, getXTexto(t), 0.001, "X deve ser atualizado para 300.0");
    
    setYTexto(t, 400.0);
    ASSERT_FLOAT_EQUAL(400.0, getYTexto(t), 0.001, "Y deve ser atualizado para 400.0");
    
    setAncoraTexto(t, 'f');
    ASSERT_EQUAL('f', getAncoraTexto(t), "Âncora deve ser atualizada para 'f'");
    
    setCorBTexto(t, "purple");
    ASSERT_STR_EQUAL("purple", getCorBTexto(t), "Cor da borda deve ser atualizada para purple");
    
    setCorPTexto(t, "orange");
    ASSERT_STR_EQUAL("orange", getCorPTexto(t), "Cor de preenchimento deve ser atualizada para orange");
    
    setTxtTexto(t, "Novo Texto");
    ASSERT_STR_EQUAL("Novo Texto", getTxtTexto(t), "Conteúdo do texto deve ser atualizado para 'Novo Texto'");
    
    desalocaTexto(t);
}

/* Teste: Cálculo de área */
void teste_area_texto() {
    TEXTO t = criaTexto(1, 0.0, 0.0, "red", "blue", 'i', "Olá");
    
    float area = calculoAreaTexto(t);
    ASSERT_TRUE(area > 0, "Área do texto deve ser positiva");
    
    desalocaTexto(t);
}

/* Teste: Clonar texto */
void teste_clonar_texto() {
    TEXTO t1 = criaTexto(1, 100.0, 200.0, "red", "blue", 'i', "Original");
    TEXTO t2 = clonaTexto(t1, 2, 10.0, 20.0);
    
    ASSERT_NOT_NULL(t2, "Texto clonado deve ser criado");
    ASSERT_EQUAL(2, getIDTexto(t2), "Texto clonado deve ter novo ID");
    ASSERT_FLOAT_EQUAL(110.0, getXTexto(t2), 0.001, "X clonado deve ser deslocado por dx");
    ASSERT_FLOAT_EQUAL(220.0, getYTexto(t2), 0.001, "Y clonado deve ser deslocado por dy");
    ASSERT_EQUAL('i', getAncoraTexto(t2), "Âncora clonada deve ser igual");
    ASSERT_STR_EQUAL("red", getCorBTexto(t2), "Cor da borda clonada deve ser igual");
    ASSERT_STR_EQUAL("blue", getCorPTexto(t2), "Cor de preenchimento clonada deve ser igual");
    ASSERT_STR_EQUAL("Original", getTxtTexto(t2), "Conteúdo do texto clonado deve ser igual");
    
    desalocaTexto(t1);
    desalocaTexto(t2);
}

/* Teste: Diferentes âncoras */
void teste_diferentes_ancoras() {
    TEXTO t1 = criaTexto(1, 100.0, 200.0, "red", "blue", 'i', "Esquerda");
    TEXTO t2 = criaTexto(2, 100.0, 200.0, "red", "blue", 'm', "Centro");
    TEXTO t3 = criaTexto(3, 100.0, 200.0, "red", "blue", 'f', "Direita");
    
    ASSERT_EQUAL('i', getAncoraTexto(t1), "Primeiro texto deve ter âncora 'i'");
    ASSERT_EQUAL('m', getAncoraTexto(t2), "Segundo texto deve ter âncora 'm'");
    ASSERT_EQUAL('f', getAncoraTexto(t3), "Terceiro texto deve ter âncora 'f'");
    
    desalocaTexto(t1);
    desalocaTexto(t2);
    desalocaTexto(t3);
}

/* Teste: Texto vazio */
void teste_texto_vazio() {
    TEXTO t = criaTexto(1, 100.0, 200.0, "red", "blue", 'i', "");
    
    ASSERT_STR_EQUAL("", getTxtTexto(t), "Texto vazio deve ter string vazia");
    
    desalocaTexto(t);
}

int main() {
    RESETAR_ESTATISTICAS();
    
    EXECUTAR_TESTE(teste_criar_destruir_texto);
    EXECUTAR_TESTE(teste_texto_getters);
    EXECUTAR_TESTE(teste_texto_setters);
    EXECUTAR_TESTE(teste_area_texto);
    EXECUTAR_TESTE(teste_clonar_texto);
    EXECUTAR_TESTE(teste_diferentes_ancoras);
    EXECUTAR_TESTE(teste_texto_vazio);
    
    IMPRIMIR_RESUMO_TESTES("Módulo Texto");
    
    return CODIGO_SAIDA_TESTE();
}
