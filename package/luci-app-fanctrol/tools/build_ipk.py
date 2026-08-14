#!/usr/bin/env python3
"""Build and verify an ImmortalWrt 23.05 tar-based OpenWrt IPK."""

from __future__ import annotations

import gzip
import hashlib
import io
import re
import stat
import struct
import subprocess
import sys
import tarfile
import zipfile
from datetime import datetime, timezone
from pathlib import Path, PurePosixPath


ROOT = Path(__file__).resolve().parents[1]
WORKSPACE = ROOT.parent
PACKAGE_NAME = "luci-app-fanctrol"
SOURCE_ROOT_NAME = PACKAGE_NAME
I18N_PACKAGE_NAME = "luci-i18n-fanctrol-zh-cn"
COMBINED_LMO = "./usr/lib/lua/luci/i18n/fancontrol-bundled.zh-cn.lmo"
TRANSLATION_SOURCE = ROOT / "i18n-src/fancontrol.zh-cn.po"
SOURCE_DATE_EPOCH = 1786064400
SOURCE_ZIP_TIME = datetime.fromtimestamp(
    SOURCE_DATE_EPOCH, timezone.utc
).timetuple()[:6]


def read_make_value(name: str) -> str:
    text = (ROOT / "Makefile").read_text(encoding="utf-8")
    match = re.search(rf"(?m)^{re.escape(name)}:=(.+)$", text)
    if not match:
        raise RuntimeError(f"Makefile is missing {name}")
    return match.group(1).strip()


def read_make_block(suffix: str) -> str:
    text = (ROOT / "Makefile").read_text(encoding="utf-8")
    name = re.escape(f"Package/{PACKAGE_NAME}/{suffix}")
    match = re.search(rf"(?ms)^define {name}\n(.*?)\nendef$", text)
    if not match:
        raise RuntimeError(f"Makefile is missing the {suffix} block")
    return match.group(1).replace("$$", "$").rstrip() + "\n"


def read_router_text(path: Path) -> bytes:
    return path.read_bytes().replace(b"\r\n", b"\n").replace(b"\r", b"\n")


def sfh_hash(data: bytes) -> int:
    mask = 0xFFFFFFFF
    length = len(data)
    value = length
    blocks, remainder = divmod(length, 4)
    offset = 0

    def get16(position: int) -> int:
        return data[position] | (data[position + 1] << 8)

    for _ in range(blocks):
        value = (value + get16(offset)) & mask
        temporary = ((get16(offset + 2) << 11) ^ value) & mask
        value = ((value << 16) ^ temporary) & mask
        offset += 4
        value = (value + (value >> 11)) & mask

    if remainder == 3:
        value = (value + get16(offset)) & mask
        value ^= (value << 16) & mask
        signed = data[offset + 2]
        if signed >= 128:
            signed -= 256
        value ^= (signed << 18) & mask
        value = (value + (value >> 11)) & mask
    elif remainder == 2:
        value = (value + get16(offset)) & mask
        value ^= (value << 11) & mask
        value = (value + (value >> 17)) & mask
    elif remainder == 1:
        signed = data[offset]
        if signed >= 128:
            signed -= 256
        value = (value + signed) & mask
        value ^= (value << 10) & mask
        value = (value + (value >> 1)) & mask

    value ^= (value << 3) & mask
    value = (value + (value >> 5)) & mask
    value ^= (value << 4) & mask
    value = (value + (value >> 17)) & mask
    value ^= (value << 25) & mask
    value = (value + (value >> 6)) & mask
    return value & mask


def parse_po(path: Path) -> list[tuple[str, str]]:
    entries: list[tuple[str, str]] = []
    current_id: str | None = None
    current_value: str | None = None
    field: str | None = None

    def quoted(line: str) -> str:
        import ast
        return ast.literal_eval(line[line.index('"'):])

    def flush() -> None:
        nonlocal current_id, current_value
        if current_id and current_value and current_id != current_value:
            entries.append((current_id, current_value))
        current_id = None
        current_value = None

    lines = path.read_text(encoding="utf-8").splitlines()
    unsupported = ("msgctxt ", "msgid_plural ", "msgstr[", "#, fuzzy")
    for raw in lines:
        if raw.strip().startswith(unsupported):
            raise RuntimeError(f"Unsupported PO construct in {path}: {raw.strip()}")

    for raw in lines:
        line = raw.strip()
        if line.startswith("msgid "):
            flush()
            current_id = quoted(line)
            field = "id"
        elif line.startswith("msgstr "):
            current_value = quoted(line)
            field = "value"
        elif line.startswith('"'):
            if field == "id" and current_id is not None:
                current_id += quoted(line)
            elif field == "value" and current_value is not None:
                current_value += quoted(line)
    flush()
    return entries


