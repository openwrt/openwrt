FIT_DT_ROOTDISK="/sys/firmware/devicetree/base/chosen/rootdisk"
FIT_PROD_NAMES="production fit firmware"

export_fitblk_bootdev() {
	[ -e "$FIT_DT_ROOTDISK" ] || return

	local rootdisk="$(cat "$FIT_DT_ROOTDISK")"
	local handle bootdev

	for handle in /sys/class/mtd/mtd*/of_node/volumes/*/phandle; do
		[ ! -e "$handle" ] && continue
		if [ "$rootdisk" = "$(cat "$handle")" ]; then
			if [ -e "${handle%/phandle}/volname" ]; then
				export CI_KERNPART="$(cat "${handle%/phandle}/volname")"
			elif [ -e "${handle%/phandle}/volid" ]; then
				export CI_KERNVOLID="$(cat "${handle%/phandle}/volid")"
			else
				return
			fi
			export CI_UBIPART="$(cat "${handle%%/of_node*}/name")"
			export CI_METHOD="ubi"
			return
		fi
	done

	for handle in /sys/class/mtd/mtd*/of_node/phandle; do
		[ ! -e "$handle" ] && continue
		if [ "$rootdisk" = "$(cat $handle)" ]; then
			bootdev="${handle%/of_node/phandle}"
			bootdev="${bootdev#/sys/class/mtd/}"
			export PART_NAME="/dev/$bootdev"
			export CI_METHOD="default"
			return
		fi
	done

	for handle in /sys/class/block/*/of_node/phandle; do
		[ ! -e "$handle" ] && continue
		if [ "$rootdisk" = "$(cat $handle)" ]; then
			bootdev="${handle%/of_node/phandle}"
			bootdev="${bootdev#/sys/class/block/}"
			export EMMC_KERN_DEV="/dev/$bootdev"
			export CI_METHOD="emmc"
			return
		fi
	done
}

fit_find_production() {
	local matchdev="$1"
	local __var="$2"
	local name ubidev __proddev

	for name in $FIT_PROD_NAMES; do
		case "$matchdev" in
		ubiblock[0-9]*_[0-9]*)
			ubidev="ubi${matchdev#ubiblock}"
			ubidev="${ubidev%%_*}"
			__proddev="$(nand_find_volume "$ubidev" "$name")" || continue
			[ -n "$__proddev" ] || continue
			if [ -e "/dev/ubiblock${__proddev#ubi}" ]; then
				export "$__var=/dev/ubiblock${__proddev#ubi}"
			else
				export "$__var=/dev/$__proddev"
			fi
			return 0
			;;
		ubi[0-9]*_[0-9]*)
			ubidev="${matchdev%%_*}"
			__proddev="$(nand_find_volume "$ubidev" "$name")" || continue
			[ -n "$__proddev" ] || continue
			export "$__var=/dev/$__proddev"
			return 0
			;;
		mtdblock[0-9]*)
			__proddev="$(find_mtd_part "$name")"
			[ -n "$__proddev" ] || continue
			export "$__var=$__proddev"
			return 0
			;;
		*)
			export_blk_part __proddev "$name" "${matchdev%%p*}"
			[ -n "$__proddev" ] || continue
			export "$__var=$__proddev"
			return 0
			;;
		esac
	done
	return 1
}

fit_export_bootdev() {
	local fit_bootdev="$1"
	local volbase volsysfs ubidev mtdnum

	case "$fit_bootdev" in
	/dev/ubiblock[0-9]*_[0-9]*|/dev/ubi[0-9]*_[0-9]*)
		volbase="${fit_bootdev##*/}"
		case "$volbase" in
			ubiblock*) volsysfs="ubi${volbase#ubiblock}" ;;
			*)         volsysfs="$volbase" ;;
		esac
		ubidev="${volsysfs%%_*}"
		export CI_KERNPART="$(cat "/sys/class/ubi/${volsysfs}/name" 2>/dev/null)"
		mtdnum="$(cat "/sys/class/ubi/${ubidev}/mtd_num" 2>/dev/null)"
		[ -n "$mtdnum" ] && \
			export CI_UBIPART="$(cat "/sys/class/mtd/mtd${mtdnum}/name" 2>/dev/null)"
		export CI_METHOD="ubi"
		;;
	/dev/mtdblock[0-9]*)
		export PART_NAME="$fit_bootdev"
		export CI_METHOD="default"
		;;
	/dev/mmcblk[0-9]*)
		export EMMC_KERN_DEV="$fit_bootdev"
		export CI_METHOD="emmc"
		;;
	*)
		return 1
		;;
	esac
}

fit_scan_bootdev() {
	local devpath devname fstype bootdev

	fstype="$(rootfs_type)"

	# Scan block devices (mmcblk*, ubiblock*, mtdblock*)
	for devpath in /sys/class/block/*/uevent; do
		[ -e "$devpath" ] || continue
		devname="${devpath%/uevent}"
		devname="${devname##*/}"
		fit_set_uuid -m -f "/dev/$devname" >/dev/null 2>&1 || continue
		if [ "$fstype" = "tmpfs" ]; then
			fit_find_production "$devname" bootdev || continue
		else
			bootdev="/dev/$devname"
		fi
		fit_export_bootdev "$bootdev"
		return 0
	done

	# Scan UBI volumes (character devices, not in /sys/class/block)
	for devpath in /sys/class/ubi/ubi[0-9]*_[0-9]*/; do
		[ -d "$devpath" ] || continue
		devname="${devpath%/}"
		devname="${devname##*/}"
		fit_set_uuid -m -f "/dev/$devname" >/dev/null 2>&1 || continue
		if [ "$fstype" = "tmpfs" ]; then
			fit_find_production "$devname" bootdev || continue
		else
			bootdev="/dev/$devname"
		fi
		fit_export_bootdev "$bootdev"
		return 0
	done

	return 1
}

fit_do_upgrade() {
	if [ -e "$FIT_DT_ROOTDISK" ]; then
		export_fitblk_bootdev
	else
		fit_scan_bootdev
	fi
	[ -n "$CI_METHOD" ] || return 1
	[ -e /dev/fit0 ] && fitblk /dev/fit0
	[ -e /dev/fitrw ] && fitblk /dev/fitrw

	fit_set_uuid -f "$1" 2>&1 >/dev/null

	case "$CI_METHOD" in
	emmc)
		emmc_do_upgrade "$1"
		;;
	default)
		default_do_upgrade "$1"
		;;
	ubi)
		nand_do_upgrade "$1"
		;;
	esac
}

fit_check_image() {
	local magic="$(get_magic_long "$1")"
	[ "$magic" != "d00dfeed" ] && {
		echo "Invalid image type."
		return 74
	}

	fit_check_sign -f "$1" >/dev/null || return 74
}
