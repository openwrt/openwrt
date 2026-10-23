#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only

# Read the primaryboot flag for one named Qualcomm bootconfig entry.
ln6001_bootconfig_primaryboot() {
	local file="$1" wanted="$2" count i name value found=0

	[ -r "$file" ] || return 1
	[ "$(hexdump -v -n 4 -e '4/1 "%02x"' "$file")" = a0a1a2a3 ] || return 1
	[ "$(hexdump -v -s 332 -n 4 -e '4/1 "%02x"' "$file")" = b0b1b2b3 ] || return 1

	count="$(hexdump -v -s 8 -n 4 -e '1/4 "%u"' "$file")" || return 1
	case "$count" in
		''|*[!0-9]*) return 1 ;;
	esac
	[ "$count" -ge 1 ] && [ "$count" -le 16 ] || return 1

	i=0
	while [ "$i" -lt "$count" ]; do
		name="$(dd if="$file" bs=1 skip=$((12 + i * 20)) count=16 2>/dev/null |
			tr -d '\000')" || return 1
		if [ "$name" = "$wanted" ]; then
			[ "$found" -eq 0 ] || return 1
			value="$(hexdump -v -s $((28 + i * 20)) -n 4 \
				-e '1/4 "%u"' "$file")" || return 1
			case "$value" in
				0|1) ;;
				*) return 1 ;;
			esac
			found=1
		fi
		i=$((i + 1))
	done

	[ "$found" -eq 1 ] || return 1
	printf '%s\n' "$value"
}

ln6001_bootconfig_slot() {
	local hlos rootfs wififw

	hlos="$(ln6001_bootconfig_primaryboot "$1" '0:HLOS')" || return 1
	rootfs="$(ln6001_bootconfig_primaryboot "$1" rootfs)" || return 1
	wififw="$(ln6001_bootconfig_primaryboot "$1" '0:WIFIFW')" || return 1
	[ "$hlos" = "$rootfs" ] && [ "$hlos" = "$wififw" ] || return 1
	printf '%s\n' "$hlos"
}

ln6001_bootconfig_pair_slot() {
	local first second

	first="$(ln6001_bootconfig_slot "$1")" || return 1
	second="$(ln6001_bootconfig_slot "$2")" || return 1
	[ "$first" = "$second" ] || return 1
	printf '%s\n' "$first"
}

ln6001_check_partition() {
	local number="$1" label="$2" start="$3" size="$4"
	local sysfs="${5:-/sys/class/block}"
	local path="$sysfs/mmcblk0p$number"

	[ "$(sed -n 's/^DEVNAME=//p' "$path/uevent")" = "mmcblk0p$number" ] || return 1
	[ "$(sed -n 's/^PARTNAME=//p' "$path/uevent")" = "$label" ] || return 1
	[ "$(cat "$path/start")" = "$start" ] || return 1
	[ "$(cat "$path/size")" = "$size" ] || return 1
	[ "$(cat "$path/ro")" = 0 ] || return 1
}

# These are GPT sectors observed on the retail 8 GiB eMMC. Refuse other layouts.
ln6001_check_layout() {
	local sysfs="${1:-/sys/class/block}"

	ln6001_check_partition 3 '0:BOOTCONFIG' 4130 512 "$sysfs" &&
	ln6001_check_partition 4 '0:BOOTCONFIG1' 4642 512 "$sysfs" &&
	ln6001_check_partition 21 u_env 24098 512 "$sysfs" &&
	ln6001_check_partition 22 s_env 24610 512 "$sysfs" &&
	ln6001_check_partition 26 '0:WIFIFW' 27170 20480 "$sysfs" &&
	ln6001_check_partition 27 '0:WIFIFW_1' 47650 20480 "$sysfs" &&
	ln6001_check_partition 28 '0:HLOS' 68130 16384 "$sysfs" &&
	ln6001_check_partition 29 rootfs 84514 1024000 "$sysfs" &&
	ln6001_check_partition 30 '0:HLOS_1' 1108514 16384 "$sysfs" &&
	ln6001_check_partition 31 rootfs_1 1124898 1024000 "$sysfs"
}

