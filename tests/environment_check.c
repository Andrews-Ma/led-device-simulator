#include <math.h>
#include <stdio.h>
int main(void)
{
    volatile double value = 0.5;
    long count = lround(pow(value, 2.0) * 255.0);
    printf("Environment check: count=%ld (expected 64)\n", count);
    return count == 64 ? 0 : 1;
}
