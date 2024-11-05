# Script for dual image A/B system boots
# Copyright (C) 2024 MediaTek Inc.
# Author: Weijie Gao <weijie.gao@mediatek.com>

ubi_prepare_u_boot_env() {
	local ubidev="$1"

	local env_size=$(cat /sys/firmware/devicetree/base/mediatek,env-size 2>/dev/null)
	[ -z "${env_size}" ] && return

	local env_vol=$(cat /sys/firmware/devicetree/base/mediatek,env-ubi-volume 2>/dev/null)
	if [ -n "${env_vol}" ]; then
		local env_ubivol="$( nand_find_volume $ubidev $env_vol )"
		[ -z "$env_ubivol" ] && ubimkvol /dev/$ubidev -N ${env_vol} -s ${env_size} 2>/dev/null || :
	fi

	local env2_vol=$(cat /sys/firmware/devicetree/base/mediatek,env-ubi-volume-redund 2>/dev/null)
	if [ -n "${env2_vol}" ]; then
		local env2_ubivol="$( nand_find_volume $ubidev $env2_vol )"
		[ -z "$env2_ubivol" ] && ubimkvol /dev/$ubidev -N ${env2_vol} -s ${env_size} 2>/dev/null || :
	fi
}

ubi_dual_boot_restore_config() {
	local rootfs_data_vol=$(cat /sys/firmware/devicetree/base/mediatek,upgrade-rootfs_data-part 2>/dev/null)
	local ubidev=$( nand_find_ubi "${CI_ROOT_UBIPART:-$CI_UBIPART}" )
	local ubivol="$( nand_find_volume $ubidev $rootfs_data_vol )"
	if [ ! "$ubivol" ]; then
		ubivol="$( nand_find_volume $ubidev "$CI_ROOTPART" )"
		if [ ! "$ubivol" ]; then
			echo "cannot find ubifs data volume"
			return 1
		fi
	fi
	mkdir /tmp/new_root
	if ! mount -t ubifs /dev/$ubivol /tmp/new_root; then
		echo "cannot mount ubifs volume $ubivol"
		rmdir /tmp/new_root
		return 1
	fi
	if mv "$1" "/tmp/new_root/$BACKUP_FILE"; then
		if umount /tmp/new_root; then
			echo "configuration saved"
			rmdir /tmp/new_root
			return 0
		fi
	else
		umount /tmp/new_root
	fi
	echo "could not save configuration to ubifs volume $ubivol"
	rmdir /tmp/new_root
	return 1
}

ubi_dual_boot_do_restore_config() {
	local conf_tar="/tmp/sysupgrade.tgz"
	[ ! -f "$conf_tar" ] || ubi_dual_boot_restore_config "$conf_tar"
}

ubi_dual_boot_do_upgrade_success() {
	if ubi_dual_boot_do_restore_config && sync; then
		echo "sysupgrade successful"
		umount -a
		reboot -f
	fi
	nand_do_upgrade_failed
}

dual_boot_upgrade_prepare_ubi() {
	local boot_firmware_vol_name="$1"
	local firmware_vol_name="$2"
	local firmware_length="$3"
	local reserve_rootfs_data="$4"

	local ubidev="$( nand_attach_ubi "$CI_UBIPART" 0 )"

	local boot_fw_ubivol="$( nand_find_volume $ubidev $boot_firmware_vol_name )"
	local fw_ubivol="$( nand_find_volume $ubidev $firmware_vol_name )"
	local data_ubivol="$( nand_find_volume $ubidev rootfs_data )"

	# remove ubiblocks
	[ "$boot_fw_ubivol" ] && { nand_remove_ubiblock $boot_fw_ubivol || return 1; }
	[ "$fw_ubivol" ] && { nand_remove_ubiblock $fw_ubivol || return 1; }
	[ "$data_ubivol" ] && { nand_remove_ubiblock $data_ubivol || return 1; }

	# kill firmware volume
	[ "$fw_ubivol" ] && ubirmvol /dev/$ubidev -N "$firmware_vol_name" || :

	local rootfs_data_vol=$(cat /sys/firmware/devicetree/base/mediatek,upgrade-rootfs_data-part 2>/dev/null)
	if [ x"${reserve_rootfs_data}" != xY ]; then
		# kill rootfs_data volume
		[ "$data_ubivol" ] && ubirmvol /dev/$ubidev -N "$rootfs_data_vol" || :
	fi

	# create firmware vol
	if ! ubimkvol /dev/$ubidev -N "$firmware_vol_name" -s "$firmware_length"; then
		echo "cannot create firmware volume"
		return 1;
	fi

	# create u-boot environment volume
	ubi_prepare_u_boot_env $ubidev

	if [ x"${reserve_rootfs_data}" = xY ]; then
		# Do not touch rootfs_data
		sync
		return 0
	fi

	# create rootfs_data vol
	local rootfs_data_size=$(cat /sys/firmware/devicetree/base/mediatek,rootfs_data-size-limit 2>/dev/null)

	if [ -n "${rootfs_data_size}" ]; then
		rootfs_data_length="-s $rootfs_data_size"
	else
		rootfs_data_length="-m"
	fi

	if ! ubimkvol /dev/$ubidev -N "$rootfs_data_vol" "$rootfs_data_length"; then
		if [ -n "${rootfs_data_size}" ]; then
			if ! ubimkvol /dev/$root_ubidev -N "$rootfs_data_vol" -m; then
				echo "cannot initialize $rootfs_data_vol volume"
				return 1
			fi
		else
			echo "cannot initialize $rootfs_data_vol volume"
			return 1
		fi
	fi

	sync
	return 0
}

