#!/bin/sh
#
# Detect a rotated/unknown GPG signing key and, if requested, pin the
# newly discovered key in the package Makefile
#
# Meant to run as part of `make .../check`, after a `make .../download`
# that fetched a source archive and its detached signature (using
# PKG_VALIDPGPKEYS=skip to bypass the normal pinned-key verification if
# the currently pinned key no longer matches). It never trusts a key by
# short key ID alone: a candidate key is only accepted once a real gpgv
# run confirms it actually produced the signature on this exact payload.
#
# Arguments:
#   $1 - payload file (e.g. source tarball)
#   $2 - detached signature file
#   $3 - key cache directory (a newly discovered key is cached here,
#        same layout fetch-gpg-key.sh and verify-sig.sh use)
#   $4 - current PKG_VALIDPGPKEYS value: one fingerprint, several
#        space-separated fingerprints, "skip", or empty
#   $5 - path to the Makefile to update
#   $6 - variable name to pass to fixup-makefile.pl, e.g. PKG_VALIDPGPKEYS
#        or Download/foo:VALIDPGPKEYS
#   $7 - "1" to rewrite the Makefile when a new key is confirmed, "0" to
#        only report
#
# Environment:
#   GPGV - preferred gpgv binary, falls back to gpgv in PATH
#   GPG  - preferred gpg binary, falls back to gpg in PATH
#
# Scope: only single-fingerprint pins (or no pin yet) are auto-fixed.
# Packages that pin more than one fingerprint (kernel.org's two release
# keys, gcc/binutils per-version keys, ...) are ambiguous - which of
# several keys rotated? - so this only reports the discovered
# fingerprint for those and leaves the Makefile untouched.

set -eu

warn() {
  echo " >>> WARNING: $1" >&2
}

error() {
  echo " >>> ERROR: $1" >&2
  exit 1
}

resolve_tool() {
  if [ -n "$1" ] && [ -x "$1" ]; then
    echo "$1"
  elif command -v "$2" >/dev/null 2>&1; then
    command -v "$2"
  else
    error "$2 not found. Install GnuPG on the build host."
  fi
}

if [ "$#" -ne 7 ]; then
  echo "Usage: $0 <payload> <signature> <keydir> <current keys> <makefile> <var> <fixup 0|1>" >&2
  exit 2
fi

PAYLOAD="$1"
SIGFILE="$2"
KEYDIR="$3"
# Word-splitting on the unquoted expansion collapses the stray leading/
# trailing whitespace Make's macro expansion tends to leave in this arg.
CURKEYS="$(set -- $4; echo "$*")"
MAKEFILE="$5"
VARNAME="$6"
FIXUP="$7"

[ -f "$PAYLOAD" ] || error "payload file not found: $PAYLOAD"
[ -f "$SIGFILE" ] || error "signature file not found: $SIGFILE"

GPGV="$(resolve_tool "${GPGV:-}" gpgv)"
GPG="$(resolve_tool "${GPG:-}" gpg)"

umask 077
WORKDIR="$(mktemp -d)"
cleanup() { rm -rf "$WORKDIR"; }
trap cleanup EXIT INT TERM

# Same compression-detection rule as verify-sig.sh: verify the exact bytes
# that were signed, decompressing first if the signature was made against
# the uncompressed tarball.
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

if [ -n "$DECOMPRESS_CMD" ]; then
  DECOMPRESS_UTIL="${DECOMPRESS_CMD%% *}"
  command -v "$DECOMPRESS_UTIL" >/dev/null 2>&1 || \
    error "required decompression utility not found: $DECOMPRESS_UTIL"
  VERIFY_FILE="$WORKDIR/payload.tar"
  $DECOMPRESS_CMD "$PAYLOAD" >"$VERIFY_FILE" || error "failed to decompress payload"
fi

is_single_fpr() {
  expr "$1" : '[0-9A-F]\{40\}$' >/dev/null
}

