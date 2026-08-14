#!/usr/bin/env python3
"""Exercise fancontrol configuration preservation in an isolated filesystem."""

from __future__ import annotations

import os
import shlex
import shutil
import subprocess
import sys
import tempfile
import time
from pathlib import Path

from build_ipk import ROOT, read_make_block


DEFAULT = (ROOT / "root/etc/config/fancontrol").read_bytes()
CUSTOM = b"config fancontrol 'settings'\n    option mode '2'\n"
MULTI_SOURCE = (
    b"config fancontrol 'settings'\n"
    b"    option mode '1'\n"
    b"    option temp_sources 'phy nvme2 nvme1'\n"
)


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

    located = shutil.which("sh")
    if located:
        return Path(located)

    candidate = (
        Path.home()
        / ".cache/codex-runtimes/codex-primary-runtime/dependencies/native/git/usr/bin/sh.exe"
    )
    if candidate.is_file():
        return candidate
    raise RuntimeError("No POSIX sh was found for the upgrade behavior test")


def optional_bytes(path: Path) -> bytes | None:
    return path.read_bytes() if path.is_file() else None


def test_path(root: Path, shell: Path) -> str:
    fake_bin = root / "bin"
    fake_bin.mkdir()
    for name in ("chmod", "mkdir"):
        command = fake_bin / name
        command.write_bytes(b"#!/bin/sh\nexit 0\n")
        command.chmod(0o755)
    sleep = fake_bin / "sleep"
    sleep.write_bytes(
        b"#!/bin/sh\n"
        b'"$FANCONTROL_TEST_PYTHON" -c '
        b"'import sys,time; time.sleep(float(sys.argv[1]))' \"$1\"\n"
    )
    sleep.chmod(0o755)
    return shell_path(fake_bin) + ":" + shell_path(shell.parent) + ":/usr/bin:/bin"