ubi_dual_boot_upgrade_itb() {
	local fit_file="$1"
	local gz="$2"

	local boot_firmware_vol_name=$(cat /sys/firmware/devicetree/base/mediatek,boot-firmware-part 2>/dev/null)
	[ -z "${boot_firmware_vol_name}" -o $? -ne 0 ] && return 1

	local firmware_vol_name=$(cat /sys/firmware/devicetree/base/mediatek,upgrade-firmware-part 2>/dev/null)
	[ -z "${firmware_vol_name}" -o $? -ne 0 ] && return 1

	local fit_length=$( (${gz}cat "$fit_file" | wc -c) 2> /dev/null)

	local reserve_rootfs_data=$([ -f /sys/firmware/devicetree/base/mediatek,reserve-rootfs_data ] && echo Y)
	dual_boot_upgrade_prepare_ubi "${boot_firmware_vol_name}" "${firmware_vol_name}" "${fit_length}" "${reserve_rootfs_data}" || return 1

	local fit_ubidev="$(nand_find_ubi "$CI_UBIPART")"
	local fit_ubivol="$(nand_find_volume $fit_ubidev "${firmware_vol_name}")"

	${gz}cat "$fit_file" | ubiupdatevol /dev/$fit_ubivol -s "$fit_length" - || return 1

	local upgrade_image_slot=$(cat /sys/firmware/devicetree/base/mediatek,upgrade-image-slot 2>/dev/null)
	if [ -n "${upgrade_image_slot}" ]; then
		v "Set new boot image slot to ${upgrade_image_slot}"
		# Force the creation of fw_printenv.lock
		mkdir -p /var/lock
		touch /var/lock/fw_printenv.lock
		fw_setenv "dual_boot.current_slot" "${upgrade_image_slot}"
		fw_setenv "dual_boot.slot_${upgrade_image_slot}_invalid" "0"
	fi

	if [ x"${reserve_rootfs_data}" != xY ]; then
		# do normal upgrade flow
		ubi_dual_boot_do_upgrade_success
	fi

	# Do not touch rootfs_data
	sync

	echo "sysupgrade successful"
	umount -a
	reboot -f
}

# Write the FIT image to UBI kernel volume
nand_upgrade_itb() {
	local fit_file="$1"
	local gz="$2"

	local fit_length=$( (${gz}cat "$fit_file" | wc -c) 2> /dev/null)

	nand_upgrade_prepare_ubi "" "" "$fit_length" "" || return 1

	local fit_ubidev="$(nand_find_ubi "$CI_UBIPART")"
	local fit_ubivol="$(nand_find_volume $fit_ubidev "$CI_KERNPART")"
	${gz}cat "$fit_file" | ubiupdatevol /dev/$fit_ubivol -s "$fit_length" -

	# create u-boot environment volume
	ubi_prepare_u_boot_env $fit_ubidev
}

ubi_do_upgrade() {
	local file=$1
	local dual_boot=$([ -f /sys/firmware/devicetree/base/mediatek,dual-boot ] && echo Y)

	local gz="$(identify_if_gzip "$file")"
	local file_type="$(identify "$file" "" "$gz")"

	case "$file_type" in
		"fit")
			sync
			nand_verify_if_gzip_file "$file" "$gz" || return 1

			if [ x"${dual_boot}" != xY ]; then
				nand_upgrade_itb "$file" "$gz" && nand_do_upgrade_success
				nand_do_upgrade_failed
			else
				ubi_dual_boot_upgrade_itb "$file" "$gz"
			fi
			;;
		*)
			v "Unsupported firmware type: $file_type"
			;;
	esac
}
