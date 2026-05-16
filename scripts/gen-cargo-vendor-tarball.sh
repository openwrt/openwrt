#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0-only
#
# gen-cargo-vendor-tarball.sh — generate reproducible vendor tarballs for
# Rust packages used in toolchain/.
#
# The generated tarball contains the upstream source together with a vendor/
# directory (from `cargo vendor`) and a .cargo/config.toml that configures
# offline vendored-source mode.  Place the result in dl/ and update PKG_HASH
# in the corresponding Makefile.
#
# IMPORTANT: Run this script AFTER building toolchain/rust so that
# $(STAGING_DIR_HOST)/bin/cargo is used instead of the host-installed cargo.
# Using the OpenWrt-built cargo ensures consistent dependency resolution.
# Set CARGO=/path/to/cargo explicitly to override.
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
#
# Requirements:
#   cargo (prefer OpenWrt-built), curl, GNU tar >=1.28, zstd, sha256sum

set -euo pipefail

TOPDIR="$(cd "$(dirname "$0")/.." && pwd)"
DL_DIR="${DL_DIR:-$TOPDIR/dl}"
TMPDIR="$(mktemp -d)"
trap 'rm -rf "$TMPDIR"' EXIT

die() { echo "ERROR: $*" >&2; exit 1; }

# Resolve cargo: use OpenWrt-built cargo; abort if not found.
# The OpenWrt cargo is installed by toolchain/rust into staging_dir/host/bin/.
# This script is called either by DownloadMethod (after toolchain/rust/compile)
# or by maintainers who must build toolchain/rust first.
# Set CARGO=/path/to/cargo explicitly to override.
if [ -z "${CARGO:-}" ]; then
	OW_CARGO="$(ls "${TOPDIR}"/staging_dir/host/bin/cargo 2>/dev/null || true)"
	if [ -n "$OW_CARGO" ] && "$OW_CARGO" --version &>/dev/null; then
		CARGO="$OW_CARGO"
	else
		die "OpenWrt cargo not found at staging_dir/host/bin/cargo." \
		    "Build toolchain/rust first ('make toolchain/rust/compile')" \
		    "or set CARGO=/path/to/cargo."
	fi
fi

# Create a reproducible .tar.zst with deterministic metadata
make_tarball() {
	local srcdir="$1"   # directory to pack (relative to cwd)
	local outfile="$2"  # output path
	tar \
		--numeric-owner --owner=0 --group=0 \
		--mode=a-s --sort=name \
		--mtime='@0' \
		-c "$srcdir" \
	| zstd -T0 --ultra -20 > "$outfile"
}

case "${1:-}" in

