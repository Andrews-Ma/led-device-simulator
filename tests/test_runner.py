"""Checks for the test runner's EOF and timeout behavior."""
from pathlib import Path
import os
import subprocess
import sys
import unittest

BINARY = Path(__file__).resolve().parents[1] / 'build/device-sim'

class RunnerTests(unittest.TestCase):
    def test_device_waits_for_input_then_exits_on_eof(self):
        process = subprocess.Popen([str(BINARY)], stdin=subprocess.PIPE,
                                   stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        try:
            process.stdin.write('SET 50\nGET\n')
            process.stdin.flush()
            with self.assertRaises(subprocess.TimeoutExpired):
                process.wait(timeout=0.2)
            # communicate closes stdin, allowing the device loop to see EOF.
            output, errors = process.communicate(timeout=3)
            self.assertEqual(process.returncode, 0)
            self.assertEqual(output.splitlines(), ['OK', 'brightness = 50 pwm = 64'])
            self.assertEqual(errors, '')
        finally:
            if process.poll() is None:
                process.kill()
            process.communicate()

    def test_run_timeout_reaps_direct_child(self):
        with self.assertRaises(subprocess.TimeoutExpired) as caught:
            subprocess.run([sys.executable, '-c',
                'import os,time; print(os.getpid(),flush=True); time.sleep(30)'],
                capture_output=True, text=True, timeout=0.5)
        raw = caught.exception.stdout
        if isinstance(raw, bytes):
            raw = raw.decode()
        self.assertTrue(raw, 'fixture must report PID before timeout')
        pid = int(raw.strip())
        with self.assertRaises(ProcessLookupError):
            os.kill(pid, 0)

if __name__ == '__main__':
    unittest.main()