def compile_lmo(path: Path) -> bytes:
    values = bytearray()
    index: list[tuple[int, int, int, int]] = []
    for key, value in parse_po(path):
        key_bytes = key.encode("utf-8")
        value_bytes = value.encode("utf-8")
        offset = len(values)
        values.extend(value_bytes)
        values.extend(b"\0" * ((4 - len(value_bytes) % 4) % 4))
        index.append((sfh_hash(key_bytes), 1, offset, len(value_bytes)))

    index.sort(key=lambda item: item[0])
    index_offset = len(values)
    for entry in index:
        values.extend(struct.pack(">IIII", *entry))
    values.extend(struct.pack(">I", index_offset))
    return bytes(values)


def normalize_name(name: str) -> str:
    return name[2:] if name.startswith("./") else name


def tar_info(name: str, mode: int, size: int = 0) -> tarfile.TarInfo:
    info = tarfile.TarInfo(name)
    info.mode = mode
    info.uid = info.gid = 0
    info.uname = info.gname = "root"
    info.mtime = SOURCE_DATE_EPOCH
    info.size = size
    return info


def parent_directories(names: list[str]) -> set[str]:
    directories = {"./"}
    for name in names:
        path = PurePosixPath(normalize_name(name))
        for parent in path.parents:
            if str(parent) == ".":
                continue
            directories.add(f"./{parent.as_posix()}/")
    return directories


def make_tar_gz(
    files: dict[str, tuple[bytes, int]],
    directories: set[str] | tuple[()],
    *,
    sort_files: bool = True,
) -> bytes:
    output = io.BytesIO()
    with gzip.GzipFile(
        filename="", fileobj=output, mode="wb", compresslevel=9, mtime=0
    ) as compressed:
        with tarfile.open(
            fileobj=compressed, mode="w", format=tarfile.GNU_FORMAT
        ) as archive:
            for directory in sorted(
                directories,
                key=lambda item: (len(PurePosixPath(item).parts), item),
            ):
                name = directory if directory.endswith("/") else directory + "/"
                info = tar_info(name, 0o755)
                info.type = tarfile.DIRTYPE
                archive.addfile(info)

            names = sorted(files) if sort_files else files
            for name in names:
                content, mode = files[name]
                archive.addfile(
                    tar_info(name, mode, len(content)), io.BytesIO(content)
                )
    return output.getvalue()


def collect_data() -> dict[str, tuple[bytes, int]]:
    files: dict[str, tuple[bytes, int]] = {}

    for path in sorted((ROOT / "root").rglob("*")):
        if not path.is_file():
            continue
        relative = path.relative_to(ROOT / "root").as_posix()
        mode = 0o755 if relative in {
            "etc/init.d/fancontrol",
            "etc/uci-defaults/99-fancontrol-config",
            "usr/bin/fancontrol",
        } else 0o644
        content = path.read_bytes() if path.suffix == ".lmo" else read_router_text(path)
        files[f"./{relative}"] = (content, mode)

    mappings = {
        ROOT / "luasrc/controller/fancontrol.lua":
            "./usr/lib/lua/luci/controller/fancontrol.lua",
        ROOT / "luasrc/view/fancontrol/main.htm":
            "./usr/lib/lua/luci/view/fancontrol/main.htm",
        ROOT / "htdocs/luci-static/resources/view/fancontrol.css":
            "./www/luci-static/resources/view/fancontrol.css",
    }
    for source, target in mappings.items():
        files[target] = (read_router_text(source), 0o644)

    expected_lmo = compile_lmo(TRANSLATION_SOURCE)
    if COMBINED_LMO not in files:
        raise RuntimeError("Source tree is missing the bundled Chinese LMO")
    if files[COMBINED_LMO] != (expected_lmo, 0o644):
        raise RuntimeError("Bundled Chinese LMO does not match its translation source")

    return files


