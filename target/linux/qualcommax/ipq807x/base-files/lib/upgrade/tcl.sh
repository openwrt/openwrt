# based on target/linux/qualcommax/ipq50xx/base-files/lib/upgrade/elecom.sh

. /lib/functions.sh
. /lib/functions/bootconfig.sh

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
	local mtdidx
	local tempfile
	local current

	if [ -z "$bootcfg" ] || [ -z "$partname" ]; then
		echo "no value specified for bootconfig or partition entry"
		return 1
	fi

	case "$index" in
	0|1|"") ;;
	*) echo "invalid bootconfig index specified \"$index\""; return 1 ;;
	esac

	mtdidx=$(find_mtd_index "$bootcfg")
	[ ! "$mtdidx" ] && {
		echo "cannot find mtd index for $partname"
		return 1
	}

	tempfile=/tmp/mtd"$mtdidx".bin
	dd if=/dev/mtd"$mtdidx" of="$tempfile" bs=1 count=336
	[ $? -ne 0 ] || [ ! -f "$tempfile" ] && {
		echo "failed to create a temp copy of /dev/mtd$mtdidx"
		return 1
	}

	current=$(get_bootconfig_primaryboot "$tempfile" "$partname")

	if [ -z "$index" ]; then
		echo "$current"
	elif [ "$current" != "$index" ]; then
		set_bootconfig_primaryboot "$tempfile" "$partname" "$index"
		mtd write "$tempfile" /dev/mtd"$mtdidx" 2>/dev/null
		[ $? -ne 0 ] && {
			echo "failed to write temp copy back to /dev/mtd$mtdidx"
			return 1
		}
	fi
	
	rm "$tempfile"
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
