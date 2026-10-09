#!/usr/bin/env bash
set -euo pipefail
PATH="/usr/sbin:/sbin:$PATH"

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
helper_dir="$script_dir/scripts"
[ -d "$helper_dir" ] || helper_dir="$script_dir"

usage() {
	cat <<'EOF'
Usage:
  install.sh --usb /dev/sdX [options]

Composes three dedicated steps for a fresh router install:
  1. upload firmware by bootloader TFTP recovery,
  2. prepare USB storage on the host,
  3. verify that firmware booted with USB overlay and USB swap.

Options:
  --get-deps                  print required host utilities for the selected options
  --firmware FILE             firmware image to upload by TFTP; autodetected when omitted
  --usb /dev/sdX              host USB disk to format
  --overlay-mib N             overlay partition size, default 512
  --swap-mib N                swap partition size, default 128
  --router root@192.168.1.1   router SSH target
  --url http://192.168.1.1    router web URL
  --tftp-host 192.168.1.1     bootloader TFTP host
  --tftp-client-ip CIDR       host IPv4 CIDR required for TFTP recovery, default 192.168.1.2/24

Required host utilities:
  collected from:
    ./scripts/upload_firmware_tftp.sh --get-deps
    ./scripts/prepare_usb_overlay_swap.sh --get-deps [--swap-mib N]
    ./scripts/verify_router_overlay_swap.sh --get-deps

Optional host utilities:
  xdg-open                     open the router web URL at the end
EOF
}

die() {
	local code="${2:-1}"
	echo "ERROR: $1" >&2
	exit "$code"
}

usage_die() {
	usage >&2
	echo >&2
	die "$1" "${2:-1}"
}

refuse_root() {
	[ "$EUID" -ne 0 ] || die "do not run install.sh as root; helper scripts use sudo only for USB disk operations"
}

run_step() {
	local name="$1"
	local code
	shift

	echo
	echo "==> $name ($(date '+%Y-%m-%d %H:%M:%S %Z'))"
	if "$@"; then
		echo "COMPLETED: $name ($(date '+%Y-%m-%d %H:%M:%S %Z'))"
	else
		code=$?
		echo "ERROR: $name failed at $(date '+%Y-%m-%d %H:%M:%S %Z') with exit code $code" >&2
		exit "$code"
	fi
}

note_after_tftp_upload() {
	echo
	echo "Prepare USB storage before power-cycling the router into the new firmware."
	read -r -p "To start preparing USB Press Enter: " _
}

pause_after_usb_prepare() {
	echo
	echo "USB storage prepared and ejected."
	echo "Insert the USB flash into the router before it boots the new firmware."
	echo "Power-cycle the router only after programming is finished and the USB flash is inserted."
	read -r -p "After inserting USB and power-cycling the router Press Enter: " _
}

require_cmds() {
	local missing=()
	local cmd
	for cmd in "$@"; do
		command -v "$cmd" >/dev/null 2>&1 || missing+=("$cmd")
	done
	[ "${#missing[@]}" -eq 0 ] || die "missing required utilities: ${missing[*]}"
}

add_unique_dep() {
	local dep="$1"
	local existing
	[ -n "$dep" ] || return 0
	for existing in "${deps[@]}"; do
		[ "$existing" = "$dep" ] && return 0
	done
	deps+=("$dep")
}

add_deps_from_script() {
	local script="$1"
	local output dep
	shift
	output="$("$script" "$@" --get-deps)"
	while IFS= read -r dep; do
		add_unique_dep "$dep"
	done <<< "$output"
}

get_deps() {
	deps=()
	add_deps_from_script "$helper_dir/upload_firmware_tftp.sh"
	add_deps_from_script "$helper_dir/prepare_usb_overlay_swap.sh" --swap-mib "$swap_mib"
	add_deps_from_script "$helper_dir/verify_router_overlay_swap.sh"
	printf '%s\n' "${deps[@]}"
}

disk_size_mib() {
	local bytes
	bytes="$(lsblk -bdn -o SIZE "$1")"
	bytes="${bytes//[[:space:]]/}"
	[[ "$bytes" =~ ^[0-9]+$ ]] || die "cannot determine byte size of $1"
	[ "$bytes" -gt 0 ] || {
		lsblk -o NAME,SIZE,MODEL,TRAN,MOUNTPOINTS "$1" >&2
		die "$1 reports size 0; unplug/replug the USB device or check the adapter/media"
	}
	printf '%s\n' "$((bytes / 1024 / 1024))"
}

has_active_swap() {
	local dev="$1"
	local swapdev rest
	[ -r /proc/swaps ] || return 1
	while read -r swapdev rest; do
		[[ "$swapdev" =~ ^${dev}[0-9]+$ ]] && return 0
	done < /proc/swaps
	return 1
}

print_before_start() {
	cat <<EOF
Planned operation:
  router:           $tftp_host
  client IP:        $tftp_client_ip
  firmware:         ${firmware:-single firmware/SHA256SUMS entry}
  host USB disk:    $usb
  USB overlay size: ${overlay_mib} MiB
  USB swap size:    ${swap_mib} MiB
  router SSH:       $router
  router web URL:   $router_url
EOF
}

