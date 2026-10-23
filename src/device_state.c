#include "device_state.h"
#include "pwm.h"

int device_set(DeviceState *state, double brightness)
{
    long result = pwm_count(brightness, 2, 8);
    if(result == -1)
    {
        return -1;
    }
    state->brightness = brightness;
    state->pwm = result;
    return 0;
}
