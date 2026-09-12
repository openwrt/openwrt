#!/usr/bin/env python3
#
# Copyright (C) 2006 OpenWrt.org
# Copyright (C) 2016 LEDE project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
"""Download a source file from a list of mirrors and verify its checksum."""

import hashlib
import json
import os
import re
import shlex
import shutil
import subprocess
import sys
import uuid
from pathlib import Path


topdir = Path(os.environ.get("TOPDIR", ""))
scriptdir = Path(sys.argv[0]).parent

argv = sys.argv[1:]
if len(argv) <= 2:
    sys.exit(
        f"Syntax: {sys.argv[0]} <target dir> <filename> <hash> "
        "<url filename> [<mirror> ...]"
    )

target = Path(argv.pop(0)).expanduser()

filename = argv.pop(0)
file_hash = argv.pop(0)

dl_path = target / f"{filename}.dl"
final_path = target / filename

url_filename = None
# The url-filename override is the 4th positional argument. It is only
# present when it neither looks like a full URL (contains "://") nor like a
# mirror token (starts with "@"), so that tokens such as @SF/project are never
# mistaken for the override and silently dropped from the mirror list.
if argv and "://" not in argv[0] and not argv[0].startswith("@"):
    url_filename = argv.pop(0)
# remaining argv entries are the requested mirrors

check_certificate = os.environ.get("DOWNLOAD_CHECK_CERTIFICATE") == "y"
if not check_certificate:
    sys.stderr.write(
        "WARNING: DOWNLOAD_CHECK_CERTIFICATE is not set; TLS verification is "
        "disabled for all downloads.\n"
    )
custom_tool = os.environ.get("DOWNLOAD_TOOL_CUSTOM")

if not url_filename:
    url_filename = filename

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
    """Return the mirrors listed for project in projectsmirrors.json."""
    path = scriptdir / "projectsmirrors.json"
    try:
        with open(path, encoding="utf-8") as mirror_file:
            mirror = json.load(mirror_file)
    except OSError as e:
        sys.exit(f"Can't open {path}: {e}")

    return [entry + "/" + (append or "") for entry in mirror.get(project, [])]


def hash_algorithm():
    """Return the hash algorithm for file_hash, or None to skip verification.

    Only SHA-256 (64 hex chars) and MD5 (32 hex chars) are supported. The
    literal "skip" disables verification; any other value is rejected so a
    malformed or truncated hash aborts the build instead of being silently
    accepted.
    """
    if file_hash == "skip":
        return None
    if len(file_hash) == 64:
        return "sha256"
    if len(file_hash) == 32:
        return "md5"
    sys.exit(
        "Cannot find appropriate hash command, ensure the provided hash is "
        "either a MD5 or SHA256 checksum."
    )


def compute_hash(input_path, algo):
    """Hash input_path with algo; return the hex digest."""
    h = hashlib.new(algo)
    with open(input_path, "rb") as inp:
        for chunk in iter(lambda: inp.read(65536), b""):
            h.update(chunk)
    return h.hexdigest()


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
    # GNU tools print a "GNU <Tool> ..." banner (e.g. "GNU Wget 1.25.0"), so
    # drop a leading "GNU " and match case-insensitively (the tool name is
    # capitalized in the banner).
    first_line = re.sub(r"^GNU\s+", "", first_line, flags=re.IGNORECASE)
    # Accept the tool name followed by whitespace or a colon (busybox-style
    # "tool: ..." banners). aria2c's banner is "aria2 version", so it is
    # matched against the name without the trailing "c".
    names = {tool_name}
    if tool_name == "aria2c":
        # aria2c's --version prints "aria2 version 1.x", so also accept the
        # name without the trailing "c". Deliberate; do not "simplify" away.
        names.add("aria2")
    return any(
        re.match(rf"^{re.escape(name)}(?:\s|:)", first_line, flags=re.IGNORECASE)
        for name in names
    )


# Auto-selection preference: aria2c is preferred because it downloads from
# all mirrors in parallel; curl and wget are fallbacks.
TOOL_PREFERENCE = ("aria2c", "curl", "wget")


