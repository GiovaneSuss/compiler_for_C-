#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcs.h"

Token *current_token;
Token *lookahead_token;
int current_index = 0;
int lookahead_index = 1;

// função para pegar o prox token
void get_next_token(Buffer *buffer) {
    // verifica se o index é menor que o tamanho do vetor de tokens
    if (current_index < buffer->token_count) {
        // se for, pega o prox token
        current_token = &buffer->tokens[current_index++];

    // verifica se o index para o lookahead é menor que o token_count - 1
    if (lookahead_index < buffer->token_count - 1) {
        // se for, pega o prox lookahead token
        lookahead_token = &buffer->tokens[lookahead_index++];
        //printf("Token atual: %s, Próximo: %s\n", current_token->type, lookahead_token->type);
    } else { // se não for, bota como NULL para não dar erro de memória
        lookahead_token = NULL;
        //printf("Token atual: %s, Próximo: EOF\n", current_token->type);
    }
    // se der isso para o current token, os dois são NULL
    } else {
        current_token = NULL;
        lookahead_token = NULL;
    }
}
// função para consumir os símbolos terminais
void match(Buffer *buffer, const char *expected_type) {
    // verifica se o token é NULL
    if (current_token == NULL) {
        printf("ERRO SINTÁTICO: \"%s\" INVÁLIDO [linha: %i], COLUNA %i.\n", current_token->value,current_token->line,current_token->column);
        exit(1);
    }
    // verifica se o token atual corresponde com o tipo esperado
    if (strcmp(current_token->type, expected_type) != 0) {
        printf("ERRO SINTÁTICO: \"%s\" INVÁLIDO [linha: %i], COLUNA %i.\n", current_token->value,current_token->line,current_token->column);
        exit(1);
    }
    // se for o esperado, pega o prox token para o current e lookahead
    get_next_token(buffer);
}

// 1. programa → declaração-lista
void programa(Buffer *buffer) {
    declaracao_lista(buffer);
}

// 2. declaracao-lista -> declaração { declaração } (EBNF)
void declaracao_lista(Buffer *buffer) {
    declaracao(buffer);
    while (lookahead_token) {
        declaracao(buffer);
    }
}

// 3. declaração -> tipo-especificador var-declaracao | tipo-especificador fun-declaracao
void declaracao(Buffer *buffer) {
    // verifica se o proximo é ID
    if (lookahead_token && strcmp(lookahead_token->type, "IDENTIFIER") == 0) {
        // se for, pega o tipo especificador (INT, VOID)
        tipo_especificador(buffer);
        // verifica se o prox é ',' ou '[', significando q é variável
        if (strcmp(lookahead_token->type, "SYMBOL_SEMICOLON") == 0 || strcmp(lookahead_token->type, "SYMBOL_LBRACKET") == 0) {
            var_declaracao(buffer);
        // se o prox for '(', é função
        } else if (strcmp(lookahead_token->type, "SYMBOL_LPAREN") == 0) {
            fun_declaracao(buffer);
        }
    // se não for o prox, dá erro pois não pode ser declaração
    } else {
        printf("ERRO SINTÁTICO: \"%s\" INVÁLIDO [linha: %i], COLUNA %i.\n", current_token->value,current_token->line,current_token->column);
        exit(1);
    }
}

// 5. tipo-especificador -> int | void
void tipo_especificador(Buffer *buffer) {
    // se for 'INT' ou 'VOID', consome o terminal
    if (current_token && (strcmp(current_token->type, "INT") == 0 || strcmp(current_token->type, "VOID") == 0)) {
        match(buffer, current_token->type);
    // se não for, gera um erro sintático
    } else {
        printf("ERRO SINTÁTICO: \"%s\" INVÁLIDO [linha: %i], COLUNA %i.\n", current_token->value,current_token->line,current_token->column);
        //printf("Erro de sintaxe: tipo especificador inválido\n");
        exit(1);
    }
}

// 4. var-declaracao -> ID ; | ID [ NUM ] ;
void var_declaracao(Buffer *buffer) {
    // começa com ID, ent precisa dar match, se n lança um erro
    match(buffer, "IDENTIFIER");
    // se o lookahead for '[' é vetor
    if (lookahead_token && strcmp(current_token->type, "SYMBOL_LBRACKET") == 0) {
        match(buffer, "SYMBOL_LBRACKET");
        match(buffer, "NUM");
        match(buffer, "SYMBOL_RBRACKET");
    }
    // se n for vetor joga direto o ';'
    match(buffer, "SYMBOL_SEMICOLON");
}

// 6. fun-declaracao -> ID ( params ) composto-decl
void fun_declaracao(Buffer *buffer) {
    match(buffer, "IDENTIFIER");
    match(buffer, "SYMBOL_LPAREN");
    // 7. params -> param-lista | void
    if (strcmp(current_token->type,"VOID") == 0) {
        match(buffer,"VOID");
    } else {
        param_lista(buffer);
    }
    match(buffer, "SYMBOL_RPAREN");
    composto_decl(buffer);
}

