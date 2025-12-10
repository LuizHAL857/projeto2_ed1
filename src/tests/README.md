# Testes Unitários - Projeto ED1

Este diretório contém os testes unitários para todos os módulos do projeto.

## Estrutura

- `test_framework.h` - Framework de testes com macros de asserção
- `test_lista.c` - Testes para o módulo de lista
- `test_arvore_binaria.c` - Testes para o módulo de árvore binária
- `test_circulo.c` - Testes para o módulo de círculo
- `test_retangulo.c` - Testes para o módulo de retângulo
- `test_linha.c` - Testes para o módulo de linha
- `test_texto.c` - Testes para o módulo de texto
- `test_anteparo.c` - Testes para o módulo de anteparo
- `test_sort.c` - Testes para os algoritmos de ordenação
- `Makefile` - Sistema de compilação dos testes

## Como Compilar

Para compilar todos os testes:

```bash
make all
```

Para compilar um teste específico:

```bash
make test_lista
make test_arvore
make test_circulo
# etc...
```

## Como Executar

Para executar todos os testes:

```bash
make test
```

Para executar um teste específico:

```bash
./test_lista
./test_arvore_binaria
./test_circulo
# etc...
```

Ou usando os alvos do Makefile:

```bash
make test_lista
make test_arvore
make test_circulo
# etc...
```

## Limpar Arquivos Compilados

```bash
make clean
```

## Recompilar Tudo

```bash
make rebuild
```

## Saída dos Testes

Os testes usam cores para facilitar a visualização:
- ✓ Verde: Teste passou
- ✗ Vermelho: Teste falhou
- Amarelo: Cabeçalhos e informações

Cada suíte de testes exibe:
- Nome do teste sendo executado
- Resultado de cada asserção
- Resumo final com total de testes, aprovados e falhados

## Verificação de Memória

Para verificar vazamentos de memória com valgrind:

```bash
valgrind --leak-check=full ./test_lista
valgrind --leak-check=full ./test_arvore_binaria
# etc...
```
