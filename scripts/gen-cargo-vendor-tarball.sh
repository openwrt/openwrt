#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0-only
#
# gen-cargo-vendor-tarball.sh: generate reproducible vendor tarballs for
# Rust host tools in tools/.
#
# The generated tarball contains the upstream source together with a vendor/
# directory (from `cargo vendor`) and a .cargo/config.toml that configures
# offline vendored-source mode.  Place the result in dl/ and update PKG_HASH
# in the corresponding Makefile.
#
# Any cargo produces the same tarball. Set CARGO=/path/to/cargo to choose one.
#
# Usage (standalone / maintainer):
#
#   Crate from crates.io:
#     scripts/gen-cargo-vendor-tarball.sh crate <name> <version>
#
#   GitHub archive with a Rust sub-project:
#     scripts/gen-cargo-vendor-tarball.sh github <owner/repo> <tag> \
#         <pkgname> <version> [<cargo-subdir>]
#
# Usage (from DownloadMethod, with pre-downloaded source):
#
#   scripts/gen-cargo-vendor-tarball.sh crate <name> <version> \
#       --cached-source <path> --output <path> [--quiet]
#
#   scripts/gen-cargo-vendor-tarball.sh github <owner/repo> <tag> \
#       <pkgname> <version> \
#       --cached-source <path> --output <path> \
#       [--subdir <cargo-subdir>] [--quiet]
#
# Options:
#   --cached-source PATH  Use pre-downloaded source file; skip network download.
#   --output PATH         Write vendor tarball to PATH instead of DL_DIR/name-version-vendor.tar.zst.
#   --subdir DIR          Subdirectory within archive that contains Cargo.toml (github mode only).
#   --quiet               Suppress "Update Makefile:" instructions; for DownloadMethod use.
#   --dl-dir DIR          Override DL_DIR (default: $TOPDIR/dl).
#   --hash HASH           Verify the generated archive before publishing it.
#
# Requirements:
#   cargo, curl, GNU tar, python3, OpenWrt-built zstd and mkhash

set -euo pipefail

