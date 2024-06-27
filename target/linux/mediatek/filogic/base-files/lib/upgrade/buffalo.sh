#
# Copyright (C) 2024 OpenWrt.org
#

. /lib/functions.sh

#
# * This function allocates 'kernel' and 'rootfs' UBI volumes as 'static' and writes the sysupgrade image to them.
#    - If the UBI volumes are allocated dynamically, u-boot displays an 'ECC Error', so allocate them statically as in the stock firmware.
#
buffalo_wsr_6000ax6_do_upgrade() {
	local tar_file="$1"
	local gz="$(identify_if_gzip "$tar_file")"

	CI_UBIPART="ubi"

	local board_dir="$(tar t${gz}f "$tar_file" | grep -m 1 '^sysupgrade-.*/$')"
	board_dir="${board_dir%/}"

	local kernel_length=$( (tar xO${gz}f "$tar_file" "$board_dir/kernel" | wc -c) 2> /dev/null)
	[ "$kernel_length" = 0 ] && return 1
	local rootfs_length=$( (tar xO${gz}f "$tar_file" "$board_dir/root" | wc -c) 2> /dev/null)
	[ "$rootfs_length" = 0 ] && return 1

	local mtdnum="$( find_mtd_index "$CI_UBIPART" )"
	if [ ! "$mtdnum" ]; then
		echo "unable to find mtd partition $CI_UBIPART"
		return 1
	fi

	ubidetach -m "$mtdnum"
	ubiformat /dev/mtd$mtdnum -y

	ubiattach -m "$mtdnum"
	sync
	local ubidev="$( nand_find_ubi "$CI_UBIPART" )"

	if ! ubimkvol /dev/$ubidev -N kernel -s $kernel_length -t static; then
		echo "cannot create kernel volume"
		return 1;
	fi
	if ! ubimkvol /dev/$ubidev -N rootfs -s $rootfs_length -t static; then
		echo "cannot create rootfs volume"
		return 1;
	fi
	if ! ubimkvol /dev/$ubidev -N rootfs_data -m; then
		echo "cannot initialize rootfs_data volume"
		return 1
	fi
	sync

	local kern_ubivol="$( nand_find_volume $ubidev "$CI_KERNPART" )"
	tar xO${gz}f "$tar_file" "$board_dir/kernel" | \
		ubiupdatevol /dev/$kern_ubivol -s "$kernel_length" -

	local root_ubivol="$( nand_find_volume $ubidev "$CI_ROOTPART" )"
	tar xO${gz}f "$tar_file" "$board_dir/root" | \
		ubiupdatevol /dev/$root_ubivol -s "$rootfs_length" -
	sync

	return 0
}
