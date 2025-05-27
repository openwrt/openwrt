#!/usr/bin/env bash
# Script to perform verified file downloads.
# Exit codes:
#  0 - File downloaded successfully and verified
#  1 - Failed to download requested file
#  2 - Failed to download sha256sums file
#  3 - Failed to download sha256sums.gpg file
#  4 - GnuPG is available but fails to verify the signature (missing pubkey, file integrity error, ...)
#  5 - The checksums do not match
#  6 - Unable to copy the requested file to its final destination
#  254 - The script got interrupted by a signal
#  255 - A suitable download or checksum utility is missing

[ -n "$1" ] || {
	echo "$0 - Download and verify build artifacts"
	echo "Usage: $0 <url>" >&2
	exit 1
}

finish() {
	[ -e "/tmp/verify.$$" ] && {
		echo "Cleaning up."
		rm -r "/tmp/verify.$$"
	}
	exit "$1"
}

trap "finish 254" INT TERM

destdir="$(pwd)"
image_url="$1"
image_file="${image_url##*/}"
sha256_url="${image_url%/*}/sha256sums"
gpgsig_url="${image_url%/*}/sha256sums.asc"
keyserver_url="hkp://keyserver.ubuntu.com"

# Find a suitable download utility
if which curl >/dev/null; then
	download() { curl --progress-bar -o "$1" "$2"; }
elif which wget >/dev/null; then
	download() { wget -O "$1" "$2"; }
elif which fetch >/dev/null; then
	download() { fetch -o "$1" "$2"; }
else
	echo "No suitable download utility found, cannot download files!" >&2
	finish 255
fi

# Find a suitable checksum utility
if which sha256sum >/dev/null; then
	checksum() { sha256sum -c --ignore-missing "sha256sums"; }
elif which shasum >/dev/null; then
	checksum() {
		local sum
		sum="$(shasum -a 256 "$image_file")";
		grep -xF "${sum%% *} *$image_file" "sha256sums";
	}
else
	echo "No SHA256 checksum executable installed, cannot verify checksums!" >&2
	finish 255
fi

# Check for gpg availability
if which gpg >/dev/null; then
	runpgp() { gpg "$@"; }
else
	runpgp() {
		echo "WARNING: No GnuPG installed, cannot verify digital signature!" >&2
		return 0
	}
fi

tmpdir="$(mktemp -d)"
cd "$tmpdir" || {
	echo "Failed to create temporary directory!" >&2
	finish 255
}

echo ""
echo "1) Downloading artifact file"
echo "========================="
download "$image_file" "$image_url" || {
	echo "Failed to download image file!" >&2
	finish 1
}

echo ""
echo "2) Downloading checksum file"
echo "============================"
download "sha256sums" "$sha256_url" || {
	echo "Failed to download checksum file!" >&2
	finish 2
}

echo ""
echo "3) Downloading the GPG signature"
echo "================================"
download "sha256sums.gpg" "$gpgsig_url" || {
	echo "Failed to download GPG signature!" >&2
	finish 3
}

echo ""
echo "4) Verifying GPG signature"
echo "=========================="
missing_key=$(runpgp --status-fd 1 --with-fingerprint --verify \
	"sha256sums.gpg" "sha256sums" 2>/dev/null | sed -ne 's!^.* NO_PUBKEY !!p')

if [ -n "$missing_key" ]; then
	echo "The signature was signed by a public key with the id $missing_key" >&2
	echo "which is not present on this system."                              >&2
	echo ""                                                                  >&2

	echo "Provide a public keyserver url below or press enter to accept the" >&2
	echo "default suggestion. Hit Ctrl-C to abort the operation."            >&2
	echo ""                                                                  >&2

	while true; do
		printf 'Keyserver to use? [%s] > ' "$keyserver_url"
		read -r url; case "${url:-$keyserver_url}" in
			hkp://*)
				gpg --keyserver "${url:-$keyserver_url}" --recv-keys "$missing_key" || {
					echo "Failed to download public key." >&2
					finish 7
				}
				break
			;;
			*)
				echo "Expecting a key server url in the form 'hkp://hostname'." >&2
			;;
		esac
	done
fi

runpgp --with-fingerprint --verify "sha256sums.gpg" "sha256sums" || {
	echo "Failed to verify checksum file with GPG signature!" >&2
	finish 4
}

echo ""
echo "5) Verifying SHA256 checksum"
echo "============================"
checksum || {
	echo "Checksums do not match!" >&2
	finish 5
}

cp "$image_file" "$destdir/$image_file" || {
	echo "Failed to write '$destdir/$image_file'" >&2
	finish 6
}

echo ""
echo "Verification done!"
echo "=================="
echo "Downloaded artifact placed in '$destdir/$image_file'."
echo ""

finish 0
