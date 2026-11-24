#!/usr/bin/env python3
"""Prepare a V-MS3000K layout v2 rescue bundle on the build host."""
import argparse
import hashlib
import io
import json
from pathlib import Path
import subprocess
import tarfile
import tempfile

LEB = 126_976
FORMAT = b"tenbay,v-ms3000k:ubi1:v2\n"

def digest(data):
    return hashlib.sha256(data).hexdigest()

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("image", type=Path)
parser.add_argument("config_backup", type=Path)
parser.add_argument("output", type=Path)
parser.add_argument("--host-bin", type=Path, required=True)
args = parser.parse_args()

with tempfile.TemporaryDirectory() as directory:
    work = Path(directory)
    metadata = work / "metadata.json"
    subprocess.run([args.host_bin / "fwtool", "-i", metadata, args.image], check=True)
    if "tenbay,v-ms3000k" not in json.loads(metadata.read_text())["supported_devices"]:
        parser.error("image metadata does not identify the V-MS3000K")
    with tarfile.open(args.image, "r:") as archive:
        members = archive.getmembers()
        if [item.name for item in members] != ["format", "kernel", "rootfs", "sha256"] or not all(item.isfile() for item in members):
            parser.error("invalid sysupgrade archive")
        if archive.extractfile("format").read() != FORMAT:
            parser.error("image uses the wrong slot layout")
        payloads = {name: archive.extractfile(name).read() for name in ("kernel", "rootfs")}
        checksums = "".join(f"{digest(data)}  {name}\n" for name, data in payloads.items())
        if archive.extractfile("sha256").read() != checksums.encode():
            parser.error("sysupgrade payload checksum mismatch")

    backup = args.config_backup.read_bytes()
    with tarfile.open(fileobj=io.BytesIO(backup), mode="r:gz") as archive:
        if not any(member.name.lstrip("./") == "etc/config/network" for member in archive):
            parser.error("expected an OpenWrt sysupgrade settings backup with etc/config/network")

    seed = work / "seed"
    seed.mkdir()
    (seed / "upper").mkdir()
    (seed / "work").mkdir()
    seed.chmod(0o755)
    (seed / "upper").chmod(0o755)
    (seed / ".fs_state").symlink_to("2")
    (seed / ".vms-openwrt-overlay").write_bytes(FORMAT)
    (seed / "sysupgrade.tgz").write_bytes(backup)
    (seed / "sysupgrade.tgz").chmod(0o600)
    overlay = work / "overlay.ubifs"
    subprocess.run([args.host_bin / "mkfs.ubifs", "-r", seed, "-m", "2048", "-e", str(LEB),
                    "-c", "225", "-U", "-o", overlay], check=True)
    payloads["overlay.ubifs"] = overlay.read_bytes()

limits = dict(kernel=64 * LEB, rootfs=72 * LEB)
limits["overlay.ubifs"] = 225 * LEB
for name, data in payloads.items():
    if not data or len(data) > limits[name]:
        parser.error(f"{name} exceeds the layout v2 reservation")
args.output.mkdir(mode=0o700)
manifest = {}
for name, data in payloads.items():
    full_hash = (digest(data.ljust(limits[name], b"\xff")) + "\n").encode()
    for filename, contents in ((name, data), (f"{name}.full.sha256", full_hash)):
        (args.output / filename).write_bytes(contents)
        manifest[filename] = digest(contents)
(args.output / "SHA256SUMS").write_text("".join(f"{sha}  {name}\n" for name, sha in manifest.items()))