def run_preinst(
    shell: Path,
    *,
    config: bytes | None,
    saved: bytes | None = None,
    backup: bytes | None = None,
    offline: bool = False,
    guard: bool = False,
) -> tuple[int, bytes | None, bytes | None]:
    with tempfile.TemporaryDirectory(prefix="fan-preinst-") as temporary:
        root = Path(temporary)
        config_path = root / "etc/config/fancontrol"
        save_path = root / "etc/.fancontrol-upgrade-backup"
        pending_path = root / "tmp/fancontrol-package.pending"
        default_path = root / "usr/share/fancontrol/fancontrol.default"
        backup_path = Path(str(config_path) + "-opkg.backup")
        for directory in (
            config_path.parent,
            save_path.parent,
            default_path.parent,
            pending_path.parent,
        ):
            directory.mkdir(parents=True, exist_ok=True)
        default_path.write_bytes(DEFAULT)
        if config is not None:
            config_path.write_bytes(config)
        if saved is not None:
            save_path.write_bytes(saved)
        if backup is not None:
            backup_path.write_bytes(backup)

        script = root / "preinst"
        script.write_text(read_make_block("preinst"), encoding="utf-8", newline="\n")
        script.chmod(0o755)
        environment = os.environ.copy()
        environment.update(
            {
                "PATH": test_path(root, shell),
                "IPKG_INSTROOT": "offline" if offline else "",
                "FANCONTROL_CONFIG_FILE": shell_path(config_path),
                "FANCONTROL_CONFIG_SAVE": shell_path(save_path),
                "FANCONTROL_DEFAULT_FILE": shell_path(default_path),
                "FANCONTROL_PENDING_FILE": shell_path(pending_path),
                "FANCONTROL_TEST_NO_GUARD": "0" if guard else "1",
                "FANCONTROL_TEST_PYTHON": shell_path(Path(sys.executable)),
            }
        )
        command = [str(shell), shell_path(script), "upgrade", "3.2.13-4"]
        if guard:
            body = (
                f"{shlex.quote(shell_path(script))} upgrade 3.2.13-4; "
                "exit $?"
            )
            command = [str(shell), "-c", body]
        result = subprocess.run(
            command,
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
        if guard:
            deadline = time.monotonic() + 8
            while time.monotonic() < deadline:
                if optional_bytes(config_path) == config:
                    break
                time.sleep(0.1)
        return (
            result.returncode,
            optional_bytes(config_path),
            optional_bytes(save_path),
        )


def run_defaults(
    shell: Path,
    *,
    config: bytes | None,
    saved: bytes | None = None,
    backup: bytes | None = None,
) -> tuple[int, bytes | None]:
    with tempfile.TemporaryDirectory(prefix="fan-defaults-") as temporary:
        root = Path(temporary)
        config_path = root / "etc/config/fancontrol"
        save_path = root / "etc/.fancontrol-upgrade-backup"
        default_path = root / "usr/share/fancontrol/fancontrol.default"
        backup_path = Path(str(config_path) + "-opkg.backup")
        for directory in (config_path.parent, save_path.parent, default_path.parent):
            directory.mkdir(parents=True, exist_ok=True)
        default_path.write_bytes(DEFAULT)
        if config is not None:
            config_path.write_bytes(config)
        if saved is not None:
            save_path.write_bytes(saved)
        if backup is not None:
            backup_path.write_bytes(backup)

        source = ROOT / "root/etc/uci-defaults/99-fancontrol-config"
        script = root / "99-fancontrol-config"
        script.write_bytes(source.read_bytes())
        script.chmod(0o755)
        environment = os.environ.copy()
        environment.update(
            {
                "PATH": test_path(root, shell),
                "FANCONTROL_CONFIG_FILE": shell_path(config_path),
                "FANCONTROL_CONFIG_SAVE": shell_path(save_path),
                "FANCONTROL_DEFAULT_FILE": shell_path(default_path),
            }
        )
        result = subprocess.run(
            [str(shell), shell_path(script)],
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
        return result.returncode, optional_bytes(config_path)


def main() -> None:
    shell = find_shell()

    rc, config, saved = run_preinst(shell, config=CUSTOM)
    assert (rc, config, saved) == (0, DEFAULT, CUSTOM)

    rc, config, saved = run_preinst(shell, config=None)
    assert (rc, config, saved) == (0, DEFAULT, None)

    rc, config, saved = run_preinst(shell, config=None, backup=CUSTOM)
    assert (rc, config, saved) == (0, DEFAULT, CUSTOM)

    rc, config, saved = run_preinst(shell, config=None, saved=CUSTOM)
    assert (rc, config, saved) == (0, DEFAULT, CUSTOM)

    rc, config, saved = run_preinst(shell, config=None, offline=True)
    assert (rc, config, saved) == (0, None, None)

    rc, config, saved = run_preinst(shell, config=CUSTOM, guard=True)
    assert (rc, config, saved) == (0, CUSTOM, CUSTOM)

    rc, config = run_defaults(shell, config=DEFAULT, saved=CUSTOM)
    assert rc == 0 and config is not None
    assert b"option mode '2'" in config
    assert b"option temp_sources 'cpu'" in config

    rc, config = run_defaults(shell, config=CUSTOM, saved=b"stale\n")
    assert rc == 0 and config is not None
    assert b"option mode '2'" in config
    assert b"option temp_sources 'cpu'" in config

    rc, config = run_defaults(shell, config=None, backup=CUSTOM)
    assert rc == 0 and config is not None
    assert b"option mode '2'" in config
    assert b"option temp_sources 'cpu'" in config

    rc, config = run_defaults(shell, config=None)
    assert (rc, config) == (0, DEFAULT)

    rc, config = run_defaults(shell, config=MULTI_SOURCE)
    assert rc == 0 and config is not None
    assert b"option temp_sources 'phy'" in config
    assert b"phy nvme2 nvme1" not in config

    print("PASS: fancontrol isolated configuration upgrade paths")


if __name__ == "__main__":
    main()
