# SPDX-License-Identifier: GPL-2.0-or-later

# Match the VID header offset the factory bootloader expects.
TURRIS1X_UBI_VID_OFFSET=2048

turris1x_boots_from_nand() {
	# /proc/cmdline rather than /proc/mounts, which changes once sysupgrade
	# runs from ramfs. A NAND boot attaches the UBI and leaves root= to
	# ubiblock, both card paths spell out an mmcblk root.
	grep -q 'ubi\.mtd=ubi' /proc/cmdline &&
		! grep -q 'root=/dev/mmcblk' /proc/cmdline
}
