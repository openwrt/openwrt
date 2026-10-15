#!/usr/bin/env python3
"""Package Hasivo OpenWrt images for stock CLI 'download patch'.

Requires prepared BusyBox sources and U-Boot objects from a full build.
Stock patch validation writes patch.txt to /mnt before installer preflight.
"""

import argparse
import gzip
import hashlib
import io
import os
import re
import shlex
import shutil
import struct
import subprocess
import tarfile
import tempfile
import zlib
from pathlib import Path


MTD5_SIZE = 0x700000
MTD6_SIZE = 0x1600000
BLOCK_SIZE = 0x10000
MAGIC = 0x27051956
BOARD_HOSTS = {
    "hasivo_f1100w-4sx-4xgt": "F1100W-4SX-4XGT",
    "hasivo_f1100w-4sx-4xgt-512mb": "F1100W-4SX-4XGT",
}
APPLET_CONFIG = (
    "ASH", "ASH_BASH_COMPAT", "ASH_ECHO", "ASH_PRINTF", "ASH_TEST",
    "FEATURE_MD5_SHA1_SUM_CHECK", "GREP", "MKNOD", "REBOOT", "RM", "SED",
    "SHA256SUM", "SYNC", "TR",
)
STATIC_CONFIG = ("LFS", "STATIC", *APPLET_CONFIG)
ENV_OBJECTS = (
    "fw_env_main.o", "fw_env.o", "crc32.o", "ctype.o",
    "linux_string.o", "env_attr.o", "env_flags.o",
)

INSTALL = """#!/bin/sh
set -eu
set -o pipefail
bb=/home/patch/busybox
envtool=/home/patch/fw_printenv
setenv=/home/patch/fw_setenv
status=/tmp/hasivo-openwrt-install.status
envdev=/tmp/hasivo-env-$$
runtime=/tmp/hasivo-runtime-$$
runtime2=/tmp/hasivo-runtime2-$$
envconfig=/tmp/hasivo-fw_env-$$.config
enverr=/tmp/hasivo-fw_env-$$.err
phase=preflight
unmounted=no
flash_started=no
env_write_started=no
echo STARTED > "$status"

cleanup() {
    result=$?
    trap - EXIT
    if ! "$bb" rm -f "$envdev" "$runtime" "$runtime2" "$envconfig" "$enverr"; then
        echo "WARNING: could not remove installer temporary files" >&2
    fi
    if [ "$unmounted" = yes ] && [ "$flash_started" = no ]; then
        if mount -t jffs2 -o sync /dev/mtdblock6 /mnt; then
            echo REMOUNT_OK
        else
            echo REMOUNT_FAILED > "$status"
            echo "FAILED: remount /mnt; reboot into stock firmware"
            exit 1
        fi
    fi
    if [ "$result" -ne 0 ]; then
        if [ "$phase" = reboot ]; then
            echo FLASHED_REBOOT_REQUIRED > "$status"
            echo "FAILED to reboot: firmware verified; reboot manually"
        elif [ "$flash_started" = yes ]; then
            echo RECOVERY_REQUIRED > "$status"
            echo "FAILED in $phase: firmware may be incomplete; use serial/U-Boot recovery"
        elif [ "$env_write_started" = yes ]; then
            echo ENV_RECOVERY_REQUIRED > "$status"
            echo "FAILED in $phase: U-Boot environment may need serial recovery"
        else
            echo "FAILED in $phase" > "$status"
            echo "FAILED in $phase: firmware flashing not started"
        fi
    fi
    exit "$result"
}
trap cleanup EXIT

"$bb" grep -qx 'host: @HOST@' /mnt/switch_info/swinfo.txt
for spec in '1 00010000 BDINFO' '5 00700000 RUNTIME' '6 01600000 RUNTIME2'; do
    set -- $spec
    "$bb" grep -Eq "^mtd$1: $2 [[:xdigit:]]+ \\"$3\\"$" /proc/mtd
done
for node in "$envdev" "$runtime" "$runtime2" "$envconfig" "$enverr"; do
    [ ! -e "$node" ] || exit 1
done
"$bb" sha256sum -c /home/patch/sha256sums

raw=$("$bb" tr 'A-F' 'a-f' < /mnt/mac.txt | "$bb" tr -d '\\r\\n')
printf '%s\\n' "$raw" | "$bb" grep -Eq '^[[:xdigit:]][02468aAcCeE][[:xdigit:]]{2}\\.[[:xdigit:]]{4}\\.[[:xdigit:]]{4}$'
[ "$raw" != '0000.0000.0000' ]
mac=$(printf '%s\\n' "$raw" | "$bb" tr -d '.' | "$bb" sed 's/../&:/g;s/:$//')

"$bb" mknod "$envdev" c 90 2
"$bb" mknod "$runtime" c 90 10
"$bb" mknod "$runtime2" c 90 12
echo "$envdev 0x0 0x10000 0x10000" > "$envconfig"
phase=environment
current=$("$envtool" -c "$envconfig" -n ethaddr 2>"$enverr")
[ ! -s "$enverr" ]
current=$(printf '%s\\n' "$current" | "$bb" tr 'A-F' 'a-f')
case "$current" in
    00:e0:4c:00:00:00) ;;
    "$mac") ;;
    *) echo "FAILED: refusing to replace non-default ethaddr"; exit 1 ;;
esac
if [ "$current" != "$mac" ]; then
    env_write_started=yes
    "$setenv" -c "$envconfig" ethaddr "$mac"
fi
saved=$("$envtool" -c "$envconfig" -n ethaddr 2>"$enverr")
[ ! -s "$enverr" ]
saved=$(printf '%s\\n' "$saved" | "$bb" tr 'A-F' 'a-f')
[ "$saved" = "$mac" ]
env_write_started=no
echo MAC_SAVED

phase=unmount
umount /mnt
unmounted=yes
echo UNMOUNT_OK
phase=flash-mtd6
flash_started=yes
echo WRITING_MTD6
/bin/flashcp /home/patch/mtd6.bin "$runtime2"
echo VERIFIED_MTD6
phase=flash-mtd5
echo WRITING_MTD5
/bin/flashcp /home/patch/mtd5.bin "$runtime"
echo VERIFIED_MTD5
"$bb" sync
echo OK > "$status"
echo "INSTALL_OK: rebooting into OpenWrt"
phase=reboot
if "$bb" reboot -f; then
    exit 0
fi
echo "REBOOT_FAILED: reboot manually; do not remount /mnt"
exit 1
"""


