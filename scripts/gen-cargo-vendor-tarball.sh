#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0-only
#
# gen-cargo-vendor-tarball.sh: generate reproducible vendor tarballs for
# Rust host tools and packages.
#
# The generated tarball contains the upstream source together with a vendor/
# directory (from `cargo vendor`) and a .cargo/config.toml that configures
# offline vendored-source mode.  Place the result in dl/ and update PKG_HASH
# in the corresponding Makefile.
#
# The meson-wraps mode vendors no crates: it adds the files of the named
# [wrap-file] wraps to subprojects/packagecache instead, which meson uses with
# --wrap-mode=nodownload.
#
# Any cargo produces the same tarball. Set CARGO=/path/to/cargo to choose one.
#
# Usage (standalone / maintainer):
#
#   Crate from crates.io:
#     scripts/gen-cargo-vendor-tarball.sh crate <name> <version>
#
#   GitHub tag archive:
#     scripts/gen-cargo-vendor-tarball.sh github <owner/repo> <tag> \
#         <pkgname> <version> [--subdir <cargo-subdir>]
#
#   Any source archive:
#     scripts/gen-cargo-vendor-tarball.sh archive <pkgname> <version> \
#         --url <url> [--subdir <cargo-subdir>]
#
#   Source archive with meson wraps:
#     scripts/gen-cargo-vendor-tarball.sh meson-wraps <pkgname> <version> \
#         --url <url> --wraps '<pattern> ...'
#
# Usage (from DownloadMethod, with pre-downloaded source): as above, with
#   --cached-source <path> --output <path> [--top-dir <dir>] [--quiet]
# in place of --url.
#
# Options:
#   --cached-source PATH  Use pre-downloaded source file; skip network download.
#   --url URL             Download the source archive from URL (archive and meson-wraps modes).
#   --output PATH         Write vendor tarball to PATH instead of DL_DIR/name-version-vendor.tar.zst.
#   --subdir DIR          Subdirectory within archive that contains Cargo.toml (github and archive modes).
#   --top-dir DIR         Top directory of the vendor tarball, default name-version.
#   --wraps PATTERNS      Space separated wrap names, globs allowed, without .wrap (meson-wraps mode).
#   --quiet               Suppress "Update Makefile:" instructions; for DownloadMethod use.
#   --dl-dir DIR          Override DL_DIR (default: $TOPDIR/dl).
#   --hash HASH           Verify the generated archive before publishing it.
#
# Requirements:
#   curl, GNU tar, python3, OpenWrt-built zstd and mkhash, and the OpenWrt-built
#   cargo unless CARGO is set

set -euo pipefail

TOPDIR="$(cd "$(dirname "$0")/.." && pwd)"
DL_DIR="${DL_DIR:-$TOPDIR/dl}"

for var in $(compgen -e); do
	case "$var" in CARGO_*) unset "$var" ;; esac
done
export CARGO_HOME="${TOPDIR}/staging_dir/hostpkg/cargo"

mkdir -p "${TOPDIR}/tmp"
WORK_DIR="$(mktemp -d "${TOPDIR}/tmp/gen-cargo-vendor.XXXXXX")"
OUTPUT_TMP=""
trap 'rm -rf "$WORK_DIR"; [ -z "$OUTPUT_TMP" ] || rm -f "$OUTPUT_TMP"' EXIT

die() { echo "ERROR: $*" >&2; exit 1; }

TAR=tar

tools_prepare() {
	ZSTD="${TOPDIR}/staging_dir/host/bin/zstd"
	MKHASH="${MKHASH:-${TOPDIR}/staging_dir/host/bin/mkhash}"
	[ -x "$ZSTD" ] || die "OpenWrt zstd not found at $ZSTD"
	[ -x "$MKHASH" ] || die "OpenWrt mkhash not found at $MKHASH"
	export MKHASH
}

