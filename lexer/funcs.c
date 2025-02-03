#include "funcs.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char get_next_char(FILE *arquivo, Buffer *buffer) {
  char ch = fgetc(arquivo); // Lê o próximo caractere do arquivo

  if (ch == '\n') {
    buffer->line_number++; // Atualiza o número da linha ao encontrar '\n'
    buffer->column = 0; // o valor da coluna volta para zero
  }

  return ch; // Retorna o caractere lido
}

// extrutura para o mapa de lexemas
typedef struct {
    const char *lexeme;       // O lexema (palavra ou símbolo)
    TokenType token;          // Tipo do token
    const char *token_name;   // Nome textual do token
} LexemeTokenMap;

const LexemeTokenMap lexeme_token_map[] = {
    // Palavras-chave
    {"else", ELSE, "ELSE"}, {"return", RETURN, "RETURN"}, {"void", VOID, "VOID"},
    {"if", IF, "IF"}, {"int", INT, "INT"}, {"while", WHILE, "WHILE"},

    // Símbolos simples
    {";", SYMBOL_SEMICOLON, "SYMBOL_SEMICOLON"}, 
    {",", SYMBOL_COMMA, "SYMBOL_COMMA"}, 
    {"=", SYMBOL_EQUALS, "SYMBOL_EQUALS"}, 
    {"+", SYMBOL_PLUS, "SYMBOL_PLUS"}, 
    {"-", SYMBOL_MINUS, "SYMBOL_MINUS"}, 
    {"*", SYMBOL_STAR, "SYMBOL_STAR"}, 
    {"/", SYMBOL_SLASH, "SYMBOL_SLASH"}, 
    {"<", SYMBOL_LESS, "SYMBOL_LESS"}, 
    {">", SYMBOL_GREATER, "SYMBOL_GREATER"}, 
    {"(", SYMBOL_LPAREN, "SYMBOL_LPAREN"}, 
    {")", SYMBOL_RPAREN, "SYMBOL_RPAREN"}, 
    {"{", SYMBOL_LBRACE, "SYMBOL_LBRACE"}, 
    {"}", SYMBOL_RBRACE, "SYMBOL_RBRACE"}, 
    {"[", SYMBOL_LBRACKET, "SYMBOL_LBRACKET"},
    {"]", SYMBOL_RBRACKET, "SYMBOL_RBRACKET"},

    // Símbolos compostos
    {"==", EQ_OP, "EQ_OP"},
    {"!=", NE_OP, "NE_OP"},
    {">=", GE_OP, "GE_OP"},
    {"<=", LE_OP, "LE_OP"},
};

// pega o tamanho do mapa de lexemas
const int lexeme_token_map_size = sizeof(lexeme_token_map) / sizeof(lexeme_token_map[0]);

// Busca na tabela unificada
const char* find_token_name(const char *lexeme) {
    for (int i = 0; i < lexeme_token_map_size; i++) {
        if (strcmp(lexeme, lexeme_token_map[i].lexeme) == 0) {
            return lexeme_token_map[i].token_name; // Retorna o nome do token
        }
    }
    return "IDENTIFIER"; // Se não encontrar, assume que é um identificador
}

// função para guardar o token no buffer
void store_token(Token *token, Buffer *buffer, const char *token_name) {
    // copia o token_name para o token->type e adiciona o \0 no final
    strncpy(token->type, token_name, sizeof(token->type) - 1);
    token->type[sizeof(token->type) - 1] = '\0';

    // faz a mesma coisa para o lexema
    strncpy(token->value,buffer->data, sizeof(token->value) - 1);
    token->value[sizeof(token->value) - 1] = '\0';

    // adiciona a linha, coluna e aumenta o indice que indica o prox valor livre no vetor de tokens em 1
    token->line = buffer->line_number;
    token->column = buffer->column;
    buffer->token_count++;
}

// Função para comparar lexemas
void compare_lexema(Buffer *buffer) {
    // Busca o token correspondente na tabela
    const char *token_name = find_token_name(buffer->data);
    // Se encontrou na tabela de palavras reservadas
    if (strcmp(token_name, "IDENTIFIER") != 0) {
        // guarda o token no vetor de tokens
        store_token(&buffer->tokens[buffer->token_count], buffer, token_name);
        // retorna para pegar o proximo lexema
        return;
    }

    // Verifica se é um número
    int is_number = 1;
    for (int i = 0; buffer->data[i] != '\0'; i++) {
        // se não for, is_number vai para zero
        if (!isdigit(buffer->data[i])) {
            is_number = 0;
            break;
        }
    }
    // se is_number = 1, guarda o lexema no vetor de tokens
    if (is_number) {
        store_token(&buffer->tokens[buffer->token_count], buffer, "NUM");
        return;
    }

    // Verifica se é um identificador válido
    // verifica se o primeiro é caracter ou '_'
    if (isalpha(buffer->data[0]) || buffer->data[0] == '_') {
        // se for, ele itera pelo lexema verificando se é numero, letra ou '_'
        for (int i = 1; buffer->data[i] != '\0'; i++) {
            // se for um diferente de num, letra ou '_', dá um erro léxico
            if (!isalnum(buffer->data[i]) && buffer->data[i] != '_') {
                // Não é um identificador válido
                report_lexical_error(buffer);
            }
        }
        // se for IDENTIFIER válido, guarda o token e continua para o prox lexema
        store_token(&buffer->tokens[buffer->token_count], buffer, "IDENTIFIER");
        return;
    }

    // Caso contrário, trata como erro genérico
    report_lexical_error(buffer);
}

// função para dar 'peek' no prox caracter do arquivo, pegando o prox e devolvendo ao FILE
char peek_char(FILE *arquivo) {
    char ch = fgetc(arquivo);
    ungetc(ch, arquivo); // Devolve o caractere ao fluxo
    return ch;
}

// função para pular comentários se tiver
void skip_comment(FILE *arquivo, Buffer *buffer) {
    // pega o prox char
    char ch = get_next_char(arquivo, buffer);
    if (ch == '*') { // Comentário de bloco
        while (1) {
            ch = get_next_char(arquivo, buffer);
            if (ch == '*' && peek_char(arquivo) == '/') {
                get_next_char(arquivo, buffer); // Consome '/'
                break;
            }
            if (ch == EOF) {
                fprintf(stderr, "ERRO LEXICO: Comentário de bloco não terminado [linha: %d], COLUNA %d.\n", buffer->line_number, buffer->column);
                exit(1);
            }
        }
    }
}

// função para reportar um erro léxico, se tiver
void report_lexical_error(Buffer *buffer) {
    printf("ERRO LEXICO: \"%s\" INVALIDO [linha: %d], COLUNA %d\n", buffer->data,buffer->line_number,buffer->column);
    exit(1);
}