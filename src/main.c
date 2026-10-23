#include "pwm.h"
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static int parse_number(const char *text, double *value)
{
    char *end;
    errno = 0;
    *value = strtod(text, &end);
    return end != text && *end == '\0' && errno != ERANGE && isfinite(*value);
}

static int error(const char *message, const char *program)
{
    fprintf(stderr, "Error: %s\nUsage: %s BRIGHTNESS GAMMA BITS\n", message, program);
    return 1;
}

int main(int argc, char **argv)
{
    if (argc != 4) return error("expected exactly 3 parameters", argv[0]);
    double brightness, gamma;
    if (!parse_number(argv[1], &brightness))
        return error("brightness must be a finite number", argv[0]);
    if (!parse_number(argv[2], &gamma))
        return error("gamma must be a finite number", argv[0]);
    if (*argv[3] == '\0') return error("bits must be an integer from 1 to 16", argv[0]);
    for (const char *p = argv[3]; *p; ++p)
        if (!isdigit((unsigned char)*p))
            return error("bits must be an integer from 1 to 16", argv[0]);
    char *end;
    errno = 0;
    unsigned long bits = strtoul(argv[3], &end, 10);
    if (errno == ERANGE || *end || bits < 1 || bits > 16)
        return error("bits must be an integer from 1 to 16", argv[0]);
    long result = pwm_count(brightness, gamma, (unsigned)bits);
    if (result < 0) return error("brightness must be 0..100 and gamma must be positive", argv[0]);
    printf("brightness = %g, pwm = %ld\n", brightness, result);
    return 0;
}
