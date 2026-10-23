#include "device_state.h"
#include <math.h>
#include <stdio.h>

int main(void)
{
    DeviceState state = {0.0, 0};
    int failed = 0;
    if (device_set(&state, 50.0) != 0 || state.brightness != 50.0 || state.pwm != 64) {
        puts("FAIL: SET 50 must store brightness=50, pwm=64");
        ++failed;
    }
    const double invalid[] = {-10.0, 101.0, NAN, INFINITY};
    for (unsigned i = 0; i < sizeof invalid / sizeof invalid[0]; ++i) {
        DeviceState previous = state;
        if (device_set(&state, invalid[i]) != -1 ||
            state.brightness != previous.brightness || state.pwm != previous.pwm) {
            puts("FAIL: invalid SET must preserve both state fields");
            ++failed;
        }
    }
    if (device_set(&state, 75.0) != 0 || state.brightness != 75.0 || state.pwm != 143) {
        puts("FAIL: valid SET after a rejection must still work");
        ++failed;
    }
    printf("State checks: %s\n", failed ? "FAIL" : "PASS");
    return failed ? 1 : 0;
}
