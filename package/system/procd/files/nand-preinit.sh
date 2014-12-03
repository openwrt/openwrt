#!/bin/sh
# Copyright (C) 2014 OpenWrt.org

nand_takeover() {
	. /lib/upgrade/nand.sh
	mtd=$(find_mtd_index "$CI_UBIPART")
	local file_type="$(identify $2)"
	mtd -l 400 dump /dev/mtd$mtd > /tmp/takeover.hdr
	MAGIC=$(dd if=/tmp/takeover.hdr bs=1 skip=261 count=5 2> /dev/null)
	SIZE=$(printf "%d" 0x$(dd if=/tmp/takeover.hdr bs=4 count=1 2> /dev/null | hexdump -v -n 4 -e '1/1 "%02x"'))
	[ "$MAGIC" = "ustar" ] && {
		mtd -l $((SIZE + 4))  dump /dev/mtd$mtd | dd bs=1 skip=4 of=/tmp/sysupgrade.tar
		nand_do_upgrade_stage2 /tmp/sysupgrade.tar
	}
}

boot_hook_add initramfs nand_takeover
