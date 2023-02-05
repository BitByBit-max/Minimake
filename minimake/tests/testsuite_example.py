#!/usr/bin/env python3
"""
DESCRIPTION
    This script will help you quickly get started running simple tests on
    your project. It is very basic, and you are encouraged to modify it to suit
    your needs (handling commandline options, checking stderr, checking return
    codes, etc.).

HOW TO USE
    - Make sure the `minimake` binary is in the parent directory of this script.
      (or modify the script to specify the path of the binary).

    - Place files with a ".makefile" extension in the same directory as this
      program.

    - For each ".makefile" file, there must be a ".expected_output" with the
      same base name in the same directory. This file must contain the expected
      contents of stdout when minimake is run using the ".makefile" file.

    - Run the script. The script will run Minimake on all ".makefile" files and
      point out any differences between the output and the ".expected_output"
      files.

DISCLAIMER
    This script is provided only for your convenience. The Assistants may choose
    not to help you if you encounter problems during its use.
"""

import os
import subprocess
import sys
from pathlib import Path

script_directory = Path(__file__).resolve().parent

# Check that the minimake binary exists
minimake_binary = script_directory.parent.joinpath('minimake')
if not minimake_binary.exists():
    print('minimake binary not found in parent directory')
    exit(1)

# List all `.makefile` files in the script's directory
makefiles = list()
with os.scandir(script_directory) as iterator:
    for entry in iterator:
        if entry.name.endswith('.makefile') and entry.is_file():
            makefiles.append(entry)

total_tests = 0
failed_tests = 0

for makefile in makefiles:
    basename = makefile.name.removesuffix('.makefile')
    expected_output_file = script_directory.joinpath(f'{basename}.expected_output')

    # Read the corresponding `.expected_output` file
    if expected_output_file.exists():
        with open(expected_output_file, 'r') as file:
            expected_output = file.read()
    else:
        print(f'File `{expected_output_file.name}` not found. Skipping test...')
        continue

    # Run minimake
    minimake_command = f'"{str(minimake_binary)}" -f "{str(makefile.path)}"'
    completed_process = subprocess.run(minimake_command , shell=True, capture_output=True)
    minimake_output = completed_process.stdout.decode(sys.stdout.encoding)

    if minimake_output != expected_output:
        print(f'Test `{basename}` failed.')
        print(f'Expected output:\n{expected_output}')
        print(f'Actual output:\n`{minimake_output}`')

        failed_tests += 1

    total_tests += 1

print('Testsuite results:')
print(f'  {total_tests} tests run')
print(f'  ({total_tests - failed_tests} successful, {failed_tests} failed)')
