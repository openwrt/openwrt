#
# Copyright (C) 2023 Mauri Sandberg
#

# The vendor UBI is split in volumes 0-3. Volumes 0 and 1 contain U-Boot
# environments env1 and env2, respectively. The vendor root file systems
# are in volumes 2 (rootfs_0) and 3 (rootfs_1). Drop the two roots and
# explicitly use rootfs_0 as a boot partition that contains the dtb and the
# OpenWrt kernel. This is because the vendor U-Boot expects to find them there.
# Then continue upgrade with the default method - a SquashFS rootfs will be
# installed and the rest of UBI will be used as an overlay.

# The 'kernel' inside the sysupgrage.tar is an UBIFS image that contains
# /boot/dtb and /boot/kernel. The 'root' is an OpenWrt SquashFS root

. /lib/functions.sh
. /lib/upgrade/nand.sh

inteno_do_upgrade () {
	local tar_file=$1
	local cmd=cat
	# WARNING: This fails if tar contains more than one 'sysupgrade-*' directory.
	local board_dir="$(tar tf "$tar_file" | grep -m 1 '^sysupgrade-.*/$')"
	board_dir="${board_dir%/}"
	tar -xaf "$tar_file"

	# get the size of the new bootfs
	local _bootfs_size=$(wc -c < "$board_dir/kernel")
	[ -n "$_bootfs_size" -a "$_bootfs_size" -gt "0" ] || nand_do_upgrade_failed

	# remove existing rootfses and recreate rootfs_0
	ubirmvol /dev/ubi0 --name=rootfs_0 > /dev/null 2>&1
	ubirmvol /dev/ubi0 --name=rootfs_1 > /dev/null 2>&1
	ubirmvol /dev/ubi0 --name=rootfs > /dev/null 2>&1
	ubirmvol /dev/ubi0 --name=rootfs_data > /dev/null 2>&1
	ubimkvol /dev/ubi0 --type=static --size=${_bootfs_size} --name=rootfs_0

	# update the rootfs_0 contents
	local _kern_ubivol=$( nand_find_volume "ubi0" "rootfs_0" )
	ubiupdatevol "/dev/$_kern_ubivol" "$board_dir/kernel"

	fw_setenv root_vol rootfs_0
	fw_setenv boot_cnt_primary 0
	fw_setenv boot_cnt_alt 0

	# proceed to upgrade the default way
	CI_KERNPART=none
	nand_do_upgrade "$1"
}
