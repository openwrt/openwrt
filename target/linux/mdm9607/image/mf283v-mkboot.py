#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2026 Dmitry Sinyavin
"""Build an LK-authenticated Android boot image for the ZTE MF283V."""

import argparse
import hashlib
import os
import struct
import subprocess
import sys
import tempfile
from pathlib import Path


BOOT_MAGIC = b"ANDROID!"
QCDT_MAGIC = b"QCDT"
PAGE_SIZE = 2048
PARTITION_SIZE = 0x00A40000
KERNEL_ADDR = 0x80008000
RAMDISK_ADDR = 0x80008000
SECOND_ADDR = 0x80F00000
TAGS_ADDR = 0x81E00000
RSA_SIGNATURE_SIZE = 256

# SHA-256 of the DER SubjectPublicKeyInfo accepted by the tested MF283V LK.
KNOWN_TEST_KEY_SHA256 = \
    "ef8a870a341e64fa02e1c808aa9f5fdbd6fdcc4542deca433d6a91ecd19dcc3b"

# QCDT v3 stores eight match words before the DT offset and size.  Keep the
# five SoC-revision tuples used by platform 0x129 in the stock MF283V image.
QCDT_MATCHES = (
    (0x129, 0x01, 0x00, 0x10000, 0, 0, 0, 0),
    (0x129, 0x08, 0x00, 0x10000, 0, 0, 0, 0),
    (0x129, 0x08, 0x01, 0x10000, 0, 0, 0, 0),
    (0x129, 0x0F, 0x00, 0x10000, 0, 0, 0, 0),
    (0x129, 0x15, 0x00, 0x10000, 0, 0, 0, 0),
)


def align(value, alignment):
    return (value + alignment - 1) // alignment * alignment


def padded(data):
    return data + bytes(align(len(data), PAGE_SIZE) - len(data))


def validate_dtb(dtb):
    if len(dtb) < 8 or dtb[:4] != b"\xd0\x0d\xfe\xed":
        raise ValueError("input has no flattened-device-tree magic")

    total_size = struct.unpack_from(">I", dtb, 4)[0]
    if total_size != len(dtb):
        raise ValueError(f"DTB header size {total_size} != file size {len(dtb)}")


def build_qcdt(dtb):
    validate_dtb(dtb)
    table_size = 12 + len(QCDT_MATCHES) * 40 + 4
    dtb_offset = align(table_size, PAGE_SIZE)
    dtb_size = align(len(dtb), PAGE_SIZE)
    qcdt = bytearray(dtb_offset + dtb_size)

    struct.pack_into("<4sII", qcdt, 0, QCDT_MAGIC, 3, len(QCDT_MATCHES))
    for index, match in enumerate(QCDT_MATCHES):
        struct.pack_into(
            "<10I", qcdt, 12 + index * 40,
            *match, dtb_offset, dtb_size
        )
    qcdt[dtb_offset:dtb_offset + len(dtb)] = dtb

    return bytes(qcdt)


def boot_id(kernel, qcdt):
    digest = hashlib.sha1()
    for component in (kernel, b"", b"", qcdt):
        digest.update(component)
        digest.update(struct.pack("<I", len(component)))
    return digest.digest()


def build_signed_region(kernel, qcdt, cmdline):
    cmdline = cmdline.encode("ascii")
    if len(cmdline) >= 512:
        raise ValueError("kernel command line does not fit Android v0 header")

    header = bytearray(PAGE_SIZE)
    struct.pack_into(
        "<8s10I", header, 0,
        BOOT_MAGIC, len(kernel), KERNEL_ADDR, 0, RAMDISK_ADDR,
        0, SECOND_ADDR, TAGS_ADDR, PAGE_SIZE, len(qcdt), 0
    )
    header[64:64 + len(cmdline)] = cmdline
    header[576:596] = boot_id(kernel, qcdt)

    return bytes(header) + padded(kernel) + padded(qcdt)


def run(command, **kwargs):
    return subprocess.run(command, check=True, **kwargs)


def sign(openssl, key, digest, strict_key_fingerprint):
    public_der = run(
        [openssl, "pkey", "-in", str(key), "-pubout", "-outform", "DER"],
        stdout=subprocess.PIPE,
    ).stdout
    fingerprint = hashlib.sha256(public_der).hexdigest()
    if fingerprint != KNOWN_TEST_KEY_SHA256:
        message = (
            "signing key does not match the known Qualcomm test-key "
            f"fingerprint (public key SHA-256 {fingerprint}); verify the "
            "key trusted by this device variant's aboot before flashing"
        )
        if strict_key_fingerprint:
            raise ValueError(message)
        print(f"warning: {message}", file=sys.stderr)

    with tempfile.TemporaryDirectory(prefix="mf283v-mkboot-") as temp_name:
        temp = Path(temp_name)
        digest_path = temp / "digest"
        signature_path = temp / "signature"
        public_path = temp / "public.pem"
        recovered_path = temp / "recovered"
        digest_path.write_bytes(digest)

        run([
            openssl, "pkey", "-in", str(key), "-pubout",
            "-out", str(public_path),
        ])
        run([
            openssl, "pkeyutl", "-sign", "-inkey", str(key),
            "-in", str(digest_path), "-pkeyopt", "rsa_padding_mode:pkcs1",
            "-out", str(signature_path),
        ])
        run([
            openssl, "pkeyutl", "-verifyrecover", "-pubin",
            "-inkey", str(public_path), "-in", str(signature_path),
            "-pkeyopt", "rsa_padding_mode:pkcs1", "-out", str(recovered_path),
        ])
        signature = signature_path.read_bytes()
        recovered = recovered_path.read_bytes()

    if len(signature) != RSA_SIGNATURE_SIZE:
        raise ValueError(f"unexpected RSA signature size: {len(signature)}")
    if recovered != digest:
        raise ValueError("signature verification failed")

    return signature


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--kernel", required=True, type=Path)
    parser.add_argument("--dtb", required=True, type=Path)
    parser.add_argument("--key", required=True, type=Path)
    parser.add_argument("--openssl-bin", required=True, type=Path)
    parser.add_argument("--strict-key-fingerprint", action="store_true")
    parser.add_argument("--cmdline", required=True)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()

    try:
        if not args.key.is_file():
            raise ValueError(f"signing key is not a readable file: {args.key}")
        if not args.openssl_bin.is_file() or not os.access(args.openssl_bin,
                                                           os.X_OK):
            raise ValueError(
                f"OpenSSL executable is not available: {args.openssl_bin}"
            )

        kernel = args.kernel.read_bytes()
        qcdt = build_qcdt(args.dtb.read_bytes())
        signed_region = build_signed_region(kernel, qcdt, args.cmdline)
        signature = sign(
            str(args.openssl_bin), args.key,
            hashlib.sha256(signed_region).digest(),
            args.strict_key_fingerprint,
        )

        if len(signed_region) + len(signature) > PARTITION_SIZE:
            raise ValueError("boot image does not fit the boot partition")

        image = bytearray(b"\xff" * PARTITION_SIZE)
        image[:len(signed_region)] = signed_region
        image[len(signed_region):len(signed_region) + len(signature)] = signature
        args.output.write_bytes(image)
    except (OSError, subprocess.CalledProcessError, ValueError) as error:
        parser.error(str(error))


if __name__ == "__main__":
    main()
