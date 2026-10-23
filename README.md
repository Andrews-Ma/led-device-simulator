# LED Device Simulator & Protocol Tests

A C-based LED device simulator with a command-line calculator and Python integration tests. The project connects brightness mapping, validated state updates, and process-level testing in a reproducible macOS/Linux workflow.

## Highlights

- Parameterized brightness normalization, gamma mapping, and integer PWM count calculation.
- Configurable 1–16-bit resolution in the calculator; finite-number and range validation.
- Stateful `SET` / `GET` interaction: rejected commands preserve the last valid state.
- Python tests for command sequences, malformed input, long-line recovery, EOF, and direct-child timeout cleanup.
- Verified on macOS ARM64 and Ubuntu 24.04 ARM64.

## Quick start

Requirements: Clang, Make, Python 3. The code uses the C11 standard and Python's standard library.

```sh
make
./build/led-dimming 75 2 8
```

```text
brightness = 75, pwm = 143
```

The arguments are brightness percent, gamma, and PWM resolution in bits. Valid input requires brightness in `[0, 100]`, positive finite gamma, and an integer resolution in `[1, 16]`. Invalid calculator arguments produce a diagnostic and usage on stderr, with exit status 1.

## Stateful device simulation

```sh
make -s device
```

Enter one command per line:

```text
SET 50
OK
GET
brightness = 50 pwm = 64
SET -10
ERROR invalid brightness
GET
brightness = 50 pwm = 64
```

The simulator uses gamma 2.0 and 8-bit PWM. Its initial state is brightness 0 / PWM 0. Each successful `SET` computes and validates the candidate before updating both state fields. Invalid commands return an error response while the session continues. `GET` reads state without changing it. End input with Ctrl+D on an empty line.

## Build and verification

```sh
make -s clean
make -s test test-cli test-device test-integration csv demo run
```

| Check | Coverage |
|---|---|
| `make test` | 22 fixed expected-value cases: numerical results, endpoints, ranges, NaN and infinity |
| `make test-cli` | 6 valid and 16 invalid command-line scenarios |
| `make test-device` | Successful updates, rejection without state changes, subsequent recovery |
| `make test-integration` | 6 protocol tests and 2 EOF / timeout runner tests |

The protocol tests exercise the compiled executable through stdin/stdout. The runner tests distinguish waiting for open input from calculation failure, and check that a timed-out direct child is terminated and reaped.

A GitHub Actions workflow in `.github/workflows/test.yml` runs the verification command on Ubuntu. Execution results are available in the repository's Actions tab after a run.

## Calculation and CSV export

The mapping convention is:

```text
normalized = brightness / 100
duty       = normalized ^ gamma
max_count  = 2^bits - 1
count      = round(duty * max_count)
```

For brightness 75%, gamma 2 and 8-bit resolution, `round(0.75² × 255) = 143`.

```sh
make -s csv
```

This generates `build/dimming.csv` with a header and 101 rows, covering brightness 0–100 at gamma 2.2 / 8 bits. Columns include linear and gamma-mapped duty cycles and integer counts. `make demo` runs the five-level demonstration.

## Project layout

```text
src/pwm.c             Numerical mapping and validation
src/device_state.c    Validated state updates
src/device_sim.c      Line-oriented SET / GET interface
src/main.c            Calculator CLI
src/export_csv.c      CSV export
src/demo.c            Five-level demonstration
tests/                C unit checks and Python process tests
Dockerfile            Ubuntu development environment
Makefile              Build, test, and demo entry points
```

## Linux environment on macOS

An optional Docker development image supports Linux verification:

```sh
docker build -t led-dimming-linux:local .
docker run --rm led-dimming-linux:local make -s test test-cli test-device test-integration csv demo run
```

For the configured Colima setup, see [Linux development](docs/LINUX.md).

## Design scope

This is a host-side device simulation and numerical verification project. The PWM values represent integer counts under the stated mapping convention. The simulator provides a deterministic text interface for testing state behavior and command recovery.
