#include "pwm.h"
#include <math.h>
#include <stdio.h>

int main(void)
{
    const double gamma = 2.2;
    const unsigned bits = 8;
    puts("brightness_percent,linear_duty,gamma_duty,linear_count,gamma_count");
    for (int brightness = 0; brightness <= 100; ++brightness) {
        double duty = brightness / 100.0;
        long linear_count = pwm_count(brightness, 1.0, bits);
        long gamma_count = pwm_count(brightness, gamma, bits);
        if (linear_count < 0 || gamma_count < 0) return 1;
        printf("%d,%.10f,%.10f,%ld,%ld\n", brightness, duty,
               pow(duty, gamma), linear_count, gamma_count);
    }
    return ferror(stdout) ? 1 : 0;
}
