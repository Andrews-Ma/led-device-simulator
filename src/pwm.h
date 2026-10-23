#ifndef PWM_H
#define PWM_H
/* brightness: finite 0..100; gamma: finite >0; bits: 1..16.
 * Returns a nonnegative PWM count, or -1 for invalid input.
 * Host-side calculation convention; no hardware verification.
 */
long pwm_count(double brightness, double gamma, unsigned bits);
#endif
