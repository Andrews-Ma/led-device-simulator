#include "pwm.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>

int main(void)
{
    const struct {
        const char *name;
        double brightness, gamma;
        unsigned bits;
        long expected;
    } cases[] = {
        {"learner: 2-bit rounding", 50, 2, 2, 1},
        {"learner: upper endpoints", 100, 2, 16, 65535},
        {"learner: negative brightness", -10, 3, 5, -1},
        {"learner: negative gamma", 50, -2, 3, -1},
        /* -1 converts to UINT_MAX before entering the unsigned parameter. */
        {"learner: converted negative bits", 50, 2, (unsigned)-1, -1},
        {"zero bits", 50, 2, 0, -1},
        {"too many bits", 50, 2, 17, -1},
        {"maximum unsigned bits", 50, 2, UINT_MAX, -1},
        {"linear midpoint", 50, 1, 8, 128},
        {"gamma midpoint", 50, 2, 8, 64},
        {"zero brightness", 0, 2, 8, 0},
        {"full brightness", 100, 2, 8, 255},
        {"minimum bits", 100, 2, 1, 1},
        {"brightness over 100", 110, 2, 8, -1},
        {"zero gamma", 50, 0, 8, -1},
        {"positive gamma below one", 25, 0.5, 8, 128},
        {"NaN brightness", NAN, 2, 8, -1},
        {"positive infinite brightness", INFINITY, 2, 8, -1},
        {"negative infinite brightness", -INFINITY, 2, 8, -1},
        {"NaN gamma", 50, NAN, 8, -1},
        {"positive infinite gamma", 50, INFINITY, 8, -1},
        {"negative infinite gamma", 50, -INFINITY, 8, -1}
    };
    unsigned failures = 0;
    const unsigned total = sizeof cases / sizeof cases[0];
    for (unsigned i = 0; i < total; ++i) {
        long actual = pwm_count(cases[i].brightness, cases[i].gamma, cases[i].bits);
        printf("%s [%u/%u] %s: expected=%ld actual=%ld\n",
               actual == cases[i].expected ? "PASS" : "FAIL", i + 1, total,
               cases[i].name, cases[i].expected, actual);
        failures += actual != cases[i].expected;
    }
    printf("Summary: %u passed, %u failed, %u total\n", total - failures, failures, total);
    return failures ? 1 : 0;
}