# If the currently pinned key(s) still validate the signature, there is
# nothing to discover or fix.
if [ -n "$CURKEYS" ] && [ "$CURKEYS" != skip ] && [ -d "$KEYDIR" ]; then
  GPGVHOME="$WORKDIR/gpgvhome"
  mkdir -m 700 "$GPGVHOME"
  KEYRING="$WORKDIR/current.gpg"
  : >"$KEYRING"
  all_present=1
  for fpr in $CURKEYS; do
    [ -f "$KEYDIR/$fpr.asc" ] || { all_present=0; break; }
    GNUPGHOME="$GPGVHOME" "$GPG" --batch --quiet --no-autostart \
      --no-default-keyring --keyring "$KEYRING" \
      --import "$KEYDIR/$fpr.asc" >/dev/null 2>&1 || true
  done
  if [ "$all_present" = 1 ] && GNUPGHOME="$GPGVHOME" "$GPGV" --keyring "$KEYRING" \
      "$SIGFILE" "$VERIFY_FILE" >/dev/null 2>&1; then
    exit 0
  fi
fi

# Discovery: ask gpg who signed this, without trusting anything yet. A
# fresh, empty GNUPGHOME guarantees NO_PUBKEY/ERRSIG fires rather than an
# accidental match against a key already sitting in some ambient keyring.
PROBEHOME="$WORKDIR/probehome"
mkdir -m 700 "$PROBEHOME"
STATUS_OUT="$WORKDIR/probe.status"
GNUPGHOME="$PROBEHOME" "$GPG" --batch --status-fd 1 --no-default-keyring \
  --verify "$SIGFILE" "$VERIFY_FILE" >"$STATUS_OUT" 2>/dev/null || true

KEYIDS="$(awk '/^\[GNUPG:\] (NO_PUBKEY|ERRSIG)/ { print $3 }' "$STATUS_OUT" | sort -u)"
[ -n "$KEYIDS" ] || error "could not determine the signing key ID from $SIGFILE"

KEYID_COUNT="$(printf '%s\n' "$KEYIDS" | wc -l)"
[ "$KEYID_COUNT" -eq 1 ] || \
  error "signature references multiple key IDs, inspect manually: $KEYIDS"

KEYID="$KEYIDS"

echo "Signature was made by key ID $KEYID, not currently pinned. Fetching candidate key..."

# Try each source in turn, fully validating before accepting: fetched key
# must contain exactly one primary key with the searched key ID as its
# primary or a subkey fingerprint (a keyserver could otherwise be tricked
# into serving an unrelated key for a colliding short ID), it must
# actually import into an isolated keyring (keys.openpgp.org silently
# strips the user ID for unverified addresses, which gpg --import then
# discards despite exiting 0), and gpgv must accept the signature against
# it. Any rejection falls through to the next source rather than giving up.
CANDIDATE="$WORKDIR/candidate.asc"
NEWFPR=""
for url in \
    "https://keys.openpgp.org/vks/v1/by-keyid/$KEYID" \
    "https://keyserver.ubuntu.com/pks/lookup?op=get&options=mr&search=0x$KEYID"; do
  rm -f "$CANDIDATE"
  if command -v curl >/dev/null 2>&1; then
    curl -f --connect-timeout 5 --retry 2 --location --silent --show-error \
      -o "$CANDIDATE" "$url" 2>/dev/null || continue
  elif command -v wget >/dev/null 2>&1; then
    wget --tries=2 --timeout=5 --quiet -O "$CANDIDATE" "$url" || continue
  else
    error "neither curl nor wget found to fetch GPG keys"
  fi
  [ -s "$CANDIDATE" ] || continue

  CANDIDATE_INFO="$("$GPG" --batch --quiet --no-autostart --show-keys --with-colons "$CANDIDATE" 2>/dev/null)" || continue
  CANDIDATE_PUBCOUNT="$(printf '%s\n' "$CANDIDATE_INFO" | awk -F: '$1 == "pub"' | wc -l)"
  [ "$CANDIDATE_PUBCOUNT" -eq 1 ] || continue
  printf '%s\n' "$CANDIDATE_INFO" | awk -F: '$1 == "fpr" { print $10 }' | grep -qi "$KEYID\$" || continue

  CONFIRMHOME="$WORKDIR/confirmhome"
  rm -rf "$CONFIRMHOME"
  mkdir -m 700 "$CONFIRMHOME"
  CONFIRMRING="$WORKDIR/confirm.gpg"
  : >"$CONFIRMRING"
  GNUPGHOME="$CONFIRMHOME" "$GPG" --batch --quiet --no-autostart \
    --no-default-keyring --keyring "$CONFIRMRING" --import "$CANDIDATE" >/dev/null 2>&1

  # gpg --import exits 0 even when it silently skips a UID-less key, so
  # confirm the key actually landed in the keyring before trusting it.
  GNUPGHOME="$CONFIRMHOME" "$GPG" --batch --quiet --no-autostart \
      --no-default-keyring --keyring "$CONFIRMRING" --list-keys --with-colons 2>/dev/null \
    | awk -F: '$1 == "fpr" { print $10 }' | grep -qi "$KEYID\$" || continue

  CONFIRM_STATUS="$WORKDIR/confirm.status"
  GNUPGHOME="$CONFIRMHOME" "$GPGV" --keyring "$CONFIRMRING" --status-fd 3 \
    "$SIGFILE" "$VERIFY_FILE" >/dev/null 2>&1 3>"$CONFIRM_STATUS" || continue

  NEWFPR="$(awk '$2 == "VALIDSIG" { print $NF }' "$CONFIRM_STATUS" | head -n1)"
  [ -n "$NEWFPR" ] && break
