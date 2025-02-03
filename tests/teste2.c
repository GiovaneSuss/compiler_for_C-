int max(int a, int b)
{
    if (a > b) return a;
    else return b;
}

void main(void)
{
    int x; int y;
    x = input(); y = input();
    output(max(x, y));
}