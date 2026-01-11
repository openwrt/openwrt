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

set -e

PAYLOAD="$1"
SIGFILE="$2"
KEYDIR="$3"
VALIDKEYS="$4"

if [ -z "$GPGV" ]; then
	echo "ERROR: GPGV not set" >&2
	exit 1
fi

if [ ! -f "$PAYLOAD" ]; then
	echo "ERROR: payload file not found: $PAYLOAD" >&2
	exit 1
fi

if [ ! -f "$SIGFILE" ]; then
	echo "ERROR: signature file not found: $SIGFILE" >&2
	exit 1
fi

# Detect if payload is compressed and needs decompression for verification
# Only decompress if signature file is for the uncompressed tarball
# Examples:
#   - foo.tar.gz + foo.tar.gz.sig -> don't decompress (sig is for compressed)
#   - foo.tar.xz + foo.tar.sign -> decompress (sig is for uncompressed .tar)
VERIFY_FILE="$PAYLOAD"
DECOMPRESS_NEEDED=0
DECOMPRESS_CMD=""

# Check if signature filename suggests it's for compressed or uncompressed file
case "$PAYLOAD" in
	*.tar.gz|*.tgz)
		# Only decompress if sig is NOT .tar.gz.sig or .tgz.sig
		case "$SIGFILE" in
			*.tar.gz.sig|*.tar.gz.asc|*.tgz.sig|*.tgz.asc)
				DECOMPRESS_NEEDED=0
				;;
			*)
				DECOMPRESS_CMD="gzip -dc"
				DECOMPRESS_NEEDED=1
				;;
		esac
		;;
	*.tar.xz|*.txz)
		# Only decompress if sig is NOT .tar.xz.sig or .txz.sig
		case "$SIGFILE" in
			*.tar.xz.sig|*.tar.xz.asc|*.txz.sig|*.txz.asc)
				DECOMPRESS_NEEDED=0
				;;
			*)
				DECOMPRESS_CMD="xz -dc"
				DECOMPRESS_NEEDED=1
				;;
		esac
		;;
	*.tar.bz2|*.tbz2)
		# Only decompress if sig is NOT .tar.bz2.sig or .tbz2.sig
		case "$SIGFILE" in
			*.tar.bz2.sig|*.tar.bz2.asc|*.tbz2.sig|*.tbz2.asc)
				DECOMPRESS_NEEDED=0
				;;
			*)
				DECOMPRESS_CMD="bzip2 -dc"
				DECOMPRESS_NEEDED=1
				;;
		esac
		;;
	*.tar.zst)
		# Only decompress if sig is NOT .tar.zst.sig
		case "$SIGFILE" in
			*.tar.zst.sig|*.tar.zst.asc)
				DECOMPRESS_NEEDED=0
				;;
			*)
				DECOMPRESS_CMD="zstd -dc"
				DECOMPRESS_NEEDED=1
				;;
		esac
		;;
esac

if [ ! -d "$KEYDIR" ]; then
	echo "ERROR: GPG key directory not found: $KEYDIR" >&2
	exit 1
fi

if [ -z "$VALIDKEYS" ]; then
	echo "ERROR: no valid key fingerprints provided" >&2
	exit 1
fi

KEYFILES="$(ls -1 "$KEYDIR" 2>/dev/null || true)"
if [ -z "$KEYFILES" ]; then
	echo "ERROR: no GPG keys found in $KEYDIR" >&2
	exit 1
fi

TMPDIR="$(mktemp -d)"
cleanup() {
	rm -rf "$TMPDIR"
}
trap cleanup EXIT INT TERM

# If payload is compressed, decompress it first
if [ "$DECOMPRESS_NEEDED" -eq 1 ]; then
	echo "Decompressing payload for signature verification..."
	VERIFY_FILE="$TMPDIR/payload.tar"
	if ! $DECOMPRESS_CMD "$PAYLOAD" > "$VERIFY_FILE"; then
		echo "ERROR: failed to decompress payload" >&2
		exit 1
	fi
fi

# Build a hermetic keyring from all key files
KEYRING="$TMPDIR/keyring.gpg"
for key in "$KEYDIR"/*; do
	if [ -f "$key" ]; then
		cat "$key" >> "$KEYRING"
	fi
done

# Run gpgv and capture output (including stderr)
GPGV_OUT="$TMPDIR/gpgv.out"
if ! "$GPGV" --keyring "$KEYRING" "$SIGFILE" "$VERIFY_FILE" > "$GPGV_OUT" 2>&1; then
	# Show the output to user
	cat "$GPGV_OUT"
	echo "ERROR: GPG signature verification failed" >&2
	exit 1
fi

# Show gpgv output to user
cat "$GPGV_OUT"

# Check if the signature was made by one of the valid keys
FOUND_VALID_KEY=0
for validkey in $VALIDKEYS; do
	if grep -q "$validkey" "$GPGV_OUT"; then
		FOUND_VALID_KEY=1
		break
	fi
done

if [ "$FOUND_VALID_KEY" -eq 0 ]; then
	echo "ERROR: Signature is valid but was not made by any key in PKG_VALIDPGPKEYS" >&2
	echo "Expected one of: $VALIDKEYS" >&2
	exit 1
fi

exit 0
