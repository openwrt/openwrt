#!/usr/bin/env bash
set -euo pipefail
PATH="/usr/sbin:/sbin:$PATH"

usage() {
	cat <<'EOF'
Usage:
  prepare_usb_overlay_swap.sh /dev/sdX [--overlay-mib N] [--swap-mib N]

Prepares a USB flash drive for router overlay and swap.

Layout:
  partition 1: ext4 label overlay
  partition 2: swap label swap
  partition 3: ext4 label data, remaining space

Defaults:
  --overlay-mib 512
  --swap-mib     128

Required host utilities:
  ./scripts/prepare_usb_overlay_swap.sh --get-deps [--swap-mib N]

Safety:
  - accepts only whole /dev/sdX disks reported by lsblk as TRAN=usb
  - refuses partition paths such as /dev/sdX1
  - refuses any partition already active as swap
  - waits until you type FORMAT before writing

Example:
  lsblk -o NAME,SIZE,MODEL,TRAN,MOUNTPOINTS
  ./scripts/prepare_usb_overlay_swap.sh /dev/sdb --overlay-mib 512 --swap-mib 128
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
	printf '%s\n' eject findmnt lsblk mkfs.ext4 mkswap parted partprobe sleep sync umount
	if [ "$EUID" -ne 0 ]; then
		printf '%s\n' sudo
	fi
}

settle_devices() {
	"${SUDO[@]}" partprobe "$dev"
	sleep 2
}

unmount_partitions() {
	local part
	while IFS= read -r part; do
		[ -n "$part" ] || continue
		if findmnt -rn -S "$part" >/dev/null; then
			echo "Unmounting $part"
			"${SUDO[@]}" umount "$part"
		fi
	done < <(lsblk -nrpo PATH "$dev")
}

has_active_swap() {
	local swapdev rest
	[ -r /proc/swaps ] || return 1
	while read -r swapdev rest; do
		[[ "$swapdev" =~ ^${dev}[0-9]+$ ]] && return 0
	done < /proc/swaps
	return 1
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

dev=""
overlay_mib=512
swap_mib=128
print_deps=0

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
		/dev/*)
			[ -z "$dev" ] || die "device specified more than once"
			dev="$1"
			shift
			;;
		*)
			die "unknown argument: $1"
			;;
	esac
done

if [ "$print_deps" -eq 1 ]; then
	get_deps
	exit 0
fi

[ -n "$dev" ] || {
	usage >&2
	exit 2
}

[[ "$overlay_mib" =~ ^[0-9]+$ ]] || die "--overlay-mib must be an integer MiB value"
overlay_mib=$((10#$overlay_mib))
[ "$overlay_mib" -gt 0 ] || die "--overlay-mib must be positive"
[[ "$swap_mib" =~ ^[0-9]+$ ]] || die "--swap-mib must be an integer MiB value"
swap_mib=$((10#$swap_mib))
[ "$swap_mib" -gt 0 ] || die "--swap-mib must be greater than 0; this firmware requires USB swap"

[ -b "$dev" ] || die "$dev is not a block device"

if [ "$EUID" -eq 0 ]; then
	SUDO=()
else
	SUDO=(sudo)
fi

require_cmds $(get_deps)

case "$dev" in
	/dev/sd[a-z]) ;;
	*)
		die "refusing non-/dev/sdX path: $dev; use a whole USB disk such as /dev/sdb"
		;;
esac

tran="$(lsblk -dn -o TRAN "$dev")"
tran="${tran//[[:space:]]/}"
[ "$tran" = "usb" ] || die "$dev is not reported as USB by lsblk (TRAN=$tran)"

disk_mib="$(disk_size_mib "$dev")"
requested_mib=$((1 + overlay_mib + swap_mib))
[ "$requested_mib" -lt "$disk_mib" ] || {
	die "overlay+swap layout is too large: needs more than ${requested_mib} MiB including 1 MiB start offset, disk is ${disk_mib} MiB"
}

if has_active_swap; then
	die "$dev has an active swap partition; run swapoff for that partition first"
fi

overlay_start=1
overlay_end=$((overlay_start + overlay_mib))
swap_start="$overlay_end"
swap_end=$((swap_start + swap_mib))
dev_name="${dev##*/}"

overlay_part="${dev}1"

echo "Current layout on $dev:"
lsblk -o NAME,SIZE,FSTYPE,LABEL,MODEL,TRAN,MOUNTPOINTS "$dev"
echo
echo "Planned layout on $dev after FORMAT:"
printf '%-8s %-12s %-8s %-8s %s\n' "NAME" "SIZE" "FSTYPE" "LABEL" "PURPOSE"
printf '%-8s %-12s %-8s %-8s %s\n' "$dev_name" "${disk_mib}MiB" "gpt" "-" "USB disk"
printf '%-8s %-12s %-8s %-8s %s\n' "${dev_name}1" "${overlay_mib}MiB" "ext4" "overlay" "router overlay"
swap_part="${dev}2"
data_part="${dev}3"
printf '%-8s %-12s %-8s %-8s %s\n' "${dev_name}2" "${swap_mib}MiB" "swap" "swap" "router swap"
printf '%-8s %-12s %-8s %-8s %s\n' "${dev_name}3" "remaining" "ext4" "data" "user data"
echo
echo "WARNING: ALL EXISTING DATA on $dev will be DESTROYED !!!"
echo
while true; do
	read -r -p "Type FORMAT to continue, or press Ctrl+C to abort: " typed
	[ "$typed" = "FORMAT" ] && break
	echo "Typed '$typed', expected FORMAT."
done

unmount_partitions

disk_mib="$(disk_size_mib "$dev")"
[ "$requested_mib" -lt "$disk_mib" ] || {
	die "overlay+swap layout is too large after unmount: needs more than ${requested_mib} MiB including 1 MiB start offset, disk is ${disk_mib} MiB"
}

"${SUDO[@]}" parted -s "$dev" mklabel gpt
"${SUDO[@]}" parted -s "$dev" mkpart overlay ext4 "${overlay_start}MiB" "${overlay_end}MiB"
"${SUDO[@]}" parted -s "$dev" mkpart swap linux-swap "${swap_start}MiB" "${swap_end}MiB"
"${SUDO[@]}" parted -s "$dev" mkpart data ext4 "${swap_end}MiB" 100%
settle_devices

"${SUDO[@]}" mkfs.ext4 -F -L overlay "$overlay_part"
"${SUDO[@]}" mkswap -L swap "$swap_part"
"${SUDO[@]}" mkfs.ext4 -F -L data "$data_part"
sync
settle_devices

echo
echo "USB prepared:"
lsblk -o NAME,SIZE,FSTYPE,LABEL,UUID,MOUNTPOINTS "$dev"
echo
echo "Finalizing USB device: $dev"
sync
unmount_partitions
"${SUDO[@]}" eject "$dev"
echo
echo "DONE: USB storage prepared and ejected: $dev"
echo "WARNING: NEVER REMOVE the USB flash drive while the router is running."
echo "NEXT: insert the USB flash before booting the router into the new firmware."
echo