ln6001_cmdline_slot() {
	local cmdline="${1:-/proc/cmdline}" sysfs="${2:-/sys/class/block}"
	local args arg root count=0 uuid a b

	[ -r "$cmdline" ] || return 1
	read -r args < "$cmdline"
	for arg in $args; do
		case "$arg" in
			root=*) root="${arg#root=}"; count=$((count + 1)) ;;
		esac
	done
	[ "$count" -eq 1 ] || return 1

	case "$root" in
		/dev/mmcblk0p29) printf '0\n'; return 0 ;;
		/dev/mmcblk0p31) printf '1\n'; return 0 ;;
		PARTUUID=*) uuid="${root#PARTUUID=}" ;;
		*) return 1 ;;
	esac

	printf '%s\n' "$uuid" | grep -Eqi \
		'^[0-9a-f]{8}(-[0-9a-f]{4}){3}-[0-9a-f]{12}$' || return 1
	uuid="$(printf '%s' "$uuid" | tr 'A-F' 'a-f')"
	[ "$uuid" != 00000000-0000-0000-0000-000000000000 ] || return 1
	a="$(sed -n 's/^PARTUUID=//p' "$sysfs/mmcblk0p29/uevent" | tr 'A-F' 'a-f')"
	b="$(sed -n 's/^PARTUUID=//p' "$sysfs/mmcblk0p31/uevent" | tr 'A-F' 'a-f')"
	[ -n "$a" ] && [ -n "$b" ] && [ "$a" != "$b" ] || return 1
	if [ "$uuid" = "$a" ]; then
		printf '0\n'
	elif [ "$uuid" = "$b" ]; then
		printf '1\n'
	else
		return 1
	fi
}

ln6001_bootenv_slot() {
	local boot_part ready recovery

	boot_part="$(fw_printenv -n boot_part 2>/dev/null)" || return 1
	ready="$(fw_printenv -n boot_part_ready 2>/dev/null)" || return 1
	recovery="$(fw_printenv -n auto_recovery 2>/dev/null)" || return 1
	[ "$ready" = 3 ] && [ "$recovery" = yes ] || return 1
	case "$boot_part" in
		1) printf '0\n' ;;
		2) printf '1\n' ;;
		*) return 1 ;;
	esac
}

ln6001_selected_slot() {
	local bootconfig bootenv

	ln6001_check_layout || return 1
	bootconfig="$(ln6001_bootconfig_pair_slot \
		/dev/mmcblk0p3 /dev/mmcblk0p4)" || return 1
	bootenv="$(ln6001_bootenv_slot)" || return 1
	[ "$bootconfig" = "$bootenv" ] || return 1
	printf '%s\n' "$bootconfig"
}

ln6001_active_slot() {
	local selected cmdline

	selected="$(ln6001_selected_slot)" || return 1
	cmdline="$(ln6001_cmdline_slot)" || return 1
	[ "$selected" = "$cmdline" ] || return 1
	printf '%s\n' "$selected"
}

ln6001_rambooted() {
	local args arg

	[ "$(cat /tmp/sysinfo/board_name)" = 'linksys,ln6001' ] || return 1
	[ "$(awk '$2 == "/" { print $1 " " $3 }' /proc/mounts)" = 'tmpfs tmpfs' ] || return 1
	read -r args < /proc/cmdline
	for arg in $args; do
		case "$arg" in
			root=*) return 1 ;;
		esac
	done
}

ln6001_upgrade_slot() {
	local selected cmdline

	selected="$(ln6001_selected_slot)" || return 1
	if cmdline="$(ln6001_cmdline_slot)"; then
		[ "$selected" = "$cmdline" ] || return 1
	else
		ln6001_rambooted || return 1
	fi
	printf '%s\n' "$selected"
}

