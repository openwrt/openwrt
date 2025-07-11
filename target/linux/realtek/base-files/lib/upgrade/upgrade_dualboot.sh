set_boot_part() {
	local part_num="$1"
	local setenv_script="/tmp/fw_env_upgrade"
	board=$(board_name)

	case "$board" in
		plasmacloud,psx8)
			if [ "$part_num" = "1" ]; then
				echo "bootargs mtdparts=spi0.0:896k(u-boot),64k(u-boot-env),64k(u-boot-env2),15872k(firmware1),15872k(inactive)" > $setenv_script
				echo "bootcmd rtk init; bootm 0xb4100000" >> $setenv_script
			elif [ "$part_num" = "2" ]; then
				echo "bootargs mtdparts=spi0.0:896k(u-boot),64k(u-boot-env),64k(u-boot-env2),15872k(inactive),15872k(firmware2)" > $setenv_script
				echo "bootcmd rtk init; bootm 0xb5080000" >> $setenv_script
			else
				echo "Partition number $part_num is not supported for ${board}" 2>&1
				return 1
			fi
			;;
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

platform_do_upgrade_dualboot_psx() {
	local tar_file="$1"
	local restore_backup
	local primary_firmware_mtd

	local inactive_mtd="$(find_mtd_index $PART_NAME)"

	local board_dir=$(tar tf $tar_file | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}
	local img_board_target=$(tar xf $tar_file ${board_dir}/CONTROL -O | cut -d '=' -f 2)

	# Handle case when u-boot-env is not correctly written and we are running from initramfs
	# Only install firmware in 1st partition
	if [ -z "$inactive_mtd" ]; then
		PART_NAME=firmware1
		inactive_mtd="$(find_mtd_index $PART_NAME)"
	fi

	if [ -z "$inactive_mtd" ]; then
		echo "Cannot find firmware1 or inactive partition to work with. Abort!"
		return 1
	fi

	local next_boot_part="1"
	case "$(board_name)" in
	plasmacloud,psx8)
		primary_firmware_mtd=3
		;;
	*)
		echo "failed to detect primary kernel mtd partition for board"
		return 1
		;;
	esac
	[ "$inactive_mtd" = "$primary_firmware_mtd" ] || next_boot_part="2"

	local kernel_length=$(tar -xf $tar_file ${board_dir}/kernel -O | wc -c)

	[ -n "$UPGRADE_BACKUP" ] && restore_backup="${MTD_CONFIG_ARGS} -j ${UPGRADE_BACKUP}"

	tar -xf $tar_file ${board_dir}/kernel ${board_dir}/root -O | mtd $restore_backup write - $PART_NAME

	set_boot_part "$next_boot_part"
	return $?
}
