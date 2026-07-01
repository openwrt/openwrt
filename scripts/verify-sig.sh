#!/bin/sh
#
# Hermetic GPG signature verification using gpgv
#
# Arguments:
#   $1 - payload file (e.g. source tarball)
#   $2 - detached signature file
#   $3 - directory containing trusted public keys, one <fingerprint>.asc each
#   $4 - space-separated list of valid key fingerprints (VALIDPGPKEYS)
#
# Environment:
#   GPGV - preferred gpgv binary, falls back to gpgv in PATH
#   GPG  - preferred gpg binary, falls back to gpg in PATH

set -eu

error() {
  echo " >>> ERROR: $1" >&2
  exit 1
}

# Prefer $1 (e.g. the staging dir binary) if executable, otherwise fall
# back to $2 from PATH.
resolve_tool() {
  if [ -n "$1" ] && [ -x "$1" ]; then
    echo "$1"
  elif command -v "$2" >/dev/null 2>&1; then
    command -v "$2"
  else
    echo " >>> ERROR: $2 not found. Install GnuPG on the build host" \
      "or disable CONFIG_DOWNLOAD_VERIFY_SIGNATURES." >&2
    return 1
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

GPGV="$(resolve_tool "${GPGV:-}" gpgv)"
GPG="$(resolve_tool "${GPG:-}" gpg)"

[ -f "$PAYLOAD" ] || error "payload file not found: $PAYLOAD"
[ -f "$SIGFILE" ] || error "signature file not found: $SIGFILE"
[ -d "$KEYDIR" ] || error "GPG key directory not found: $KEYDIR"
[ -n "$VALIDKEYS" ] || error "no valid key fingerprints provided"

# Enforce strict fingerprint format: exactly 40 chars, uppercase hex only
for fpr in $VALIDKEYS; do
  if ! expr "$fpr" : '[0-9A-F]\{40\}$' >/dev/null; then
    error "invalid fingerprint format (must be 40 uppercase hex chars, no whitespace): $fpr"
  fi
done

# Detect compression: if the signature matches the compressed filename extension,
# verify the compressed file directly; otherwise decompress to a temporary file
# so gpgv sees the exact tarball content.
VERIFY_FILE="$PAYLOAD"
DECOMPRESS_CMD=""

case "$PAYLOAD" in
  *.tar.gz|*.tgz)
    case "$SIGFILE" in
      *.tar.gz.sig|*.tar.gz.asc|*.tgz.sig|*.tgz.asc) ;;
      *) DECOMPRESS_CMD="gzip -dc" ;;
    esac ;;
  *.tar.xz|*.txz)
    case "$SIGFILE" in
      *.tar.xz.sig|*.tar.xz.asc|*.txz.sig|*.txz.asc) ;;
      *) DECOMPRESS_CMD="xz -dc" ;;
    esac ;;
  *.tar.bz2|*.tbz2)
    case "$SIGFILE" in
      *.tar.bz2.sig|*.tar.bz2.asc|*.tbz2.sig|*.tbz2.asc) ;;
      *) DECOMPRESS_CMD="bzip2 -dc" ;;
    esac ;;
  *.tar.zst)
    case "$SIGFILE" in
      *.tar.zst.sig|*.tar.zst.asc) ;;
      *) DECOMPRESS_CMD="zstd -dc" ;;
    esac ;;
esac

# Create a hermetic workspace for decompression, keyring assembly, and gpgv
# output. All key material stays inside this directory (including the GnuPG
# home, so the user's ~/.gnupg is never touched) and is removed automatically.
umask 077
WORKDIR="$(mktemp -d)"
cleanup() { rm -rf "$WORKDIR"; }
trap cleanup EXIT INT TERM

export GNUPGHOME="$WORKDIR/gnupghome"
mkdir "$GNUPGHOME"

if [ -n "$DECOMPRESS_CMD" ]; then
  DECOMPRESS_UTIL="${DECOMPRESS_CMD%% *}"
  command -v "$DECOMPRESS_UTIL" >/dev/null 2>&1 || \
    error "required decompression utility not found: $DECOMPRESS_UTIL"

  echo "Decompressing payload for signature verification..."
  VERIFY_FILE="$WORKDIR/payload.tar"
  $DECOMPRESS_CMD "$PAYLOAD" >"$VERIFY_FILE" || \
    error "failed to decompress payload"
fi

KEYRING="$WORKDIR/keyring.gpg"
: >"$KEYRING"

# Assemble a temporary keyring from the required keys. gpgv treats this as
# a flat keyring; no trustdb or external key sources are used. Each key file
# must belong to a single identity (exactly one primary key), and the pinned
# fingerprint must be that identity's primary key or one of its subkeys
# (signing subkeys are a normal, common place to pin), so a mislabeled or
# swapped-in key for a different identity cannot satisfy VALIDPGPKEYS.
for fpr in $VALIDKEYS; do
  keyfile="$KEYDIR/$fpr.asc"

  [ -f "$keyfile" ] || error "required GPG key not found: $keyfile"

  keyinfo="$("$GPG" --batch --quiet --no-autostart --show-keys --with-colons "$keyfile" 2>/dev/null)"

  pub_count="$(printf '%s\n' "$keyinfo" | awk -F: '$1 == "pub"' | wc -l)"
  [ "$pub_count" -eq 1 ] || \
    error "key file $keyfile must contain exactly one primary key, found $pub_count"

  matched=0
  for key_fpr in $(printf '%s\n' "$keyinfo" | awk -F: '$1 == "fpr" { print $10 }'); do
    [ "$key_fpr" = "$fpr" ] && matched=1
  done
  [ "$matched" -eq 1 ] || \
    error "key file $keyfile does not contain key $fpr as its primary key or a subkey"

  "$GPG" --batch --quiet --no-autostart --no-default-keyring \
    --keyring "$KEYRING" --import "$keyfile" || \
    error "failed to import key file: $keyfile"
done

# Capture gpgv output so success and failure messages are cleanly separated,
# and the status output to check which key actually made the signature.
GPGV_OUT="$WORKDIR/gpgv.out"
GPGV_STATUS="$WORKDIR/gpgv.status"
if ! "$GPGV" --keyring "$KEYRING" --status-fd 3 \
    "$SIGFILE" "$VERIFY_FILE" >"$GPGV_OUT" 2>&1 3>"$GPGV_STATUS"; then
  cat "$GPGV_OUT"
  error "GPG signature verification failed"
fi

# The keyring only contains the keys listed in VALIDPGPKEYS, but check the
# VALIDSIG status line anyway so a good signature is only ever accepted from
# an explicitly pinned (sub)key.
SIG_FPRS="$(awk '$2 == "VALIDSIG" { print $3; print $NF }' "$GPGV_STATUS" | sort -u)"
[ -n "$SIG_FPRS" ] || { cat "$GPGV_OUT"; error "no valid signature found"; }

SIG_MATCH=0
for f in $SIG_FPRS; do
  case " $VALIDKEYS " in
    *" $f "*) SIG_MATCH=1 ;;
  esac
done
[ "$SIG_MATCH" = 1 ] || {
  cat "$GPGV_OUT"
  error "signature was not made by any key listed in VALIDPGPKEYS"
}

cat "$GPGV_OUT"
exit 0
