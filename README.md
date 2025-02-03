##**INSTRUÇÕES DE COMO RODAR O COMPILADOR**

## Compilação do programa principal (compiler)

1. crie o executável com árvore sintática
```bash
gcc -Wall -o compiler compiler.c lexer/lexer.c lexer/funcs.c parser_tree/parser_tree.c parser_tree/funcs_tree.c semantico/semantico.c semantico/funcs.c
```

**OU**

1. crie o executável sem árvore sintática
```bash
gcc -Wall -o compiler compiler.c lexer/lexer.c lexer/funcs.c parser/parser.c parser/funcs.c semantico/semantico.c semantico/funcs.c
```

## Execução do programa principal (compiler)

1. crie um código de teste para testar o compiler e bote ele na root do repositório

2. Rode o compiler para o seu arquivo de teste
```bash
./compiler seu_arquivo_de_teste.c
```