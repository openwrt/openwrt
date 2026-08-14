#!/usr/bin/env python3
"""Exercise the E87N 0%/100% cooling-state mapping from the real shell code."""

from __future__ import annotations

import os
import re
import shutil
import subprocess
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DAEMON = ROOT / "root/usr/bin/fancontrol"


def shell_path(path: Path) -> str:
    value = path.resolve().as_posix()
    if len(value) >= 3 and value[1:3] == ":/":
        return f"/{value[0].lower()}{value[2:]}"
    return value


def find_shell() -> Path:
    override = os.environ.get("FANCONTROL_TEST_SHELL")
    if override:
        candidate = Path(override)
        if candidate.is_file():
            return candidate
        raise RuntimeError(f"FANCONTROL_TEST_SHELL does not exist: {override}")

    candidate = (
        Path.home()
        / ".cache/codex-runtimes/codex-primary-runtime/dependencies/native/git/usr/bin/sh.exe"
    )
    if candidate.is_file():
        return candidate

    located = shutil.which("sh")
    if located:
        return Path(located)
    raise RuntimeError("No POSIX sh was found for the fan power-state test")


def function_source(text: str, name: str) -> str:
    match = re.search(
        rf"(?ms)^{re.escape(name)}\(\)\n\{{\n.*?^\}}\n",
        text,
    )
    if not match:
        raise AssertionError(f"missing shell function: {name}")
    return match.group(0)


def main() -> None:
    daemon = DAEMON.read_text(encoding="utf-8")
    clamp = function_source(daemon, "clamp")
    set_percent = function_source(daemon, "set_fan_percent")
    failsafe = function_source(daemon, "set_failsafe_full_speed")
    off = function_source(daemon, "turn_fan_off")
    cleanup = function_source(daemon, "cleanup")

    assert "set_fan_percent 100" in failsafe
    assert "set_fan_percent 0" not in failsafe
    assert "set_fan_percent 100 >/dev/null 2>&1 && result=0" in failsafe
    assert 'return "$result"' in failsafe
    assert "set_fan_percent 0" in off
    assert "set_fan_percent 100" not in off
    assert "set_failsafe_full_speed" in cleanup
    assert "turn_fan_off" not in cleanup
    assert "get_option" not in cleanup

    parser = daemon.index('case "${1-}" in')
    off_branch = daemon.index('if [ "${1-}" = "--off" ]; then')
    stop_branch = daemon.index('if [ "${1-}" = "--stop" ]; then')
    lock_call = daemon.index("\nacquire_controller_lock\n", stop_branch)
    assert parser < off_branch < stop_branch < lock_call
    assert "set_failsafe_full_speed\n    exit 1" in daemon[off_branch:stop_branch]
    acquire = function_source(daemon, "acquire_controller_lock")
    assert "printf '%s\\n' \"$$\" > \"$PID_FILE\"" in acquire

    write_failure = daemon.index('if ! set_fan_percent "$speed"; then')
    write_failure_end = daemon.index(
        '\n\n    state="$(cat "$COOLING_PATH/cur_state"', write_failure
    )
    write_failure_block = daemon[write_failure:write_failure_end]
    assert "speed=100" in write_failure_block
    assert "set_failsafe_full_speed" in write_failure_block
    assert "write_status" in write_failure_block
    assert "exit 1" in write_failure_block
    assert "全速保护" in write_failure_block

    harness = """#!/bin/sh
set -eu
cat()
{
    local line
    while IFS= read -r line || [ -n "$line" ]; do
        printf '%%s\\n' "$line"
    done < "$1"
}

%s
%s

SPEED_FILE="$1/speed"
COOLING_PATH="$1/cooling"
LAST_STATE=''
BOARD_NAME='edgepi,e87n'

set_fan_percent 0
[ "$(cat "$SPEED_FILE")" = '0' ]
[ "$(cat "$COOLING_PATH/cur_state")" = '0' ]

set_fan_percent 100
[ "$(cat "$SPEED_FILE")" = '100' ]
[ "$(cat "$COOLING_PATH/cur_state")" = '3' ]
""" % (clamp, set_percent)

    shell = find_shell()
    with tempfile.TemporaryDirectory(
        prefix="fan-power-states-", dir=ROOT.parent
    ) as temporary:
        root = Path(temporary)
        cooling = root / "cooling"
        cooling.mkdir()
        (cooling / "max_state").write_text("3\n", encoding="ascii")
        (cooling / "cur_state").write_text("1\n", encoding="ascii")
        script = root / "test.sh"
        script.write_text(harness, encoding="utf-8", newline="\n")
        script.chmod(0o755)
        environment = os.environ.copy()
        environment["PATH"] = str(shell.parent) + os.pathsep + environment.get(
            "PATH", ""
        )
        result = subprocess.run(
            [str(shell), shell_path(script), shell_path(root)],
            env=environment,
            capture_output=True,
            timeout=10,
            check=False,
        )
        if result.returncode != 0:
            raise AssertionError(
                result.stderr.decode("utf-8", errors="replace")
                or result.stdout.decode("utf-8", errors="replace")
            )

    print("PASS: E87N fan off/fail-safe power-state mapping")


if __name__ == "__main__":
    main()