ln6001_check_image_contents() {
	local image="$1" dir="$2" members expected control kernel_size root_size

	members="$(tar -tf "$image" 2>/dev/null)" || return 1
	expected="$(printf '%s\n' \
		'sysupgrade-linksys_ln6001/' \
		'sysupgrade-linksys_ln6001/CONTROL' \
		'sysupgrade-linksys_ln6001/kernel' \
		'sysupgrade-linksys_ln6001/root')"
	[ "$members" = "$expected" ] || return 1
	control="$(tar -xOf "$image" sysupgrade-linksys_ln6001/CONTROL)" || return 1
	[ "$control" = BOARD=linksys_ln6001 ] || return 1

	tar -xOf "$image" sysupgrade-linksys_ln6001/kernel > "$dir/kernel" || return 1
	tar -xOf "$image" sysupgrade-linksys_ln6001/root > "$dir/root" || return 1
	kernel_size="$(wc -c < "$dir/kernel")"
	root_size="$(wc -c < "$dir/root")"
	[ "$kernel_size" -gt 0 ] && [ "$kernel_size" -le 8388608 ] || return 1
	# The archive and extracted root both occupy tmpfs during sysupgrade.
	[ "$root_size" -gt 0 ] && [ "$root_size" -le 134217728 ] || return 1
	[ "$(hexdump -v -n 4 -e '4/1 "%02x"' "$dir/kernel")" = d00dfeed ] || return 1
	[ "$(hexdump -v -n 4 -e '4/1 "%02x"' "$dir/root")" = 68737173 ] || return 1
}

ln6001_check_image() {
	local dir rc

	dir="$(mktemp -d /tmp/ln6001-image.XXXXXX)" || return 1
	ln6001_check_image_contents "$1" "$dir"
	rc=$?
	rm -f "$dir/kernel" "$dir/root"
	rmdir "$dir"
	return "$rc"
}

ln6001_bootconfig_age() {
	local age

	ln6001_bootconfig_slot "$1" >/dev/null || return 1
	age="$(hexdump -v -s 4 -n 4 -e '1/4 "%u"' "$1")" || return 1
	case "$age" in
		''|*[!0-9]*) return 1 ;;
	esac
	printf '%s\n' "$age"
}

ln6001_bootconfig_offset() {
	local file="$1" wanted="$2" count i name offset found=

	count="$(hexdump -v -s 8 -n 4 -e '1/4 "%u"' "$file")" || return 1
	[ "$count" -ge 1 ] && [ "$count" -le 16 ] || return 1
	i=0
	while [ "$i" -lt "$count" ]; do
		name="$(dd if="$file" bs=1 skip=$((12 + i * 20)) count=16 2>/dev/null |
			tr -d '\000')" || return 1
		if [ "$name" = "$wanted" ]; then
			[ -z "$found" ] || return 1
			offset=$((28 + i * 20))
			found=1
		fi
		i=$((i + 1))
	done
	[ "$found" = 1 ] || return 1
	printf '%s\n' "$offset"
}

ln6001_write_le32() {
	local file="$1" offset="$2" value="$3" bytes

	[ -f "$file" ] && [ "$value" -ge 0 ] &&
		[ "$value" -le 4294967295 ] || return 1
	bytes="$(printf '\\%03o\\%03o\\%03o\\%03o' \
		$((value & 255)) $(((value >> 8) & 255)) \
		$(((value >> 16) & 255)) $(((value >> 24) & 255)))"
	printf '%b' "$bytes" | dd of="$file" bs=1 seek="$offset" count=4 \
		conv=notrunc 2>/dev/null
}

# Change only a regular-file snapshot; callers must validate before any device I/O.
ln6001_prepare_bootconfig() {
	local file="$1" slot="$2" age="$3" name offset

	[ -f "$file" ] || return 1
	case "$slot" in
		0|1) ;;
		*) return 1 ;;
	esac
	[ "$age" -ge 1 ] && [ "$age" -le 4294967294 ] || return 1
	ln6001_bootconfig_slot "$file" >/dev/null || return 1
	for name in '0:HLOS' rootfs '0:WIFIFW'; do
		offset="$(ln6001_bootconfig_offset "$file" "$name")" || return 1
		ln6001_write_le32 "$file" "$offset" "$slot" || return 1
	done
	ln6001_write_le32 "$file" 4 "$age" || return 1
	[ "$(ln6001_bootconfig_slot "$file")" = "$slot" ] || return 1
	[ "$(ln6001_bootconfig_age "$file")" = "$age" ]
}

