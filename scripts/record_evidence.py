#!/usr/bin/env python3
"""Capture local build evidence; never commit, upload, or claim learner mastery."""
import argparse
import datetime as dt
import hashlib
import json
import platform
from pathlib import Path
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--actor', required=True, choices=['assistant', 'user'])
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    now = dt.datetime.now(dt.timezone(dt.timedelta(hours=9)))
    folder = root / 'docs' / 'evidence' / now.strftime('%Y%m%dT%H%M%S_%f%z')
    folder.mkdir(parents=True, exist_ok=False)
    tracked = [root / 'Makefile']
    for dirname in ('src', 'tests', 'scripts'):
        tracked.extend(p for p in (root / dirname).rglob('*')
                       if p.is_file() and p.suffix in ('.c', '.h', '.py'))
    hashes = {str(p.relative_to(root)): hashlib.sha256(p.read_bytes()).hexdigest()
              for p in sorted(tracked)}
    commands = [
        ['git', 'rev-parse', '--verify', 'HEAD'],
        ['git', 'status', '--short'],
        ['/usr/bin/clang', '--version'],
        ['make', '-B', 'test', 'test-cli', 'test-device', 'test-integration', 'csv', 'demo', 'run'],
    ]
    results = []
    for number, command in enumerate(commands, 1):
        try:
            result = subprocess.run(command, cwd=root, capture_output=True,
                                    text=True, timeout=120)
            code, stdout, stderr = result.returncode, result.stdout, result.stderr
        except subprocess.TimeoutExpired as error:
            code, stdout, stderr = 124, '', 'Command exceeded 120 seconds.\n'
        except OSError as error:
            code, stdout, stderr = 127, '', str(error) + '\n'
        filename = f'{number:02d}.log'
        (folder / filename).write_text(
            '$ ' + ' '.join(command) + f'\nexit_code={code}\n'
            + '\n[stdout]\n' + stdout + '\n[stderr]\n' + stderr)
        results.append({'argv': command, 'exit_code': code, 'log': filename})
    manifest = {
        'captured_at': now.isoformat(),
        'clock_source': 'local system clock; UTC+09:00',
        'actor': args.actor,
        'platform': platform.system(), 'release': platform.release(),
        'architecture': platform.machine(),
        'python': platform.python_version(),
        'source_sha256': hashes, 'commands': results,
        'limitations': 'Local execution only. Actor is declared, not authenticated. '
                        'No proof of independent understanding, Linux, hardware or publication.',
    }
    (folder / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print(folder.relative_to(root))
    print('Build/test/demo exit code:', results[-1]['exit_code'])
    return 0 if results[-1]['exit_code'] == 0 else 1


if __name__ == '__main__':
    raise SystemExit(main())