def select_tool(requested_tool):
    """Pick the download tool: custom override, else the preferred autodetect."""
    if requested_tool:
        requested_tool = requested_tool.replace('"', "")
    if requested_tool:
        if requested_tool in TOOL_PREFERENCE:
            # Known tool: require it to actually be usable.
            if tool_present(requested_tool):
                return requested_tool
            sys.exit(f"Requested download tool '{requested_tool}' is not available.")
        elif shutil.which(shlex.split(requested_tool)[0]):
            # Arbitrary custom command: accept it as-is if the binary at the
            # start resolves on PATH. The full string (with its own arguments)
            # is later executed via the shell, so arguments are supported.
            return requested_tool
        else:
            sys.exit(f"Requested download tool '{requested_tool}' is not available.")

    for tool in TOOL_PREFERENCE:
        if tool_present(tool):
            return tool

    return "wget"


def download_cmd(download_tool, url, dl_filename, additional_mirrors=()):
    """Build the fetch command. Return (command, use_shell)."""

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
        tmp = shlex.quote(tmpdir)
        additional = " ".join(
            shlex.quote(f"{m}/{dl_filename}") for m in additional_mirrors
        )
        rfn = f"{dl_filename}_{uuid.uuid4().hex}"

        parts = [
            f"[ -d {tmp}/aria2c ] || mkdir {tmp}/aria2c;",
            f"touch {tmp}/aria2c/{rfn}_spp;",
            "aria2c --stderr -c -x2 -s10 -j10 -k1M --max-tries=3",
            shlex.quote(url),
            additional,
        ]
        if not check_certificate:
            parts.append("--check-certificate=false")
        parts += [
            f"--server-stat-of={tmp}/aria2c/{rfn}_spp",
            f"--server-stat-if={tmp}/aria2c/{rfn}_spp",
        ]
        aria2c_options = os.environ.get("ARIA2C_OPTIONS")
        if aria2c_options:
            parts.append(aria2c_options)
        parts += [
            "--daemon=false --no-conf",
            f"-d {tmp}/aria2c -o {shlex.quote(rfn)};",
            f"cat {tmp}/aria2c/{rfn};",
            f"RC=$?; rm -f {tmp}/aria2c/{rfn} {tmp}/aria2c/{rfn}_spp; exit $RC",
        ]
        return " ".join(parts), True

    # Custom/unknown tool: invoked verbatim via the shell. Each token of the
    # tool string is quoted individually so a DOWNLOAD_TOOL_CUSTOM value with
    # its own arguments (e.g. "mytool --flag") is preserved correctly, and the
    # URL is appended as the final argument.
    cmd = " ".join(shlex.quote(t) for t in shlex.split(download_tool))
    return f"{cmd} {shlex.quote(url)}", True


def cleanup():
    """Remove the partial download."""
    dl_path.unlink(missing_ok=True)


def verify_hash(label="downloaded"):
    """Hash dl_path, compare against file_hash.

    Returns True when the digest matches. On any mismatch it reports the
    mismatch and cleans up the partial file, returning False.
    """
    total = compute_hash(dl_path, hash_algorithm())
    if total != file_hash:
        sys.stderr.write(
            f"Hash of the {label} file does not match (file: {total}, "
            f"requested: {file_hash}) - deleting download.\n"
        )
        cleanup()
        return False

    return True


def download_local(mirror):
    """Copy filename out of a file:// cache mirror. Return True on success."""
    if not Path(mirror).is_dir():
        sys.stderr.write(f"Wrong local cache directory {mirror}.\n")
        return False

    target.mkdir(parents=True, exist_ok=True)

    found = sorted(p for p in Path(mirror).rglob(filename) if p.is_file())
    if len(found) > 1:
        sys.stderr.write(
            f"{len(found)} or more instances of {filename} in {mirror} found . "
            "Only one instance allowed.\n"
        )
        return False

    if not found:
        sys.stderr.write(f"No instances of {filename} found in {mirror}.\n")
        return False

    link = found[0]
    sys.stderr.write(f"Copying {filename} from {link}\n")
    shutil.copy(link, dl_path)
    return True