ln6001_check_bootconfig_pair() {
	local dir rc=1 a b age_a age_b count_a count_b

	[ "$(ln6001_bootconfig_pair_slot "$1" "$2")" = "$3" ] || return 1
	count_a="$(hexdump -v -s 8 -n 4 -e '1/4 "%u"' "$1")" || return 1
	count_b="$(hexdump -v -s 8 -n 4 -e '1/4 "%u"' "$2")" || return 1
	[ "$count_a" = 8 ] && [ "$count_b" = 8 ] || return 1
	age_a="$(ln6001_bootconfig_age "$1")" || return 1
	age_b="$(ln6001_bootconfig_age "$2")" || return 1
	[ "$age_a" -lt 4294967294 ] && [ "$age_b" -lt 4294967294 ] || return 1

	dir="$(mktemp -d /tmp/ln6001-bootconfig.XXXXXX)" || return 1
	a="$dir/a"
	b="$dir/b"
	if dd if="$1" of="$a" bs=336 count=1 2>/dev/null &&
		dd if="$2" of="$b" bs=336 count=1 2>/dev/null &&
		[ "$(wc -c < "$a")" -eq 336 ] &&
		[ "$(wc -c < "$b")" -eq 336 ] &&
		ln6001_write_le32 "$a" 4 0 &&
		ln6001_write_le32 "$b" 4 0 &&
		cmp -s "$a" "$b"; then
		rc=0
	fi
	rm -f "$a" "$b"
	rmdir "$dir"
	return "$rc"
}

ln6001_preflight() {
	local slot

	slot="$(ln6001_upgrade_slot)" || return 1
	ln6001_check_bootconfig_pair /dev/mmcblk0p3 /dev/mmcblk0p4 "$slot" || return 1
	cmp -s /dev/mmcblk0p26 /dev/mmcblk0p27 || return 1
	ln6001_check_image "$1"
}

ln6001_write_verified() {
	local source="$1" device="$2" size expected actual

	size="$(wc -c < "$source")" || return 1
	expected="$(sha256sum "$source" | cut -d ' ' -f 1)" || return 1
	dd if="$source" of="$device" bs=65536 conv=notrunc 2>/dev/null || return 1
	sync
	actual="$(head -c "$size" "$device" | sha256sum | cut -d ' ' -f 1)" || return 1
	[ "$actual" = "$expected" ]
}

ln6001_write_overlay() {
	local root="$1" root_size="$2" backup="$3" blocks size expected actual

	[ $((root_size % 65536)) -eq 0 ] || return 1
	blocks=$((root_size / 512))
	dd if=/dev/zero of="$root" bs=512 seek="$blocks" count=8 \
		conv=notrunc 2>/dev/null || return 1
	sync
	[ -n "$backup" ] || return 0
	[ -f "$backup" ] || return 1
	size="$(wc -c < "$backup")" || return 1
	[ $((root_size + size)) -le 134217728 ] || return 1
	expected="$(sha256sum "$backup" | cut -d ' ' -f 1)" || return 1
	dd if="$backup" of="$root" bs=512 seek="$blocks" \
		conv=notrunc 2>/dev/null || return 1
	sync
	actual="$(dd if="$root" bs=512 skip="$blocks" 2>/dev/null |
		head -c "$size" | sha256sum | cut -d ' ' -f 1)" || return 1
	[ "$actual" = "$expected" ]
}

ln6001_write_sector_verified() {
	local source="$1" device="$2" readback="$3"

	[ "$(wc -c < "$source")" -eq 512 ] || return 1
	dd if="$source" of="$device" bs=512 count=1 \
		conv=notrunc 2>/dev/null || return 1
	sync
	dd if="$device" of="$readback" bs=512 count=1 2>/dev/null || return 1
	[ "$(wc -c < "$readback")" -eq 512 ] || return 1
	cmp -s "$source" "$readback"
}

