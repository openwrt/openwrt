# SPDX-License-Identifier: GPL-2.0-only

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	return 0
}

platform_do_upgrade() {
	# The backup normally arrives through procd's UPGRADE_BACKUP environment.
	# Keep the fallback from the Giga II sysupgrade fix for the ramfs pivot.
	if [ -z "$UPGRADE_BACKUP" ] && [ -s /tmp/sysupgrade.tgz ]; then
		export UPGRADE_BACKUP=/tmp/sysupgrade.tgz
	fi

	# mtd write only erases blocks covered by the new image. Erase the whole
	# combined firmware partition first so stale JFFS2 data cannot survive a
	# sysupgrade when the new squashfs image is shorter than the old one.
	#
	# The following -n prevents mtd from erasing the written blocks twice.
	MTD_ARGS="-e firmware -n"

	default_do_upgrade "$1"
}
