#
# Copyright (C) 2014 OpenWrt.org
#

linksys_get_target_firmware() {
	cur_boot_part=`/usr/sbin/fw_printenv -n boot_part`
	target_firmware=""
	if [ "$cur_boot_part" = "1" ]
	then
		# current primary boot - update alt boot
		target_firmware="kernel2"
		fw_setenv boot_part 2
		fw_setenv bootcmd "run altnandboot"
	elif [ "$cur_boot_part" = "2" ]
	then
		# current alt boot - update primary boot
		target_firmware="kernel1"
		fw_setenv boot_part 1
		fw_setenv bootcmd "run nandboot"
	fi

	echo "$target_firmware"
}

platform_do_upgrade_linksys() {
	local magic_long="$(get_magic_long "$1")"

	mkdir -p /var/lock
	local part_label="$(linksys_get_target_firmware)"
	touch /var/lock/fw_printenv.lock

	if [ ! -n "$part_label" ]
	then
		echo "cannot find target partition"
		exit 1
	fi

	# we don't know, what filesystem does the other partition use,
	# nuke it tobe safe
	mtd erase $part_label
	get_image "$1" | mtd -n write - $part_label
}

linksys_preupgrade() {
	export RAMFS_COPY_BIN="${RAMFS_COPY_BIN} /usr/sbin/fw_printenv /usr/sbin/fw_setenv"
	export RAMFS_COPY_BIN="${RAMFS_COPY_BIN} /bin/mkdir /bin/touch"
	export RAMFS_COPY_DATA="${RAMFS_COPY_DATA} /etc/fw_env.config /var/lock/fw_printenv.lock"

	[ -f /tmp/sysupgrade.tgz ] && {
		cp /tmp/sysupgrade.tgz /tmp/syscfg/sysupgrade.tgz
	}
}

append sysupgrade_pre_upgrade linksys_preupgrade