ln6001_cleanup_upgrade() {
	local dir="$1" file

	for file in kernel root old0 old1 new0 new1 check0 check1 readback; do
		rm -f "$dir/$file"
	done
	rmdir "$dir"
}

ln6001_stage_inactive_slot() {
	local image="$1" dir="$2" kernel_dev="$3" root_dev="$4"
	local slot="$5" backup="$6" age0 age1 age root_size

	ln6001_check_image_contents "$image" "$dir" || return 1
	dd if=/dev/mmcblk0p3 of="$dir/old0" bs=512 count=1 2>/dev/null || return 1
	dd if=/dev/mmcblk0p4 of="$dir/old1" bs=512 count=1 2>/dev/null || return 1
	[ "$(wc -c < "$dir/old0")" -eq 512 ] || return 1
	[ "$(wc -c < "$dir/old1")" -eq 512 ] || return 1
	age0="$(ln6001_bootconfig_age "$dir/old0")" || return 1
	age1="$(ln6001_bootconfig_age "$dir/old1")" || return 1
	age="$age0"
	[ "$age1" -le "$age" ] || age="$age1"
	age=$((age + 1))
	cp "$dir/old0" "$dir/new0" || return 1
	cp "$dir/old1" "$dir/new1" || return 1
	ln6001_prepare_bootconfig "$dir/new0" "$slot" "$age" || return 1
	ln6001_prepare_bootconfig "$dir/new1" "$slot" "$age" || return 1

	root_size="$(wc -c < "$dir/root")" || return 1
	ln6001_write_verified "$dir/root" "$root_dev" || return 1
	ln6001_write_overlay "$root_dev" "$root_size" "$backup" || return 1
	ln6001_write_verified "$dir/kernel" "$kernel_dev" || return 1

	dd if=/dev/mmcblk0p3 of="$dir/check0" bs=512 count=1 2>/dev/null || ln6001_commit_failure
	dd if=/dev/mmcblk0p4 of="$dir/check1" bs=512 count=1 2>/dev/null || ln6001_commit_failure
	cmp -s "$dir/old0" "$dir/check0" &&
		cmp -s "$dir/old1" "$dir/check1" || ln6001_commit_failure
}

ln6001_commit_failure() {
	echo "LN6001 selector state uncertain; staying in RAM for serial recovery" >&2
	exit 1
}

ln6001_do_upgrade() {
	local image="$1" active target kernel_dev root_dev dir

	ln6001_preflight "$image" || return 1
	active="$(ln6001_upgrade_slot)" || return 1
	case "$active" in
		0) target=1; kernel_dev=/dev/mmcblk0p30; root_dev=/dev/mmcblk0p31 ;;
		1) target=0; kernel_dev=/dev/mmcblk0p28; root_dev=/dev/mmcblk0p29 ;;
		*) return 1 ;;
	esac
	dir="$(mktemp -d /tmp/ln6001-upgrade.XXXXXX)" || return 1
	if ! ln6001_stage_inactive_slot "$image" "$dir" "$kernel_dev" "$root_dev" \
		"$target" "$UPGRADE_BACKUP"; then
		ln6001_cleanup_upgrade "$dir"
		return 1
	fi

	ln6001_write_sector_verified "$dir/new0" /dev/mmcblk0p3 \
		"$dir/readback" || ln6001_commit_failure
	ln6001_write_sector_verified "$dir/new1" /dev/mmcblk0p4 \
		"$dir/readback" || ln6001_commit_failure
	[ "$(ln6001_bootconfig_pair_slot /dev/mmcblk0p3 /dev/mmcblk0p4)" = \
		"$target" ] || ln6001_commit_failure
	fw_setenv boot_part "$((target + 1))" || ln6001_commit_failure
	[ "$(ln6001_bootenv_slot)" = "$target" ] || ln6001_commit_failure
	ln6001_cleanup_upgrade "$dir"
}
