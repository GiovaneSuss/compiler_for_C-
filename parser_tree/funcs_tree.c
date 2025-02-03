#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcs.h"

Token *current_token;
Token *lookahead_token;
int current_index = 0;
int lookahead_index = 1;

void get_next_token(Buffer *buffer) {
    if (current_index < buffer->token_count) {
        current_token = &buffer->tokens[current_index++];

    if (lookahead_index < buffer->token_count - 1) {
        lookahead_token = &buffer->tokens[lookahead_index++];
        //printf("Token atual: %s, Próximo: %s\n", current_token->type, lookahead_token->type);
    } else {
        lookahead_token = NULL;
        //printf("Token atual: %s, Próximo: EOF\n", current_token->type);
    }
    } else {
        current_token = NULL;
        lookahead_token = NULL;
    }
}

ASTNode *create_node(const char *type, const char *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    strcpy(node->type, type);
    if (value != NULL) {
        strcpy(node->value, value);
    } else {
        node->value[0] = '\0';
    }
    node->child_count = 0;
    return node;
}

void add_child(ASTNode *parent, ASTNode *child) {
    if (parent->child_count < 16) {
        parent->children[parent->child_count++] = child;
    } else {
        printf("Erro: número máximo de filhos excedido\n");
        exit(1);
    }
}

void match(Buffer *buffer, const char *expected_type) {
    if (current_token == NULL) {
        printf("Erro de sintaxe: esperado '%s', mas não há mais tokens\n", expected_type);
        exit(1);
    }
    if (strcmp(current_token->type, expected_type) != 0) {
        printf("Erro de sintaxe na linha %d, coluna %d: esperado '%s', encontrado '%s'\n",
               current_token->line, current_token->column, expected_type, current_token->type);
        exit(1);
    }
    get_next_token(buffer);
}

void print_ast(ASTNode *node, int depth) {
    if (node == NULL) return;

    // Imprime a indentação baseada na profundidade
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    // Imprime o nó atual
    if (strlen(node->value) > 0) {
        printf("<%s: %s>\n", node->type, node->value);
    } else {
        printf("<%s>\n", node->type);
    }

    // Imprime os filhos do nó
    for (int i = 0; i < node->child_count; i++) {
        print_ast(node->children[i], depth + 1);
    }
}

// Começo da lógica da árvore

ASTNode *programa(Buffer *buffer) {
    ASTNode *root = create_node("Programa", NULL);
    add_child(root, declaracao_lista(buffer));
    return root;
}


ASTNode *declaracao_lista(Buffer *buffer) {
    ASTNode *decl_list = create_node("Declaração_Lista", NULL);
    while (lookahead_token) {
        add_child(decl_list, declaracao(buffer));
    }
    return decl_list;
}


ASTNode *declaracao(Buffer *buffer) {
    ASTNode *decl = NULL;
    if (lookahead_token && strcmp(lookahead_token->type, "IDENTIFIER") == 0) {
        tipo_especificador(buffer);
        if (strcmp(lookahead_token->type, "SYMBOL_SEMICOLON") == 0 || strcmp(lookahead_token->type, "SYMBOL_LBRACKET") == 0) {
            decl = var_declaracao(buffer);
        } else if (strcmp(lookahead_token->type, "SYMBOL_LPAREN") == 0) {
            decl = fun_declaracao(buffer);
        }
    } else {
        printf("Erro de sintaxe: tipo de declaração inválido\n");
        exit(1);
    }
    return decl;
}

void tipo_especificador(Buffer *buffer) {
    if (current_token && (strcmp(current_token->type, "INT") == 0 || strcmp(current_token->type, "VOID") == 0)) {
        match(buffer, current_token->type);
    } else {
        printf("Erro de sintaxe: tipo especificador inválido\n");
        exit(1);
    }
}

ASTNode *var_declaracao(Buffer *buffer) {
    ASTNode *var_decl = create_node("Var_Declaração", NULL);
    ASTNode *identifier = create_node("IDENTIFIER", current_token->value);
    match(buffer, "IDENTIFIER");
    add_child(var_decl, identifier);
    if (lookahead_token && strcmp(current_token->type, "SYMBOL_LBRACKET") == 0) {
        match(buffer, "SYMBOL_LBRACKET");
        ASTNode *size = create_node("NUM", current_token->value);
        match(buffer, "NUM");
        match(buffer, "SYMBOL_RBRACKET");
        add_child(var_decl, size);
    }
    match(buffer, "SYMBOL_SEMICOLON");
    return var_decl;
}