def download_remote(mirror, dl_filename, download_tool, additional_mirrors=()):
    """Fetch dl_filename from a network mirror. Return True on success."""
    # Only per-repo gitweb snapshot URLs (a=snapshot) are self-contained here;
    # the shared OpenWrt /snapshots/ dirs handled by is_snapshot() for mirror
    # ordering are normal directory mirrors and must keep their additional
    # mirrors, so they deliberately do not match this branch.
    if "a=snapshot" in mirror:
        # Snapshot URLs are self-contained, so additional mirrors are not
        # added (aria2c would only need the single URL).
        cmd, use_shell = download_cmd(download_tool, mirror, dl_filename)
    else:
        cmd, use_shell = download_cmd(
            download_tool, f"{mirror}/{dl_filename}", dl_filename, additional_mirrors
        )

    printable = cmd if use_shell else " ".join(cmd)
    sys.stderr.write(f"+ {printable}\n")

    try:
        with open(dl_path, "wb") as out:
            rc = subprocess.call(cmd, shell=use_shell, stdout=out)
    except OSError:
        sys.exit(f"Cannot launch {download_tool}.")

    if rc != 0:
        sys.stderr.write("Download failed.\n")
        cleanup()
        return False

    return True


def download(mirror, dl_filename, download_tool, additional_mirrors=()):
    """Download filename from mirror, verify its hash and install it."""
    if mirror.startswith("file://"):
        if not download_local(mirror[len("file://") :]):
            return
    elif not download_remote(mirror, dl_filename, download_tool, additional_mirrors):
        return

    if hash_algorithm() is not None and not verify_hash():
        return

    # shutil.move replaces an existing destination atomically (POSIX rename),
    # so there is no need to unlink final_path first.
    # str() for Python 3.8: shutil.move only accepts path-like objects from 3.9.
    shutil.move(str(dl_path), str(final_path))
    cleanup()


def is_snapshot(mirror):
    """Return True for snapshot original-source mirrors.

    These are tried last as a fallback. Matches gitweb snapshot URLs
    (?a=snapshot) and OpenWrt snapshot paths (/snapshots/), without
    false-matching an unrelated path that merely contains "snapshot".
    """
    lowered = mirror.lower()
    return "a=snapshot" in lowered or "/snapshots/" in lowered


def resolve_mirrors():
    """Expand the requested mirror tokens into the mirror list."""
    result = []
    for mirror in argv:
        simple = None
        for prefix, times in SIMPLE_MIRRORS.items():
            if m := re.match(rf"^{re.escape(prefix)}/(.+)$", mirror):
                simple = (prefix, times, m.group(1))
                break

        if simple:
            prefix, times, sub = simple
            for _ in range(times):
                result.extend(projectsmirrors(prefix, sub))
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
                result.extend(projectsmirrors("@KERNEL", d))
            continue

        result.append(mirror)

    result.extend(projectsmirrors("@OPENWRT"))
    return result


def check_existing():
    """Exit 0 if a valid local copy already exists; clean up a mismatch.

    A corrupt cached file is removed so that the download is retried from the
    mirrors instead of hard-failing the build.
    """
    if not final_path.is_file():
        return
    if hash_algorithm() is None:
        # Verification is explicitly skipped: there is no way to validate the
        # existing file, so treat a present copy as already done and avoid
        # re-downloading it on every invocation.
        sys.exit(0)

    total = compute_hash(final_path, hash_algorithm())
    if total == file_hash:
        sys.exit(0)

    # Remove the corrupt cached file so it is re-downloaded from a mirror.
    sys.stderr.write(
        f"Hash of the local file {filename} does not match (file: {total}, "
        f"requested: {file_hash}) - deleting download.\n"
    )
    final_path.unlink(missing_ok=True)


def main():
    """Resolve mirrors and download filename from the first one that works.

    Returns the process exit code.
    """
    try:
        # Normalize trailing slashes once here so URLs built later never get a
        # doubled "//" regardless of how a mirror was supplied.
        mirrors = [m.rstrip("/") for m in localmirrors() + resolve_mirrors()]
        check_existing()

        download_tool = select_tool(custom_tool)

        if not mirrors:
            sys.exit("No more mirrors to try - giving up.")

        # Try snapshot original sources last, as a fallback.
        non_snapshot = [m for m in mirrors if not is_snapshot(m)]
        snapshot = [m for m in mirrors if is_snapshot(m)]
        mirrors = non_snapshot + snapshot

        while not final_path.is_file() and mirrors:
            mirror = mirrors.pop(0)

            download(mirror, url_filename, download_tool, list(mirrors))
            if not final_path.is_file() and url_filename != filename:
                download(mirror, filename, download_tool, list(mirrors))

        if not final_path.is_file():
            sys.exit("No more mirrors to try - giving up.")
    finally:
        cleanup()

    return 0


if __name__ == "__main__":
    sys.exit(main())