def make_control(
    version: str,
    release: str,
    data_files: dict[str, tuple[bytes, int]],
    data_size: int,
) -> bytes:
    control_text = (
        f"Package: {PACKAGE_NAME}\n"
        f"Version: {version}-{release}\n"
        "Depends: libc, luci-base, luci-compat, kmod-hwmon-pwmfan\n"
        f"Provides: {I18N_PACKAGE_NAME}\n"
        f"Source: package/{PACKAGE_NAME}\n"
        f"SourceName: {PACKAGE_NAME}\n"
        "Architecture: all\n"
        "Maintainer: ZJJCKA\n"
        "Section: luci\n"
        "Priority: optional\n"
        f"Installed-Size: {data_size}\n"
        "Description: EdgePi E87N 风扇控制（内置简体中文）\n"
    )

    md5sums = "".join(
        f"{hashlib.md5(data).hexdigest()}  {normalize_name(name)}\n"
        for name, (data, _mode) in sorted(data_files.items())
    )
    postinst_wrapper = b'''#!/bin/sh
[ "${IPKG_NO_SCRIPT}" = "1" ] && exit 0
[ -s "${IPKG_INSTROOT}/lib/functions.sh" ] || exit 0
. ${IPKG_INSTROOT}/lib/functions.sh
default_postinst $0 $@
'''
    prerm_wrapper = b'''#!/bin/sh
[ -s "${IPKG_INSTROOT}/lib/functions.sh" ] || exit 0
. ${IPKG_INSTROOT}/lib/functions.sh
default_prerm $0 $@
'''

    # Match OpenWrt's package-ipkg layout. The package-specific hooks are
    # sourced by default_postinst/default_prerm, while the wrappers own the
    # init-script lifecycle exactly once.
    control_files = {
        "./control": (control_text.encode("utf-8"), 0o644),
        "./conffiles": (read_make_block("conffiles").encode("utf-8"), 0o644),
        "./md5sums": (md5sums.encode("ascii"), 0o644),
        "./preinst": (read_make_block("preinst").encode("utf-8"), 0o755),
        "./postinst": (postinst_wrapper, 0o755),
        "./postinst-pkg": (read_make_block("postinst").encode("utf-8"), 0o755),
        "./prerm": (prerm_wrapper, 0o755),
        "./prerm-pkg": (read_make_block("prerm").encode("utf-8"), 0o755),
    }
    return make_tar_gz(control_files, {"./"})


def archive_members(payload: bytes) -> dict[str, tuple[bytes, int]]:
    members: dict[str, tuple[bytes, int]] = {}
    with tarfile.open(fileobj=io.BytesIO(payload), mode="r:gz") as archive:
        for member in archive.getmembers():
            if not member.isfile():
                continue
            extracted = archive.extractfile(member)
            if extracted is None:
                raise RuntimeError(f"Cannot extract {member.name}")
            members[normalize_name(member.name)] = (extracted.read(), member.mode)
    return members


