#!/usr/bin/env python3
#
# Copyright (C) 2006 OpenWrt.org
# Copyright (C) 2016 LEDE project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
"""Download a source file from a list of mirrors and verify its checksum."""

import glob as globmod
import json
import os
import re
import secrets
import shlex
import shutil
import signal
import string
import subprocess
import sys
from pathlib import Path


topdir = Path(os.environ.get("TOPDIR", ""))
scriptdir = Path(sys.argv[0]).parent

argv = sys.argv[1:]
if len(argv) <= 2:
    sys.exit(
        f"Syntax: {sys.argv[0]} <target dir> <filename> <hash> "
        "<url filename> [<mirror> ...]"
    )

raw_target = Path(argv.pop(0)).expanduser()
matches = globmod.glob(str(raw_target))
target = Path(matches[0]) if matches else raw_target

filename = argv.pop(0)
file_hash = argv.pop(0)

dl_path = target / f"{filename}.dl"
hash_path = target / f"{filename}.hash"
final_path = target / filename

url_filename = None
if argv and "://" not in argv[0]:
    url_filename = argv.pop(0)
# remaining argv entries are the requested mirrors

check_certificate = os.environ.get("DOWNLOAD_CHECK_CERTIFICATE") == "y"
custom_tool = os.environ.get("DOWNLOAD_TOOL_CUSTOM")
download_tool = None

if not url_filename:
    url_filename = filename

mirrors = []

# Mirror prefixes resolved through projectsmirrors.json, with the number of
# times each is appended (sourceforge and github redirect to varying mirrors,
# so give them a few more tries).
SIMPLE_MIRRORS = {
    "@SF": 5,
    "@DEBIAN": 1,
    "@APACHE": 1,
    "@GITHUB": 5,
    "@GNU": 1,
    "@SAVANNAH": 1,
    "@GNOME": 1,
}


def localmirrors():
    """Collect mirrors from the localmirrors file, .config and environment."""
    mlist = []

    try:
        with open(scriptdir / "localmirrors", encoding="utf-8") as local_file:
            for line in local_file:
                line = line.rstrip("\n")
                if line:
                    mlist.append(line)
    except OSError:
        pass

    try:
        with open(topdir / ".config", encoding="utf-8") as config_file:
            for line in config_file:
                if m := re.match(r'^CONFIG_LOCALMIRROR="(.+)"', line):
                    mlist.extend(m.group(1).split(";"))
    except OSError:
        pass

    mirror = os.environ.get("DOWNLOAD_MIRROR")
    if mirror:
        mlist.extend(mirror.split(";"))

    return mlist


def projectsmirrors(project, append=""):
    """Append the mirrors listed for project in projectsmirrors.json."""
    path = scriptdir / "projectsmirrors.json"
    try:
        with open(path, encoding="utf-8") as mirror_file:
            mirror = json.load(mirror_file)
    except OSError as e:
        sys.exit(f"Can´t open {path}: {e}")

    for entry in mirror.get(project, []):
        mirrors.append(entry + "/" + (append or ""))


def hash_cmd():
    """Return the hash command for the requested checksum, or None."""
    length = len(file_hash)
    mkhash = os.environ.get("MKHASH", "mkhash")
    if length == 64:
        return [mkhash, "sha256"]
    if length == 32:
        return [mkhash, "md5"]
    return None


def tool_present(tool_name):
    """Return True if tool_name --version starts with tool_name."""
    try:
        result = subprocess.run(
            [tool_name, "--version"],
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            text=True,
            check=False,
        )
    except OSError:
        return False

    first_line = result.stdout.partition("\n")[0]
    return bool(re.match(rf"^{re.escape(tool_name)} ", first_line))


def select_tool():
    """Pick the download tool: custom override, else curl, else wget."""
    global custom_tool
    if custom_tool:
        custom_tool = custom_tool.replace('"', "")
        if custom_tool:
            return custom_tool

    # Try to use curl if available
    if tool_present("curl"):
        return "curl"

    # No tool found, fallback to wget
    return "wget"