preflight() {
	local tran
	local disk_mib
	local requested_mib

	echo "==> Preflight checks"

	[[ "$overlay_mib" =~ ^[0-9]+$ ]] || die "--overlay-mib must be an integer MiB value"
	[[ "$swap_mib" =~ ^[0-9]+$ ]] || die "--swap-mib must be an integer MiB value"
	overlay_mib=$((10#$overlay_mib))
	swap_mib=$((10#$swap_mib))
	[ "$overlay_mib" -gt 0 ] || die "--overlay-mib must be positive"
	[ "$swap_mib" -gt 0 ] || die "--swap-mib must be greater than 0; this firmware requires USB swap"

	require_cmds $(get_deps)

	print_before_start
	echo

	case "$usb" in
		/dev/sd[a-z]) ;;
		*) die "refusing non-/dev/sdX path: $usb; use a whole USB disk such as /dev/sdb" ;;
	esac
	[ -b "$usb" ] || die "$usb is not a block device; insert the USB flash before starting"

	tran="$(lsblk -dn -o TRAN "$usb")"
	tran="${tran//[[:space:]]/}"
	[ "$tran" = "usb" ] || die "$usb is not reported as USB by lsblk (TRAN=$tran)"

	disk_mib="$(disk_size_mib "$usb")"
	requested_mib=$((1 + overlay_mib + swap_mib))
	[ "$requested_mib" -lt "$disk_mib" ] || {
		die "overlay+swap layout is too large: needs more than ${requested_mib} MiB including 1 MiB start offset, disk is ${disk_mib} MiB"
	}

	if has_active_swap "$usb"; then
		die "$usb has an active swap partition; run swapoff for that partition first"
	fi

	echo "Current host USB layout:"
	lsblk -o NAME,SIZE,FSTYPE,LABEL,MODEL,TRAN,MOUNTPOINTS "$usb"
	echo
	echo "OK: preflight checks passed."
}

firmware=""
usb=""
overlay_mib=512
swap_mib=128
router="root@192.168.1.1"
router_url="http://192.168.1.1"
tftp_host="192.168.1.1"
tftp_client_ip="192.168.1.2/24"
print_deps=0

refuse_root

while [ "$#" -gt 0 ]; do
	case "$1" in
		-h|--help)
			usage
			exit 0
			;;
		--get-deps)
			print_deps=1
			shift
			;;
		--firmware)
			[ "$#" -ge 2 ] || die "--firmware requires a value"
			firmware="$2"
			shift 2
			;;
		--usb)
			[ "$#" -ge 2 ] || die "--usb requires a value"
			usb="$2"
			shift 2
			;;
		--overlay-mib)
			[ "$#" -ge 2 ] || die "--overlay-mib requires a value"
			overlay_mib="$2"
			shift 2
			;;
		--swap-mib)
			[ "$#" -ge 2 ] || die "--swap-mib requires a value"
			swap_mib="$2"
			shift 2
			;;
		--router)
			[ "$#" -ge 2 ] || die "--router requires a value"
			router="$2"
			shift 2
			;;
		--url)
			[ "$#" -ge 2 ] || die "--url requires a value"
			router_url="$2"
			shift 2
			;;
		--tftp-host)
			[ "$#" -ge 2 ] || die "--tftp-host requires a value"
			tftp_host="$2"
			shift 2
			;;
		--tftp-client-ip)
			[ "$#" -ge 2 ] || die "--tftp-client-ip requires a value"
			tftp_client_ip="$2"
			shift 2
			;;
		*)
			die "unknown argument: $1"
			;;
	esac
done

if [ "$print_deps" -eq 1 ]; then
	[[ "$swap_mib" =~ ^[0-9]+$ ]] || die "--swap-mib must be an integer MiB value"
	swap_mib=$((10#$swap_mib))
	[ "$swap_mib" -gt 0 ] || die "--swap-mib must be greater than 0; this firmware requires USB swap"
	get_deps
	exit 0
fi

[ -n "$usb" ] || usage_die "--usb is required"

preflight

tftp_args=(--host "$tftp_host" --client-ip "$tftp_client_ip")
[ -z "$firmware" ] || tftp_args=("$firmware" "${tftp_args[@]}")

run_step "Step 1/3: TFTP firmware upload" \
	"$helper_dir/upload_firmware_tftp.sh" "${tftp_args[@]}"
note_after_tftp_upload

run_step "Step 2/3: prepare USB storage" \
	"$helper_dir/prepare_usb_overlay_swap.sh" "$usb" --overlay-mib "$overlay_mib" --swap-mib "$swap_mib"
pause_after_usb_prepare

run_step "Step 3/3: verify router USB overlay and swap" \
	"$helper_dir/verify_router_overlay_swap.sh" --router "$router" --url "$router_url"

echo
echo "WARNING: NEVER REMOVE the USB flash drive while the router is running."
echo "DONE: First install with USB overlay and swap completed."
echo "NEXT: Restore host networking if needed and setup your router at $router_url"
if command -v xdg-open >/dev/null 2>&1; then
	xdg-open "$router_url" >/dev/null 2>&1 || true
fi
echo
echo "Congratulations on giving this router and USB drive a second life!"
echo