crate)
	[[ $# -ge 3 ]] || die "Usage: $0 crate <name> <version> [options]"
	NAME="$2"
	VERSION="$3"
	shift 3

	# Parse optional flags
	CACHED_SOURCE=""
	OUTPUT=""
	QUIET=0
	while [[ $# -gt 0 ]]; do
		case "$1" in
			--cached-source) CACHED_SOURCE="$2"; shift 2 ;;
			--output)        OUTPUT="$2";         shift 2 ;;
			--quiet)         QUIET=1;             shift   ;;
			--dl-dir)        DL_DIR="$2";         shift 2 ;;
			*) die "Unknown option: $1" ;;
		esac
	done

	VENDOR_FILE="${NAME}-${VERSION}-vendor.tar.zst"
	OUTFILE="${OUTPUT:-${DL_DIR}/${VENDOR_FILE}}"

	if [[ -n "$CACHED_SOURCE" ]]; then
		[[ -f "$CACHED_SOURCE" ]] || die "Cached source not found: $CACHED_SOURCE"
		CRATE_PATH="$CACHED_SOURCE"
	else
		CRATE_FILE="${NAME}-${VERSION}.crate"
		CRATE_PATH="${TMPDIR}/${CRATE_FILE}"
		echo ">>> Downloading ${CRATE_FILE} from crates.io ..."
		curl -fL --progress-bar \
			-o "$CRATE_PATH" \
			"https://static.crates.io/crates/${NAME}/${VERSION}/download"
	fi

	echo ">>> Extracting ..."
	mkdir -p "${TMPDIR}/src"
	# .crate files are tar.gz archives
	tar -xzf "$CRATE_PATH" -C "${TMPDIR}/src"

	SRCDIR="${TMPDIR}/src/${NAME}-${VERSION}"
	[[ -d "$SRCDIR" ]] || die "Expected directory ${NAME}-${VERSION}/ in crate"

	echo ">>> Running cargo vendor ..."
	mkdir -p "${SRCDIR}/.cargo"
	(cd "$SRCDIR" && "$CARGO" vendor vendor > .cargo/config.toml)

	# Enforce offline mode so the build never needs network access
	printf '\n[net]\noffline = true\n' >> "${SRCDIR}/.cargo/config.toml"

	echo ">>> Creating vendor tarball: ${OUTFILE} ..."
	mkdir -p "$(dirname "$OUTFILE")"
	(cd "${TMPDIR}/src" && make_tarball "${NAME}-${VERSION}" "$OUTFILE")

	if [[ $QUIET -eq 0 ]]; then
		echo ""
		echo "Done: ${OUTFILE}"
		echo "SHA-256: $(sha256sum "$OUTFILE" | cut -d' ' -f1)"
		echo ""
		echo "Update toolchain/${NAME}/Makefile:"
		echo "  PKG_HASH:=$(sha256sum "$OUTFILE" | cut -d' ' -f1)"
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

	# Parse optional flags
	CACHED_SOURCE=""
	OUTPUT=""
	QUIET=0
	while [[ $# -gt 0 ]]; do
		case "$1" in
			--cached-source) CACHED_SOURCE="$2"; shift 2 ;;
			--output)        OUTPUT="$2";         shift 2 ;;
			--subdir)        CARGO_SUBDIR="$2";   shift 2 ;;
			--quiet)         QUIET=1;             shift   ;;
			--dl-dir)        DL_DIR="$2";         shift 2 ;;
			*) die "Unknown option: $1" ;;
		esac
	done

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
		ARCHIVE_PATH="${TMPDIR}/archive.tar.gz"
		curl -fL --progress-bar \
			-o "$ARCHIVE_PATH" \
			"https://github.com/${REPO}/archive/refs/tags/${TAG}.tar.gz"
	fi

	echo ">>> Extracting ..."
	mkdir -p "${TMPDIR}/src"
	tar -xzf "$ARCHIVE_PATH" -C "${TMPDIR}/src"

	SRCDIR="${TMPDIR}/src/${EXTRACTED_DIR}"
	[[ -d "$SRCDIR" ]] || \
		die "Expected directory ${EXTRACTED_DIR}/ in archive (got: $(ls "${TMPDIR}/src/"))"

	# Rename extracted dir to pkgname-version if they differ
	if [[ "${EXTRACTED_DIR}" != "${PKGNAME}-${VERSION}" ]]; then
		mv "${SRCDIR}" "${TMPDIR}/src/${PKGNAME}-${VERSION}"
		SRCDIR="${TMPDIR}/src/${PKGNAME}-${VERSION}"
	fi

	CARGO_DIR="${SRCDIR}${CARGO_SUBDIR:+/${CARGO_SUBDIR}}"
	[[ -f "${CARGO_DIR}/Cargo.toml" ]] || die "No Cargo.toml in ${CARGO_DIR}"

	echo ">>> Running cargo vendor in ${CARGO_DIR} ..."
	mkdir -p "${CARGO_DIR}/.cargo"
	(cd "$CARGO_DIR" && "$CARGO" vendor vendor > .cargo/config.toml)

	printf '\n[net]\noffline = true\n' >> "${CARGO_DIR}/.cargo/config.toml"

	echo ">>> Creating vendor tarball: ${OUTFILE} ..."
	mkdir -p "$(dirname "$OUTFILE")"
	(cd "${TMPDIR}/src" && make_tarball "${PKGNAME}-${VERSION}" "$OUTFILE")

	if [[ $QUIET -eq 0 ]]; then
		echo ""
		echo "Done: ${OUTFILE}"
		echo "SHA-256: $(sha256sum "$OUTFILE" | cut -d' ' -f1)"
		echo ""
		echo "Update toolchain/${PKGNAME}/Makefile:"
		echo "  PKG_HASH:=$(sha256sum "$OUTFILE" | cut -d' ' -f1)"
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

Run AFTER building toolchain/rust so the OpenWrt-built cargo is used.
Set CARGO=/path/to/cargo to override.

Examples:
  scripts/gen-cargo-vendor-tarball.sh crate bindgen-cli 0.71.1
  scripts/gen-cargo-vendor-tarball.sh crate cbindgen 0.28.0
  scripts/gen-cargo-vendor-tarball.sh github corrosion-rs/corrosion v0.5.1 corrosion 0.5.1 generator
EOF
	exit 1
	;;
esac
