#include "funcs.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// calcula o indice do bucket baseado no nome do símbolo
unsigned long hash(const char *str, size_t size) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % size;
}

// inicialização da tabela hash
SymbolHashTable *init_symbol_hash_table(size_t size) {
    // instancia a tabela
    SymbolHashTable *table = (SymbolHashTable *)malloc(sizeof(SymbolHashTable));
    // se a table for null deu erro na hora de alocar memória pra tabela
    if (table == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a tabela hash\n");
        exit(EXIT_FAILURE);
    }
    // size da table é passado como parâmetro
    table->size = size;
    // aloca os buckets para a tabela
    table->buckets = (Symbol **)calloc(size, sizeof(Symbol *));
    // se for NULL deu erro na alocação de memória
    if (table->buckets == NULL) {
        fprintf(stderr, "Erro ao alocar memória para os buckets\n");
        free(table);
        exit(EXIT_FAILURE);
    }
    // retorna a tabela
    return table;
}

// insere um símbolo na tabela             nome do símbolo   escopo do símbolo  se é var ou fun      tipo do token          linha     indice     
void insert_symbol(SymbolHashTable *table, const char *name, const char *scope, const char *type_ID, const char *type_data, int line, int index) {
    // calcula a posição
    unsigned long index_table = hash(name, table->size);
    // cria um nobo símbolo 
    Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
    if (new_symbol == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o símbolo\n");
        exit(EXIT_FAILURE);
    }
    // dá os parâmetros para o novo símbolo
    new_symbol->name = strdup(name);
    new_symbol->scope = strdup(scope);
    new_symbol->type_ID = type_ID ? strdup(type_ID) : NULL;
    new_symbol->type_data = strdup(type_data);
    new_symbol->line = line;
    new_symbol->index = index;
    new_symbol->next = table->buckets[index_table];
    // adiciona o novo símbolo na tabela
    table->buckets[index_table] = new_symbol;
}

// procura o símbolo na tabela
Symbol *lookup_symbol(SymbolHashTable *table, const char *name) {
    // calcula o hash pra posição usando o nome
    unsigned long index = hash(name, table->size);
    // pega o símbolo usando o index
    Symbol *current = table->buckets[index];
    // em caso do símbolo for global
    Symbol *global_symbol = NULL;

    // compara o nome e escopo com o símbolo pego da tabela
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            if (strcmp(current->scope, table->scope) == 0) {
                return current;
            }
            // se o escopo for global é um global_symbol
            if (strcmp(current->scope, "global") == 0) {
                global_symbol = current;
            }
        }
        current = current->next;
    }

    // se não encontrou no escopo atual, retorna a versão do escopo global se existir
    return global_symbol;
}

// libera a memória da tabela de símbolos
void free_symbol_hash_table(SymbolHashTable *table) {
    for (size_t i = 0; i < table->size; ++i) {
        Symbol *current = table->buckets[i];
        while (current != NULL) {
            Symbol *temp = current;
            current = current->next;
            free(temp->name);
            free(temp->scope);
            if (temp->type_ID) free(temp->type_ID);
            if (temp->type_data) free(temp->type_data);
            free(temp);
        }
    }
    free(table->buckets);
    free(table);
}

// da print na tabela de símbolos
void print_symbol_table(SymbolHashTable *table) {
    int count = 0;
    // Alocamos memória para o pior caso, que é quando cada bucket tem um símbolo
    Symbol **symbols = (Symbol **)malloc(table->size * sizeof(Symbol *));
    if (symbols == NULL) {
        fprintf(stderr, "Erro ao alocar memória para ordenar símbolos\n");
        return;
    }

    // Coletamos todos os símbolos
    collect_symbols(table, symbols, &count);

    // Ordenamos os símbolos pelo índice
    sort_symbols_by_index(symbols, count);

    // Imprimimos o cabeçalho
    printf("\nNome_ID;Escopo;Tipo_ID;Tipo_dado;Linha\n");

    // Imprimimos cada símbolo na ordem correta pelo índice
    for (int i = 0; i < count; i++) {
        if (symbols[i]->index == -1) continue;

        printf("<%s>;<", symbols[i]->name);
        if (symbols[i]->scope) printf("%s", symbols[i]->scope);
        printf(">;<");
        if (symbols[i]->type_ID) printf("%s", symbols[i]->type_ID);
        printf(">;<");
        if (symbols[i]->type_data) printf("%s", symbols[i]->type_data);
        printf(">;<");
        printf("%d", symbols[i]->line);
        printf(">\n");
    }

    // Liberamos a memória alocada
    free(symbols);
}

