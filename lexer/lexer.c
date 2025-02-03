#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "funcs.h"
#include <string.h>

void lexer(Buffer *buffer, FILE *arquivo) {
    char ch;
    // pega char por char no arquivo enviado
    while ((ch = get_next_char(arquivo, buffer)) != EOF) {
        // Ignora espaços, tabulações e novas linhas
        if (isspace(ch)) {
            if (buffer->position > 0) { // Finaliza o lexema atual
                buffer->data[buffer->position] = '\0';
                compare_lexema(buffer); // Processa o lexema
                buffer->position = 0;   // Reinicia o buffer
                buffer->column++; // aumenta a coluna
            }
            continue;
        }

        // Detecta números
        else if (isdigit(ch)) {
            buffer->data[buffer->position++] = ch;

            // Lê os próximos caracteres enquanto forem dígitos
            while (isdigit(peek_char(arquivo))) {
                // vai adicionando o char no buffer enquanto é digito
                buffer->data[buffer->position++] = get_next_char(arquivo, buffer);
                // se o tamanho do número passar do tamanho do buffer, sobe um erro
                if (buffer->position >= buffer->size - 1) {
                    printf("Erro: Número muito longo.\n");
                    exit(1);
                }
            }
            // se tiver uma letra, sobe um erro
            if (isalpha(peek_char(arquivo))) {
                report_lexical_error(buffer);
                exit(1);
            }

            buffer->data[buffer->position] = '\0'; // Finaliza o lexema
            compare_lexema(buffer); // Processa o número (NUM ou DIGIT)
            buffer->position = 0;   // Reinicia o buffer
            buffer->column++; // aumenta a coluna
            continue;
        }

        // Detecta identificadores e palavras-chave
        else if (isalpha(ch) || ch == '_') {
            // pega o primeiro caso for letra ou '_'
            buffer->data[buffer->position++] = ch;
            // vai verificando se o prox caracter do arquivo é letra, num ou '_'
            while (isalnum(peek_char(arquivo)) || peek_char(arquivo) == '_') {
                buffer->data[buffer->position++] = get_next_char(arquivo, buffer);
                // se o tamando do ID passar do tamanho do buffer, sobe um erro
                if (buffer->position >= buffer->size - 1) {
                    printf("Erro: Lexema muito longo.\n");
                    exit(1);
                }
            }
            buffer->data[buffer->position] = '\0'; // Finaliza o lexema
            compare_lexema(buffer); // Processa o identificador ou palavra-chave
            buffer->position = 0;   // Reinicia o buffer
            buffer->column++; // aumenta a coluna
            continue;
        }

        // Ignora comentários
        else if (ch == '/' && peek_char(arquivo) == '*') {
            skip_comment(arquivo, buffer);
            continue;
        }

        // Detecta símbolos especiais (incluindo duplos como '==' ou '!=')
        else {
            buffer->data[0] = ch;
            buffer->data[1] = '\0';
            char next_ch = peek_char(arquivo);

            // Verifica se é um símbolo composto
            if ((ch == '=' && next_ch == '=') ||
                (ch == '!' && next_ch == '=') ||
                (ch == '<' && next_ch == '=') ||
                (ch == '>' && next_ch == '=')) {
                buffer->data[1] = get_next_char(arquivo, buffer);
                buffer->data[2] = '\0'; // Finaliza o símbolo duplo
            }
            compare_lexema(buffer); // Processa o símbolo
            buffer->position = 0;   // Reinicia o buffer
            buffer->column++;
        }
    }
}
