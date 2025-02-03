#ifndef FUNCS_H
#define FUNCS_H

#include <stdio.h>

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


char get_next_char(FILE *arquivo, Buffer *buffer);
void reload_buffer(FILE *arquivo, Buffer *buffer);
int check_next_char(char c);
void compare_lexema(Buffer *buffer);
char peek_char(FILE *arquivo);
void skip_comment(FILE *arquivo, Buffer *buffer);
void report_lexical_error(Buffer *buffer);

#endif
