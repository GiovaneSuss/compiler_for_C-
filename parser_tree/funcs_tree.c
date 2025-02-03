#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcs.h"

// declaração de variáveis globais
Token *current_token; //token atual
Token *lookahead_token; // token seguinte
int current_index = 0; // index do token atual
int lookahead_index = 1; // index do token seguinte

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


// Função para criar um nó na Árvore Sintática Abstrata (AST)
ASTNode *create_node(const char *type, const char *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode)); // Aloca memória para o nó
    strcpy(node->type, type);  // Define o tipo do nó
    
    if (value != NULL) {
        strcpy(node->value, value); // Atribui o valor se fornecido
    } else {
        node->value[0] = '\0'; // Se não houver valor, define como string vazia
    }
    
    node->child_count = 0; // Inicializa o número de filhos como zero
    return node;
}

// Função para adicionar um filho a um nó da AST
void add_child(ASTNode *parent, ASTNode *child) {
    if (parent->child_count < 16) {  // Verifica se ainda há espaço para adicionar filhos
        parent->children[parent->child_count++] = child;
    } else {
        printf("Erro: número máximo de filhos excedido\n");
        exit(1);
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

// Função para imprimir a árvore sintática abstrata (AST)
void print_ast(ASTNode *node, int depth) {
    if (node == NULL) return;

    for (int i = 0; i < depth; i++) {  // Imprime a indentação com base na profundidade
        printf("  ");
    }

    if (strlen(node->value) > 0) {
        printf("<%s: %s>\n", node->type, node->value);
    } else {
        printf("<%s>\n", node->type);
    }

    for (int i = 0; i < node->child_count; i++) {  // Imprime os filhos do nó
        print_ast(node->children[i], depth + 1);
    }
}

// Começo da lógica da árvore

// 1. programa → declaração-lista
ASTNode *programa(Buffer *buffer) {
    ASTNode *root = create_node("Programa", NULL);
    add_child(root, declaracao_lista(buffer));
    return root;
}

// 2. declaracao-lista -> declaração { declaração } (EBNF)
ASTNode *declaracao_lista(Buffer *buffer) {
    ASTNode *decl_list = create_node("Declaração_Lista", NULL);
    while (lookahead_token) {
        add_child(decl_list, declaracao(buffer));
    }
    return decl_list;
}

// 3. declaração -> tipo-especificador var-declaracao | tipo-especificador fun-declaracao
ASTNode *declaracao(Buffer *buffer) {
    ASTNode *decl = NULL; // declara o node como NULL
    // verifica se o proximo é ID
    if (lookahead_token && strcmp(lookahead_token->type, "IDENTIFIER") == 0) {
        // se for, pega o tipo especificador (INT, VOID)
        tipo_especificador(buffer);
        // verifica se o prox é ',' ou '[', significando q é variável
        if (strcmp(lookahead_token->type, "SYMBOL_SEMICOLON") == 0 || strcmp(lookahead_token->type, "SYMBOL_LBRACKET") == 0) {
            decl = var_declaracao(buffer);
        // se o prox for '(', é função
        } else if (strcmp(lookahead_token->type, "SYMBOL_LPAREN") == 0) {
            decl = fun_declaracao(buffer);
        }
    // se não for o prox, dá erro pois não pode ser declaração
    } else {
        printf("Erro de sintaxe: tipo de declaração inválido\n");
        exit(1);
    }
    return decl;
}

// 5. tipo-especificador -> int | void
void tipo_especificador(Buffer *buffer) {
    // se for 'INT' ou 'VOID', consome o terminal
    if (current_token && (strcmp(current_token->type, "INT") == 0 || strcmp(current_token->type, "VOID") == 0)) {
        match(buffer, current_token->type);
    // se não for, gera um erro sintático
    } else {
        printf("ERRO SINTÁTICO: \"%s\" INVÁLIDO [linha: %i], COLUNA %i.\n", current_token->value,current_token->line,current_token->column);
        exit(1);
    }
}

// 4. var-declaracao -> ID ; | ID [ NUM ] ;
ASTNode *var_declaracao(Buffer *buffer) {
    ASTNode *var_decl = create_node("Var_Declaração", NULL); // cria um node pra var_decl
    ASTNode *identifier = create_node("IDENTIFIER", current_token->value); // cria um node pra ID
    // começa com ID, ent precisa dar match, se n lança um erro
    match(buffer, "IDENTIFIER");
    add_child(var_decl, identifier);
    // se o lookahead for '[' é vetor
    if (lookahead_token && strcmp(current_token->type, "SYMBOL_LBRACKET") == 0) {
        match(buffer, "SYMBOL_LBRACKET");
        ASTNode *size = create_node("NUM", current_token->value);
        match(buffer, "NUM");
        match(buffer, "SYMBOL_RBRACKET");
        add_child(var_decl, size);
    }
    // se n for vetor joga direto o ';'
    match(buffer, "SYMBOL_SEMICOLON");
    return var_decl;
}


// 6. fun-declaracao -> ID ( params ) composto-decl
ASTNode *fun_declaracao(Buffer *buffer) {
    ASTNode *fun_decl = create_node("Fun_Declaração", NULL);
    ASTNode *identifier = create_node("IDENTIFIER", current_token->value);
    match(buffer, "IDENTIFIER");
    add_child(fun_decl, identifier);
    match(buffer, "SYMBOL_LPAREN");
    // 7. params -> param-lista | void
    ASTNode *param_list = NULL; // Evita criar Param_Lista desnecessária
    if (strcmp(current_token->type, "VOID") == 0) {
        param_list = create_node("Param_Lista", NULL); // Criamos a lista apenas aqui
        ASTNode *param_type = create_node("VOID", NULL);
        match(buffer, "VOID");
        add_child(param_list, param_type);
    } else {
        param_list = param_lista(buffer); // Se já existir, usamos o que já foi criado
    }

    add_child(fun_decl, param_list); // Adicionamos apenas uma vez

    match(buffer, "SYMBOL_RPAREN");

    ASTNode *body = composto_decl(buffer);
    add_child(fun_decl, body);

    return fun_decl;
}

// 8. param-lista -> param { , param } (EBNF)
ASTNode *param_lista(Buffer *buffer) {
    ASTNode *param_list = create_node("Param_Lista", NULL);

    ASTNode *first_param = param(buffer); // Primeiro parâmetro
    add_child(param_list, first_param);

    while (current_token && strcmp(current_token->type, "SYMBOL_COMMA") == 0) {
        match(buffer, "SYMBOL_COMMA");
        ASTNode *next_param = param(buffer);
        add_child(param_list, next_param);
    }

    return param_list;
}

// 9. param -> tipo-especificador ID | tipo-especificador ID []
ASTNode *param(Buffer *buffer) {
    ASTNode *param_node = create_node("Param", NULL);
    tipo_especificador(buffer);
    ASTNode *type = create_node(current_token->type, NULL);
    add_child(param_node, type);

    ASTNode *identifier = create_node("IDENTIFIER", current_token->value);
    match(buffer, "IDENTIFIER");
    add_child(param_node, identifier);

    if (lookahead_token && strcmp(lookahead_token->type, "SYMBOL_LBRACKET") == 0) {
        match(buffer, "SYMBOL_LBRACKET");
        ASTNode *array = create_node("Array", NULL);
        match(buffer, "SYMBOL_RBRACKET");
        add_child(param_node, array);
    }
    return param_node;
}

// 10. composto-decl -> { local-declaracoes statement-lista }
ASTNode *composto_decl(Buffer *buffer){
    ASTNode *compound = create_node("Composto_Decl", NULL);
    match(buffer, "SYMBOL_LBRACE");
    add_child(compound, local_declaracoes(buffer));
    add_child(compound, statement_lista(buffer));
    match(buffer, "SYMBOL_RBRACE");
    return compound;
}

// 11. local-declaracoes -> vazio { var-declaracao } (EBNF)
ASTNode *local_declaracoes(Buffer *buffer) {
    // Nada a fazer aqui para 'vazio', mas podemos verificar se há declarações
    ASTNode *local_decls = create_node("Local_Declarações", NULL);
    while (lookahead_token && (strcmp(current_token->type, "INT") == 0 || strcmp(current_token->type, "VOID") == 0)) {
        tipo_especificador(buffer);
        add_child(local_decls, var_declaracao(buffer));
    }
    return local_decls;
}

// 12. statement-lista -> vazio { statement } (EBNF)
ASTNode *statement_lista(Buffer *buffer) {
    // Nada a fazer aqui para 'vazio', mas podemos verificar se há statements
    ASTNode *stmt_list = create_node("Statement_Lista", NULL);
    while (lookahead_token && 
          (strcmp(current_token->type, "INT") == 0 || strcmp(current_token->type, "VOID") == 0 || 
           strcmp(current_token->type, "IDENTIFIER") == 0 || strcmp(current_token->type, "NUM") == 0 ||
           strcmp(current_token->type, "SYMBOL_LPAREN") == 0 || strcmp(current_token->type, "SYMBOL_LBRACE") == 0 || 
           strcmp(current_token->type, "IF") == 0 || strcmp(current_token->type, "WHILE") == 0 || 
           strcmp(current_token->type, "RETURN") == 0)) {
        add_child(stmt_list, statement(buffer));
    }
    return stmt_list;
}

// 13. statement -> expressao-decl | composto-decl | selecao-decl | iteracao-decl | retorno-decl
ASTNode *statement(Buffer *buffer) {
    if (lookahead_token) {
        // se o current_token for 'ID' o 'NUM' ou '(', vamos para expressao_decl
        if (strcmp(current_token->type, "IDENTIFIER") == 0 || strcmp(current_token->type, "NUM") == 0 ||
            strcmp(current_token->type, "SYMBOL_LPAREN") == 0) {
                return expressao_decl(buffer);
        // se for '{' é composto_decl
        } else if (strcmp(current_token->type, "SYMBOL_LBRACE") == 0) {
            return composto_decl(buffer);
        // se for 'IF' vai para selecao_decl
        } else if (strcmp(current_token->type, "IF") == 0) {
            return selecao_decl(buffer);
        // se for 'WHILE' vai para iteracao_decl
        } else if (strcmp(current_token->type, "WHILE") == 0) {
            return iteracao_decl(buffer);
        // se for 'RETURN' vai para retorno_decl
        } else if (strcmp(current_token->type, "RETURN") == 0) {
            return retorno_decl(buffer);
        // se for nenhum desses joga um erro sintático
        } else {
            printf("Erro de sintaxe: statement inválido\n");
            exit(1);
        }
    }
    return NULL;
}

// 14. expressao-decl -> expressão ; | ;
ASTNode *expressao_decl(Buffer *buffer) {
    // verifica se é ';'
    if (current_token && strcmp(current_token->type, "SYMBOL_SEMICOLON") == 0) {
        match(buffer, "SYMBOL_SEMICOLON");
        return create_node("Empty_Statement", NULL);
    // se n for ';' direto vai pra expressao
    } else {
        ASTNode *expr = expressao(buffer);
        match(buffer, "SYMBOL_SEMICOLON");
        return expr;
    }
}

// 18. expressão -> var = expressão | simples-expressao
ASTNode *expressao(Buffer *buffer) {
    // se for ID o current_token vai pra var e dps pra '=' e expressao
    if (current_token && strcmp(current_token->type, "IDENTIFIER") == 0) {
        ASTNode *var_node = var(buffer);
        if (current_token && strcmp(current_token->type, "SYMBOL_EQUALS") == 0) {
            ASTNode *assign_node = create_node("Atribuição", "=");
            match(buffer, "SYMBOL_EQUALS");
            add_child(assign_node, var_node);
            add_child(assign_node, expressao(buffer));
            return assign_node;
        } else {
            return simples_expressao(buffer);
        }
    // se n for ID vai pra simples-expressao
    } else {
        return simples_expressao(buffer);
    }
}

// 19. var -> ID | ID [ expressao ]
ASTNode *var(Buffer *buffer) {
    ASTNode *var_node = create_node("IDENTIFIER", current_token->value);
    match(buffer, "IDENTIFIER");
    // se o lookahead for '[' vai ser um vetor
    if (lookahead_token && strcmp(current_token->type, "SYMBOL_LBRACKET") == 0) {
        match(buffer, "SYMBOL_LBRACKET");
        ASTNode *index = expressao(buffer);
        match(buffer, "SYMBOL_RBRACKET");
        ASTNode *array_access = create_node("Array_Acesso", NULL);
        add_child(array_access, var_node);
        add_child(array_access, index);
        return array_access;
    }
    // se n for é um ID e retorna
    return var_node;
}

// 20. simples-expressao -> soma-expressao relacional soma-expressao | soma-expressao
ASTNode *simples_expressao(Buffer *buffer) {
    // começa com soma_expressao
    ASTNode *left = soma_expressao(buffer);
    // verifica se o prox é relacional, se n for retorna
    while (lookahead_token &&
          (strcmp(current_token->type, "LE_OP") == 0 || strcmp(current_token->type, "GE_OP") == 0 ||
           strcmp(current_token->type, "EQ_OP") == 0 || strcmp(current_token->type, "NE_OP") == 0 ||
           strcmp(current_token->type, "SYMBOL_LESS") == 0 || strcmp(current_token->type, "SYMBOL_GREATER") == 0)) {
        ASTNode *op = create_node(current_token->type, NULL);
        match(buffer, current_token->type);
        ASTNode *right = soma_expressao(buffer);
        add_child(op, left);
        add_child(op, right);
        left = op;
    }
    return left;
}

// 22. soma-expressao -> termo { soma termo } (EBNF)
ASTNode *soma_expressao(Buffer *buffer) {
    ASTNode *left = termo(buffer);
    while (lookahead_token &&
           (strcmp(current_token->type, "SYMBOL_PLUS") == 0 ||
            strcmp(current_token->type, "SYMBOL_MINUS") == 0)) {
        ASTNode *op = create_node(current_token->type, NULL); // 23. soma → + | -
        match(buffer, current_token->type);
        ASTNode *right = termo(buffer);
        add_child(op, left);
        add_child(op, right);
        left = op;
    }
    return left;
}

// 24. termo -> fator { mult fator } (EBNF)
ASTNode *termo(Buffer *buffer) {
    ASTNode *left = fator(buffer);
    while (lookahead_token &&
           (strcmp(current_token->type, "SYMBOL_STAR") == 0 ||
            strcmp(current_token->type, "SYMBOL_SLASH") == 0)) {
        ASTNode *op = create_node(current_token->type, NULL); // 25. mult → * | /
        match(buffer, current_token->type);
        ASTNode *right = fator(buffer);
        add_child(op, left);
        add_child(op, right);
        left = op;
    }
    return left;
}

// 26. fator -> ( expressão ( , expressão ) * ) | ID | ID ativação | NUM
ASTNode *fator(Buffer *buffer) {
    if (current_token) {
        // se for '(' é ( expressão ( , expressão )* )
        if (strcmp(current_token->type, "SYMBOL_LPAREN") == 0) {
            match(buffer, "SYMBOL_LPAREN");
            ASTNode *expr = expressao(buffer);
            ASTNode *paren_expr = create_node("Parentesis", NULL);
            add_child(paren_expr, expr);
            while (lookahead_token && strcmp(current_token->type, "SYMBOL_COMMA") == 0) {
                match(buffer,"SYMBOL_COMMA");
                add_child(paren_expr, expressao(buffer));
            }
            match(buffer, "SYMBOL_RPAREN");
            return paren_expr;
        // se for ID é ID ou ativação
        } else if (strcmp(current_token->type, "IDENTIFIER") == 0) {
            ASTNode *id_node = create_node("IDENTIFIER", current_token->value);
            match(buffer, "IDENTIFIER");
            if (current_token && strcmp(current_token->type, "SYMBOL_LPAREN") == 0) {
                return ativacao(buffer);
            }
            return id_node;
        // se for NUM é NUM
        } else if (strcmp(current_token->type, "NUM") == 0) {
            ASTNode *num_node = create_node("NUM", current_token->value);
            match(buffer, "NUM");
            return num_node;
        }
    // se n for nenhum desses é um erro
    } else {
        printf("Erro de sintaxe: esperado um fator, encontrado EOF\n");
        exit(1);
    }
    return NULL; // Para evitar warnings de retorno
}

// 27. ativação → ( args )
ASTNode *ativacao(Buffer *buffer) {
    ASTNode *call = create_node("Função_Chamada", current_token->value);
    match(buffer, "SYMBOL_LPAREN");
    add_child(call, args(buffer));
    match(buffer, "SYMBOL_RPAREN");
    return call;
}

// 28. args → arg-lista | vazio
ASTNode *args(Buffer *buffer) {
    if (lookahead_token && (strcmp(current_token->type, "IDENTIFIER") == 0 || strcmp(current_token->type, "NUM") == 0)) {
        return arg_lista(buffer);
    } 
    return create_node("Vazio", NULL); 
    // Nada a fazer para 'vazio'
}

// 29. arg-lista -> expressão { , expressão } (EBNF)
ASTNode *arg_lista(Buffer *buffer) {
    ASTNode *arg_list = create_node("Arg_Lista", NULL);
    add_child(arg_list, expressao(buffer));
    while (lookahead_token && strcmp(lookahead_token->type, "SYMBOL_COMMA") == 0) {
        match(buffer, "SYMBOL_COMMA");
        add_child(arg_list, expressao(buffer));
    }
    return arg_list;
}

// 15. selecao-decl -> if ( expressão ) statement | if ( expressão ) statement else statement
ASTNode *selecao_decl(Buffer *buffer) {
    ASTNode *if_stmt = create_node("Se_Declaração", NULL);
    match(buffer, "IF");
    match(buffer, "SYMBOL_LPAREN");
    ASTNode *condition = expressao(buffer);
    add_child(if_stmt, condition);
    match(buffer, "SYMBOL_RPAREN");
    ASTNode *if_body = statement(buffer);
    add_child(if_stmt, if_body);
    if (lookahead_token && strcmp(current_token->type, "ELSE") == 0) {
        match(buffer, "ELSE");
        ASTNode *else_body = statement(buffer);
        add_child(if_stmt, else_body);
    }
    return if_stmt;
}

// 16. iteracao-decl -> while ( expressão ) statement
ASTNode *iteracao_decl(Buffer *buffer) {
    ASTNode *while_stmt = create_node("Enquanto_Declaração", NULL);
    match(buffer, "WHILE");
    match(buffer, "SYMBOL_LPAREN");
    ASTNode *condition = expressao(buffer);
    add_child(while_stmt, condition);
    match(buffer, "SYMBOL_RPAREN");
    ASTNode *body = statement(buffer);
    add_child(while_stmt, body);
    return while_stmt;
}

// 17. retorno-decl -> return ; | return  expressão ;
ASTNode *retorno_decl(Buffer *buffer) {
    ASTNode *return_stmt = create_node("Retorno_Declaração", NULL);
    match(buffer, "RETURN");
    if (lookahead_token && strcmp(current_token->type, "SYMBOL_SEMICOLON") != 0) {
        add_child(return_stmt, expressao(buffer));
    }
    match(buffer, "SYMBOL_SEMICOLON");
    return return_stmt;
}