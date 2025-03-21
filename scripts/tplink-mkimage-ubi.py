#!/usr/bin/env python3
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# tplink-mkimage-ubi.py: Creates TP-Link firmware image (Header+UBI)
# for TP-Link Archer AX80V1, but perhaps may work on other devices.
#
# Copyright © 2025 Sergey Shlukov <ichizakurain@gmail.com>
#

import argparse
import datetime
import hashlib
import os
import random
import struct
import tempfile
from typing import Optional, List, Tuple

HEADER_SIZE = 0x1014
MODULE_HEADER_SIZE = 0x2C

MD5_KEY = bytes.fromhex("7a2b15ed9b98596de504ab44ac2a9f4e")

UPGRADE_HEADER_MAGIC = bytes.fromhex("aa554c5e831f534ba1f8f7c918df8fbf7da155aa")
TP_HEADER_MAGIC = bytes.fromhex("aa559dd1a8c88331c969fbbfbcf0d43270c755aa")

SIGNATURE_DATA = bytes.fromhex(
    "ae40e869f0a150cee9a8d0330ca828b7fb27b1a2a50a55510dbd74c2461123f2"
    "20ec7aa8956893399698c6a7eae2b0d6494cd7cc4f6d9543418988b655866237"
    "dcc2ead8b800cdcbdc6a991dd6c08839fdef1fe7850fcf57d8f78fc6627f7a6a"
    "7bfec686e87fb7a9650d6a061238fd58130bcb72c7bbef06375ce1844463"
)


def create_firmware_id(input_data: str) -> str:
    return hashlib.md5(input_data.encode("ascii")).hexdigest().upper()


def format_support_string(support_text: str) -> str:
    if "\n" in support_text and "product_name:" in support_text:
        return support_text

    if "SupportList:" not in support_text:
        support_text = "SupportList:\n" + support_text

    support_text = " ".join(support_text.split())
    support_text = support_text.replace("} {", "}\n{")
    support_text = support_text.replace("SupportList:", "SupportList:\n")
    support_text = support_text.replace("SupportList:\n ", "SupportList:\n")

    if not support_text.endswith("\n"):
        support_text += "\n"

    return support_text


def create_support_list(
    device_name: str, device_ver: str, special_ids: list[str]
) -> str:
    items = [
        f"{{product_name:{device_name},product_ver:{device_ver},special_id:{sid}}}"
        for sid in special_ids
    ]
    return "SupportList:\n" + "\n".join(items) + "\n"


def create_version_string(
    version: str = "1.1.2",
    build_date: str = "20240126",
    release_num: str = "68209",
    fw_id: str = "",
) -> str:
    return f"soft_ver:{version} Build {build_date} rel.{release_num}\nfw_id:{fw_id}\n\n"


def create_header(total_size: int, fw_id: Optional[str] = None) -> bytearray:
    header = bytearray([0xFF] * HEADER_SIZE)
    header[0:4] = struct.pack(">I", total_size)
    header[4:20] = MD5_KEY

    header[0x14:0x134] = bytes(0x134 - 0x14)
    header[0x134:0x136] = struct.pack("<H", 0x0100)
    header[0x136:0x136 + len(UPGRADE_HEADER_MAGIC)] = UPGRADE_HEADER_MAGIC
    header[0x14A:0x152] = bytes(0x152 - 0x14A)
    header[0x152:0x152 + len(SIGNATURE_DATA)] = SIGNATURE_DATA
    header[0x1D2:0x202] = bytes(0x202 - 0x1D2)
    header[0x202:0x204] = struct.pack("<H", 0x0100)
    header[0x204:0x204 + len(TP_HEADER_MAGIC)] = TP_HEADER_MAGIC

    if fw_id:
        fw_id_bytes = fw_id.encode("ascii")
        header[0x2A8:0x2A8 + len(fw_id_bytes)] = fw_id_bytes

    return header


