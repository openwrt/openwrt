#
# Copyright (C) 2023 OpenWrt.org
#

. /lib/functions.sh

zyxel_mstc_prepare_fail() {
	echo "failed to check and prepare the environment, rebooting..."
	umount -a
	reboot -f
}

zyxel_mstc_upgrade_prepare() {
	local persist_mtd="$(find_mtd_part persist)"
	local firmware_mtd="$(find_mtd_part firmware)"

	if [ -z "$persist_mtd" ] || [ -z "$firmware_mtd" ]; then
		echo 'cannot find mtd partition(s) "persist" or "firmware"'
		zyxel_mstc_prepare_fail
	fi

	local bootnum=$(hexdump -s 4 -n 1 -e '"%x"' ${persist_mtd})

	if [ "$bootnum" != "1" ] && [ "$bootnum" != "2" ]; then
		echo "failed to get bootnum, please check the value at 0x4 in ${persist_mtd}"
		zyxel_mstc_prepare_fail
	fi
	echo "current: bootnum => ${bootnum}"

	[ "$(fw_printenv -n bootmenu_delay)" = "3" ] || fw_setenv bootmenu_delay 3

	if [ "$bootnum" = "2" ]; then
		if ! ( echo -ne "\x01" | dd of=${persist_mtd} count=1 bs=1 seek=4 conv=notrunc 2>/dev/null ); then
			echo "failed to set new boot partition"
			zyxel_mstc_prepare_fail
		fi
		echo "### switch to 1st os-image on next boot ###"
	fi
}
