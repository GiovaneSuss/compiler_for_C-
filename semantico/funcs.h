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

// Tabela de símbolos

typedef struct symbol {
    char *name; //  lexema do símbolo
    char *scope; // escopo do símbolo
    char *type_ID; // se é var ou fun
    char *type_data; // qual token é
    int line; //  linha
    int index; // index (começa com 0 e por aí vai)
    struct symbol *next;  // Para lidar com colisões (encadeamento)
} Symbol;

typedef struct {
    Symbol **buckets; // (tabela)
    size_t size; // tamanho da tabela
    char *scope; // escopo do simbolo atual
} SymbolHashTable;

unsigned long hash(const char *str, size_t size);
SymbolHashTable *init_symbol_hash_table(size_t size);
void insert_symbol(SymbolHashTable *table, const char *name, const char *scope, const char *type_ID, const char *type_data, int line, int index);
Symbol *lookup_symbol(SymbolHashTable *table, const char *name);
void free_symbol_hash_table(SymbolHashTable *table);
void print_symbol_table(SymbolHashTable *table);
void collect_symbols(SymbolHashTable *table, Symbol **symbols, int *count);
void sort_symbols_by_index(Symbol **symbols, int count);
void create_symbol_table(SymbolHashTable *symbol_table, Buffer *buffer);
void semantic_analysis(SymbolHashTable *symbol_table, Buffer *buffer);

#endif
