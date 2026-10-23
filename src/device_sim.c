#include "device_state.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    DeviceState state = {0.0, 0};
    char line[256];
    while (fgets(line, sizeof line, stdin)) {
        if (!strchr(line, '\n') && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            puts("ERROR command too long");
            fflush(stdout);
            continue;
        }
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "GET") == 0) {
            printf("brightness = %g pwm = %ld\n", state.brightness, state.pwm);
        } else if (strncmp(line, "SET ", 4) == 0) {
            char *end;
            errno = 0;
            double brightness = strtod(line + 4, &end);
            int parsed = end != line + 4 && errno != ERANGE;
            while (isspace((unsigned char)*end)) ++end;
            if (!parsed || *end || device_set(&state, brightness) != 0)
                puts("ERROR invalid brightness");
            else
                puts("OK");
        } else {
            puts("ERROR expected GET or SET <brightness>");
        }
        fflush(stdout);
    }
    return ferror(stdin) || ferror(stdout) ? 1 : 0;
}