// 8. param-lista -> param { , param } (EBNF)
void param_lista(Buffer *buffer) {
    param(buffer);
    while (current_token && strcmp(current_token->type,"SYMBOL_COMMA") == 0) {
        match(buffer, "SYMBOL_COMMA");
        param(buffer);
    }
}

// 9. param -> tipo-especificador ID | tipo-especificador ID []
void param(Buffer *buffer) {
    tipo_especificador(buffer);
    match(buffer, "IDENTIFIER");
    if (lookahead_token && strcmp(lookahead_token->type, "SYMBOL_LBRACKET") == 0) {
        match(buffer, "SYMBOL_LBRACKET");
        match(buffer, "SYMBOL_RBRACKET");
    }
}

// 10. composto-decl -> { local-declaracoes statement-lista }
void composto_decl(Buffer *buffer){
    match(buffer, "SYMBOL_LBRACE");
    local_declaracoes(buffer);
    statement_lista(buffer);
    match(buffer, "SYMBOL_RBRACE");
}

// 11. local-declaracoes -> vazio { var-declaracao } (EBNF)
void local_declaracoes(Buffer *buffer) {
    // Nada a fazer aqui para 'vazio', mas podemos verificar se há declarações
    while (lookahead_token && (strcmp(current_token->type, "INT") == 0 || strcmp(current_token->type, "VOID") == 0)) {
        tipo_especificador(buffer);
        var_declaracao(buffer);
    }
}

// 12. statement-lista -> vazio { statement } (EBNF)
void statement_lista(Buffer *buffer) {
    // Nada a fazer aqui para 'vazio', mas podemos verificar se há statements
    while (lookahead_token && 
          (strcmp(current_token->type, "INT") == 0 || strcmp(current_token->type, "VOID") == 0 || 
           strcmp(current_token->type, "IDENTIFIER") == 0 || strcmp(current_token->type, "NUM") == 0 ||
           strcmp(current_token->type, "SYMBOL_LPAREN") == 0 || strcmp(current_token->type, "SYMBOL_LBRACE") == 0 || 
           strcmp(current_token->type, "IF") == 0 || strcmp(current_token->type, "WHILE") == 0 || 
           strcmp(current_token->type, "RETURN") == 0)) {
        statement(buffer);
    }
}

// 13. statement -> expressao-decl | composto-decl | selecao-decl | iteracao-decl | retorno-decl
void statement(Buffer *buffer) {
    if (lookahead_token) {
        // se o current_token for 'ID' o 'NUM' ou '(', vamos para expressao_decl
        if (strcmp(current_token->type, "IDENTIFIER") == 0 || strcmp(current_token->type, "NUM") == 0 ||
            strcmp(current_token->type, "SYMBOL_LPAREN") == 0) {
                expressao_decl(buffer);
        // se for '{' é composto_decl
        } else if (strcmp(current_token->type, "SYMBOL_LBRACE") == 0) {
            composto_decl(buffer);
        // se for 'IF' vai para selecao_decl
        } else if (strcmp(current_token->type, "IF") == 0) {
            selecao_decl(buffer);
        // se for 'WHILE' vai para iteracao_decl
        } else if (strcmp(current_token->type, "WHILE") == 0) {
            iteracao_decl(buffer);
        // se for 'RETURN' vai para retorno_decl
        } else if (strcmp(current_token->type, "RETURN") == 0) {
            retorno_decl(buffer);
        // se for nenhum desses joga um erro sintático
        } else {
            printf("ERRO SINTÁTICO: \"%s\" INVÁLIDO [linha: %i], COLUNA %i.\n", current_token->value,current_token->line,current_token->column);
            exit(1);
        }
    }
}

// 14. expressao-decl -> expressão ; | ;
void expressao_decl(Buffer *buffer) {
    // verifica se é ';'
    if (current_token && strcmp(current_token->type, "SYMBOL_SEMICOLON") == 0) {
        match(buffer, "SYMBOL_SEMICOLON");
    // se n for ';' direto vai pra expressao
    } else {
        expressao(buffer);
        match(buffer, "SYMBOL_SEMICOLON");
    }
}

// 18. expressão -> var = expressão | simples-expressao
void expressao(Buffer *buffer) {
    // se for ID o current_token vai pra var e dps pra '=' e expressao
    if (current_token && strcmp(current_token->type, "IDENTIFIER") == 0) {
        var(buffer);
        if (current_token && strcmp(current_token->type, "SYMBOL_EQUALS") == 0) {
            match(buffer, "SYMBOL_EQUALS");
            expressao(buffer);
        } else {
            simples_expressao(buffer);
        }
    // se n for ID vai pra simples-expressao
    } else {
        simples_expressao(buffer);
    }
}

// 19. var -> ID | ID [ expressao ]
void var(Buffer *buffer) {
    match(buffer, "IDENTIFIER");
    // se o lookahead for '[' vai ser um vetor
    if (lookahead_token && strcmp(current_token->type, "SYMBOL_LBRACKET") == 0) {
        match(buffer, "SYMBOL_LBRACKET");
        expressao(buffer);
        match(buffer, "SYMBOL_RBRACKET");
    }
    // se n for é um ID e retorna
}

