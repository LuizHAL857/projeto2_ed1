#include "test_framework.h"
#include "../src/lista.h"
#include <stdlib.h>

/* Função auxiliar: Criar ponteiro de inteiro */
int* criar_int(int valor) {
    int* p = malloc(sizeof(int));
    *p = valor;
    return p;
}

/* Teste: Criar e destruir lista vazia */
void teste_criar_destruir_lista() {
    Lista l = criaLista();
    ASSERT_NOT_NULL(l, "Lista deve ser criada");
    ASSERT_TRUE(listaVazia(l), "Nova lista deve estar vazia");
    ASSERT_EQUAL(0, getTamanhoLista(l), "Tamanho da lista vazia deve ser 0");
    liberaLista(l);
}

/* Teste: Inserir no início */
void teste_inserir_inicio() {
    Lista l = criaLista();
    int* val1 = criar_int(10);
    int* val2 = criar_int(20);
    
    insereLista(l, val1);
    ASSERT_FALSE(listaVazia(l), "Lista não deve estar vazia após inserção");
    ASSERT_EQUAL(1, getTamanhoLista(l), "Tamanho da lista deve ser 1");
    
    insereLista(l, val2);
    ASSERT_EQUAL(2, getTamanhoLista(l), "Tamanho da lista deve ser 2");
    
    Celula c = getInicioLista(l);
    ASSERT_NOT_NULL(c, "Primeira célula deve existir");
    ASSERT_EQUAL(20, *(int*)getConteudoCelula(c), "Primeiro elemento deve ser 20");
    
    liberaLista(l);
}

/* Teste: Inserir no final */
void teste_inserir_final() {
    Lista l = criaLista();
    int* val1 = criar_int(10);
    int* val2 = criar_int(20);
    int* val3 = criar_int(30);
    
    insereFinalLista(l, val1);
    insereFinalLista(l, val2);
    insereFinalLista(l, val3);
    
    ASSERT_EQUAL(3, getTamanhoLista(l), "Tamanho da lista deve ser 3");
    
    Celula c = getInicioLista(l);
    ASSERT_EQUAL(10, *(int*)getConteudoCelula(c), "Primeiro elemento deve ser 10");
    
    c = getFimLista(l);
    ASSERT_EQUAL(30, *(int*)getConteudoCelula(c), "Último elemento deve ser 30");
    
    liberaLista(l);
}

/* Teste: Remover do início */
void teste_remover_inicio() {
    Lista l = criaLista();
    int* val1 = criar_int(10);
    int* val2 = criar_int(20);
    
    insereFinalLista(l, val1);
    insereFinalLista(l, val2);
    
    int* removido = (int*)removeInicioLista(l);
    ASSERT_NOT_NULL(removido, "Elemento removido não deve ser NULL");
    ASSERT_EQUAL(10, *removido, "Elemento removido deve ser 10");
    ASSERT_EQUAL(1, getTamanhoLista(l), "Tamanho da lista deve ser 1 após remoção");
    free(removido);
    
    liberaLista(l);
}

/* Teste: Remover do final */
void teste_remover_final() {
    Lista l = criaLista();
    int* val1 = criar_int(10);
    int* val2 = criar_int(20);
    
    insereFinalLista(l, val1);
    insereFinalLista(l, val2);
    
    int* removido = (int*)removeFinalLista(l);
    ASSERT_NOT_NULL(removido, "Elemento removido não deve ser NULL");
    ASSERT_EQUAL(20, *removido, "Elemento removido deve ser 20");
    ASSERT_EQUAL(1, getTamanhoLista(l), "Tamanho da lista deve ser 1 após remoção");
    free(removido);
    
    liberaLista(l);
}

/* Teste: Navegação */
void teste_navegacao() {
    Lista l = criaLista();
    int* val1 = criar_int(10);
    int* val2 = criar_int(20);
    int* val3 = criar_int(30);
    
    insereFinalLista(l, val1);
    insereFinalLista(l, val2);
    insereFinalLista(l, val3);
    
    Celula c = getInicioLista(l);
    ASSERT_EQUAL(10, *(int*)getConteudoCelula(c), "Valor da primeira célula deve ser 10");
    
    c = getProxCelula(c);
    ASSERT_EQUAL(20, *(int*)getConteudoCelula(c), "Valor da segunda célula deve ser 20");
    
    c = getProxCelula(c);
    ASSERT_EQUAL(30, *(int*)getConteudoCelula(c), "Valor da terceira célula deve ser 30");
    
    Celula anterior = getAntCelula(c);
    ASSERT_EQUAL(20, *(int*)getConteudoCelula(anterior), "Valor da célula anterior deve ser 20");
    
    liberaLista(l);
}

/* Teste: Operações em lista vazia */
void teste_operacoes_lista_vazia() {
    Lista l = criaLista();
    
    ASSERT_NULL(removeInicioLista(l), "Remover de lista vazia deve retornar NULL");
    ASSERT_NULL(removeFinalLista(l), "Remover de lista vazia deve retornar NULL");
    ASSERT_NULL(getInicioLista(l), "Obter primeiro de lista vazia deve retornar NULL");
    ASSERT_NULL(getFimLista(l), "Obter último de lista vazia deve retornar NULL");
    
    liberaLista(l);
}

/* Teste: Copiar lista */
void teste_copiar_lista() {
    Lista l1 = criaLista();
    Lista l2 = criaLista();
    
    int* val1 = criar_int(10);
    int* val2 = criar_int(20);
    int* val3 = criar_int(30);
    
    insereFinalLista(l1, val1);
    insereFinalLista(l1, val2);
    insereFinalLista(l1, val3);
    
    copiaListas(l2, l1);
    
    ASSERT_EQUAL(getTamanhoLista(l1), getTamanhoLista(l2), "Lista copiada deve ter mesmo tamanho");
    
    Celula c1 = getInicioLista(l1);
    Celula c2 = getInicioLista(l2);
    
    while (c1 != NULL && c2 != NULL) {
        ASSERT_EQUAL(*(int*)getConteudoCelula(c1), *(int*)getConteudoCelula(c2), 
                     "Elementos copiados devem ter mesmos valores");
        c1 = getProxCelula(c1);
        c2 = getProxCelula(c2);
    }
    
    liberaLista(l1);
    liberaLista(l2);
}

int main() {
    RESETAR_ESTATISTICAS();
    
    EXECUTAR_TESTE(teste_criar_destruir_lista);
    EXECUTAR_TESTE(teste_inserir_inicio);
    EXECUTAR_TESTE(teste_inserir_final);
    EXECUTAR_TESTE(teste_remover_inicio);
    EXECUTAR_TESTE(teste_remover_final);
    EXECUTAR_TESTE(teste_navegacao);
    EXECUTAR_TESTE(teste_operacoes_lista_vazia);
    EXECUTAR_TESTE(teste_copiar_lista);
    
    IMPRIMIR_RESUMO_TESTES("Módulo Lista");
    
    return CODIGO_SAIDA_TESTE();
}
