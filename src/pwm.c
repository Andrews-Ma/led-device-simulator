#include "pwm.h"
#include <math.h>
long pwm_count(double brightness, double gamma, unsigned bits)
{
    if(brightness < 0 || brightness > 100)
    {
        return -1;
    }
    if(gamma < 0 || gamma == 0)
    {
        return -1;
    }
    if(bits < 1 || bits > 16)
    {
        return -1;
    }
    if(isfinite(brightness)==0 || isfinite(gamma)==0)
    {
        return -1;
    }
    double duty = brightness / 100;
    double mapped_duty = pow(duty, gamma);
    unsigned max_count = pow(2, bits) - 1;
    long pwm = lround(mapped_duty*max_count);
    return pwm;
}