def verify_ipk(
    destination: Path,
    version: str,
    release: str,
    source_files: dict[str, tuple[bytes, int]],
) -> str:
    payload = destination.read_bytes()
    if not payload.startswith(b"\x1f\x8b"):
        raise RuntimeError("Outer IPK is not gzip-compressed tar")

    outer = archive_members(payload)
    if list(outer) != ["debian-binary", "data.tar.gz", "control.tar.gz"]:
        raise RuntimeError(f"Unexpected outer members: {list(outer)}")
    if outer["debian-binary"][0] != b"2.0\n":
        raise RuntimeError("Invalid debian-binary marker")

    control = archive_members(outer["control.tar.gz"][0])
    expected_control_names = {
        "control",
        "conffiles",
        "md5sums",
        "preinst",
        "postinst",
        "postinst-pkg",
        "prerm",
        "prerm-pkg",
    }
    if set(control) != expected_control_names:
        raise RuntimeError(f"Unexpected control members: {sorted(control)}")
    for script in ("preinst", "postinst", "postinst-pkg", "prerm", "prerm-pkg"):
        if control[script][1] != 0o755:
            raise RuntimeError(f"Invalid executable mode for {script}")
    if control.get("conffiles") != (b"/etc/config/fancontrol\n", 0o644):
        raise RuntimeError("The staged runtime configuration must remain an opkg conffile")

    postinst_wrapper = control["postinst"][0]
    prerm_wrapper = control["prerm"][0]
    postinst_pkg = control["postinst-pkg"][0]
    prerm_pkg = control["prerm-pkg"][0]
    expected_postinst_wrapper = b'''#!/bin/sh
[ "${IPKG_NO_SCRIPT}" = "1" ] && exit 0
[ -s "${IPKG_INSTROOT}/lib/functions.sh" ] || exit 0
. ${IPKG_INSTROOT}/lib/functions.sh
default_postinst $0 $@
'''
    expected_prerm_wrapper = b'''#!/bin/sh
[ -s "${IPKG_INSTROOT}/lib/functions.sh" ] || exit 0
. ${IPKG_INSTROOT}/lib/functions.sh
default_prerm $0 $@
'''
    if postinst_wrapper != expected_postinst_wrapper:
        raise RuntimeError("postinst is not the canonical OpenWrt wrapper")
    if prerm_wrapper != expected_prerm_wrapper:
        raise RuntimeError("prerm is not the canonical OpenWrt wrapper")

    def uncommented_shell(payload: bytes) -> str:
        text = payload.decode("utf-8")
        return "\n".join(line.split("#", 1)[0] for line in text.splitlines())

    postinst_commands = uncommented_shell(postinst_pkg)
    prerm_commands = uncommented_shell(prerm_pkg)
    forbidden_postinst = (
        r"(?m)^\s*ubus(?:\s|$)",
        r"/etc/init\.d/fancontrol\s+(?:start|stop|restart|running)(?:\s|$)",
        r"(?m)^\s*(?:exit|return)\s+1(?:\s|$)",
    )
    for pattern in forbidden_postinst:
        if re.search(pattern, postinst_commands):
            raise RuntimeError(
                f"postinst-pkg contains forbidden runtime action: {pattern}"
            )
    if re.search(r"(?m)^\s*ubus(?:\s|$)|/etc/init\.d/fancontrol", prerm_commands):
        raise RuntimeError("prerm-pkg contains a duplicate service action")
    control_text = control["control"][0].decode("utf-8")
    for field in (
        f"Package: {PACKAGE_NAME}",
        f"Version: {version}-{release}",
        "Architecture: all",
        f"Provides: {I18N_PACKAGE_NAME}",
    ):
        if field not in control_text:
            raise RuntimeError(f"Missing control field: {field}")

    data = archive_members(outer["data.tar.gz"][0])
    if "etc/config/fancontrol" not in data:
        raise RuntimeError("The staged runtime configuration is missing from package data")
    if data["etc/config/fancontrol"][0] != data[
        "usr/share/fancontrol/fancontrol.default"
    ][0]:
        raise RuntimeError("The packaged runtime and maintainer defaults differ")
    expected = {normalize_name(name): value for name, value in source_files.items()}
    if data != expected:
        raise RuntimeError("Packaged files or modes do not match the source tree")

    lmo_paths = sorted(name for name in data if name.endswith(".lmo"))
    if lmo_paths != [normalize_name(COMBINED_LMO)]:
        raise RuntimeError(f"Unexpected bundled language catalogs: {lmo_paths}")
    if "usr/lib/lua/luci/i18n/fancontrol.zh-cn.lmo" in data:
        raise RuntimeError("Combined package must not reuse the split package path")

    for name, (content, _mode) in data.items():
        if not name.endswith(".lmo") and b"\r" in content:
            raise RuntimeError(f"Non-Unix line ending in {name}")

    return hashlib.sha256(payload).hexdigest()