def download_cmd(url, dl_filename, additional_mirrors):
    """Build the fetch command. Return (command, use_shell)."""
    global mirrors

    if download_tool == "curl":
        cmd = ["curl", "-f", "--connect-timeout", "5", "--retry", "3", "--location"]
        if not check_certificate:
            cmd.append("--insecure")
        cmd += shlex.split(os.environ.get("CURL_OPTIONS", ""))
        cmd.append(url)
        return cmd, False

    if download_tool == "wget":
        cmd = ["wget", "--tries=3", "--timeout=5", "--output-document=-"]
        if not check_certificate:
            cmd.append("--no-check-certificate")
        cmd += shlex.split(os.environ.get("WGET_OPTIONS", ""))
        cmd.append(url)
        return cmd, False

    if download_tool == "aria2c":
        tmpdir = os.environ.get("TMPDIR", "/tmp")
        additional = " ".join(f"{m}/{dl_filename}" for m in additional_mirrors)
        char_array = string.ascii_lowercase + string.ascii_uppercase + string.digits
        rfn = f"{dl_filename}_" + "".join(secrets.choice(char_array) for _ in range(10))

        mirrors = []

        parts = [
            f"[ -d {tmpdir}/aria2c ] || mkdir {tmpdir}/aria2c;",
            f"touch {tmpdir}/aria2c/{rfn}_spp;",
            "aria2c --stderr -c -x2 -s10 -j10 -k1M",
            url,
            additional,
        ]
        if not check_certificate:
            parts.append("--check-certificate=false")
        parts += [
            f"--server-stat-of={tmpdir}/aria2c/{rfn}_spp",
            f"--server-stat-if={tmpdir}/aria2c/{rfn}_spp",
            "--daemon=false --no-conf",
            f"-d {tmpdir}/aria2c -o {rfn};",
            f"cat {tmpdir}/aria2c/{rfn};",
            f"rm {tmpdir}/aria2c/{rfn} {tmpdir}/aria2c/{rfn}_spp",
        ]
        aria2c_options = os.environ.get("ARIA2C_OPTIONS")
        if aria2c_options:
            parts.append(aria2c_options)
        return " ".join(parts), True

    return f"{download_tool} {url}", True


def gen_hash(input_path, output_path):
    """Hash input_path into output_path. Return True on success."""
    if HASH_CMD is None:
        sys.exit("Cannot find appropriate hash command.")
    with open(input_path, "rb") as inp, open(output_path, "w", encoding="utf-8") as out:
        return subprocess.call(HASH_CMD, stdin=inp, stdout=out) == 0


def read_hash(path):
    """Return the first whitespace-delimited token from the hash file."""
    with open(path, encoding="utf-8") as f:
        data = f.read()
    if not (m := re.match(r"^(\w+)\s*", data)):
        sys.exit("Could not generate file hash")
    return m.group(1)


def cleanup(*_):
    """Remove the partial download and its hash file."""
    for path in (dl_path, hash_path):
        path.unlink(missing_ok=True)


def download_local(mirror):
    """Copy filename out of a file:// cache mirror. Return True on success."""
    if not Path(mirror).is_dir():
        sys.stderr.write(f"Wrong local cache directory -{mirror}-.\n")
        cleanup()
        return False

    target.mkdir(parents=True, exist_ok=True)

    try:
        found = subprocess.check_output(
            ["find", mirror, "-follow", "-name", filename],
            stderr=subprocess.DEVNULL,
            text=True,
        ).splitlines()
    except (OSError, subprocess.CalledProcessError):
        print(f"Failed to search for {filename} in {mirror}")
        return False

    if len(found) > 1:
        print(
            f"{len(found)} or more instances of {filename} in {mirror} found . "
            "Only one instance allowed."
        )
        return False

    if not found:
        print(f"No instances of {filename} found in {mirror}.")
        return False

    link = found[0]
    print(f"Copying {filename} from {link}")
    shutil.copy(link, dl_path)

    if HASH_CMD and not gen_hash(dl_path, hash_path):
        print(f"Failed to generate hash for {filename}")
        return False

    return True


