#!/bin/sh
#
# Fetch and cache GPG public keys by pinned fingerprint
#
# Arguments:
#   $1   - key cache directory, created if missing (one <fingerprint>.asc
#          per key, the layout verify-sig.sh expects as its key directory)
#   $2   - space-separated list of key fingerprints (VALIDPGPKEYS)
#   $3.. - optional override URLs of ASCII-armored key files
#          (PKG_GPG_KEY_URLS), tried before the built-in keyservers
#
# Environment:
#   GPG - preferred gpg binary, falls back to gpg in PATH
#   DOWNLOAD_CHECK_CERTIFICATE - "y" to enforce TLS certificate checks
#
# A fingerprint is a cryptographic hash of the key material, so the pinned
# fingerprints in VALIDPGPKEYS are the trust anchor and any key source is
# just untrusted transport: a fetched blob is only accepted if it contains
# exactly one primary key and the pinned fingerprint is that key's primary
# fingerprint or one of its subkey fingerprints (the same rule
# verify-sig.sh enforces again before use). Anything else is discarded and
# the next source is tried.

set -eu

error() {
  echo " >>> ERROR: $1" >&2
  exit 1
}

if [ "$#" -lt 2 ]; then
  echo "Usage: $0 <cache dir> <valid fingerprints> [key url ...]" >&2
  exit 2
fi

CACHEDIR="$1"
VALIDKEYS="$2"
shift 2

[ -n "$VALIDKEYS" ] || error "no valid key fingerprints provided"

# Prefer $GPG (e.g. the staging dir binary) if executable, otherwise fall
# back to gpg from PATH.
if [ -n "${GPG:-}" ] && [ -x "${GPG:-}" ]; then
  :
elif command -v gpg >/dev/null 2>&1; then
  GPG="$(command -v gpg)"
else
  error "gpg not found. Install GnuPG on the build host or disable CONFIG_DOWNLOAD_VERIFY_SIGNATURES."
fi

if [ "${DOWNLOAD_CHECK_CERTIFICATE:-}" = "y" ]; then
  CURL_CERT_FLAG=""
  WGET_CERT_FLAG=""
else
  CURL_CERT_FLAG="--insecure"
  WGET_CERT_FLAG="--no-check-certificate"
fi

fetch_url() {
  if command -v curl >/dev/null 2>&1; then
    curl -f --connect-timeout 5 --retry 2 --location --silent --show-error \
      $CURL_CERT_FLAG -o "$2" "$1"
  elif command -v wget >/dev/null 2>&1; then
    wget --tries=2 --timeout=5 --quiet \
      $WGET_CERT_FLAG -O "$2" "$1"
  else
    error "neither curl nor wget found to fetch GPG keys"
  fi
}

# The key file must contain exactly one primary key, the pinned fingerprint
# must be that key's primary fingerprint or one of its subkey fingerprints,
# and gpg must be able to import it (a keyserver may serve a stripped or
# damaged blob).
validate_keyfile() {
  vk_file="$1"
  vk_fpr="$2"

  vk_keyinfo="$("$GPG" --batch --quiet --no-autostart --show-keys --with-colons "$vk_file" 2>/dev/null)" || return 1

  vk_pub_count="$(printf '%s\n' "$vk_keyinfo" | awk -F: '$1 == "pub"' | wc -l)"
  [ "$vk_pub_count" -eq 1 ] || return 1

  printf '%s\n' "$vk_keyinfo" | awk -F: '$1 == "fpr" { print $10 }' | grep -qx "$vk_fpr" || return 1

  rm -rf "$WORKDIR/testhome"
  mkdir -m 700 "$WORKDIR/testhome"
  GNUPGHOME="$WORKDIR/testhome" "$GPG" --batch --quiet --no-autostart \
    --no-default-keyring --keyring "$WORKDIR/testhome/keyring.gpg" \
    --import "$vk_file" >/dev/null 2>&1

  # --import exits 0 even when it silently skips a key, which it does for
  # any key with no user ID. keys.openpgp.org's by-fingerprint endpoint
  # strips the UID for addresses the owner hasn't verified there, so a
  # non-error exit isn't enough evidence the key is actually usable -
  # confirm it landed in the keyring before accepting this source.
  GNUPGHOME="$WORKDIR/testhome" "$GPG" --batch --quiet --no-autostart \
    --no-default-keyring --keyring "$WORKDIR/testhome/keyring.gpg" \
    --list-keys --with-colons 2>/dev/null \
    | awk -F: '$1 == "fpr" { print $10 }' | grep -qx "$vk_fpr"
}

WORKDIR="$(mktemp -d)"
cleanup() { rm -rf "$WORKDIR"; }
trap cleanup EXIT INT TERM

mkdir -p "$CACHEDIR"

for fpr in $VALIDKEYS; do
  if ! expr "$fpr" : '[0-9A-F]\{40\}$' >/dev/null; then
    error "invalid fingerprint format (must be 40 uppercase hex chars, no whitespace): $fpr"
  fi

  keyfile="$CACHEDIR/$fpr.asc"
  if [ -f "$keyfile" ] && validate_keyfile "$keyfile" "$fpr"; then
    continue
  fi

  found=0
  for url in "$@" \
      "https://keys.openpgp.org/vks/v1/by-fingerprint/$fpr" \
      "https://keyserver.ubuntu.com/pks/lookup?op=get&options=mr&search=0x$fpr"; do
    echo "Fetching GPG key $fpr from $url"
    rm -f "$WORKDIR/key.asc"
    fetch_url "$url" "$WORKDIR/key.asc" || continue
    if validate_keyfile "$WORKDIR/key.asc" "$fpr"; then
      tmpfile="$(mktemp "$CACHEDIR/.$fpr.XXXXXX")"
      cat "$WORKDIR/key.asc" >"$tmpfile"
      mv -f "$tmpfile" "$keyfile"
      found=1
      break
    fi
    echo "Rejected key data from $url (does not match pinned fingerprint $fpr)" >&2
  done

  [ "$found" = 1 ] || error "unable to fetch GPG key $fpr from any source"
done

exit 0