def version(topdir: Path, package: str) -> str:
    manifest = topdir / f"package/{package}/Makefile"
    match = re.search(r"^PKG_VERSION:=(\S+)$", manifest.read_text(), re.MULTILINE)
    if match is None:
        raise ValueError(f"cannot read version from {manifest}")
    return match.group(1)


def check_static_mips(path: Path) -> None:
    data = path.read_bytes()
    if (
        len(data) < 52
        or data[:6] != b"\x7fELF\x01\x02"
        or struct.unpack_from(">H", data, 18)[0] != 8
    ):
        raise ValueError(f"{path} is not a big-endian 32-bit MIPS executable")
    phoff = struct.unpack_from(">I", data, 28)[0]
    phsize, phnum = struct.unpack_from(">HH", data, 42)
    if not phsize or phoff + phsize * phnum > len(data):
        raise ValueError(f"{path} has invalid ELF program headers")
    for offset in range(phoff, phoff + phsize * phnum, phsize):
        if struct.unpack_from(">I", data, offset)[0] in (2, 3):
            raise ValueError(f"{path} is dynamically linked; a static binary is required")


def check_image(image: bytes) -> None:
    if len(image) < 64 or len(image) > MTD5_SIZE + MTD6_SIZE:
        raise ValueError("factory firmware does not fit the 29 MiB runtime region")
    magic, header_crc, _timestamp, size, load, entry, data_crc = struct.unpack_from(
        ">7I", image
    )
    if magic != MAGIC or load != 0x80100000 or entry != load:
        raise ValueError("unexpected uImage magic or kernel load/entry address")
    header = bytearray(image[:64])
    header[4:8] = b"\0" * 4
    if size < 1 or 64 + size > len(image):
        raise ValueError("uImage kernel extends past factory image")
    if header_crc != zlib.crc32(header) or data_crc != zlib.crc32(image[64 : 64 + size]):
        raise ValueError("invalid uImage header or kernel checksum")
    if image[28:32] != b"\x05\x05\x02\x03":
        raise ValueError("expected MIPS Linux lzma-compressed kernel")
    rootfs = (64 + size + BLOCK_SIZE - 1) // BLOCK_SIZE * BLOCK_SIZE
    if rootfs > MTD5_SIZE:
        raise ValueError("kernel extends past the 7 MiB boot partition")
    if image[rootfs : rootfs + 4] != b"hsqs" or rootfs + 48 > len(image):
        raise ValueError("missing SquashFS rootfs at 64 KiB kernel boundary")
    squashfs_size = struct.unpack_from("<Q", image, rootfs + 40)[0]
    if squashfs_size < 96 or rootfs + squashfs_size > len(image):
        raise ValueError("invalid SquashFS size")


def add(tar: tarfile.TarFile, name: str, content: bytes, mode: int, mtime: int) -> None:
    info = tarfile.TarInfo(f"patch/{name}")
    info.size = len(content)
    info.mode = mode
    info.mtime = mtime
    tar.addfile(info, io.BytesIO(content))