def download_remote(mirror, dl_filename, additional_mirrors):
    """Fetch dl_filename from a network mirror. Return True on success."""
    if "a=snapshot" in mirror:
        cmd, use_shell = download_cmd(mirror, dl_filename, additional_mirrors)
    else:
        cmd, use_shell = download_cmd(
            f"{mirror}/{dl_filename}", dl_filename, additional_mirrors
        )

    printable = cmd if use_shell else " ".join(cmd)
    sys.stderr.write(f"+ {printable}\n")

    try:
        with open(dl_path, "wb") as out:
            rc = subprocess.call(cmd, shell=use_shell, stdout=out)
    except OSError:
        sys.exit("Cannot launch curl or wget.")

    if rc != 0:
        sys.stderr.write("Download failed.\n")
        cleanup()
        return False

    if HASH_CMD and not gen_hash(dl_path, hash_path):
        print(f"Failed to generate hash for {filename}")
        return False

    return True


def download(mirror, dl_filename, additional_mirrors):
    """Download filename from mirror, verify its hash and install it."""
    mirror = re.sub(r"/$", "", mirror)

    if mirror.startswith("file://"):
        if not download_local(mirror[len("file://") :]):
            return
    elif not download_remote(mirror, dl_filename, additional_mirrors):
        return

    if HASH_CMD:
        total = read_hash(hash_path)
        if total != file_hash:
            sys.stderr.write(
                f"Hash of the downloaded file does not match (file: {total}, "
                f"requested: {file_hash}) - deleting download.\n"
            )
            cleanup()
            return

    # shutil.move replaces an existing destination atomically (POSIX rename),
    # so there is no need to unlink final_path first.
    # str() for Python 3.8: shutil.move only accepts path-like objects from 3.9.
    shutil.move(str(dl_path), str(final_path))
    cleanup()


HASH_CMD = hash_cmd()
if not HASH_CMD and file_hash != "skip":
    sys.exit(
        "Cannot find appropriate hash command, ensure the provided hash is "
        "either a MD5 or SHA256 checksum."
    )


def resolve_mirrors():
    """Expand the requested mirror tokens into the global mirror list."""
    for mirror in argv:
        simple = None
        for prefix, times in SIMPLE_MIRRORS.items():
            if m := re.match(rf"^{re.escape(prefix)}/(.+)$", mirror):
                simple = (prefix, times, m.group(1))
                break

        if simple:
            prefix, times, sub = simple
            for _ in range(times):
                projectsmirrors(prefix, sub)
            continue

        if re.match(r"^\@OPENWRT$", mirror):
            # use OpenWrt source server directly
            continue

        if kernel := re.match(r"^\@KERNEL/(.+)$", mirror):
            base = kernel.group(1)
            extra = [base]
            if re.search(r"linux-\d+\.\d+(?:\.\d+)?-rc", filename):
                extra.append(f"{base}/testing")
            elif km := re.search(r"linux-(\d+\.\d+(?:\.\d+)?)", filename):
                extra.append(f"{base}/longterm/v{km.group(1)}")
            for d in extra:
                projectsmirrors("@KERNEL", d)
            continue

        mirrors.append(mirror)

    projectsmirrors("@OPENWRT")


def check_existing():
    """Exit 0 if a valid local copy already exists; die on a mismatch."""
    if not final_path.is_file() or not HASH_CMD:
        return

    if not gen_hash(final_path, hash_path):
        sys.exit(f"Failed to generate hash for {filename}")

    total = read_hash(hash_path)
    cleanup()
    if total == file_hash:
        sys.exit(0)

    # Remove the corrupt cached file so a re-run re-downloads it.
    final_path.unlink(missing_ok=True)
    sys.exit(
        f"Hash of the local file {filename} does not match (file: {total}, "
        f"requested: {file_hash}) - deleting download."
    )


def main():
    """Resolve mirrors and download filename from the first one that works."""
    global mirrors, download_tool

    mirrors = localmirrors()
    resolve_mirrors()
    check_existing()

    download_tool = select_tool()

    mirror = mirrors.pop(0) if mirrors else ""

    # Try snapshot original source last
    if "snapshot" in mirror:
        mirrors.append(mirror)
        mirror = mirrors.pop(0) if mirrors else ""

    while not final_path.is_file():
        if not mirror:
            sys.exit("No more mirrors to try - giving up.")

        download(mirror, url_filename, list(mirrors))
        if not final_path.is_file() and url_filename != filename:
            download(mirror, filename, list(mirrors))

        mirror = mirrors.pop(0) if mirrors else ""


if __name__ == "__main__":
    signal.signal(signal.SIGINT, cleanup)
    main()
