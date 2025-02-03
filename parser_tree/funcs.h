#ifndef FUNCS_H
#define FUNCS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
    // Identificadores e constantes
    IDENTIFIER,                      // sequência de caracteres ( letra letra* )
    NUM,                     // sequência de digitos ( digito digito* )
    
    // Operadores e símbolos simples
    SYMBOL_LPAREN,           // (
    SYMBOL_RPAREN,           // )
    SYMBOL_LBRACE,           // {
    SYMBOL_RBRACE,           // }
    SYMBOL_LBRACKET,         // [
    SYMBOL_RBRACKET,         // ]
    SYMBOL_SEMICOLON,        // ;
    SYMBOL_COMMA,            // ,
    SYMBOL_PLUS,             // +
    SYMBOL_MINUS,            // -
    SYMBOL_STAR,             // *
    SYMBOL_SLASH,            // /
    SYMBOL_EQUALS,           // =
    SYMBOL_LESS,             // <
    SYMBOL_GREATER,          // >

    // Operadores compostos
    LE_OP,                   // <=
    GE_OP,                   // >=
    EQ_OP,                   // ==
    NE_OP,                   // !=

    // Palavras-chave
    ELSE, IF, INT, RETURN, VOID, WHILE,
} TokenType;


typedef struct {
    char type[24];     // Tipo do token (ex: "ID", "NUM", "SYMBOL")
    char value[64];       // Valor do token (ex: "int", "42")
    int line;             // Linha do token
    int column;           // Coluna do token
} Token;

typedef struct {
    char *data; // data do buffer
    int size; // tamanho do buffer
    int position; // posição que está do buffer
    int line_number; // número da linha do código lido
    int column; // coluna onde está o lexema atual
    Token tokens[4096]; // vetor de tokens
    int token_count; // próxima posição disponível do tokens[]
} Buffer;

typedef struct ASTNode {
    char type[50]; // Tipo do nó
    char value[50]; // Valor do nó, se aplicável
    int child_count; // Contagem de filhos
    struct ASTNode *children[16]; // Filhos do nó (até 16 para este exemplo)
} ASTNode;

void get_next_token(Buffer *buffer);
ASTNode *create_node(const char *type, const char *value);
void add_child(ASTNode *parent, ASTNode *child);
void print_ast(ASTNode *node, int depth);
//Funções de parsing baseadas na gramática
ASTNode *programa(Buffer *buffer);
ASTNode *declaracao_lista(Buffer *buffer);
ASTNode *declaracao(Buffer *buffer);
ASTNode *var_declaracao(Buffer *buffer);
void tipo_especificador(Buffer *buffer);
ASTNode *fun_declaracao(Buffer *buffer);
ASTNode *param_lista(Buffer *buffer);
ASTNode *param(Buffer *buffer);
ASTNode *composto_decl(Buffer *buffer);
ASTNode *local_declaracoes(Buffer *buffer);
ASTNode *statement_lista(Buffer *buffer);
ASTNode *statement(Buffer *buffer);
ASTNode *expressao_decl(Buffer *buffer);
ASTNode *expressao(Buffer *buffer);
ASTNode *var(Buffer *buffer);
ASTNode *simples_expressao(Buffer *buffer);
ASTNode *soma_expressao(Buffer *buffer);
ASTNode *termo(Buffer *buffer);
ASTNode *fator(Buffer *buffer);
ASTNode *ativacao(Buffer *buffer);
ASTNode *args(Buffer *buffer);
ASTNode *arg_lista(Buffer *buffer);
ASTNode *selecao_decl(Buffer *buffer);
ASTNode *iteracao_decl(Buffer *buffer);
ASTNode *retorno_decl(Buffer *buffer);
#endif // FUNCS_H