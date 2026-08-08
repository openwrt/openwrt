#!/usr/bin/env python3
"""Build and verify an ImmortalWrt 23.05 tar-based OpenWrt IPK."""

from __future__ import annotations

import gzip
import hashlib
import io
import re
import struct
import tarfile
import zipfile
from pathlib import Path, PurePosixPath


ROOT = Path(__file__).resolve().parents[1]
WORKSPACE = ROOT.parent
PACKAGE_NAME = "luci-app-fanctrol"
SOURCE_DATE_EPOCH = 1786064400


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

    for raw in path.read_text(encoding="utf-8").splitlines():
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
            "usr/bin/fancontrol",
        } else 0o644
        files[f"./{relative}"] = (read_router_text(path), mode)

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

    files["./usr/lib/lua/luci/i18n/fancontrol.zh-cn.lmo"] = (
        compile_lmo(ROOT / "po/zh-cn/fancontrol.po"),
        0o644,
    )

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
        f"Source: package/{PACKAGE_NAME}\n"
        f"SourceName: {PACKAGE_NAME}\n"
        "Architecture: all\n"
        "Maintainer: ZJJCKA\n"
        "Section: luci\n"
        "Priority: optional\n"
        f"Installed-Size: {data_size}\n"
        "Description: 适配BPI-R3-MINI风扇专用\n"
    )

    md5sums = "".join(
        f"{hashlib.md5(data).hexdigest()}  {normalize_name(name)}\n"
        for name, (data, _mode) in sorted(data_files.items())
    )
    control_files = {
        "./control": (control_text.encode("utf-8"), 0o644),
        "./conffiles": (read_make_block("conffiles").encode("utf-8"), 0o644),
        "./md5sums": (md5sums.encode("ascii"), 0o644),
        "./preinst": (read_make_block("preinst").encode("utf-8"), 0o755),
        "./postinst": (read_make_block("postinst").encode("utf-8"), 0o755),
        "./prerm": (read_make_block("prerm").encode("utf-8"), 0o755),
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
    control_text = control["control"][0].decode("utf-8")
    for field in (
        f"Package: {PACKAGE_NAME}",
        f"Version: {version}-{release}",
        "Architecture: all",
    ):
        if field not in control_text:
            raise RuntimeError(f"Missing control field: {field}")

    data = archive_members(outer["data.tar.gz"][0])
    expected = {normalize_name(name): value for name, value in source_files.items()}
    if data != expected:
        raise RuntimeError("Packaged files or modes do not match the source tree")

    for name, (content, _mode) in data.items():
        if not name.endswith(".lmo") and b"\r" in content:
            raise RuntimeError(f"Non-Unix line ending in {name}")

    return hashlib.sha256(payload).hexdigest()


def build_source_zip(output: Path) -> None:
    with zipfile.ZipFile(
        output, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9
    ) as archive:
        for path in sorted(ROOT.rglob("*")):
            if not path.is_file() or ".git" in path.parts:
                continue
            if path.suffix in {".ipk", ".zip", ".pyc"} or path.name == ".DS_Store":
                continue
            archive.write(path, f"{ROOT.name}/{path.relative_to(ROOT).as_posix()}")


def main() -> None:
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

    source = WORKSPACE / f"{PACKAGE_NAME}-{version}-source.zip"
    build_source_zip(source)

    print(ipk)
    print(f"size={ipk.stat().st_size}")
    print(f"sha256={digest}")
    print(source)
    print(f"source_sha256={hashlib.sha256(source.read_bytes()).hexdigest()}")


if __name__ == "__main__":
    main()
