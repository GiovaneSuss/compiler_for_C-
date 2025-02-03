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


//declarações só analisador sintatico
void get_next_token(Buffer *buffer);
void match(Buffer *buffer, const char *expected_type);
//funcs analisador lexico
void programa(Buffer *buffer);
void declaracao_lista(Buffer *buffer);
void declaracao(Buffer *buffer);
void var_declaracao(Buffer *buffer);
void tipo_especificador(Buffer *buffer);
void fun_declaracao(Buffer *buffer);
void param_lista(Buffer *buffer);
void param(Buffer *buffer);
void composto_decl(Buffer *buffer);
void local_declaracoes(Buffer *buffer);
void statement_lista(Buffer *buffer);
void statement(Buffer *buffer);
void expressao_decl(Buffer *buffer);
void expressao(Buffer *buffer);
void var(Buffer *buffer);
void simples_expressao(Buffer *buffer);
void soma_expressao(Buffer *buffer);
void termo(Buffer *buffer);
void fator(Buffer *buffer);
void ativacao(Buffer *buffer);
void args(Buffer *buffer);
void arg_lista(Buffer *buffer);
void selecao_decl(Buffer *buffer);
void iteracao_decl(Buffer *buffer);
void retorno_decl(Buffer *buffer);
#endif // FUNCS_H