done

[ -n "$NEWFPR" ] || \
  error "could not find and confirm a valid key for key ID $KEYID from any source"

# Cache the confirmed key so a subsequent normal download/verify doesn't
# need to refetch it.
mkdir -p "$KEYDIR"
tmpfile="$(mktemp "$KEYDIR/.$NEWFPR.XXXXXX")"
cat "$CANDIDATE" >"$tmpfile"
mv -f "$tmpfile" "$KEYDIR/$NEWFPR.asc"

# PKG_SOURCE_SIG being set requires PKG_VALIDPGPKEYS to already be
# non-empty (enforced by the Download macro), so CURKEYS is only ever
# empty here as a defensive fallback, not a case that occurs in practice.
PREVKEYS="$CURKEYS"
[ -n "$PREVKEYS" ] || PREVKEYS=skip

if [ "$PREVKEYS" = skip ]; then
  echo "Confirmed signing key: $NEWFPR (no key was previously pinned)"
elif [ "$PREVKEYS" = "$NEWFPR" ]; then
  # Pinned key matched by fingerprint but gpgv rejected it earlier for some
  # other reason (e.g. a corrupt cached key file); nothing to change.
  exit 0
elif is_single_fpr "$PREVKEYS"; then
  echo "Confirmed signing key rotated: $PREVKEYS -> $NEWFPR"
else
  warn "PKG_VALIDPGPKEYS currently pins multiple keys ($PREVKEYS)."
  warn "Cannot tell which one to replace; confirmed new key is $NEWFPR."
  warn "Update PKG_VALIDPGPKEYS by hand if this key is expected."
  exit 0
fi

if [ "$FIXUP" != 1 ]; then
  warn "PKG_VALIDPGPKEYS is out of date. Re-run with FIXUP=1 to pin $NEWFPR,"
  warn "or set it manually if you have not independently verified this key."
  exit 0
fi

cp "$MAKEFILE" "$WORKDIR/makefile.before"
"$(dirname "$0")/fixup-makefile.pl" "$MAKEFILE" fix-validpgpkeys "$VARNAME" "$NEWFPR" "$PREVKEYS"
if cmp -s "$WORKDIR/makefile.before" "$MAKEFILE"; then
  warn "Could not automatically update $VARNAME in $MAKEFILE (value may be"
  warn "set indirectly through another variable). Set it to $NEWFPR by hand."
else
  echo " >>> Updated $VARNAME to $NEWFPR in $MAKEFILE"
fi
