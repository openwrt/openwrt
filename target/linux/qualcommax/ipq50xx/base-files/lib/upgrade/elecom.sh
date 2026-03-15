. /lib/functions.sh

bootconfig_find_entry() {
	local cfgbin="$1"
	local partname="$2"
	local i part parts offset

	parts=$(hexdump -n 4 -s 8 -e '1/4 "%d"' "$cfgbin")
	# partition count: <= 10
	[ -z "$parts" ] || [ "$parts" = "0" ] || [ "$parts" -gt "10" ] && \
		return 1

	for i in $(seq 1 $parts); do
		offset=$((0xc + 0x14 * (i - 1)))
		part=$(dd if="$cfgbin" iflag=skip_bytes \
			skip=$offset bs=16 count=1 2>/dev/null)
		if [ "$part" = "$partname" ]; then
			printf "0x%08x" $offset
			return
		fi
	done

	return 1
}

# Read or update an entry in Qualcomm bootconfig partition
#
# parameters:
#   $1: partition name of bootconfig (ex.: "0:bootconfig", "0:bootconfig1", etc)
#   $2: entry name in bootconfig (ex.: "0:hlos", "rootfs", etc)
#   $3: index to set for the entry (0/1)
#
# operations:
#   read : bootconfig_rw_index <bootconfig> <entry>
#   write: bootconfig_rw_index <bootconfig> <entry> <index>
bootconfig_rw_index() {
	local bootcfg="$1"
	local partname="$2"
	local index="$3"
	local mtddev
	local offset
	local current

	if [ -z "$bootcfg" ] || [ -z "$partname" ]; then
		echo "no value specified for bootconfig or partition entry"
		return 1
	fi

	case "$index" in
	0|1|"") ;;
	*) echo "invalid bootconfig index specified \"$index\""; return 1 ;;
	esac

	mtddev="$(find_mtd_part $bootcfg)"
	[ -z "$mtddev" ] && \
		return 1

	dd if=$mtddev of=/tmp/${mtddev##*/} bs=1k

	offset=$(bootconfig_find_entry "/tmp/${mtddev##*/}" $partname) || return 1
	current=$(hexdump -n 4 -s $((offset + 0x10)) -e '1/4 "%d"' /tmp/${mtddev##*/})

	[ -z "$index" ] && \
		echo "$current" && return 0

	if [ "$current" != "$index" ]; then
		printf "\x$index" | \
			dd of=$mtddev conv=notrunc bs=1 seek=$((offset + 0x10))
	fi
}

# Qcom U-Boot always sets a name of current active partition to "rootfs" and
# inactive partition is named as "rootfs_1", in the smem partition table.
# When the second partition is active, "rootfs" and "rootfs_1" are swapped.
smempart_next_root() {
	local index="$1"
	local root_idx="$(find_mtd_index rootfs)"
	local root1_idx="$(find_mtd_index rootfs_1)"
	local root_offset root1_offset

	[ -z "$root_idx" ] || [ -z "$root1_idx" ] && \
		return 1

	root_offset=$(cat /sys/block/mtdblock$root_idx/device/offset)
	root1_offset=$(cat /sys/block/mtdblock$root1_idx/device/offset)

	case "$index" in
	0)
		[ "$root_offset" -lt "$root1_offset" ] && \
			echo "rootfs" || \
			echo "rootfs_1"
		;;
	1)
		[ "$root_offset" -lt "$root1_offset" ] && \
			echo "rootfs_1" || \
			echo "rootfs"
		;;
	*)
		echo "invalid index specified..."
		return 1
		;;
	esac
}

elecom_upgrade_prepare() {
	local index

	if ! index=$(bootconfig_rw_index "0:bootconfig" rootfs); then
		v "failed to read bootconfig index..."
		nand_do_upgrade_failed
	fi

	if ! CI_UBIPART=$(smempart_next_root $index); then
		v "failed to get next root..."
		nand_do_upgrade_failed
	fi

	v "next rootfs: $index (current: $CI_UBIPART)"
}
