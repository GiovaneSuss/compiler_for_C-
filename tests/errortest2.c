int sum(int a, int b) {
    return a + b;
}

void main(void) {
    int x, y;
    x = input();
    y = input();
    output(sum(x, y @)); // Erro: '@' não é um símbolo válido em C--
}