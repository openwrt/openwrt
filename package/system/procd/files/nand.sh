#!/bin/sh
# Copyright (C) 2014 OpenWrt.org
#

. /lib/functions.sh

# 'ubi' partition on NAND contains UBI
CI_UBIPART="ubi"

nand_find_volume() {
	local ubidevdir ubivoldir
	ubidevdir="/sys/devices/virtual/ubi/$1"
	[ ! -d "$ubidevdir" ] && return 1
	for ubivoldir in $ubidevdir/${1}_*; do
		[ ! -d "$ubivoldir" ] && continue
		if [ "$( cat $ubivoldir/name )" = "$2" ]; then
			basename $ubivoldir
			return 0
		fi
	done
}

nand_find_ubi() {
	local ubidevdir ubidev mtdnum
	mtdnum="$( find_mtd_index $1 )"
	[ ! "$mtdnum" ] && return 1
	for ubidevdir in /sys/devices/virtual/ubi/ubi*; do
		[ ! -d "$ubidevdir" ] && continue
		cmtdnum="$( cat $ubidevdir/mtd_num )"
		[ ! "$mtdnum" ] && continue
		if [ "$mtdnum" = "$cmtdnum" ]; then
			ubidev=$( basename $ubidevdir )
			echo $ubidev
			return 0
		fi
	done
}

get_magic_long() {
	dd if="$1" skip=$2 bs=4 count=1 2>/dev/null | hexdump -v -n 4 -e '1/1 "%02x"'
}

get_magic_long_tar() {
	( tar xf $1 $2 -O | dd bs=4 count=1 | hexdump -v -n 4 -e '1/1 "%02x"') 2> /dev/null
}

identify_magic() {
	local magic=$1
	case "$magic" in
		"55424923")
			echo "ubi"
			;;
		"31181006")
			echo "ubifs"
			;;
		"68737173")
			echo "squashfs"
			;;
		"d00dfeed")
			echo "fit"
			;;
		"4349"*)
			echo "combined"
			;;
		*)
			echo "unknown $magic"
			;;
	esac
}


identify() {
	identify_magic $(get_magic_long "$1" "${2:-0}")
}

identify_tar() {
	identify_magic $(get_magic_long_tar "$1" "$2")
}

nand_restore_config() {
	sync
	local ubidev=$( nand_find_ubi $CI_UBIPART )
	local ubivol="$( nand_find_volume $ubidev rootfs_data )"
	[ ! "$ubivol" ] &&
		ubivol="$( nand_find_volume $ubidev rootfs )"
	mkdir /tmp/new_root
	if ! mount -t ubifs /dev/$ubivol /tmp/new_root; then
		echo "mounting ubifs $ubivol failed"
		rmdir /tmp/new_root
		return 1
	fi
	mv "$1" "/tmp/new_root/sysupgrade.tgz"
	umount /tmp/new_root
	sync
	rmdir /tmp/new_root
}

nand_upgrade_prepare_ubi() {
	local rootfs_length="$1"
	local rootfs_type="$2"
	local has_kernel="${3:-0}"
	local has_env="${4:-0}"

	local mtdnum="$( find_mtd_index "$CI_UBIPART" )"
	if [ ! "$mtdnum" ]; then
		echo "cannot find ubi mtd partition $CI_UBIPART"
		return 1
	fi

	local ubidev="$( nand_find_ubi "$CI_UBIPART" )"
	if [ ! "$ubidev" ]; then
		ubiattach -m "$mtdnum"
		sync
		ubidev="$( nand_find_ubi "$CI_UBIPART" )"
	fi

	if [ ! "$ubidev" ]; then
		ubiformat /dev/mtd$mtdnum -y
		ubiattach -m "$mtdnum"
		sync
		ubidev="$( nand_find_ubi "$CI_UBIPART" )"
	 	[ -z "$has_env" ] || {
			ubimkvol /dev/$ubidev -n 0 -N ubootenv -s 1MiB
			ubimkvol /dev/$ubidev -n 1 -N ubootenv2 -s 1MiB
		}
	fi

	local kern_ubivol="$( nand_find_volume $ubidev kernel )"
	local root_ubivol="$( nand_find_volume $ubidev rootfs )"
	local data_ubivol="$( nand_find_volume $ubidev rootfs_data )"

	# remove ubiblock device of rootfs
	local root_ubiblk="ubiblock${root_ubivol:3}"
	if [ "$root_ubivol" -a -e "/dev/$root_ubiblk" ]; then
		echo "removing $root_ubiblk"
		if ! ubiblock -r /dev/$root_ubivol; then
			echo "cannot remove $root_ubiblk"
			return 1;
		fi
	fi

	# kill volumes
	[ "$kern_ubivol" ] && ubirmvol /dev/$ubidev -N kernel || true
	[ "$root_ubivol" ] && ubirmvol /dev/$ubidev -N rootfs || true
	[ "$data_ubivol" ] && ubirmvol /dev/$ubidev -N rootfs_data || true

	# update kernel
	if [ "$has_kernel" = "1" ]; then
		if ! ubimkvol /dev/$ubidev -N kernel -s $kernel_length; then
			echo "cannot create kernel volume"
			return 1;
		fi
	fi

	# update rootfs
	local root_size_param
	if [ "$rootfs_type" = "ubifs" ]; then
		root_size_param="-m"
	else
		root_size_param="-s $rootfs_length"
	fi
	if ! ubimkvol /dev/$ubidev -N rootfs $root_size_param; then
		echo "cannot create rootfs volume"
		return 1;
	fi

	# create rootfs_data for non-ubifs rootfs
	if [ "$rootfs_type" != "ubifs" ]; then
		if ! ubimkvol /dev/$ubidev -N rootfs_data -m; then
			echo "cannot initialize rootfs_data volume"
			return 1
		fi
	fi
	sync
	return 0
}

