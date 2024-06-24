#
# Copyright (C) 2016 lede-project.org
#
. /lib/functions.sh

zyxel_do_upgrade() {
	local tar_file="$1"
	local board=$(board_name)
	local board_dir=$(tar tf $tar_file | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}

	case "$board" in
	zyxel,nbg6617)
		tar Oxf $tar_file ${board_dir}/kernel | mtd write - kernel

		if [ -n "$UPGRADE_BACKUP" ]; then
			tar Oxf $tar_file ${board_dir}/root | mtd -j "$UPGRADE_BACKUP" write - rootfs
		else
			tar Oxf $tar_file ${board_dir}/root | mtd write - rootfs
		fi
		;;
	*)
		echo "Unknown board ${board} - aborting"
		return 1
		;;
	esac

	nand_do_upgrade "$1"

	return 0
}