// coleta os símbolos da tabela pra dar print
void collect_symbols(SymbolHashTable *table, Symbol **symbols, int *count) {
    *count = 0;
    for (size_t i = 0; i < table->size; i++) {
        Symbol *current = table->buckets[i];
        while (current != NULL) {
            if (current->index != -1) {
                symbols[*count] = current;
                (*count)++;
            }
            current = current->next;
        }
    }
}

//bubble sort pra ordenar os dados da tabela usando o index
void sort_symbols_by_index(Symbol **symbols, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (symbols[j]->index > symbols[j + 1]->index) {
                Symbol *temp = symbols[j];
                symbols[j] = symbols[j + 1];
                symbols[j + 1] = temp;
            }
        }
    }
}

// função para iterar pelos tokens e criar a tabela de símbolos
void create_symbol_table(SymbolHashTable *symbol_table, Buffer *buffer) {
    int index = 0;
    Symbol *existing_symbol;

    for (int i = 0; i < buffer->token_count; i++) {
        // pega o prox token do buffer
        Token *token = &buffer->tokens[i];

        // volta pro escopo "global" se ele acha um '}'
        if (strcmp(token->type, "SYMBOL_RBRACE")== 0) {
            free(symbol_table->scope);
            symbol_table->scope = strdup("global");
        }

        if (strcmp(token->type, "IDENTIFIER") == 0) {
            // verifica se é 'INT' ou 'VOID
            if (i > 0 && (strcmp(buffer->tokens[i-1].type, "INT") == 0 || strcmp(buffer->tokens[i-1].type, "VOID") == 0)) {
                char *type_data = buffer->tokens[i-1].type;
                // se o prox token for '(' é fun
                if (i + 1 < buffer->token_count && strcmp(buffer->tokens[i+1].type, "SYMBOL_LPAREN") == 0) {
                    // verifica se já existe um símbolo com o msm nome no escopo
                    existing_symbol = lookup_symbol(symbol_table, token->value);
                    // se existe da erro
                    if (existing_symbol != NULL) {
                        printf("ERRO SEMÂNTICO: Função '%s' já declarada no escopo '%s' LINHA: %d\n", token->value, existing_symbol->scope, existing_symbol->line);
                        exit(1);
                    }
                    // se n adiciona o simbolo e aumenta o index
                    insert_symbol(symbol_table, token->value, "global", "fun", type_data, token->line, index);
                    index++;

                    // Atualiza o escopo para a função
                    free(symbol_table->scope);
                    symbol_table->scope = strdup(token->value);
                }
                // se n for '(' é uma var
                else {
                    // verifica se existe uma var ou fun com o msm nome no escopo 
                    existing_symbol = lookup_symbol(symbol_table, token->value);
                    // se tiver da erro
                    if (existing_symbol != NULL && (strcmp(existing_symbol->scope, symbol_table->scope) == 0 ||
                        strcmp(existing_symbol->scope, "global") == 0)) {
                        printf("ERRO SEMÂNTICO: Variável '%s' já declarada no escopo '%s' LINHA: %d\n", token->value, existing_symbol->scope, existing_symbol->line);
                        exit(1);
                    } 
                    // se n adiciona o simbolo e aumenta o index
                    insert_symbol(symbol_table, token->value, symbol_table->scope, "var", type_data, token->line, index);
                    index++;
                } 
            }
        }
    }
}

