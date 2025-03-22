PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='dumpimage fw_printenv fw_setenv head'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

remove_oem_ubi_volume() {
	local oem_volume_name="$1"
	local oem_ubivol
	local mtdnum
	local ubidev

	mtdnum=$(find_mtd_index "$CI_UBIPART")
	if [ ! "$mtdnum" ]; then
		return
	fi

	ubidev=$(nand_find_ubi "$CI_UBIPART")
	if [ ! "$ubidev" ]; then
		ubiattach --mtdn="$mtdnum"
		ubidev=$(nand_find_ubi "$CI_UBIPART")
	fi

	if [ "$ubidev" ]; then
		oem_ubivol=$(nand_find_volume "$ubidev" "$oem_volume_name")
		[ "$oem_ubivol" ] && ubirmvol "/dev/$ubidev" --name="$oem_volume_name"
	fi
}

linksys_mx_pre_upgrade() {
	local setenv_script="/tmp/fw_env_upgrade"

	CI_UBIPART="rootfs"
	boot_part="$(fw_printenv -n boot_part)"
	if [ -n "$UPGRADE_OPT_USE_CURR_PART" ]; then
		if [ "$boot_part" -eq "2" ]; then
			CI_KERNPART="alt_kernel"
			CI_UBIPART="alt_rootfs"
		fi
	else
		if [ "$boot_part" -eq "1" ]; then
			echo "boot_part 2" >> $setenv_script
			CI_KERNPART="alt_kernel"
			CI_UBIPART="alt_rootfs"
		else
			echo "boot_part 1" >> $setenv_script
		fi
	fi

	boot_part_ready="$(fw_printenv -n boot_part_ready)"
	if [ "$boot_part_ready" -ne "3" ]; then
		echo "boot_part_ready 3" >> $setenv_script
	fi

	auto_recovery="$(fw_printenv -n auto_recovery)"
	if [ "$auto_recovery" != "yes" ]; then
		echo "auto_recovery yes" >> $setenv_script
	fi

	if [ -f "$setenv_script" ]; then
		fw_setenv -s $setenv_script || {
			echo "failed to update U-Boot environment"
			return 1
		}
	fi
}

platform_check_image() {
	return 0;
}

glinet_validate_firmware() {
	local type='67'
	local ubi_magic='55 42 49 23'
	local img_magic='d0 0d fe ed'
	local sys_magic='73 79 73 75'

	# ubi type firmware
	local is_ubi=$( hexdump -C -n 4 "$1" | grep "$ubi_magic" | wc -c )
	if [ ${is_ubi} == ${type} ]; then
		echo "ubi"
		return
	fi
	# img type firmware
	local is_fit=$( hexdump -C -n 4 "$1" | grep "$img_magic" | wc -c )
	if [ ${is_fit} == ${type} ]; then
		echo "fit"
		return
	fi
	# sysupgrade-tar type firmware
	local is_sys=$( hexdump -C -n 4 "$1" | grep "$sys_magic" | wc -c )
	if [ ${is_sys} == ${type} ]; then
		echo "sys"
		return
	fi
	# Invalid firmware
	echo "error"
}

glinet_do_fit_upgrade() {
	echo -n "fit: Extract [ FIT IMAGE ] -x-x-> [ ubi.bin ] ... "
	local ubi=/tmp/ubi.bin
	local part=$(dumpimage -l /tmp/firmware.bin | grep -o "Image [0-9] (ubi)" | cut -f2 -d" ")

	local ubibin=$( dumpimage -T flat_dt -p ${part} -o "$ubi"  $1 )
	if [ -s "$ubi" ]; then
		echo "[ OK ]"
		local ubiMd5=$(cat $ubi | md5sum | cut -f1 -d" ")
		local ubi_size=$( cat "$ubi" | wc -c )
		echo -n "fit-copy: [ ubi.bin ] -c-c-> [ firmware.bin ] ... "
		mv "$ubi" "$1"
		local firmMd5=$(cat $1 | md5sum | cut -f1 -d" ")
		local firm_size=$( cat $1 | wc -c )
		if [ ${firm_size} -eq ${ubi_size} ] && [ "$ubiMd5" = "$firmMd5" ]; then
			echo "[ OK ]"
			echo "fit-copy: Copied "$firm_size" / "$ubi_size" bytes into firmware.bin"
			echo "fit-copy: MD5 CHECK: [ OK ]"
			echo "$ubiMd5 <=> $firmMd5"
			echo "fit: Successfully Extracted UBI from FIT IMAGE"
			echo "fit: Proceeding with sysupgrade .."
			nand_do_upgrade "$1"
			return
		fi
		echo "[ FAILED ] !!"
		echo "fit-copy: Copied "$firm_size" / "$ubi_size" bytes into firmware.bin"
		echo "ERROR: Failed to Copy UBI into firmware.bin !!"
		echo "fit: Terminating sysupgrade .."
		exit 1
	fi
	echo "[ FAILED ] !!"
	echo "fit-extract: Failed to Create Temp File ubi.bin !!"
	echo "ERROR: Failed to Extract UBI from FIT IMAGE !!"
	echo "fit: Terminating sysupgrade .."
	exit 1
}

glinet_do_ubi_upgrade() {
	echo -n "ubi: Removing Metadata Trailer from the UBI Volume ... "

	local metadata=$(fwtool -q -t -i /dev/null "$1")
	if [ -s $1 ]; then
		echo "[ OK ]"
		echo "ubi-meta: Successfully Removed Metadata from UBI Volume"
		echo "ubi: Proceeding with sysupgrade .."
		nand_do_upgrade "$1"
		return
	fi
	echo "[ FAILED ] !!"
	echo "ubi-meta: Cannot remove Metadata, the Files is Empty !!"
	echo "ERROR: Failed to Remove Metadata Trailer from UBI Volume !!"
	echo "ubi: Terminating sysupgrade .."
	exit 1
}

platform_do_upgrade() {
	case "$(board_name)" in
	glinet,gl-b3000)
		CI_UBIPART="rootfs"
		echo -n "Validating Firmware ... "
		case $(glinet_validate_firmware $1) in
		ubi)
			echo "[ OK ]"
			echo "ubi-main: Firmware is Valid: ubi"
			echo "ubi-main: Upgrading Firmware via [ UBI BIN ]"
			glinet_do_ubi_upgrade $1
			;;
		fit)
			echo "[ OK ]"
			echo "fit-main: Firmware is Valid: fit"
			echo "fit-main: Upgrading Firmware via [ FIT IMAGE ]"
			glinet_do_fit_upgrade $1
			;;
		sys)
			echo "[ OK ]"
			echo "sys-main: Firmware is Valid: sysupgrade-tar"
			echo "sys-main: Upgrading Firmware via [ SYSUPGRADE-TAR ]"
			nand_do_upgrade $1
			;;
		*)
			echo "[ FAILED ] !!"
			echo "main: Firmware Validation Failed !!"
			echo "main: Terminating sysupgrade .."
			exit 1
			;;
		esac
		;;
	linksys,mr5500|\
	linksys,mx2000|\
	linksys,mx5500|\
	linksys,spnmx56)
		linksys_mx_pre_upgrade "$1"
		remove_oem_ubi_volume squashfs
		nand_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
