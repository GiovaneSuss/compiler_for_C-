#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "funcs.h"
#include <string.h>

void semantico(Buffer *buffer) {
    // inicia a tabela hash de símbolos
    SymbolHashTable *symbol_table = init_symbol_hash_table(1024);
    // define o escopo inicial como global
    symbol_table->scope = strdup("global");

    // adiciona o input(equivalente a scanf) e output(equivalente a printf) na tabela para não dar conflitos
    insert_symbol(symbol_table, "input", "global", "fun", "INT", 0, -1);
    insert_symbol(symbol_table, "output", "global", "fun", "VOID", 0, -1);
    
    // cria a tabela de símbolos iterando pelos tokens do buffer
    create_symbol_table(symbol_table, buffer);
    print_symbol_table(symbol_table);

    // faz a análise sintática iterando pelos tokens e usando a tabela de símbolos
    semantic_analysis(symbol_table,buffer);

    // libera a memória alocada pela tabela de símbolos
    free(symbol_table->scope);
    free_symbol_hash_table(symbol_table);
}
