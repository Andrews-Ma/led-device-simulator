#ifndef DEVICE_STATE_H
#define DEVICE_STATE_H

typedef struct {
    double brightness;
    long pwm;
} DeviceState;

/* state points to an existing state object.
 * Fixed configuration: gamma=2.0, bits=8.
 * Return 0 on success, -1 on invalid brightness.
 * On failure, both fields must remain unchanged.
 */
int device_set(DeviceState *state, double brightness);

#endif
