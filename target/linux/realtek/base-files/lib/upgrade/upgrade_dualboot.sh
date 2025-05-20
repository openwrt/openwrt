platform_check_image_target()
{
	local board=$(board_name)
	local img_board_target="$1"

	case "$img_board_target" in
	plasmacloud_psx28|\
	plasmacloud_esx28)
		[ "$board" = "plasmacloud,psx28" -o "$board" = "plasmacloud,esx28" ] && return 0
		echo "Invalid image board target ($img_board_target) for this platform: $board. Use the correct image for this platform"
		return 1
		;;
	plasmacloud_psx16)
		[ "$board" = "plasmacloud,psx16" ] && return 0
		echo "Inavlid image board target ($img_board_target) for this platform: $board. Use the correct image for this platform"
		;;
	plasmacloud_psx8|\
	plasmacloud_psx10)
		[ "$board" = "plasmacloud,psx10" -o "$board" = "plasmacloud,psx8" ] && return 0
		echo "Invalid image board target ($img_board_target) for this platform: $board. Use the correct image for this platform"
		return 1
		;;
	*)
		echo "Invalid board target ($img_board_target). Use the correct image for this platform"
		return 1
		;;
	esac
}

set_boot_part() {
	local part_num="$1"
	local setenv_script="/tmp/fw_env_upgrade"
	board=$(board_name)

	case "$board" in
		plasmacloud,psx28|\
		plasmacloud,esx28|\
		plasmacloud,psx16)
			if [ "$part_num" = "1" ]; then
				echo "bootargs mtdparts=spi0.0:768k(u-boot),64k(u-boot-env),64k(u-boot-env2),5120k(reserved),13376k(firmware1),13376k(inactive)" > $setenv_script
				echo "bootcmd rtk init; bootm 0xb45e0000" >> $setenv_script
			elif [ "$part_num" = "2" ]; then
				echo "bootargs mtdparts=spi0.0:768k(u-boot),64k(u-boot-env),64k(u-boot-env2),5120k(reserved),13376k(inactive),13376k(firmware2)" > $setenv_script
				echo "bootcmd rtk init; bootm 0xb52f0000" >> $setenv_script
			else
				echo "Partition number $part_num is not supported for ${board}" 2>&1
				return 1
			fi
			;;
		plasmacloud,psx8|\
		plasmacloud,psx10)
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

platform_do_upgrade_dualboot_datachk() {
	local tar_file="$1"
	local restore_backup
	local primary_kernel_mtd

	local inactive_mtd="$(find_mtd_index $PART_NAME)"

	local board_dir=$(tar tf $tar_file | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}
	local img_board_target=$(tar xf $tar_file ${board_dir}/CONTROL -O | cut -d '=' -f 2)

	platform_check_image_target "$img_board_target" || return 1

	local next_boot_part="2"
	case "$(board_name)" in
	plasmacloud,psx28|\
	plasmacloud,esx28|\
	plasmacloud,psx16)
		primary_inactive_mtd=8
		;;
	plasmacloud,psx8|\
	plasmacloud,psx10)
		primary_inactive_mtd=7
		;;
	*)
		echo "failed to detect primary kernel mtd partition for board"
		return 1
		;;
	esac
	[ "$inactive_mtd" != "$primary_inactive_mtd" ] && next_boot_part="1"

	local kernel_length=$(tar xf $tar_file ${board_dir}/kernel -O | wc -c)

	[ -n "$UPGRADE_BACKUP" ] && restore_backup="${MTD_CONFIG_ARGS} -j ${UPGRADE_BACKUP}"

	mtd -q erase inactive
	tar xf $tar_file ${board_dir}/kernel ${board_dir}/root -O | mtd $restore_backup write - $PART_NAME

	set_boot_part "$next_boot_part"
	return $?
}

