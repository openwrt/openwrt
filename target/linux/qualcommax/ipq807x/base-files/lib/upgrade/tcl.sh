# based on target/linux/qualcommax/ipq50xx/base-files/lib/upgrade/elecom.sh

. /lib/functions.sh

bootconfig_find_entry() {
	local cfgbin="$1"
	local partname="$2"
	local i part parts offset

	parts=$(hexdump -n 4 -s 8 -e '1/4 "%d"' "$cfgbin")
	# partition count: <= 10
	[ -z "$parts" ] || [ "$parts" = "0" ] || [ "$parts" -gt "10" ] && \
		return 1

	i=1
	while [ $i -le $parts ]; do
		offset=$((0xc + 0x14 * (i - 1)))
		part=$(dd if="$cfgbin" iflag=skip_bytes \
			skip=$offset bs=16 count=1 2>/dev/null)
		if [ "$part" = "$partname" ]; then
			printf "0x%08x" $offset
			return
		fi
		i=$(( i + 1 ))
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

tcl_swap_active_root() {
	local index

	index=$(bootconfig_rw_index "0:BOOTCONFIG" rootfs)
	if [ -z "$index" ]; then
		v "failed to read bootconfig index..."
		nand_do_upgrade_failed
	fi

	if [ "$index" = "1" ]; then
		bootconfig_rw_index "0:BOOTCONFIG" rootfs 0
		bootconfig_rw_index "0:BOOTCONFIG1" rootfs 0
	else
		bootconfig_rw_index "0:BOOTCONFIG" rootfs 1
		bootconfig_rw_index "0:BOOTCONFIG1" rootfs 1
	fi
}

tcl_upgrade_prepare() {
	local delay

	delay=$(fw_printenv -n bootdelay)
	[ -z "$delay" ] || [ "$delay" -eq "0" ] && \
		fw_setenv bootdelay 3

	if [ -z "$UPGRADE_OPT_USE_CURR_PART" ]; then
		tcl_swap_active_root
		CI_UBIPART="rootfs_1"
	fi
}
