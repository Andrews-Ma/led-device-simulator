"""Exercise device process behavior through its text interface."""
from pathlib import Path
import subprocess
import unittest

BINARY = Path(__file__).resolve().parents[1] / 'build/device-sim'

class ProtocolTests(unittest.TestCase):
    def run_commands(self, text):
        run = subprocess.run([str(BINARY)], input=text, capture_output=True,
                             text=True, timeout=3)
        self.assertEqual(run.returncode, 0, run.stderr)
        self.assertEqual(run.stderr, '')
        return run.stdout.splitlines()

    def test_rejected_set_preserves_state(self):
        self.assertEqual(self.run_commands('SET 50\nGET\nSET -10\nGET\n'),
                         ['OK', 'brightness = 50 pwm = 64',
                          'ERROR invalid brightness', 'brightness = 50 pwm = 64'])

    def test_long_line_does_not_poison_next_command(self):
        output = self.run_commands('SET 50\n' + 'X'*1000 + '\nGET\nSET 75\nGET\n')
        self.assertEqual(output, ['OK', 'ERROR command too long',
                         'brightness = 50 pwm = 64', 'OK', 'brightness = 75 pwm = 143'])

    def test_bad_commands_then_recovery(self):
        output = self.run_commands('SET 50\nSET Null\nSET nan\nUNKNOWN\n\nGET\n')
        self.assertEqual(output[0], 'OK')
        self.assertEqual(len(output), 6)
        self.assertTrue(all(line.startswith('ERROR') for line in output[1:5]))
        self.assertEqual(output[-1], 'brightness = 50 pwm = 64')

    def test_eof_without_final_newline(self):
        self.assertEqual(self.run_commands('SET 50\nGET'),
                         ['OK', 'brightness = 50 pwm = 64'])

    def test_empty_input_exits(self):
        self.assertEqual(self.run_commands(''), [])

    def test_rejection_after_75(self):
        output = self.run_commands("SET 75\nSET -10\nGET\n")
        self.assertEqual(output, [
            "OK",
            "ERROR invalid brightness",
            "brightness = 75 pwm = 143",
        ])

if __name__ == '__main__':
    unittest.main()
