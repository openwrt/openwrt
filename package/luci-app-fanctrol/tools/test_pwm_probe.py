#!/usr/bin/env python3
"""Exercise fancontrol --check against simulated normal/inverted sysfs trees."""

from __future__ import annotations

import os
import shutil
import subprocess
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def write_executable(path: Path, text: str) -> None:
    path.write_text(text, encoding="utf-8", newline="\n")
    path.chmod(0o755)


def parse_report(stdout: str) -> dict[str, str]:
    report: dict[str, str] = {}
    for line in stdout.splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            report[key] = value
    return report


def run_case(mapping: str, config_direction: str, with_hwmon: bool) -> tuple[int, dict[str, str], str]:
    sh = os.environ.get("TEST_SH") or shutil.which("sh")
    if not sh:
        raise RuntimeError("Set TEST_SH to a POSIX shell executable")

    with tempfile.TemporaryDirectory(prefix="fancontrol-test-") as temporary:
        test_root = Path(temporary)
        thermal = test_root / "thermal"
        cooling = thermal / "cooling_device0"
        hwmon_root = test_root / "hwmon"
        hwmon = hwmon_root / "hwmon0"
        run_root = test_root / "run"
        mock_bin = test_root / "bin"
        for directory in (cooling, run_root, mock_bin):
            directory.mkdir(parents=True, exist_ok=True)

        (cooling / "type").write_text("pwm-fan\n", encoding="ascii")
        (cooling / "max_state").write_text("3\n", encoding="ascii")
        state_file = cooling / "cur_state"
        state_file.write_text("1\n", encoding="ascii")

        if with_hwmon:
            hwmon.mkdir(parents=True)
            (hwmon / "name").write_text("pwmfan\n", encoding="ascii")
            (hwmon / "pwm1").write_text("0\n", encoding="ascii")

        write_executable(
            mock_bin / "cat",
            """#!/bin/sh
case "$1" in
    */pwm1)
        state="$(/usr/bin/cat "$TEST_STATE")"
        case "$TEST_MAPPING:$state" in
            normal:0) printf '0\\n' ;;
            normal:1) printf '64\\n' ;;
            normal:2) printf '128\\n' ;;
            normal:3) printf '192\\n' ;;
            inverted:0) printf '255\\n' ;;
            inverted:1) printf '128\\n' ;;
            inverted:2) printf '80\\n' ;;
            inverted:3) printf '0\\n' ;;
            *) exit 1 ;;
        esac
        ;;
    *) exec /usr/bin/cat "$@" ;;
esac
""",
        )
        write_executable(
            mock_bin / "uci",
            """#!/bin/sh
case "$*" in
    *fancontrol.settings.direction*) printf '%s\\n' "$TEST_CONFIG_DIRECTION" ;;
    *) exit 1 ;;
esac
""",
        )
        write_executable(mock_bin / "logger", "#!/bin/sh\nexit 0\n")

        source = (ROOT / "root/usr/bin/fancontrol").read_text(encoding="utf-8")
        source = source.replace("/sys/class/thermal", thermal.as_posix())
        source = source.replace("/sys/class/hwmon", hwmon_root.as_posix())
        source = source.replace("/var/run", run_root.as_posix())
        test_script = test_root / "fancontrol"
        write_executable(test_script, source)

        env = os.environ.copy()
        env["PATH"] = str(mock_bin) + os.pathsep + env.get("PATH", "")
        env["TEST_STATE"] = state_file.as_posix()
        env["TEST_MAPPING"] = mapping
        env["TEST_CONFIG_DIRECTION"] = config_direction
        completed = subprocess.run(
            [sh, str(test_script), "--check"],
            env=env,
            capture_output=True,
            text=True,
            encoding="utf-8",
            timeout=10,
            check=False,
        )
        restored = state_file.read_text(encoding="ascii").strip()
        return completed.returncode, parse_report(completed.stdout), restored


def main() -> None:
    for mapping, expected_zero, expected_max in (
        ("normal", "0", "192"),
        ("inverted", "255", "0"),
    ):
        code, report, restored = run_case(mapping, "auto", True)
        assert code == 0, report
        assert report["result"] == "ok"
        assert report["direction"] == mapping
        assert report["pwm_at_state0"] == expected_zero
        assert report["pwm_at_max_state"] == expected_max
        assert report["write_restore_check"] == "ok"
        assert report["physical_fan_check"] == "not_verified"
        assert restored == "1"

    code, report, restored = run_case("normal", "auto", False)
    assert code != 0
    assert report["result"] == "failed"
    assert restored == "1"

    code, report, restored = run_case("normal", "normal", False)
    assert code == 0, report
    assert report["direction"] == "normal"
    assert report["write_restore_check"] == "ok"
    assert restored == "1"

    init_text = (ROOT / "root/etc/init.d/fancontrol").read_text(encoding="utf-8")
    view_text = (ROOT / "luasrc/view/fancontrol/main.htm").read_text(encoding="utf-8")
    assert "procd_add_reload_trigger" not in init_text
    assert "act=restart" not in view_text
    print("PASS: normal/inverted detection, state restoration, safe fallback, and hot reload")


if __name__ == "__main__":
    main()
