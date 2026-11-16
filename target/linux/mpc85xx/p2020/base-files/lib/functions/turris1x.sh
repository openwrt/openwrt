# SPDX-License-Identifier: GPL-2.0-or-later

turris1x_boots_from_nand() {
	# /proc/mounts changes once sysupgrade runs from the ramfs
	grep -q 'ubi\.mtd=nand_ubi,' /proc/cmdline &&
		! grep -q 'root=/dev/mmcblk' /proc/cmdline
}