cargo_prepare() {
	if [ -z "${CARGO:-}" ] && "${TOPDIR}/staging_dir/host/bin/cargo" --version &>/dev/null; then
		CARGO="${TOPDIR}/staging_dir/host/bin/cargo"
	fi
	[ -z "${CARGO:-}" ] || return 0
	[ -n "${RUST_CARGO_URL:-}" ] && [ -n "${RUST_CARGO_HASH:-}" ] || \
		die "No cargo in staging_dir/host and no RUST_CARGO_URL and RUST_CARGO_HASH. Set CARGO=/path/to/cargo."
	local cargo_file="${RUST_CARGO_URL##*/}"
	local flock="${TOPDIR}/staging_dir/host/bin/flock"
	[ -x "$flock" ] || die "OpenWrt flock not found at $flock"
	mkdir -p "$DL_DIR"
	"$flock" "${DL_DIR}/.${cargo_file}.flock" \
		"${TOPDIR}/scripts/download.pl" "$DL_DIR" "$cargo_file" "$RUST_CARGO_HASH" "" \
		"${RUST_CARGO_URL%/*}"
	mkdir -p "${WORK_DIR}/cargo"
	python3 -c 'import sys, tarfile; tarfile.open(sys.argv[1]).extractall(sys.argv[2])' \
		"${DL_DIR}/${cargo_file}" "${WORK_DIR}/cargo"
	CARGO="$(echo "${WORK_DIR}"/cargo/*/cargo/bin/cargo)"
}

options_parse() {
	CACHED_SOURCE=""
	OUTPUT=""
	EXPECTED_HASH=""
	CARGO_SUBDIR=""
	TOP_DIR=""
	WRAPS=""
	QUIET=0
	while [[ $# -gt 0 ]]; do
		case "$1" in
			--quiet) QUIET=1; shift; continue ;;
			--cached-source|--url|--output|--subdir|--top-dir|--wraps|--dl-dir|--hash)
				[[ $# -ge 2 ]] || die "Missing argument for $1" ;;
			*) die "Unknown option: $1" ;;
		esac
		case "$1" in
			--cached-source) CACHED_SOURCE="$2" ;;
			--url) SOURCE_URL="$2" ;;
			--output) OUTPUT="$2" ;;
			--subdir) CARGO_SUBDIR="$2" ;;
			--top-dir) TOP_DIR="$2" ;;
			--wraps) WRAPS="$2" ;;
			--dl-dir) DL_DIR="$2" ;;
			--hash) EXPECTED_HASH="$2" ;;
		esac
		shift 2
	done
	[[ "$DL_DIR" = /* ]] || DL_DIR="${PWD}/${DL_DIR}"
	[[ -z "$OUTPUT" || "$OUTPUT" = /* ]] || OUTPUT="${PWD}/${OUTPUT}"
	[[ -z "$CACHED_SOURCE" || "$CACHED_SOURCE" = /* ]] || \
		CACHED_SOURCE="${PWD}/${CACHED_SOURCE}"
	TOP_DIR="${TOP_DIR:-${NAME}-${VERSION}}"
	OUTFILE="${OUTPUT:-${DL_DIR}/${NAME}-${VERSION}-vendor.tar.zst}"
}

source_fetch() {
	if [[ -n "$CACHED_SOURCE" ]]; then
		[[ -f "$CACHED_SOURCE" ]] || die "Cached source not found: $CACHED_SOURCE"
		SOURCE_PATH="$CACHED_SOURCE"
		return 0
	fi
	[[ -n "$SOURCE_URL" ]] || die "No source, use --url or --cached-source"
	echo ">>> Downloading ${SOURCE_URL} ..."
	SOURCE_PATH="${WORK_DIR}/source"
	curl -fL --progress-bar -o "$SOURCE_PATH" "$SOURCE_URL"
}

# A .crate holds a single directory named after the crate and its version.
crate_extract() {
	echo ">>> Extracting ..."
	mkdir -p "${WORK_DIR}/src"
	"$TAR" -xf "$SOURCE_PATH" -C "${WORK_DIR}/src"
	[[ -d "${WORK_DIR}/src/${NAME}-${VERSION}" ]] || \
		die "Expected directory ${NAME}-${VERSION}/ in crate"
	[[ "$TOP_DIR" = "${NAME}-${VERSION}" ]] || \
		mv "${WORK_DIR}/src/${NAME}-${VERSION}" "${WORK_DIR}/src/${TOP_DIR}"
}

archive_extract() {
	echo ">>> Extracting ..."
	mkdir -p "${WORK_DIR}/src/${TOP_DIR}"
	"$TAR" -xf "$SOURCE_PATH" -C "${WORK_DIR}/src/${TOP_DIR}" --strip-components=1
}

# Remove the parts of the vendor directory that depend on the cargo version.
normalise_vendor() {
	find "$1" -name .cargo-checksum.json -exec python3 -c '
import json
import sys

for path in sys.argv[1:]:
	with open(path) as f:
		data = json.load(f)
	data.pop("$comment", None)
	with open(path, "w") as f:
		f.write(json.dumps(data, sort_keys=True, separators=(",", ":")))
' {} +
}

# The source configuration of the vendor directory goes after any cargo
# configuration of the project, which it must not replace. Both are in the
# top directory, where cargo finds them from any subdirectory it runs in.
cargo_vendor() {
	local dir="$1"
	local manifest="$2"
	local config="${dir}/.cargo/config.toml"
	[[ -f "${dir}/${manifest}" ]] || die "No ${manifest} in ${dir}"
	echo ">>> Running cargo vendor for ${manifest} in ${dir} ..."
	mkdir -p "${dir}/.cargo"
	[[ ! -s "$config" ]] || printf '\n' >> "$config"
	(cd "$dir" && "$CARGO" vendor --locked --manifest-path "$manifest" vendor >> .cargo/config.toml)
	normalise_vendor "${dir}/vendor"

	# Enforce offline mode so the build never needs network access
	printf '\n[net]\noffline = true\n' >> "$config"
}

wrap_fetch() {
	local cache="$1"
	local wrap="$2"
	local info file hash url
	info="$(python3 -c '
import configparser
import sys

config = configparser.ConfigParser(interpolation=None)
config.read(sys.argv[1])
wrap = config["wrap-file"]
print(wrap["source_filename"], wrap["source_hash"], wrap["source_url"])
' "$wrap")" || die "No [wrap-file] source in ${wrap}"
	read -r file hash url <<< "$info"
	"${TOPDIR}/scripts/download.pl" "$cache" "$file" "$hash" "${url##*/}" "${url%/*}" || \
		die "Download of ${url} for ${wrap##*/} failed"
}

