#!/bin/sh
# Dual nand helper functions

# Set these per-device in platform.sh before calling helper
# CI_UBIPART_A="rootfs"
# CI_UBIPART_B="rootfs_1"
# CI_BOOT_VAR="partition"
# CI_BOOT_A="0"
# CI_BOOT_B="1"

dual_nand_prepare() {
	local target_part
	local current_boot
	local ubi_root

	current_boot="$(fw_printenv -n "$CI_BOOT_VAR" 2>/dev/null)"

	if [ -z "$current_boot" ]; then
		echo "ERROR: Cannot read current boot partition from $CI_BOOT_VAR"
		return 1
	fi

	if [ "$current_boot" = "$CI_BOOT_A" ]; then
		target_part="$CI_UBIPART_B"
	elif [ "$current_boot" = "$CI_BOOT_B" ]; then
		target_part="$CI_UBIPART_A"
	else
		echo "ERROR: Unknown $CI_BOOT_VAR value '$current_boot'"
		return 1
	fi

	if ! ubi_root="$(nand_attach_ubi "$target_part")"; then
		echo "failed to attach UBI volume '$target_part', rebooting..."
		reboot -f
	fi

	export CI_UBIPART="$target_part"
	return 0
}

dual_nand_do_upgrade() {
	local file="$1"

	nand_do_flash_file "$file" || nand_do_upgrade_failed

	nand_do_restore_config || nand_do_upgrade_failed

	return 0
}

dual_nand_switch_boot() {
	local current_boot
	local new_boot

	current_boot="$(fw_printenv -n "$CI_BOOT_VAR" 2>/dev/null)"

	if [ -z "$current_boot" ]; then
		echo "ERROR: No boot flag found in U-Boot env ($CI_BOOT_VAR)"
		nand_do_upgrade_failed
		return 1
	fi

	if [ "$current_boot" = "$CI_BOOT_A" ]; then
		new_boot="$CI_BOOT_B"
	elif [ "$current_boot" = "$CI_BOOT_B" ]; then
		new_boot="$CI_BOOT_A"
	else
		echo "ERROR: Unknown $CI_BOOT_VAR value '$current_boot'"
		nand_do_upgrade_failed
		return 1
	fi

	fw_setenv "$CI_BOOT_VAR" "$new_boot" || {
		echo "ERROR: Failed to write U-Boot env"
		nand_do_upgrade_failed
		return 1
	}

	local verify_boot
	verify_boot="$(fw_printenv -n "$CI_BOOT_VAR" 2>/dev/null)"
	if [ "$verify_boot" != "$new_boot" ]; then
		echo "ERROR: Boot variable verification failed (expected: $new_boot, got: $verify_boot)"
		fw_setenv "$CI_BOOT_VAR" "$current_boot" 2>/dev/null
		nand_do_upgrade_failed
		return 1
	fi

	echo "sysupgrade successful"
	echo "Switched boot partition from $current_boot to $new_boot"
	sync
	umount -a
	reboot -f
}
