#!/bin/sh
#
# Hermetic GPG signature verification using gpgv
#
# Arguments:
#   $1 - payload file (e.g. source tarball)
#   $2 - signature file
#   $3 - directory containing trusted public keys
#   $4 - space-separated list of valid key fingerprints (PKG_VALIDPGPKEYS)
#
# Environment:
#   GPGV - path to gpgv binary

set -eu

# Helper function to check if a command is executable
check_executable() {
  cmd="$1"
  if [ ! -x "$cmd" ] && ! command -v "$cmd" >/dev/null 2>&1; then
    echo " >>> ERROR: required executable not found or not executable: $cmd" >&2
    exit 1
  fi
}

if [ "$#" -ne 4 ]; then
  echo "Usage: $0 <payload> <signature> <keydir> <valid fingerprints>" >&2
  exit 2
fi

PAYLOAD="$1"
SIGFILE="$2"
KEYDIR="$3"
VALIDKEYS="$4"

if [ -z "${GPGV:-}" ]; then
  echo " >>> ERROR: GPGV not set" >&2
  exit 1
fi

# Verify GPGV is executable
check_executable "$GPGV"

if [ ! -f "$PAYLOAD" ]; then
  echo " >>> ERROR: payload file not found: $PAYLOAD" >&2
  exit 1
fi

if [ ! -f "$SIGFILE" ]; then
  echo " >>> ERROR: signature file not found: $SIGFILE" >&2
  exit 1
fi

if [ ! -d "$KEYDIR" ]; then
  echo " >>> ERROR: GPG key directory not found: $KEYDIR" >&2
  exit 1
fi

if [ -z "$VALIDKEYS" ]; then
  echo " >>> ERROR: no valid key fingerprints provided" >&2
  exit 1
fi

# Enforce strict fingerprint format: exactly 40 chars, uppercase hex only
for fpr in $VALIDKEYS; do
  if ! expr "$fpr" : '[0-9A-F]\{40\}$' >/dev/null; then
    echo " >>> ERROR: invalid fingerprint format in Makefile (must be 40 uppercase hex chars, no whitespace): $fpr" >&2
    exit 1
  fi
done

# Detect compression: if the signature matches the compressed filename extension,
# verify the compressed file directly; otherwise decompress to a temporary file
# so gpgv sees the exact tarball content.
VERIFY_FILE="$PAYLOAD"
DECOMPRESS_NEEDED=0
DECOMPRESS_CMD=""

case "$PAYLOAD" in
  *.tar.gz|*.tgz)
    case "$SIGFILE" in
      *.tar.gz.sig|*.tar.gz.asc|*.tgz.sig|*.tgz.asc) ;;
      *) DECOMPRESS_CMD="gzip -dc"; DECOMPRESS_NEEDED=1 ;;
    esac ;;
  *.tar.xz|*.txz)
    case "$SIGFILE" in
      *.tar.xz.sig|*.tar.xz.asc|*.txz.sig|*.txz.asc) ;;
      *) DECOMPRESS_CMD="xz -dc"; DECOMPRESS_NEEDED=1 ;;
    esac ;;
  *.tar.bz2|*.tbz2)
    case "$SIGFILE" in
      *.tar.bz2.sig|*.tar.bz2.asc|*.tbz2.sig|*.tbz2.asc) ;;
      *) DECOMPRESS_CMD="bzip2 -dc"; DECOMPRESS_NEEDED=1 ;;
    esac ;;
  *.tar.zst)
    case "$SIGFILE" in
      *.tar.zst.sig|*.tar.zst.asc) ;;
      *) DECOMPRESS_CMD="zstd -dc"; DECOMPRESS_NEEDED=1 ;;
    esac ;;
esac

# Create a hermetic workspace for decompression, keyring assembly, and gpgv output.
# All sensitive material stays inside this directory and is removed automatically.
TMPDIR="$(mktemp -d)"
cleanup() { rm -rf "$TMPDIR"; }
trap cleanup EXIT INT TERM

if [ "$DECOMPRESS_NEEDED" -eq 1 ]; then
  # Extract the decompression utility name and verify it's available
  DECOMPRESS_UTIL="$(echo "$DECOMPRESS_CMD" | cut -d' ' -f1)"
  check_executable "$DECOMPRESS_UTIL"

  echo "Decompressing payload for signature verification..."
  VERIFY_FILE="$TMPDIR/payload.tar"
  $DECOMPRESS_CMD "$PAYLOAD" >"$VERIFY_FILE" || {
    echo " >>> ERROR: failed to decompress payload" >&2
      exit 1
    }
fi

# Lock down permissions for keyring material
umask 077

KEYRING="$TMPDIR/keyring.gpg"
: >"$KEYRING"

# Assemble a temporary keyring by concatenating the required ASCII-armored keys.
# gpgv treats this as a flat keyring; no trustdb or external key sources are used.
for fpr in $VALIDKEYS; do
  keyfile="$KEYDIR/$fpr.asc"

  if [ ! -f "$keyfile" ]; then
    echo " >>> ERROR: required GPG key not found: $keyfile" >&2
    exit 1
  fi

  cat "$keyfile" >>"$KEYRING" || {
    echo " >>> ERROR: failed to read key file: $keyfile" >&2
      exit 1
    }
done

# Capture gpgv output so success and failure messages are cleanly separated.
# On failure, the captured output is shown before the explicit error message.
GPGV_OUT="$TMPDIR/gpgv.out"
if ! "$GPGV" --keyring "$KEYRING" "$SIGFILE" "$VERIFY_FILE" >"$GPGV_OUT" 2>&1; then
  cat "$GPGV_OUT"
  echo " >>> ERROR: GPG signature verification failed" >&2
  exit 1
fi

cat "$GPGV_OUT"
exit 0

