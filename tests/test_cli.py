"""End-to-end checks for the command-line interface."""
from pathlib import Path
import subprocess
import unittest

BINARY = Path(__file__).resolve().parents[1] / 'build' / 'led-dimming'

class CommandLineTests(unittest.TestCase):
    def test_valid_results(self):
        for args, expected in [(['0','2','8'],0), (['25','2','8'],16),
                               (['50','2','8'],64), (['75','2','8'],143),
                               (['100','2','8'],255), (['50','1','8'],128)]:
            with self.subTest(args=args):
                run = subprocess.run([str(BINARY), *args], capture_output=True, text=True, timeout=5)
                self.assertEqual(run.returncode, 0)
                self.assertIn(f'pwm = {expected}\n', run.stdout)
                self.assertEqual(run.stderr, '')

    def test_invalid_inputs(self):
        cases = [[], ['50','8'], ['50','2','8','extra'], ['Null','2','8'],
                 ['50abc','2','8'], ['','2','8'], ['50','bad','8'],
                 ['-100','2','8'], ['50','-10','8'], ['50','2','30'],
                 ['50','2','8.5'], ['50','2','-1'], ['nan','2','8'],
                 ['50','inf','8'], ['1e9999','2','8'], ['50','2','99999999999999999999999999']]
        for args in cases:
            with self.subTest(args=args):
                run = subprocess.run([str(BINARY), *args], capture_output=True, text=True, timeout=5)
                self.assertNotEqual(run.returncode, 0)
                self.assertEqual(run.stdout, '')
                self.assertIn('Error:', run.stderr)
                self.assertIn('Usage:', run.stderr)

if __name__ == '__main__':
    unittest.main()