nand_do_upgrade_success() {
	local conf_tar="/tmp/sysupgrade.tgz"
	
	sync
	[ -f "$conf_tar" ] && nand_restore_config "$conf_tar"
	echo "sysupgrade successfull"
	reboot -f
}

nand_upgrade_ubinized() {
	local ubi_file="$1"
	local mtdnum="$(find_mtd_index "$CI_UBIPART")"

	[ ! "$mtdnum" ] && {
		CI_UBIPART="rootfs"
		mtdnum="$(find_mtd_index "$CI_UBIPART")"
	}

	if [ ! "$mtdnum" ]; then
		echo "cannot find mtd device $CI_UBIPART"
		reboot -f
	fi

	local mtddev="/dev/mtd${mtdnum}"
	ubidetach -p "${mtddev}" || true
	sync
	ubiformat "${mtddev}" -y -f "${ubi_file}"
	ubiattach -p "${mtddev}"
	nand_do_upgrade_success
}

nand_upgrade_ubifs() {
	local rootfs_length=`(cat $1 | wc -c) 2> /dev/null`

	nand_upgrade_prepare_ubi "$rootfs_length" "ubifs" "0" "0"
	
	local ubidev="$( nand_find_ubi "$CI_UBIPART" )"
	local root_ubivol="$(nand_find_volume $ubidev rootfs)"
	ubiupdatevol /dev/$root_ubivol -s $rootfs_length $1

	nand_do_upgrade_success
}

nand_upgrade_tar() {
	local tar_file="$1"
	local board_name="$(cat /tmp/sysinfo/board_name)"
	local kernel_mtd="$(find_mtd_index kernel)"

	local kernel_length=`(tar xf $tar_file sysupgrade-$board_name/kernel -O | wc -c) 2> /dev/null`
	local rootfs_length=`(tar xf $tar_file sysupgrade-$board_name/root -O | wc -c) 2> /dev/null`

	local rootfs_type="$(identify_tar "$tar_file" root)"

	local has_kernel=1
	local has_env=0

	[ "kernel_length" = 0 -o -z "$kernel_mtd" ] || {
		tar xf $tar_file sysupgrade-$board_name/kernel -O | mtd write - kernel
	}
	[ "kernel_length" = 0 -o ! -z "$kernel_mtd" ] && has_kernel=0

	nand_upgrade_prepare_ubi "$rootfs_length" "$rootfs_type" "$has_kernel" "$has_env"

	local ubidev="$( nand_find_ubi "$CI_UBIPART" )"
	[ "$has_kernel" = "1" ] && {
		local kern_ubivol="$(nand_find_volume $ubidev kernel)"
	 	tar xf $tar_file sysupgrade-$board_name/kernel -O | \
			ubiupdatevol /dev/$kern_ubivol -s $kernel_length -
	}

	local root_ubivol="$(nand_find_volume $ubidev rootfs)"
	tar xf $tar_file sysupgrade-$board_name/root -O | \
		ubiupdatevol /dev/$root_ubivol -s $rootfs_length -

	nand_do_upgrade_success
}

nand_do_upgrade_stage2() {
	local file_type=$(identify $1)

	[ ! "$(find_mtd_index "$CI_UBIPART")" ] && CI_UBIPART="rootfs"

	[ "$file_type" == "ubi" ] && nand_upgrade_ubinized $1
	[ "$file_type" == "ubifs" ] && nand_upgrade_ubifs $1
	nand_upgrade_tar $1
}

nand_upgrade_stage2() {
	[ $1 = "nand" ] && {
		[ -f "$2" ] && {
			touch /tmp/sysupgrade

			killall -9 telnetd
			killall -9 dropbear
			killall -9 ash

			kill_remaining TERM
			sleep 3
			kill_remaining KILL

			sleep 1

			if [ -n "$(rootfs_type)" ]; then
				v "Switching to ramdisk..."
				run_ramfs ". /lib/functions.sh; include /lib/upgrade; nand_do_upgrade_stage2 $2"
			else
				nand_do_upgrade_stage2 $2
			fi
			return 0
		}
		echo "Nand upgrade failed"
		exit 1
	}
}

nand_upgrade_stage1() {
	[ -f /tmp/sysupgrade-nand-path ] && {
		path="$(cat /tmp/sysupgrade-nand-path)"
		[ "$SAVE_CONFIG" != 1 -a -f "$CONF_TAR" ] &&
			rm $CONF_TAR

		ubus call system nandupgrade "{\"path\": \"$path\" }"
		exit 0
	}
}
append sysupgrade_pre_upgrade nand_upgrade_stage1

nand_do_platform_check() {
	local board_name="$1"
	local tar_file="$2"
	local control_length=`(tar xf $tar_file sysupgrade-$board_name/CONTROL -O | wc -c) 2> /dev/null`
	local file_type="$(identify $2)"

	[ "$control_length" = 0 -a "$file_type" != "ubi" -a "$file_type" != "ubifs" ] && {
		echo "Invalid sysupgrade file."
		return 1
	}

	echo -n $2 > /tmp/sysupgrade-nand-path
	cp /sbin/upgraded /tmp/

	return 0
}
