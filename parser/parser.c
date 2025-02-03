#include <stdio.h>
#include <string.h>
#include "funcs.h"


// Função principal do parser
void parser(Buffer *buffer) {
    // começa pegando o primeiro token
    get_next_token(buffer);

    // começa com o programa
    programa(buffer);
}