def build_source_zip(output: Path) -> None:
    executable = {
        "root/etc/init.d/fancontrol",
        "root/etc/uci-defaults/99-fancontrol-config",
        "root/usr/bin/fancontrol",
        "tools/build_ipk.py",
        "tools/render_preview.py",
        "tools/test_release_3_2_14.py",
        "tools/test_fan_power_states.py",
        "tools/test_temperature_sources.py",
        "tools/test_upgrade_paths.py",
    }
    with zipfile.ZipFile(
        output, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9
    ) as archive:
        for path in sorted(ROOT.rglob("*")):
            if not path.is_file() or ".git" in path.parts:
                continue
            if path.suffix in {".ipk", ".zip", ".pyc"} or path.name == ".DS_Store":
                continue
            relative = path.relative_to(ROOT).as_posix()
            mode = 0o755 if relative in executable else 0o644
            info = zipfile.ZipInfo(
                f"{SOURCE_ROOT_NAME}/{relative}", date_time=SOURCE_ZIP_TIME
            )
            info.create_system = 3
            info.compress_type = zipfile.ZIP_DEFLATED
            info.external_attr = (stat.S_IFREG | mode) << 16
            archive.writestr(info, path.read_bytes(), compresslevel=9)


def verify_source_zip(output: Path) -> None:
    expected = {
        path.relative_to(ROOT).as_posix(): path
        for path in sorted(ROOT.rglob("*"))
        if path.is_file()
        and ".git" not in path.parts
        and path.suffix not in {".ipk", ".zip", ".pyc"}
        and path.name != ".DS_Store"
    }
    executable = {
        "root/etc/init.d/fancontrol",
        "root/etc/uci-defaults/99-fancontrol-config",
        "root/usr/bin/fancontrol",
        "tools/build_ipk.py",
        "tools/render_preview.py",
        "tools/test_release_3_2_14.py",
        "tools/test_fan_power_states.py",
        "tools/test_temperature_sources.py",
        "tools/test_upgrade_paths.py",
    }
    with zipfile.ZipFile(output) as archive:
        infos = archive.infolist()
        assert [info.filename for info in infos] == [
            f"{SOURCE_ROOT_NAME}/{relative}" for relative in expected
        ]
        for info, (relative, source) in zip(infos, expected.items()):
            expected_mode = 0o755 if relative in executable else 0o644
            actual_mode = (info.external_attr >> 16) & 0o777
            if info.create_system != 3 or actual_mode != expected_mode:
                raise RuntimeError(f"Invalid source ZIP mode for {relative}")
            if info.date_time != SOURCE_ZIP_TIME:
                raise RuntimeError(f"Invalid source ZIP timestamp for {relative}")
            if archive.read(info) != source.read_bytes():
                raise RuntimeError(f"Source ZIP content mismatch for {relative}")


def main() -> None:
    subprocess.run(
        [sys.executable, str(ROOT / "tools/test_release_3_2_14.py")],
        check=True,
    )
    subprocess.run(
        [sys.executable, str(ROOT / "tools/test_fan_power_states.py")],
        check=True,
    )
    subprocess.run(
        [sys.executable, str(ROOT / "tools/test_temperature_sources.py")],
        check=True,
    )
    subprocess.run(
        [sys.executable, str(ROOT / "tools/test_upgrade_paths.py")],
        check=True,
    )
    version = read_make_value("PKG_VERSION")
    release = read_make_value("PKG_RELEASE")
    data_files = collect_data()
    data_tar = make_tar_gz(data_files, parent_directories(list(data_files)))
    control_tar = make_control(version, release, data_files, len(data_tar))

    outer = {
        "./debian-binary": (b"2.0\n", 0o644),
        "./data.tar.gz": (data_tar, 0o644),
        "./control.tar.gz": (control_tar, 0o644),
    }
    ipk = WORKSPACE / f"{PACKAGE_NAME}_{version}-{release}_all.ipk"
    ipk.write_bytes(make_tar_gz(outer, (), sort_files=False))
    digest = verify_ipk(ipk, version, release, data_files)

    source = WORKSPACE / f"{PACKAGE_NAME}-{version}-{release}-source.zip"
    build_source_zip(source)
    verify_source_zip(source)

    print(ipk)
    print(f"size={ipk.stat().st_size}")
    print(f"sha256={digest}")
    print(source)
    print(f"source_sha256={hashlib.sha256(source.read_bytes()).hexdigest()}")


if __name__ == "__main__":
    main()
