#!/usr/bin/env bash
set -euo pipefail

usage() {
	cat <<'EOF'
Usage:
  upload_firmware_tftp.sh [FIRMWARE_FILE] [options]

Uploads a firmware image to the router bootloader recovery TFTP endpoint.

Defaults:
  --host       192.168.1.1
  --client-ip  192.168.1.2/24

Required host utilities:
  ./scripts/upload_firmware_tftp.sh --get-deps

Example:
  ./scripts/upload_firmware_tftp.sh

The script does not change host network settings. It waits until this PC
already has --client-ip, then uploads the firmware.

SHA256SUMS must exist next to the firmware file. The firmware hash is checked
before upload.
EOF
}

die() {
	local code="${2:-1}"
	echo "ERROR: $1" >&2
	exit "$code"
}

require_cmds() {
	local missing=()
	local cmd
	for cmd in "$@"; do
		command -v "$cmd" >/dev/null 2>&1 || missing+=("$cmd")
	done
	[ "${#missing[@]}" -eq 0 ] || die "missing required utilities: ${missing[*]}"
}

get_deps() {
	printf '%s\n' awk grep ip ping readlink sha256sum sed tftp wc
}

default_firmware() {
	local script_dir
	local bundle_dir
	local sums_file
	local count
	local file

	script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
	if [ -d "$script_dir/../firmware" ]; then
		bundle_dir="$(cd -- "$script_dir/.." && pwd)"
	else
		bundle_dir="$script_dir"
	fi
	sums_file="$bundle_dir/firmware/SHA256SUMS"

	[ -f "$sums_file" ] || die "firmware file is required because $sums_file is missing"

	count="$(awk 'NF >= 2 { count++ } END { print count + 0 }' "$sums_file")"
	[ "$count" -eq 1 ] || die "firmware file is required because $sums_file must contain exactly one firmware entry"

	file="$(awk 'NF >= 2 { file = $2; sub(/^\*/, "", file); print file; exit }' "$sums_file")"
	[ -n "$file" ] || die "firmware file is required because $sums_file has no firmware file entry"
	[ "${file#/}" = "$file" ] || die "$sums_file must use a relative firmware file entry"

	printf '%s\n' "$bundle_dir/firmware/$file"
}

verify_firmware_hash() {
	local sums_file="$firmware_dir/SHA256SUMS"
	local expected
	local actual

	[ -f "$sums_file" ] || die "SHA256SUMS is required next to firmware: $sums_file"

	expected="$(awk -v name="$firmware_name" '
		{
			file = $2
			sub(/^\*/, "", file)
			if (file == name) {
				print $1
				found = 1
				exit
			}
		}
		END {
			if (!found)
				exit 1
		}
	' "$sums_file")" || die "SHA256SUMS does not contain an entry for $firmware_name"

	actual="$(sha256sum "$firmware_abs" | awk '{ print $1 }')"
	[ "$actual" = "$expected" ] || {
		echo "Expected: $expected" >&2
		echo "Actual:   $actual" >&2
		die "firmware SHA256 mismatch: $firmware_abs"
	}

	echo "Firmware SHA256 verified: $actual"
}

host_has_recovery_ip() {
	local escaped
	escaped="$(printf '%s\n' "$client_ip" | sed 's/[.[\*^$()+?{}|]/\\&/g')"
	ip -o -4 addr show scope global | grep -Eq "inet ${escaped}([[:space:]]|$)"
}

wait_for_recovery_ip() {
	local i

	echo "Waiting for this PC to have IPv4 $client_ip..."
	for ((i = 1; i <= 120; i++)); do
		if host_has_recovery_ip; then
			echo "Recovery IPv4 is ready: $client_ip"
			return 0
		fi
		if [ $((i % 10)) -eq 0 ]; then
			echo "Still waiting for IPv4 $client_ip on this PC..."
		fi
		sleep 2
	done

	die "this PC did not get IPv4 $client_ip"
}

wait_for_host_ping() {
	local i

	echo "Waiting for router recovery host to answer ping: $host"
	for ((i = 1; i <= 120; i++)); do
		if ping -c 1 -W 1 "$host" >/dev/null 2>&1; then
			echo "Host is reachable: $host"
			return 0
		fi
		if [ $((i % 10)) -eq 0 ]; then
			echo "Still waiting for router recovery host: $host"
		fi
		sleep 1
	done

	echo "WARNING: $host did not answer ping."
	echo "Some bootloaders do not answer ICMP while still accepting TFTP."
	read -r -p "Type TFTP to try upload anyway, or anything else to abort: " typed
	[ "$typed" = "TFTP" ] || die "aborted because $host is not reachable by ping"
}

firmware=""
host="192.168.1.1"
client_ip="192.168.1.2/24"

while [ "$#" -gt 0 ]; do
	case "$1" in
		-h|--help)
			usage
			exit 0
			;;
		--get-deps)
			get_deps
			exit 0
			;;
		--host)
			[ "$#" -ge 2 ] || die "--host requires a value"
			host="$2"
			shift 2
			;;
		--client-ip)
			[ "$#" -ge 2 ] || die "--client-ip requires a value"
			client_ip="$2"
			shift 2
			;;
		*)
			[ -z "$firmware" ] || die "firmware file specified more than once"
			firmware="$1"
			shift
			;;
	esac
done

[ -n "$firmware" ] || firmware="$(default_firmware)"
[ -f "$firmware" ] || die "firmware file does not exist: $firmware"
[[ "$client_ip" =~ ^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+/[0-9]+$ ]] || die "--client-ip must be IPv4 CIDR, for example 192.168.1.2/24"

require_cmds $(get_deps)

firmware_abs="$(readlink -f -- "$firmware")"
firmware_dir="${firmware_abs%/*}"
firmware_name="${firmware_abs##*/}"
verify_firmware_hash

echo "Uploading firmware by TFTP:"
printf '  router:    %s\n' "$host"
printf '  client IP: %s\n' "$client_ip"
printf '  firmware: %s\n' "$firmware_abs"
printf '  size:     %s bytes\n' "$(wc -c < "$firmware_abs")"
echo
cat <<EOF
Prepare router recovery:
  1. Configure this PC wired Ethernet with static IPv4 $client_ip.
  2. Connect this PC to a router LAN port by Ethernet.
  3. Power the router Off.
  4. Hold the router RESTORE.
  5. Power the router On while holding RESTORE button.
  6. Release RESTORE button when firmware starts uploading.

TFTP upload begins when $host answers ping.
EOF

wait_for_recovery_ip
echo
wait_for_host_ping

set +e
echo "Firmware uploading (you may release RESTORE button)..."
tftp_output="$(cd "$firmware_dir" && tftp "$host" -m binary -c put "$firmware_name" 2>&1)"
code=$?
set -e
printf '%s\n' "$tftp_output"

if [ "$code" -ne 0 ]; then
	die "TFTP upload failed with exit code $code" "$code"
fi

if printf '%s\n' "$tftp_output" | grep -Eqi 'no such file|not found|error|failed|timed out|timeout'; then
	die "TFTP upload reported an error"
fi

if [ "$code" -eq 0 ]; then
	echo
	echo "Firmware uploaded by TFTP and router flash programming started at: $(date '+%Y-%m-%d %H:%M:%S %Z')"
	echo "There is no clear visual sign that the programming of the router is finished."
	echo "NEXT: Wait about 3-4 minutes for flashing to finish while preparing USB."
	echo "Then insert USB and power-cycle the router."
	echo "Restore host networking if needed after first boot finish."
	echo
fi
