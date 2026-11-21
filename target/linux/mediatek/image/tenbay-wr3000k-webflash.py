#!/usr/bin/env python3
"""Build a reproducible WR3000K stock web update from a sysupgrade image."""

import argparse
from datetime import datetime, timezone
import gzip
import hashlib
import io
from pathlib import Path
import subprocess
import tarfile


def add_file(archive, name, data):
    info = tarfile.TarInfo(name)
    info.size = len(data)
    info.mode = 0o755 if name.endswith(".sh") else 0o644
    archive.addfile(info, io.BytesIO(data))


def stock_archive(payload, hook, epoch):
    version = datetime.fromtimestamp(epoch, timezone.utc).strftime("V1.0-%Y%m%d%H%M\n")
    raw = io.BytesIO()
    with gzip.GzipFile(fileobj=raw, mode="wb", filename="", mtime=0) as compressed:
        with tarfile.open(fileobj=compressed, mode="w", format=tarfile.USTAR_FORMAT) as archive:
            add_file(archive, "UploadBrush-bin.img", payload)
            add_file(archive, "check_MD5.txt", hashlib.md5(payload).hexdigest().encode() + b"\n")
            add_file(archive, "check_must.txt", b"WR3000K\n")
            add_file(archive, "version.txt", version.encode())
            add_file(archive, "hook_pre.sh", hook)
    return raw.getvalue()


def encrypt(archive, openssl):
    # The stock updater uses OpenSSL's legacy SHA-256 key derivation.
    password = b'QiLunSmartWL'
    salt = hashlib.sha256(archive).digest()[:8]
    key = hashlib.sha256(password + salt).digest()
    iv = hashlib.sha256(key + password + salt).digest()[:16]
    ciphertext = subprocess.check_output(
        [openssl, "enc", "-aes-256-cbc", "-nosalt", "-K", key.hex(), "-iv", iv.hex()],
        input=archive,
    )
    return b"Salted__" + salt + ciphertext + b"WR3000K".ljust(32)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--openssl", required=True)
    parser.add_argument("--hook", type=Path, required=True)
    parser.add_argument("--epoch", type=int, required=True)
    args = parser.parse_args()

    archive = stock_archive(args.source.read_bytes(), args.hook.read_bytes(), args.epoch)
    with args.output.open("xb") as output:
        output.write(encrypt(archive, args.openssl))


if __name__ == "__main__":
    main()
