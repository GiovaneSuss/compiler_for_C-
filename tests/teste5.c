int global_var;

void increment(void)
{
    global_var = global_var + 1;
}

void main(void)
{
    global_var = 0;
    increment();
    increment();
    output(global_var);
}