// 20. simples-expressao -> soma-expressao relacional soma-expressao | soma-expressao
void simples_expressao(Buffer *buffer) {
    // começa com soma_expressao
    soma_expressao(buffer);
    // verifica se o prox é relacional, se n for retorna
    while (lookahead_token &&
          (strcmp(current_token->type, "LE_OP") == 0 || strcmp(current_token->type, "GE_OP") == 0 ||
           strcmp(current_token->type, "EQ_OP") == 0 || strcmp(current_token->type, "NE_OP") == 0 ||
           strcmp(current_token->type, "SYMBOL_LESS") == 0 || strcmp(current_token->type, "SYMBOL_GREATER") == 0)) {
        match(buffer, current_token->type);
        soma_expressao(buffer);
    }
}

// 22. soma-expressao -> termo { soma termo } (EBNF)
void soma_expressao(Buffer *buffer) {
    termo(buffer);
    while (lookahead_token &&
           (strcmp(current_token->type, "SYMBOL_PLUS") == 0 ||
            strcmp(current_token->type, "SYMBOL_MINUS") == 0)) {  
        match(buffer, current_token->type); // 23. soma → + | -
        termo(buffer);
    }
}

// 24. termo -> fator { mult fator } (EBNF)
void termo(Buffer *buffer) {
    fator(buffer);
    while (lookahead_token &&
           (strcmp(current_token->type, "SYMBOL_STAR") == 0 ||
            strcmp(current_token->type, "SYMBOL_SLASH") == 0)) {
        match(buffer, current_token->type); // 25. mult → * | /
        fator(buffer);
    }
}

// 26. fator -> ( expressão ( , expressão ) * ) | ID | ID ativação | NUM
void fator(Buffer *buffer) {
    if (current_token) {
        // se for '(' é ( expressão ( , expressão )* )
        if (strcmp(current_token->type, "SYMBOL_LPAREN") == 0) {
            match(buffer, "SYMBOL_LPAREN");
            expressao(buffer);
            while (lookahead_token && strcmp(current_token->type, "SYMBOL_COMMA") == 0) {
                match(buffer,"SYMBOL_COMMA");
                expressao(buffer);
            }
            match(buffer, "SYMBOL_RPAREN");
        // se for ID é ID ou ativação
        } else if (strcmp(current_token->type, "IDENTIFIER") == 0) {
            match(buffer, "IDENTIFIER");
            if (current_token && strcmp(current_token->type, "SYMBOL_LPAREN") == 0) {
                ativacao(buffer);
            }
        // se for NUM é NUM
        } else if (strcmp(current_token->type, "NUM") == 0) {
            match(buffer, "NUM");
        }
    // se n for nenhum desses é um erro
    } else {
        //printf("Erro de sintaxe: esperado um fator, encontrado EOF\n");
        printf("ERRO SINTÁTICO: \"%s\" INVÁLIDO [linha: %i], COLUNA %i.\n", current_token->value,current_token->line,current_token->column);
        exit(1);
    }
}

// 27. ativação → ( args )
void ativacao(Buffer *buffer) {
    match(buffer, "SYMBOL_LPAREN");
    args(buffer);
    match(buffer, "SYMBOL_RPAREN");
}

// 28. args → arg-lista | vazio
void args(Buffer *buffer) {
    if (lookahead_token && (strcmp(current_token->type, "IDENTIFIER") == 0 || strcmp(current_token->type, "NUM") == 0)) {
        arg_lista(buffer);
    } 
    // Nada a fazer para 'vazio'
}

// 29. arg-lista -> expressão { , expressão } (EBNF)
void arg_lista(Buffer *buffer) {
    expressao(buffer);
    while (lookahead_token && strcmp(lookahead_token->type, "SYMBOL_COMMA") == 0) {
        match(buffer, "SYMBOL_COMMA");
        expressao(buffer);
    }
}

// 15. selecao-decl -> if ( expressão ) statement | if ( expressão ) statement else statement
void selecao_decl(Buffer *buffer) {
    match(buffer, "IF");
    match(buffer, "SYMBOL_LPAREN");
    expressao(buffer);
    match(buffer, "SYMBOL_RPAREN");
    statement(buffer);
    if (lookahead_token && strcmp(current_token->type, "ELSE") == 0) {
        match(buffer, "ELSE");
        statement(buffer);
    }
}

// 16. iteracao-decl -> while ( expressão ) statement
void iteracao_decl(Buffer *buffer) {
    match(buffer, "WHILE");
    match(buffer, "SYMBOL_LPAREN");
    expressao(buffer);
    match(buffer, "SYMBOL_RPAREN"); // Adicionado
    statement(buffer);
}

// 17. retorno-decl -> return ; | return  expressão ;
void retorno_decl(Buffer *buffer) {
    match(buffer, "RETURN");
    if (lookahead_token && strcmp(current_token->type, "SYMBOL_SEMICOLON") != 0) {
        expressao(buffer);
    }
    match(buffer,"SYMBOL_SEMICOLON");
}
