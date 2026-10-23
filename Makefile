CC = /usr/bin/clang
CFLAGS = -std=c11 -Wall -Wextra -Werror -g -O0 -Isrc
LDLIBS = -lm
.PHONY: all run test env-check clean csv demo test-cli
ARGS ?= 50 2 8
all: build/led-dimming
build:
	mkdir -p build
build/led-dimming: src/main.c src/pwm.c src/pwm.h | build
	$(CC) $(CFLAGS) src/main.c src/pwm.c $(LDLIBS) -o $@
build/test_pwm: tests/test_pwm.c src/pwm.c src/pwm.h | build
	$(CC) $(CFLAGS) tests/test_pwm.c src/pwm.c $(LDLIBS) -o $@
build/environment_check: tests/environment_check.c | build
	$(CC) $(CFLAGS) $< $(LDLIBS) -o $@
run: build/led-dimming
	./build/led-dimming $(ARGS)
test: build/test_pwm
	./build/test_pwm
env-check: build/environment_check
	./build/environment_check
clean:
	rm -rf build
build/export_csv: src/export_csv.c src/pwm.c src/pwm.h | build
	$(CC) $(CFLAGS) src/export_csv.c src/pwm.c $(LDLIBS) -o $@
csv: build/export_csv
	./build/export_csv > build/dimming.csv

build/demo: src/demo.c src/pwm.c src/pwm.h | build
	$(CC) $(CFLAGS) src/demo.c src/pwm.c $(LDLIBS) -o $@
demo: build/demo
	./build/demo
test-cli: build/led-dimming
	python3 tests/test_cli.py

.PHONY: linux-build linux-test linux-shell linux-stop
LINUX_CONTEXT = colima-led-practice
LINUX_IMAGE = led-dimming-linux:local
.PHONY: test-device
build/test_device_state: tests/test_device_state.c src/device_state.c src/device_state.h src/pwm.c src/pwm.h | build
	$(CC) $(CFLAGS) tests/test_device_state.c src/device_state.c src/pwm.c $(LDLIBS) -o $@
test-device: build/test_device_state
	./build/test_device_state
.PHONY: device
build/device-sim: src/device_sim.c src/device_state.c src/device_state.h src/pwm.c src/pwm.h | build
	$(CC) $(CFLAGS) src/device_sim.c src/device_state.c src/pwm.c $(LDLIBS) -o $@
device: build/device-sim
	./build/device-sim
linux-build:
	DOCKER_BUILDKIT=0 docker --context $(LINUX_CONTEXT) build -t $(LINUX_IMAGE) .
linux-test: linux-build
	docker --context $(LINUX_CONTEXT) run --rm $(LINUX_IMAGE) make -s test test-cli test-device test-integration csv demo run
linux-shell:
	docker --context $(LINUX_CONTEXT) run --rm -it $(LINUX_IMAGE) bash
linux-stop:
	colima stop led-practice

.PHONY: test-protocol
test-protocol: build/device-sim
	python3 tests/test_protocol.py

.PHONY: test-runner test-integration
test-runner: build/device-sim
	python3 tests/test_runner.py
test-integration: test-protocol test-runner