def create_module_files(
    support: str,
    version: Optional[str] = None,
    build_date: Optional[str] = None,
    release_num: Optional[str] = None,
    fw_id: Optional[str] = None,
) -> list[tuple[str, str]]:
    temp_dir = tempfile.mkdtemp(prefix="temp_modules_")

    support_path = os.path.join(temp_dir, "support-list.txt")
    with open(support_path, "w", encoding="ascii") as f:
        f.write(format_support_string(support))

    version_path = os.path.join(temp_dir, "soft-version.txt")
    with open(version_path, "w", encoding="ascii") as f:
        f.write(
            create_version_string(
                version or "1.1.3",
                build_date or "20240126",
                release_num or "68209",
                fw_id or "",
            )
        )

    return [("support-list", support_path), ("soft-version", version_path)]


def create_modules_buffer(module_files: list[tuple[str, str]]) -> bytearray:
    buffer = bytearray()
    total_size = 0

    for idx, (module_name, file_path) in enumerate(module_files):
        with open(file_path, "rb") as f:
            file_content = f.read()
            file_size = len(file_content)

        module_header = bytearray(MODULE_HEADER_SIZE)

        module_name_bytes = module_name.encode("ascii").ljust(32, b"\0")
        module_header[0:32] = module_name_bytes

        content_offset = total_size + MODULE_HEADER_SIZE
        module_header[32:36] = struct.pack(">I", content_offset)

        if idx == len(module_files) - 1:
            next_module_offset = 0
        else:
            next_module_offset = total_size + MODULE_HEADER_SIZE + file_size
        module_header[36:40] = struct.pack(">I", next_module_offset)
        module_header[40:44] = struct.pack(">I", file_size)
        total_size += MODULE_HEADER_SIZE + file_size

        buffer.extend(module_header)
        buffer.extend(file_content)

    return buffer


def build_firmware_image(
    output_image: str,
    support: str,
    rootfs_path: Optional[str] = None,
    version: Optional[str] = None,
    device_name: Optional[str] = None,
    device_ver: Optional[str] = None,
    special_ids: Optional[list[str]] = None,
) -> None:
    build_date = datetime.datetime.now().strftime("%Y%m%d")
    release_num = str(random.randint(10000, 99999))
    version = version or "1.1.3"

    input_data = f"{version}-{device_name or 'Generic'}"
    fw_id = create_firmware_id(input_data)

    if device_name and device_ver and special_ids:
        support_str = create_support_list(device_name, device_ver, special_ids)
    else:
        support_str = support

    module_files = create_module_files(
        support_str, version, build_date, release_num, fw_id
    )
    modules_buffer = create_modules_buffer(module_files)

    rootfs_data = b""
    if rootfs_path:
        with open(rootfs_path, "rb") as f:
            rootfs_data = f.read()

    total_size = HEADER_SIZE + len(modules_buffer) + len(rootfs_data)
    image = bytearray()
    image.extend(create_header(total_size, fw_id))
    image.extend(modules_buffer)
    image.extend(rootfs_data)

    image[4:20] = hashlib.md5(image[4:]).digest()

    with open(output_image, "wb") as f:
        f.write(image)

    for _, path in module_files:
        try:
            os.remove(path)
        except Exception:
            pass
    try:
        os.rmdir(os.path.dirname(module_files[0][1]))
    except Exception:
        pass


def main() -> None:
    parser = argparse.ArgumentParser(
        description="TP-Link Ubi-Firmware Image Creation Tool"
    )
    parser.add_argument("--create", action="store_true", help="Create image")
    parser.add_argument("image", type=str, help="Output image name")
    parser.add_argument("--support", type=str, default="", help="Support list string")
    parser.add_argument("--rootfs", type=str, help="Rootfs file")
    parser.add_argument("--version", type=str, help="Firmware version")
    parser.add_argument("--device-name", type=str, help="Device name")
    parser.add_argument("--device-ver", type=str, help="Device version")
    parser.add_argument(
        "--special-ids", type=str, help="Comma separated list of special_ids"
    )
    parser.add_argument(
        "--outputprefix", type=str, default=".", help="Output file directory"
    )

    args = parser.parse_args()

    if args.create:
        output_file = os.path.join(args.outputprefix, args.image)
        build_firmware_image(
            output_file,
            args.support,
            args.rootfs,
            args.version,
            args.device_name,
            args.device_ver,
            args.special_ids.split(",") if args.special_ids else None,
        )
    else:
        print("No image creation mode specified (--create).")
        parser.print_help()


if __name__ == "__main__":
    main()