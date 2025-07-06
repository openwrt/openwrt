# Copyright (C) 2011 OpenWrt.org

PART_NAME=firmware

REQUIRE_IMAGE_METADATA=1
platform_check_image() {
	return 0
}

RAMFS_COPY_BIN='yafut'

platform_do_upgrade_mikrotik_nand() {
	CI_KERNPART=none

	local fw_mtd=$(find_mtd_part kernel)
	fw_mtd="${fw_mtd/block/}"
	[ -n "$fw_mtd" ] || return

	local board_dir=$(tar tf "$1" | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}
	[ -n "$board_dir" ] || return

	tar xf "$1" ${board_dir}/kernel -O | yafut -d "$fw_mtd" -w -i - -o kernel -m 0755 || return

	nand_do_upgrade "$1"
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	mikrotik,routerboard-2011uias-2hnd|\
	mikrotik,routerboard-493g|\
	mikrotik,routerboard-911g-5hpacd|\
	mikrotik,routerboard-911g-xhpnd|\
	mikrotik,routerboard-912uag-2hpnd|\
	mikrotik,routerboard-921gs-5hpacd-15s|\
	mikrotik,routerboard-922uags-5hpacd|\
	mikrotik,routerboard-951g-2hnd|\
	mikrotik,routerboard-951ui-2hnd|\
	mikrotik,routerboard-sxt-5nd-r2)
		platform_do_upgrade_mikrotik_nand "$1"
		;;
	*)
		# NOR devices: erase firmware if booted from initramfs
		[ "$(rootfs_type)" = "tmpfs" ] && mtd erase firmware

		default_do_upgrade "$1"
		;;
	esac
}
