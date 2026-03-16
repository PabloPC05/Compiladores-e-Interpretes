#include <stdio.h>
#include <stdlib.h>

#include "analyzer.h"
#include "lexical_analyzer.h"
#include "input_system.h"
#include "definitions.h"

void initAnalyzer(const char *filename) {
    initInput(filename);
}

void analyze() {
    Symbol sym;
    int count = 0;

    do {
        sym = nextToken();
        printf("<%-4d, %s>\n", sym.token, sym.lexeme);
        count++;

        freeToken(&sym);
    } while (sym.token != TOKEN_EOF);

    printf("\nTotal de componentes lexicos: %d\n", count);
}

void closeAnalyzer() {
    closeInput();
}
