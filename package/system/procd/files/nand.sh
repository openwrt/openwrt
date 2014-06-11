#!/bin/sh
# Copyright (C) 2014 OpenWrt.org
#

. /lib/functions.sh

# combined-image uses 64k blocks
CI_BLKSZ=65536
# 'data' partition on NAND contains UBI
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

nand_upgrade_ubinized() {
	local upgrade_image="$1"
	local conf_tar="$2"
	local save_config="$3"
	local mtdnum="$( find_mtd_index "$CI_UBIPART" )"
	if [ ! "$mtdnum" ]; then
		echo "cannot find mtd device $CI_UBIPART"
		return 1;
	fi
	local mtddev="/dev/mtd${mtdnum}"
	ubidetach -p "${mtddev}" || true
	sync
	ubiformat "${mtddev}" -y -f "$upgrade_image"
	ubiattach -p "${mtddev}"
	sync
	if [ -f "$conf_tar" -a "$save_config" -eq 1 ]; then
		nand_restore_config "$conf_tar"
	fi
	return 0;
}

# get the first 4 bytes (magic) of a given file starting at offset in hex format
get_magic_long_at() {
	dd if="$2" skip=$1 bs=$CI_BLKSZ count=1 2>/dev/null | hexdump -v -n 4 -e '1/1 "%02x"'
}

identify() {
	local block;
	local magic=$( get_magic_long_at ${2:-0} "$1" )
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

nand_upgrade_combined_ubi() {
	local kernel_image="$1"
	local kernel_length=0
	local rootfs_image="$2"
	local rootfs_length=`ls -la $rootfs_image  | awk '{ print $5}')`
	local conf_tar="$3"
	local has_env="${4:-0}"

	local root_fs="$( identify "$rootfs_image" )"
	local mtdnum="$( find_mtd_index "$CI_UBIPART" )"
	local has_kernel=0
	
	[ -z "$kernel_image" ] || {
		has_kernel=1 
		kernel_length=`ls -la $kernel_image  | awk '{ print $5}')`
		echo "kernel length $kernel_length"
	}
	[ "$has_kernel" = 0 ] || echo "kernel is inside ubi"
	echo "rootfs type $root_fs, length $rootfs_length"

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
	if [ "$kern_ubivol" ]; then
		ubirmvol /dev/$ubidev -N kernel || true
	fi
	if [ "$root_ubivol" ]; then
		ubirmvol /dev/$ubidev -N rootfs || true
	fi
	if [ "$data_ubivol" ]; then
		ubirmvol /dev/$ubidev -N rootfs_data || true
	fi

	# update kernel
	if [ "$has_kernel" = "1" ]; then
		if ! ubimkvol /dev/$ubidev -N kernel -s $kernel_length; then
			echo "cannot create kernel volume"
			return 1;
		fi
	fi

	# update rootfs
	local root_size_param
	if [ "$root_fs" = "ubifs" ]; then
		root_size_param="-m"
	else
		root_size_param="-s $rootfs_length"
	fi
	if ! ubimkvol /dev/$ubidev -N rootfs $root_size_param; then
		echo "cannot create rootfs volume"
		return 1;
	fi

	# create rootfs_data for non-ubifs rootfs
	if [ "$root_fs" != "ubifs" ]; then
		if ! ubimkvol /dev/$ubidev -N rootfs_data -m; then
			echo "cannot initialize rootfs_data volume"
			return 1
		fi
	fi
	sync

	if [ "$has_kernel" = "1" ]; then
		local kern_ubivol="$( nand_find_volume $ubidev kernel )"
		ubiupdatevol /dev/$kern_ubivol -s $kernel_length $kernel_image
	fi

	local root_ubivol="$( nand_find_volume $ubidev rootfs )"
	ubiupdatevol /dev/$root_ubivol -s $rootfs_length $rootfs_image
	if [ -f "$conf_tar" ]; then
		nand_restore_config "$conf_tar"
	fi
	echo "sysupgrade successfull"
	return 0
}

nand_do_upgrade_stage1() {
	local board_name="$1"
	local tar_file="$2"
	local kernel_file=""
	local kernel_file=""

	tar xzf $tar_file -C /tmp/
	[ -f "/tmp/sysupgrade-$board_name/CONTROL" ] || {
		echo "failed to find /tmp/sysupgrade-$board_name/CONTROL"
		return 1
	}

	kernel_file=/tmp/sysupgrade-$board_name/kernel
	[ -f "$kernel_file" ] || {
		echo "$kernel_file is missing"
		return 1
	}

	rootfs_file=/tmp/sysupgrade-$board_name/root
	[ -f "$rootfs_file" ] || {
		echo "$rootfs_file is missing"
		return 1
	}
	
	echo -n /tmp/sysupgrade-$board_name > /tmp/sysupgrade-nand-folder
	cp /sbin/upgraded /tmp/

	return 0
}

nand_do_upgrade_stage2() {
	rootfs_file=$1/root
	kernel_file=$1/kernel
	config_file=$1/config

	[ -f $config_file ] || config_file=""

	. $1/CONTROL

	[ "$UBI_KERNEL" = "1" ] || {
		mtd write $kernel_file kernel
		kernel_file=""
	}
	nand_upgrade_combined_ubi "$kernel_file" "$rootfs_file" "$conf_tar" "$UBI_ENV"
	reboot -f
}

nand_do_upgrade() {
	[ $1 = "nand" ] && {
		[ -d "$2" ] && {
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
	[ -f /tmp/sysupgrade-nand-folder ] && {
		folder="$(cat /tmp/sysupgrade-nand-folder)"
		[ "$SAVE_CONFIG" = 1 -a -f "$CONF_TAR" ] &&
			cp $CONF_TAR $folder/config

		ubus call system nandupgrade "{\"folder\": \"$folder\" }"
		exit 0
	}
}
append sysupgrade_pre_upgrade nand_upgrade_stage1