// função para a análise semântica
void semantic_analysis(SymbolHashTable *symbol_table, Buffer *buffer) {
    // se o índice for 1 durante a análise existe main, se manter 0 n existe
    int main_found = 0; // verifica se a main foi declarada
    // começa com o escopo global
    symbol_table->scope = strdup("global");

    // itera pelos tokens de novo
    for (int i = 0; i < buffer->token_count; i++) {
        // pega o prox token
        Token *token = &buffer->tokens[i];

         // Atualiza o escopo se encontrar uma nova função
        if (i > 0 && (strcmp(buffer->tokens[i - 1].type, "INT") == 0 || strcmp(buffer->tokens[i - 1].type, "VOID") == 0)) {
            if (i + 1 < buffer->token_count && strcmp(buffer->tokens[i + 1].type, "SYMBOL_LPAREN") == 0) {
                free(symbol_table->scope);
                symbol_table->scope = strdup(token->value);
            }
        }

        // 1 - Variável não declarada (uso sem declaração)
        if (strcmp(token->type, "IDENTIFIER") == 0) {
            Symbol *symbol = lookup_symbol(symbol_table, token->value);

            // verifica se a variável existe e está no escopo 
            if (symbol == NULL || (strcmp(symbol->scope, symbol_table->scope) != 0 && strcmp(symbol->scope, "global") != 0)) {
                // verifica se é função
                if (strcmp(buffer->tokens[i+1].type,"SYMBOL_LPAREN") == 0) {
                    printf("ERRO SEMÂNTICO: função '%s' não declarada no escopo '%s' LINHA: %d\n",
                    token->value, symbol_table->scope, token->line);
                    exit(1);
                }
                // se n é func, é variável
                printf("ERRO SEMÂNTICO: Variável '%s' não declarada no escopo '%s' LINHA: %d\n",
                    token->value, symbol_table->scope, token->line);
                exit(1);
            }

            // Se for uma atribuição (exemplo: a = ...)
            if (i + 1 < buffer->token_count && strcmp(buffer->tokens[i + 1].type, "SYMBOL_EQUALS") == 0) {
                if (symbol == NULL) {
                    printf("ERRO SEMÂNTICO: Variável '%s' não declarada LINHA: %d\n", token->value, token->line);
                    exit(1);
                }
            }

            // Chamada de função não declarada
            if (i + 1 < buffer->token_count && strcmp(buffer->tokens[i + 1].type, "SYMBOL_LPAREN") == 0) {
                if (symbol == NULL || strcmp(symbol->type_ID, "fun") != 0) {
                    printf("ERRO SEMÂNTICO: Função '%s' não foi declarada antes da chamada LINHA: %d\n", token->value, token->line);
                    exit(1);
                }
            }
        }

        // atribuição inválida (ex: tipo int recebe void -> a = func())
        if (i + 2 < buffer->token_count && strcmp(token->type, "IDENTIFIER") == 0 &&
            strcmp(buffer->tokens[i + 1].type, "SYMBOL_EQUALS") == 0 && strcmp(buffer->tokens[i + 2].type, "IDENTIFIER") == 0) {
            // verifica se existe a primeira var
            Symbol *var = lookup_symbol(symbol_table, token->value);
            // verifica se existe a segunda var
            Symbol *assigned_value = lookup_symbol(symbol_table, buffer->tokens[i + 2].value);

            if (var != NULL && assigned_value != NULL) {
                if (strcmp(var->type_data, assigned_value->type_data) != 0) {
                    printf("ERRO SEMÂNTICO: Atribuição inválida. Variável '%s' é do tipo '%s' e '%s' é do tipo '%s' LINHA: %d\n",
                        var->name, var->type_data, assigned_value->name, assigned_value->type_data, token->line);
                    exit(1);
                }
            }
        }

        // declaração inválida de variável
        if (i > 0 && strcmp(token->type, "IDENTIFIER") == 0) {
            Symbol *value = lookup_symbol(symbol_table, token->value);
            if (strcmp(value->type_ID, "var") == 0 && strcmp(value->type_data,"VOID") == 0) {
                printf("ERRO SEMÂNTICO: Variável '%s' não pode ser declarada como '%s' LINHA: %d\n", value->name, value->type_data, token->line);
                exit(1);
            }
        }
        //verifica se a main existe
        if (strcmp(token->value,"main") == 0) {
            Symbol *main_exists = lookup_symbol(symbol_table,token->value);
            if(main_exists != NULL && strcmp(main_exists->type_ID, "fun") == 0) {
                main_found = 1;
            }
        }
    }
    // se a main n existe (main_found=0) dá erro semântico
    if (main_found == 0) {
        printf("ERRO SEMÂNTICO: Função 'main' não foi declarada no programa\n");
        exit(1);
    }
}