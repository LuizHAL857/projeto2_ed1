#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


static int testes_executados = 0;
static int testes_aprovados = 0;
static int testes_falhados = 0;

/* Códigos de cor para saída no terminal */
#define COR_VERMELHO  "\x1b[31m"
#define COR_VERDE     "\x1b[32m"
#define COR_AMARELO   "\x1b[33m"
#define COR_RESET     "\x1b[0m"

/* Macros de asserção */
#define ASSERT_TRUE(condicao, mensagem) \
    do { \
        testes_executados++; \
        if (condicao) { \
            testes_aprovados++; \
            printf(COR_VERDE "  ✓ " COR_RESET "%s\n", mensagem); \
        } else { \
            testes_falhados++; \
            printf(COR_VERMELHO "  ✗ " COR_RESET "%s\n", mensagem); \
            printf("    Falhou em %s:%d\n", __FILE__, __LINE__); \
        } \
    } while (0)

#define ASSERT_FALSE(condicao, mensagem) \
    ASSERT_TRUE(!(condicao), mensagem)

#define ASSERT_EQUAL(esperado, atual, mensagem) \
    ASSERT_TRUE((esperado) == (atual), mensagem)

#define ASSERT_NOT_EQUAL(esperado, atual, mensagem) \
    ASSERT_TRUE((esperado) != (atual), mensagem)

#define ASSERT_NULL(ponteiro, mensagem) \
    ASSERT_TRUE((ponteiro) == NULL, mensagem)

#define ASSERT_NOT_NULL(ponteiro, mensagem) \
    ASSERT_TRUE((ponteiro) != NULL, mensagem)

#define ASSERT_FLOAT_EQUAL(esperado, atual, epsilon, mensagem) \
    ASSERT_TRUE(fabs((esperado) - (atual)) < (epsilon), mensagem)

#define ASSERT_STR_EQUAL(esperado, atual, mensagem) \
    ASSERT_TRUE(strcmp((esperado), (atual)) == 0, mensagem)

/* Macros para executar testes */
#define EXECUTAR_TESTE(funcao_teste) \
    do { \
        printf(COR_AMARELO "\n▶ Executando: " COR_RESET "%s\n", #funcao_teste); \
        funcao_teste(); \
    } while (0)

/* Resumo da suíte de testes */
#define IMPRIMIR_RESUMO_TESTES(nome_suite) \
    do { \
        printf("\n" COR_AMARELO "═══════════════════════════════════════\n" COR_RESET); \
        printf(COR_AMARELO "Suíte de Testes: " COR_RESET "%s\n", nome_suite); \
        printf(COR_AMARELO "═══════════════════════════════════════\n" COR_RESET); \
        printf("Total de testes: %d\n", testes_executados); \
        printf(COR_VERDE "Aprovados: %d\n" COR_RESET, testes_aprovados); \
        if (testes_falhados > 0) { \
            printf(COR_VERMELHO "Falhados: %d\n" COR_RESET, testes_falhados); \
        } else { \
            printf("Falhados: %d\n", testes_falhados); \
        } \
        printf(COR_AMARELO "═══════════════════════════════════════\n" COR_RESET); \
        if (testes_falhados == 0) { \
            printf(COR_VERDE "✓ Todos os testes passaram!\n" COR_RESET); \
        } else { \
            printf(COR_VERMELHO "✗ Alguns testes falharam!\n" COR_RESET); \
        } \
        printf("\n"); \
    } while (0)

/* Resetar estatísticas dos testes */
#define RESETAR_ESTATISTICAS() \
    do { \
        testes_executados = 0; \
        testes_aprovados = 0; \
        testes_falhados = 0; \
    } while (0)

/* Retornar código de saída apropriado */
#define CODIGO_SAIDA_TESTE() (testes_falhados == 0 ? 0 : 1)

#endif 
