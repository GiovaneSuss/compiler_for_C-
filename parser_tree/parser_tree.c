#include <stdio.h>
#include <string.h>
#include "funcs.h"


// Função principal do parser
void parser(Buffer *buffer) {
    printf("\nAnálise sintática em andamento...\n");
    // int i = 0;
    // while (i < buffer->token_count){
    //     get_next_token(buffer);
    //     i++;
    // }
    get_next_token(buffer);

    ASTNode *ast_root = programa(buffer);

    print_ast(ast_root,0);

    printf("\nAnálise sintática foi finalizada com sucesso.\n");
    
}