def build_busybox(source: Path, directory: Path, cc: str, cross: str, staging: Path) -> Path:
    """Build installer-only BusyBox without changing the rootfs configuration."""
    if not (source / "Makefile").is_file():
        raise ValueError(f"missing prepared BusyBox source: {source}")
    work = directory / "busybox"
    shutil.copytree(source, work)
    env = os.environ.copy()
    env["STAGING_DIR"] = str(staging)
    command = ["make", "-s", "-C", str(work), "ARCH=mips",
               f"CROSS_COMPILE={cross}", f"LD={cc}"]
    subprocess.run([*command, "mrproper"], stdout=subprocess.DEVNULL, check=True, env=env)
    subprocess.run([*command, "allnoconfig"], stdout=subprocess.DEVNULL, check=True, env=env)
    config = work / ".config"
    settings = config.read_text()
    for option in STATIC_CONFIG:
        enabled = f"CONFIG_{option}=y\n"
        disabled = f"# CONFIG_{option} is not set\n"
        if enabled in settings:
            continue
        # An omitted Kconfig symbol cannot be enabled by editing .config.
        if disabled not in settings:
            raise ValueError(f"BusyBox allnoconfig omitted CONFIG_{option}; cannot enable it")
        settings = settings.replace(disabled, enabled)
    config.write_text(settings)
    subprocess.run([*command, "oldconfig"], input=b"n\n" * 512,
                   stdout=subprocess.DEVNULL, check=True, env=env)
    configured = config.read_text()
    # Kconfig may clear requested options when dependencies are unavailable.
    for option in (*STATIC_CONFIG, "SH_IS_ASH"):
        if f"CONFIG_{option}=y\n" not in configured:
            raise ValueError(f"BusyBox oldconfig did not enable CONFIG_{option}")
    subprocess.run([*command, "busybox"], stdout=subprocess.DEVNULL, check=True, env=env)
    binary = work / "busybox"
    check_static_mips(binary)
    return binary


def build(args: argparse.Namespace) -> None:
    image = args.image.read_bytes()
    check_image(image)
    busybox_version = version(args.topdir, "utils/busybox")
    busybox_source = (
        args.build_dir / f"busybox-{args.busybox_variant}/busybox-{busybox_version}"
    )
    env_version = version(args.topdir, "boot/uboot-tools")
    env_objects = args.build_dir / f"u-boot-{env_version}/tools/env"
    for obj in ENV_OBJECTS:
        if not (env_objects / obj).is_file():
            raise ValueError(f"missing {env_objects / obj}; build uboot-envtools first")

    epoch = int(os.environ.get("SOURCE_DATE_EPOCH", "0"))
    with tempfile.TemporaryDirectory(prefix="hasivo-stock-patch-") as directory:
        busybox = build_busybox(busybox_source, Path(directory), args.cc,
                                args.cross_compile, args.staging_dir)
        env_binary = Path(directory) / "fw_printenv"
        subprocess.run(
            [*shlex.split(args.cc), "-static", "-Wl,-z,max-page-size=4096",
             "-o", str(env_binary), *(str(env_objects / obj) for obj in ENV_OBJECTS)],
            check=True,
            env={**os.environ, "STAGING_DIR": str(args.staging_dir)},
        )
        check_static_mips(env_binary)
        firmware = image.ljust(MTD5_SIZE + MTD6_SIZE, b"\xff")
        mtd5, mtd6 = firmware[:MTD5_SIZE], firmware[MTD5_SIZE:]
        sums = (
            f"{hashlib.sha256(mtd5).hexdigest()}  /home/patch/mtd5.bin\n"
            f"{hashlib.sha256(mtd6).hexdigest()}  /home/patch/mtd6.bin\n"
        ).encode()
        patch = (
            "# This is a patch.txt file.\n"
            "cp /home/patch/patch.txt /mnt/patch.txt\n"
            "/home/patch/busybox sh /home/patch/install.sh 2>&1\n"
        ).encode()
        content = io.BytesIO()
        with gzip.GzipFile(fileobj=content, mode="wb", filename="", mtime=epoch) as gz:
            with tarfile.open(fileobj=gz, mode="w|") as tar:
                for name, data, mode in (
                    ("patch.txt", patch, 0o644),
                    ("install.sh", INSTALL.replace("@HOST@", BOARD_HOSTS[args.board]).encode(), 0o755),
                    ("sha256sums", sums, 0o644),
                    ("busybox", busybox.read_bytes(), 0o755),
                    ("fw_printenv", env_binary.read_bytes(), 0o755),
                    ("fw_setenv", env_binary.read_bytes(), 0o755),
                    ("mtd5.bin", mtd5, 0o644),
                    ("mtd6.bin", mtd6, 0o644),
                ):
                    add(tar, name, data, mode, epoch)
        payload = content.getvalue()
        args.output.write_bytes(
            payload + hashlib.md5(payload.split(b"\0", 1)[0]).digest()
        )


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--board", choices=BOARD_HOSTS, required=True)
    parser.add_argument("--image", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--topdir", type=Path, required=True)
    parser.add_argument("--build-dir", type=Path, required=True)
    parser.add_argument("--busybox-variant", choices=("default", "selinux"), required=True)
    parser.add_argument("--cross-compile", required=True)
    parser.add_argument("--staging-dir", type=Path, required=True)
    parser.add_argument("--cc", required=True)
    args = parser.parse_args()
    try:
        build(args)
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        parser.error(str(error))


if __name__ == "__main__":
    main()
