#include "pwm.h"
#include <stdio.h>

int main(void)
{
    double gamma = 2.0;
    unsigned bits =8;
    for(
        double brightness = 0;
        brightness <= 100;
        brightness += 25
    )
    {
        long result = pwm_count(brightness, gamma, bits);
        printf("brightness = %.0f, pwm = %ld\n", brightness, result);
    }
}
