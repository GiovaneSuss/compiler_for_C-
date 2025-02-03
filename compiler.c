#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semantico/semantico.h"

Buffer *allocate_buffer(int size) {
    // instancia um buffer
    Buffer *buffer = (Buffer *)malloc(sizeof(Buffer));
    if (!buffer) {
        fprintf(stderr, "ERRO FATAL: Falha ao alocar memória para o buffer.\n");
        exit(1);
    }
    // aloca um tamanho fixo pro data do buffer
    buffer->data = (char *)malloc(size * sizeof(char));
    if (!buffer->data) {
        fprintf(stderr, "ERRO FATAL: Falha ao alocar memória para os dados do buffer.\n");
        free(buffer);
        exit(1);
    }
    // define as outras características do buffer
    buffer->size = size;
    buffer->position = 0;
    buffer->line_number = 1;
    buffer->column = 0;
    buffer->token_count = 0;
    return buffer;
}

// função para desalocar a memória do buffer
void deallocate_buffer(Buffer *buffer) {
    free(buffer->data);
    free(buffer);
}

int main(int argc, char* argv[]) {
    // detecta se o comando para rodar tem 3 argumentos, se n tiver joga o erro abaixo
    if (argc != 3) {
        printf("Uso correto: %s <arquivo> <-L|-l|-P|-p|-S|-s>\n", argv[0]);
        return 1;
    }

    // Verifica o arquivo de entrada
    FILE *arquivo = fopen(argv[1], "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", argv[1]);
        return 1;
    }

    // Aloca o buffer para o lexer
    Buffer *buffer = allocate_buffer(64);
    if (buffer == NULL) {
        printf("Erro ao alocar buffer\n");
        fclose(arquivo);
        return 1;
    }

    // pega a flag para ver o que será executado
    char *flag = argv[2];
    // compara a flag com -l ou -L para rodar o lexer
    if (strcmp(flag, "-L") == 0 || strcmp(flag,"-l") == 0) {
        lexer(buffer, arquivo);
        for (int i = 0; i < buffer->token_count; i++) {
            printf("<%s>  \"%s\" [linha: %i]\n",
                   buffer->tokens[i].type,
                   buffer->tokens[i].value,
                   buffer->tokens[i].line);
        }
    // compara a flag com -P ou -p para rodar o parser
    } else if (strcmp(flag, "-P") == 0 || strcmp(flag,"-p") == 0) {
        lexer(buffer, arquivo);
        buffer->tokens[buffer->token_count++] = (Token){"EOF", "", buffer->line_number, buffer->column};
        parser(buffer);
    // compara a flag com -S ou -s para rodar o semântico
    } else if (strcmp(flag, "-S") == 0 || strcmp(flag, "-s") == 0) {
        lexer(buffer, arquivo);
        semantico(buffer);
    // se tiver a flag errada envia um erro
    } else {
        printf("Flag inválida. Use -L, -l, -P, -p, -S ou -s.\n");
    }
    
    // Fecha o arquivo e libera a memória alocada
    deallocate_buffer(buffer);
    fclose(arquivo);
    return 0;
}