#
# Copyright (C) 2014-2015 OpenWrt.org
#

find_mtd_part_arm() {
	local INDEX=$(find_mtd_index "$1")
	local PREFIX=/dev/mtd

	[ -d /dev/mtd ] && PREFIX=/dev/mtd/
	echo "${INDEX:+$PREFIX$INDEX}"
}

platform_do_upgrade_smem() {
	local kernel_mtd="$(find_mtd_part_arm "$KERNEL_NAME")"
	local rootfs_mtd="$(find_mtd_part_arm "$ROOTFS_NAME")"

	dd if="$1" of=/tmp/kernel bs=1k count=4096
	dd if="$1" of=/tmp/rootfs bs=1k skip=4096
	sync

	mtd write /tmp/kernel "$kernel_mtd"

	if [ "$SAVE_CONFIG" -eq 1 ]; then
		mtd -j "$CONF_TAR" write /tmp/rootfs "$rootfs_mtd"
	else
		mtd write /tmp/rootfs "$rootfs_mtd"
	fi
}