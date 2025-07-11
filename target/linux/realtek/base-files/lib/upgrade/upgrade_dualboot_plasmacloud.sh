set_boot_part() {
	local part_num="$1"
	local setenv_script="/tmp/fw_env_upgrade"
	local board=$(board_name)

	case "$board" in
	*)
		echo "${board} is not supported for dual boot" 1>&2
		return 1
		;;
	esac

	# store u-boot env changes
	mkdir -p /var/lock
	cat $setenv_script
	fw_setenv -s $setenv_script || {
		echo "failed to update U-Boot environment"
		return 1
	}
}

platform_do_upgrade_dualboot_plasmacloud() {
	local primary_firmware_mtd
	local restore_backup
	local next_boot_part
	local tar_file="$1"
	local inactive_mtd
	local board_dir

	inactive_mtd="$(find_mtd_index $PART_NAME)"
	if [ -z "$inactive_mtd" ]; then
		# Handle case when u-boot-env is not correctly written and we
		# are running from initramfs. Only install it in the "firmware1"
		# partition
		PART_NAME=firmware1
		inactive_mtd="$(find_mtd_index $PART_NAME)"
	fi

	if [ -z "$inactive_mtd" ]; then
		echo "Cannot find \"firmware1\" nor \"${$PART_NAME}\" partition to work with. Abort!" 2>&1
		return 1
	fi

	# identify "inactive" slot id based on the expected partition id
	# of the primary ("firmware1") slot
	case "$(board_name)" in
	*)
		echo "failed to detect primary firmware mtd partition for board" 2>&1
		return 1
		;;
	esac

	if [ "$inactive_mtd" = "$primary_firmware_mtd" ]; then
		next_boot_part="1"
	else
		next_boot_part="2"
	fi

	[ -n "$UPGRADE_BACKUP" ] && restore_backup="${MTD_CONFIG_ARGS} -j ${UPGRADE_BACKUP}"

	# write selected firmware slot
	board_dir=$(tar tf $tar_file | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}
	tar -xf $tar_file ${board_dir}/kernel ${board_dir}/root -O | mtd $restore_backup write - $PART_NAME

	# update u-boot-env to select new firmware slot
	set_boot_part "$next_boot_part"
}
