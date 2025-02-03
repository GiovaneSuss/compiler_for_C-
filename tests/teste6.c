void main(void)
{
    int array[5];
    int i;
    i = 0;
    while (i < 5) {
        array[i] = i * i;
        i = i + 1;
    }
    i = 0;
    while (i < 5) {
        output(array[i]);
        i = i + 1;
    }
}