ASTNode *fun_declaracao(Buffer *buffer) {
    ASTNode *fun_decl = create_node("Fun_Declaração", NULL);
    ASTNode *identifier = create_node("IDENTIFIER", current_token->value);
    match(buffer, "IDENTIFIER");
    add_child(fun_decl, identifier);
    match(buffer, "SYMBOL_LPAREN");

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

ASTNode *composto_decl(Buffer *buffer){
    ASTNode *compound = create_node("Composto_Decl", NULL);
    match(buffer, "SYMBOL_LBRACE");
    add_child(compound, local_declaracoes(buffer));
    add_child(compound, statement_lista(buffer));
    match(buffer, "SYMBOL_RBRACE");
    return compound;
}

ASTNode *local_declaracoes(Buffer *buffer) {
    // Nada a fazer aqui para 'vazio', mas podemos verificar se há declarações
    ASTNode *local_decls = create_node("Local_Declarações", NULL);
    while (lookahead_token && (strcmp(current_token->type, "INT") == 0 || strcmp(current_token->type, "VOID") == 0)) {
        tipo_especificador(buffer);
        add_child(local_decls, var_declaracao(buffer));
    }
    return local_decls;
}

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

ASTNode *statement(Buffer *buffer) {
    if (lookahead_token) {
        if (strcmp(current_token->type, "IDENTIFIER") == 0 || strcmp(current_token->type, "NUM") == 0 ||
            strcmp(current_token->type, "SYMBOL_LPAREN") == 0) {
                return expressao_decl(buffer);
        } else if (strcmp(current_token->type, "SYMBOL_LBRACE") == 0) {
            return composto_decl(buffer);
        } else if (strcmp(current_token->type, "IF") == 0) {
            return selecao_decl(buffer);
        } else if (strcmp(current_token->type, "WHILE") == 0) {
            return iteracao_decl(buffer);
        } else if (strcmp(current_token->type, "RETURN") == 0) {
            return retorno_decl(buffer);
        } else {
            printf("Erro de sintaxe: statement inválido\n");
            exit(1);
        }
    }
    return NULL;
}


ASTNode *expressao_decl(Buffer *buffer) {
    if (current_token && strcmp(current_token->type, "SYMBOL_SEMICOLON") == 0) {
        match(buffer, "SYMBOL_SEMICOLON");
        return create_node("Empty_Statement", NULL);
    } else {
        ASTNode *expr = expressao(buffer);
        match(buffer, "SYMBOL_SEMICOLON");
        return expr;
    }
}

ASTNode *expressao(Buffer *buffer) {
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
    } else {
        return simples_expressao(buffer);
    }
}

ASTNode *var(Buffer *buffer) {
    ASTNode *var_node = create_node("IDENTIFIER", current_token->value);
    match(buffer, "IDENTIFIER");
    if (lookahead_token && strcmp(current_token->type, "SYMBOL_LBRACKET") == 0) {
        match(buffer, "SYMBOL_LBRACKET");
        ASTNode *index = expressao(buffer);
        match(buffer, "SYMBOL_RBRACKET");
        ASTNode *array_access = create_node("Array_Acesso", NULL);
        add_child(array_access, var_node);
        add_child(array_access, index);
        return array_access;
    }
    return var_node;
}



ASTNode *simples_expressao(Buffer *buffer) {
    ASTNode *left = soma_expressao(buffer);
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

ASTNode *soma_expressao(Buffer *buffer) {
    ASTNode *left = termo(buffer);
    while (lookahead_token &&
           (strcmp(current_token->type, "SYMBOL_PLUS") == 0 ||
            strcmp(current_token->type, "SYMBOL_MINUS") == 0)) {
        ASTNode *op = create_node(current_token->type, NULL);
        match(buffer, current_token->type);
        ASTNode *right = termo(buffer);
        add_child(op, left);
        add_child(op, right);
        left = op;
    }
    return left;
}

ASTNode *termo(Buffer *buffer) {
    ASTNode *left = fator(buffer);
    while (lookahead_token &&
           (strcmp(current_token->type, "SYMBOL_STAR") == 0 ||
            strcmp(current_token->type, "SYMBOL_SLASH") == 0)) {
        ASTNode *op = create_node(current_token->type, NULL);
        match(buffer, current_token->type);
        ASTNode *right = fator(buffer);
        add_child(op, left);
        add_child(op, right);
        left = op;
    }
    return left;
}


ASTNode *fator(Buffer *buffer) {
    if (current_token) {
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
        } else if (strcmp(current_token->type, "IDENTIFIER") == 0) {
            ASTNode *id_node = create_node("IDENTIFIER", current_token->value);
            match(buffer, "IDENTIFIER");
            if (current_token && strcmp(current_token->type, "SYMBOL_LPAREN") == 0) {
                return ativacao(buffer);
            }
            return id_node;
        } else if (strcmp(current_token->type, "NUM") == 0) {
            ASTNode *num_node = create_node("NUM", current_token->value);
            match(buffer, "NUM");
            return num_node;
        }
    } else {
        printf("Erro de sintaxe: esperado um fator, encontrado EOF\n");
        exit(1);
    }
    return NULL; // Para evitar warnings de retorno
}

ASTNode *ativacao(Buffer *buffer) {
    ASTNode *call = create_node("Função_Chamada", current_token->value);
    match(buffer, "SYMBOL_LPAREN");
    add_child(call, args(buffer));
    match(buffer, "SYMBOL_RPAREN");
    return call;
}

ASTNode *args(Buffer *buffer) {
    if (lookahead_token && (strcmp(current_token->type, "IDENTIFIER") == 0 || strcmp(current_token->type, "NUM") == 0)) {
        return arg_lista(buffer);
    } 
    return create_node("Vazio", NULL); // Nada a fazer para 'vazio'
}

ASTNode *arg_lista(Buffer *buffer) {
    ASTNode *arg_list = create_node("Arg_Lista", NULL);
    add_child(arg_list, expressao(buffer));
    while (lookahead_token && strcmp(lookahead_token->type, "SYMBOL_COMMA") == 0) {
        match(buffer, "SYMBOL_COMMA");
        add_child(arg_list, expressao(buffer));
    }
    return arg_list;
}

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

ASTNode *retorno_decl(Buffer *buffer) {
    ASTNode *return_stmt = create_node("Retorno_Declaração", NULL);
    match(buffer, "RETURN");
    if (lookahead_token && strcmp(current_token->type, "SYMBOL_SEMICOLON") != 0) {
        add_child(return_stmt, expressao(buffer));
    }
    match(buffer, "SYMBOL_SEMICOLON");
    return return_stmt;
}