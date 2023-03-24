#
# Copyright (C) 2010-2013 OpenWrt.org
#

rootpartuuid() {
	local cmdline=$(cat /proc/cmdline)
	local bootpart=${cmdline##*root=}
	bootpart=${bootpart%% *}
	local uuid=${bootpart#PARTUUID=}
	echo ${uuid%-02}
}

bootdev_from_uuid() {
	blkid | grep "PTUUID=\"$(rootpartuuid)\"" | cut -d : -f1
}

bootpart_from_uuid() {
	blkid | grep $(rootpartuuid)-01 | cut -d : -f1
}

rootpart_from_uuid() {
	blkid | grep $(rootpartuuid)-02 | cut -d : -f1
}

imx_sdcard_mount_boot() {
	mkdir -p /boot
	[ -f /boot/uImage ] || {
		mount -o rw,noatime $(bootpart_from_uuid) /boot > /dev/null
	}
}

imx_sdcard_copy_config() {
	imx_sdcard_mount_boot
	cp -af "$UPGRADE_BACKUP" "/boot/$BACKUP_FILE"
	sync
	umount /boot
}

imx_sdcard_do_upgrade() {
	local board_dir="$(tar tf "${1}" | grep -m 1 '^sysupgrade-.*/$')"
	board_dir="${board_dir%/}"

	imx_sdcard_mount_boot
	get_image "$1" | tar Oxf - ${board_dir}/kernel > /boot/uImage-new && \
		mv /boot/uImage-new /boot/uImage && \
		sync && \
		get_image "$1" | tar Oxf - ${board_dir}/root > $(rootpart_from_uuid) && \
		sync

	umount /boot
}

imx_sdcard_pre_upgrade() {
	[ -z "$UPGRADE_BACKUP" ] && {
		jffs2reset -y
		umount /overlay
	}
}

imx_sdcard_move_config() {
	if [ -b $(bootpart_from_uuid) ]; then
		imx_sdcard_mount_boot
		[ -f "/boot/$BACKUP_FILE" ] && mv -f "/boot/$BACKUP_FILE" /
		umount /boot
	fi
}