wraps_fetch() {
	local subprojects="$1/subprojects"
	local pattern wrap
	[[ -n "$WRAPS" ]] || die "No wraps given, use --wraps"
	echo ">>> Fetching wrap files into subprojects/packagecache ..."
	mkdir -p "${subprojects}/packagecache"
	for pattern in $WRAPS; do
		for wrap in "${subprojects}/"${pattern}.wrap; do
			[[ -f "$wrap" ]] || die "No wrap matches ${pattern}"
			wrap_fetch "${subprojects}/packagecache" "$wrap"
		done
	done
}

# The archive is written by python3 so that the host tar cannot change it.
make_tarball() {
	local srcdir="$1"
	local outfile="$2"
	local hash
	mkdir -p "$(dirname "$outfile")"
	OUTPUT_TMP="$(mktemp "${outfile}.XXXXXX")"
	python3 -c '
import os
import sys
import tarfile


def add(tar, path):
	info = tar.gettarinfo(path, path)
	info.uid = info.gid = 0
	info.uname = info.gname = ""
	info.mtime = 0
	if info.isdir():
		info.mode = 0o755
	elif info.issym():
		info.mode = 0o777
	else:
		info.mode = 0o755 if info.mode & 0o111 else 0o644
	if info.isreg():
		with open(path, "rb") as f:
			tar.addfile(info, f)
	else:
		tar.addfile(info)
	if info.isdir():
		for name in sorted(os.listdir(path)):
			add(tar, os.path.join(path, name))


os.chdir(sys.argv[2])
with tarfile.open(fileobj=sys.stdout.buffer, mode="w|", format=tarfile.GNU_FORMAT) as tar:
	add(tar, sys.argv[1])
' "$srcdir" "${WORK_DIR}/src" | "$ZSTD" -T0 --ultra -20 > "$OUTPUT_TMP"
	hash="$("$MKHASH" sha256 "$OUTPUT_TMP")"
	[ -z "$EXPECTED_HASH" ] || [ "$EXPECTED_HASH" = skip ] || \
		[ "$hash" = "$EXPECTED_HASH" ] || \
		die "Hash mismatch for ${outfile}: expected ${EXPECTED_HASH}, got ${hash}"
	chmod 0644 "$OUTPUT_TMP"
	mv -f "$OUTPUT_TMP" "$outfile"
	OUTPUT_TMP=""
}