TOPDIR="$(cd "$(dirname "$0")/.." && pwd)"
DL_DIR="${DL_DIR:-$TOPDIR/dl}"
WORK_DIR="$(mktemp -d)"
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

	if [ -z "${CARGO:-}" ]; then
		for candidate in "${TOPDIR}/staging_dir/host/bin/cargo" "$(command -v cargo || true)"; do
			if [ -n "$candidate" ] && "$candidate" --version &>/dev/null; then
				CARGO="$candidate"
				break
			fi
		done
	fi
	[ -z "${CARGO:-}" ] || return 0
	[ -n "${RUST_CARGO_URL:-}" ] && [ -n "${RUST_CARGO_HASH:-}" ] || \
		die "No cargo found. Install cargo or set CARGO=/path/to/cargo."
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
	QUIET=0
	while [[ $# -gt 0 ]]; do
		case "$1" in
			--quiet) QUIET=1; shift; continue ;;
			--cached-source|--output|--subdir|--dl-dir|--hash)
				[[ $# -ge 2 ]] || die "Missing argument for $1" ;;
			*) die "Unknown option: $1" ;;
		esac
		case "$1" in
			--cached-source) CACHED_SOURCE="$2" ;;
			--output) OUTPUT="$2" ;;
			--subdir) CARGO_SUBDIR="$2" ;;
			--dl-dir) DL_DIR="$2" ;;
			--hash) EXPECTED_HASH="$2" ;;
		esac
		shift 2
	done
	[[ "$DL_DIR" = /* ]] || DL_DIR="${PWD}/${DL_DIR}"
	[[ -z "$OUTPUT" || "$OUTPUT" = /* ]] || OUTPUT="${PWD}/${OUTPUT}"
	[[ -z "$CACHED_SOURCE" || "$CACHED_SOURCE" = /* ]] || \
		CACHED_SOURCE="${PWD}/${CACHED_SOURCE}"
	tools_prepare
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
	mv -f "$OUTPUT_TMP" "$outfile"
	OUTPUT_TMP=""
}

case "${1:-}" in

crate)
	[[ $# -ge 3 ]] || die "Usage: $0 crate <name> <version> [options]"
	NAME="$2"
	VERSION="$3"
	shift 3

	options_parse "$@"

	VENDOR_FILE="${NAME}-${VERSION}-vendor.tar.zst"
	OUTFILE="${OUTPUT:-${DL_DIR}/${VENDOR_FILE}}"

	if [[ -n "$CACHED_SOURCE" ]]; then
		[[ -f "$CACHED_SOURCE" ]] || die "Cached source not found: $CACHED_SOURCE"
		CRATE_PATH="$CACHED_SOURCE"
	else
		CRATE_FILE="${NAME}-${VERSION}.crate"
		CRATE_PATH="${WORK_DIR}/${CRATE_FILE}"
		echo ">>> Downloading ${CRATE_FILE} from crates.io ..."
		curl -fL --progress-bar \
			-o "$CRATE_PATH" \
			"https://static.crates.io/crates/${NAME}/${VERSION}/download"
	fi

	echo ">>> Extracting ..."
	mkdir -p "${WORK_DIR}/src"
	# .crate files are tar.gz archives
	"$TAR" -xzf "$CRATE_PATH" -C "${WORK_DIR}/src"

	SRCDIR="${WORK_DIR}/src/${NAME}-${VERSION}"
	[[ -d "$SRCDIR" ]] || die "Expected directory ${NAME}-${VERSION}/ in crate"

	echo ">>> Running cargo vendor ..."
	mkdir -p "${SRCDIR}/.cargo"
	(cd "$SRCDIR" && "$CARGO" vendor --locked vendor > .cargo/config.toml)
	normalise_vendor "${SRCDIR}/vendor"

	# Enforce offline mode so the build never needs network access
	printf '\n[net]\noffline = true\n' >> "${SRCDIR}/.cargo/config.toml"

	echo ">>> Creating vendor tarball: ${OUTFILE} ..."
	make_tarball "${NAME}-${VERSION}" "$OUTFILE"

	if [[ $QUIET -eq 0 ]]; then
		echo ""
		echo "Done: ${OUTFILE}"
		echo "SHA-256: $("$MKHASH" sha256 "$OUTFILE")"
		echo ""
		echo "Update tools/${NAME}/Makefile:"
		echo "  PKG_HASH:=$("$MKHASH" sha256 "$OUTFILE")"
	fi
	;;

github)
	[[ $# -ge 5 ]] || die "Usage: $0 github <owner/repo> <tag> <pkgname> <version> [<cargo-subdir>] [options]"
	REPO="$2"        # e.g. corrosion-rs/corrosion
	TAG="$3"         # e.g. v0.5.1
	PKGNAME="$4"     # e.g. corrosion
	VERSION="$5"     # e.g. 0.5.1
	shift 5

	# Accept legacy 6th positional arg as cargo subdir (before flags)
	CARGO_SUBDIR=""
	if [[ $# -gt 0 && "${1:-}" != --* ]]; then
		CARGO_SUBDIR="$1"; shift
	fi

	options_parse "$@"

	VENDOR_FILE="${PKGNAME}-${VERSION}-vendor.tar.zst"
	OUTFILE="${OUTPUT:-${DL_DIR}/${VENDOR_FILE}}"

	# GitHub archive extracts to repo-version/ (strips leading 'v' from tag)
	REPONAME="${REPO##*/}"
	EXTRACTED_DIR="${REPONAME}-${VERSION#v}"

	if [[ -n "$CACHED_SOURCE" ]]; then
		[[ -f "$CACHED_SOURCE" ]] || die "Cached source not found: $CACHED_SOURCE"
		ARCHIVE_PATH="$CACHED_SOURCE"
	else
		echo ">>> Downloading GitHub archive ${REPO}@${TAG} ..."
		ARCHIVE_PATH="${WORK_DIR}/archive.tar.gz"
		curl -fL --progress-bar \
			-o "$ARCHIVE_PATH" \
			"https://github.com/${REPO}/archive/refs/tags/${TAG}.tar.gz"
	fi

	echo ">>> Extracting ..."
	mkdir -p "${WORK_DIR}/src"
	"$TAR" -xzf "$ARCHIVE_PATH" -C "${WORK_DIR}/src"

	SRCDIR="${WORK_DIR}/src/${EXTRACTED_DIR}"
	[[ -d "$SRCDIR" ]] || \
		die "Expected directory ${EXTRACTED_DIR}/ in archive"

	# Rename extracted dir to pkgname-version if they differ
	if [[ "${EXTRACTED_DIR}" != "${PKGNAME}-${VERSION}" ]]; then
		mv "${SRCDIR}" "${WORK_DIR}/src/${PKGNAME}-${VERSION}"
		SRCDIR="${WORK_DIR}/src/${PKGNAME}-${VERSION}"
	fi

	CARGO_DIR="${SRCDIR}${CARGO_SUBDIR:+/${CARGO_SUBDIR}}"
	[[ -f "${CARGO_DIR}/Cargo.toml" ]] || die "No Cargo.toml in ${CARGO_DIR}"

	echo ">>> Running cargo vendor in ${CARGO_DIR} ..."
	mkdir -p "${CARGO_DIR}/.cargo"
	(cd "$CARGO_DIR" && "$CARGO" vendor --locked vendor > .cargo/config.toml)
	normalise_vendor "${CARGO_DIR}/vendor"

	printf '\n[net]\noffline = true\n' >> "${CARGO_DIR}/.cargo/config.toml"

	echo ">>> Creating vendor tarball: ${OUTFILE} ..."
	make_tarball "${PKGNAME}-${VERSION}" "$OUTFILE"

	if [[ $QUIET -eq 0 ]]; then
		echo ""
		echo "Done: ${OUTFILE}"
		echo "SHA-256: $("$MKHASH" sha256 "$OUTFILE")"
		echo ""
		echo "Update tools/${PKGNAME}/Makefile:"
		echo "  PKG_HASH:=$("$MKHASH" sha256 "$OUTFILE")"
	fi
	;;

*)
	cat >&2 <<'EOF'
Usage:
  scripts/gen-cargo-vendor-tarball.sh crate <name> <version> [options]
  scripts/gen-cargo-vendor-tarball.sh github <owner/repo> <tag> <pkgname> <version> [<cargo-subdir>] [options]

Options:
  --cached-source PATH  Use pre-downloaded source; skip network fetch.
  --output PATH         Write vendor tarball to PATH instead of DL_DIR/name-version-vendor.tar.zst.
  --subdir DIR          Cargo subdirectory within GitHub archive (github mode only).
  --quiet               Suppress "Update Makefile:" instructions.
  --dl-dir DIR          Override DL_DIR.
  --hash HASH           Verify the generated archive before publishing it.

Any cargo produces the same tarball. Set CARGO=/path/to/cargo to choose one.
EOF
	exit 1
	;;
esac
