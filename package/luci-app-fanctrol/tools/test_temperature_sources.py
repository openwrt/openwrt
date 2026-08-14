#!/usr/bin/env python3
"""E87N CPU/PHY/NVMe source contract and NVMe 1/2 ordering test."""

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
    raise RuntimeError("No POSIX sh was found for the temperature-source test")


def function_source(text: str, name: str) -> str:
    match = re.search(rf"(?ms)^{re.escape(name)}\(\)\n\{{\n.*?^\}}\n", text)
    if not match:
        raise AssertionError(f"missing shell function: {name}")
    return match.group(0)


def main() -> None:
    daemon = DAEMON.read_text(encoding="utf-8")
    controller = (ROOT / "luasrc/controller/fancontrol.lua").read_text(encoding="utf-8")
    view = (ROOT / "luasrc/view/fancontrol/main.htm").read_text(encoding="utf-8")
    init = (ROOT / "root/etc/init.d/fancontrol").read_text(encoding="utf-8")

    expected_ids = '{ "cpu", "phy", "nvme1", "nvme2" }'
    assert f"TEMP_SOURCE_IDS = {expected_ids}" in controller
    assert "var tempSourceKeys = ['cpu', 'phy', 'nvme1', 'nvme2'];" in view
    for source in ("cpu", "phy", "nvme1", "nvme2"):
        assert f'runtime_source("{source}")' in controller
        assert f"source_{source}_state=" in daemon
    for removed in ("wifi", "modem", "iwpriv", "MODEM_CACHE_FILE"):
        assert removed not in daemon
    assert "procd_open_instance sensors" not in init
    assert not (ROOT / "root/usr/bin/fancontrol-sensors").exists()
    assert 'HWMON_ROOT="${FANCONTROL_HWMON_ROOT:-/sys/class/hwmon}"' in daemon
    assert 'input="$hwmon/temp1_input"' in daemon

    functions = "\n".join(
        function_source(daemon, name)
        for name in (
            "valid_integer",
            "valid_temperature_mc",
            "discover_nvme_temperature_files",
            "discover_phy_temperature_files",
            "read_temperature_file",
        )
    )
    harness = r'''#!/bin/sh
set -eu
set -f
TEMP_MIN_MC=-40000
TEMP_MAX_MC=150000
BOARD_NAME='edgepi,e87n'
HWMON_ROOT="$1"
__FUNCTIONS__

discover_nvme_temperature_files
[ "$NVME1_TEMP_FILE" = "$HWMON_ROOT/hwmon2/temp1_input" ]
[ "$NVME2_TEMP_FILE" = "$HWMON_ROOT/hwmon10/temp1_input" ]
[ "$(read_temperature_file "$NVME1_TEMP_FILE")" = '42000' ]
[ "$(read_temperature_file "$NVME2_TEMP_FILE")" = '51000' ]

discover_phy_temperature_files
[ "$PHY_TEMP_FILES" = "$HWMON_ROOT/hwmon1/temp1_input" ]
'''.replace("__FUNCTIONS__", functions)

    shell = find_shell()
    with tempfile.TemporaryDirectory(prefix="e87n-sensors-", dir=ROOT.parent) as temporary:
        base = Path(temporary)
        hwmon = base / "hwmon"
        for index, name, temp in (
            (10, "nvme", "51000\n"),
            (2, "nvme", "42000\n"),
            (1, "mdio_bus:03", "30000\n"),
            (3, "cpu_thermal", "41000\n"),
        ):
            directory = hwmon / f"hwmon{index}"
            directory.mkdir(parents=True)
            (directory / "name").write_text(name + "\n", encoding="ascii")
            (directory / "temp1_input").write_text(temp, encoding="ascii")
        script = base / "test.sh"
        script.write_text(harness, encoding="utf-8", newline="\n")
        script.chmod(0o755)
        environment = os.environ.copy()
        environment["PATH"] = str(shell.parent) + os.pathsep + environment.get(
            "PATH", ""
        )
        result = subprocess.run(
            [str(shell), shell_path(script), shell_path(hwmon)],
            env=environment,
            capture_output=True,
            timeout=10,
            check=False,
        )
        if result.returncode != 0:
            raise AssertionError(
                result.stderr.decode("utf-8", errors="replace")
                or result.stdout.decode("utf-8", errors="replace")
                or f"sensor harness rc={result.returncode}"
            )

    print("PASS: E87N CPU/PHY/NVMe 1/NVMe 2 source contract")


if __name__ == "__main__":
    main()