result_print() {
	[[ $QUIET -eq 0 ]] || return 0
	echo ""
	echo "Done: ${OUTFILE}"
	echo "SHA-256: $("$MKHASH" sha256 "$OUTFILE")"
	echo ""
	echo "Update the Makefile of ${NAME}:"
	echo "  PKG_HASH:=$("$MKHASH" sha256 "$OUTFILE")"
}

usage() {
	cat >&2 <<'EOF'
Usage:
  scripts/gen-cargo-vendor-tarball.sh crate <name> <version> [options]
  scripts/gen-cargo-vendor-tarball.sh github <owner/repo> <tag> <pkgname> <version> [options]
  scripts/gen-cargo-vendor-tarball.sh archive <pkgname> <version> [options]
  scripts/gen-cargo-vendor-tarball.sh meson-wraps <pkgname> <version> --wraps '<pattern> ...' [options]

Options:
  --cached-source PATH  Use pre-downloaded source; skip network fetch.
  --url URL             Source archive to download (archive and meson-wraps modes).
  --output PATH         Write vendor tarball to PATH instead of DL_DIR/name-version-vendor.tar.zst.
  --subdir DIR          Cargo subdirectory within the archive (github and archive modes).
  --top-dir DIR         Top directory of the vendor tarball, default name-version.
  --wraps PATTERNS      Wraps to fetch into subprojects/packagecache (meson-wraps mode).
  --quiet               Suppress "Update Makefile:" instructions.
  --dl-dir DIR          Override DL_DIR.
  --hash HASH           Verify the generated archive before publishing it.

Any cargo produces the same tarball. Set CARGO=/path/to/cargo to choose one.
EOF
	exit 1
}

MODE="${1:-}"
SOURCE_URL=""
case "$MODE" in
crate)
	[[ $# -ge 3 ]] || die "Usage: $0 crate <name> <version> [options]"
	NAME="$2"
	VERSION="$3"
	SOURCE_URL="https://static.crates.io/crates/${NAME}/${VERSION}/download"
	shift 3
	;;
github)
	[[ $# -ge 5 ]] || die "Usage: $0 github <owner/repo> <tag> <pkgname> <version> [options]"
	NAME="$4"
	VERSION="$5"
	SOURCE_URL="https://codeload.github.com/$2/tar.gz/$3"
	shift 5
	;;
archive|meson-wraps)
	[[ $# -ge 3 ]] || die "Usage: $0 ${MODE} <pkgname> <version> [options]"
	NAME="$2"
	VERSION="$3"
	shift 3
	;;
*)
	usage
	;;
esac

options_parse "$@"
tools_prepare
[[ "$MODE" = meson-wraps ]] || cargo_prepare
source_fetch

SRCDIR="${WORK_DIR}/src/${TOP_DIR}"
if [[ "$MODE" = crate ]]; then
	crate_extract
else
	archive_extract
fi

if [[ "$MODE" = meson-wraps ]]; then
	wraps_fetch "$SRCDIR"
else
	cargo_vendor "$SRCDIR" "${CARGO_SUBDIR:+${CARGO_SUBDIR}/}Cargo.toml"
fi

echo ">>> Creating vendor tarball: ${OUTFILE} ..."
make_tarball "$TOP_DIR" "$OUTFILE"
result_print
