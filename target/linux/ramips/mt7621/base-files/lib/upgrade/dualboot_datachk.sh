# The U-Boot loader with the datachk patchset for dualbooting requires image
# sizes and checksums to be provided in the U-Boot environment.
# The devices come with 2 main partitions - while one is active
# sysupgrade will flash the other. The boot order is changed to boot the
# newly flashed partition. If the new partition can't be booted due to
# upgrade failures the previously used partition is loaded.

platform_do_upgrade_dualboot_datachk() {
	local next_boot_part=1
	local tar_file="$1"
	local bootseq

	local setenv_script="/tmp/fw_env_upgrade"

	if [ "$(grep 'ubi.mtd=firmware1' /proc/cmdline)" ];  then
		next_boot_part=2
		bootseq="2,1"
	else
		next_boot_part=1
		bootseq="1,2"
	fi

	local board_dir="$(tar tf "${tar_file}" | grep -m 1 '^sysupgrade-.*/$')"
	board_dir="${board_dir%/}"

	local kernel_length="$(tar xf "${tar_file}" "${board_dir}/kernel" -O | wc -c)"
	local rootfs_length="$(tar xf "${tar_file}" "${board_dir}/root" -O | wc -c)"

	local kernel_md5="$(tar xf "${tar_file}" "${board_dir}/kernel" -O | md5sum)"
	kernel_md5="${kernel_md5%% *}"

	local rootfs_md5="$(tar xf "${tar_file}" "${board_dir}/root" -O | md5sum)"
	rootfs_md5="${rootfs_md5%% *}"

	CI_UBIPART="firmware${next_boot_part}"
	CI_KERNPART="kernel"
	CI_ROOTPART="rootfs"

	nand_upgrade_prepare_ubi "${rootfs_length}" "squashfs" "$kernel_length" "0"

	local ubidev="$(nand_find_ubi "${CI_UBIPART}")"

	local kern_ubivol="$(nand_find_volume "${ubidev}" "${CI_KERNPART}")"
	tar xf "${tar_file}" "${board_dir}/kernel" -O | \
		ubiupdatevol "/dev/${kern_ubivol}" -s "${kernel_length}" -

	local root_ubivol="$(nand_find_volume "${ubidev}" "${CI_ROOTPART}")"
	tar xf "${tar_file}" "${board_dir}/root" -O | \
		ubiupdatevol "/dev/${root_ubivol}" -s "${rootfs_length}" -

	[ -f "${UPGRADE_BACKUP}" ] && nand_restore_config "${UPGRADE_BACKUP}"

	# write new new uboot-env
	printf "bootseq ${bootseq}\n" > "${setenv_script}"

	printf "kernel_%i_size 0x%08x\n" "${next_boot_part}" "${kernel_length}" >> "${setenv_script}"
	printf "kernel_%i_checksum %s\n" "${next_boot_part}" "${kernel_md5}" >> "${setenv_script}"

	printf "rootfs_%i_size 0x%08x\n" "${next_boot_part}" "${rootfs_length}" >> "${setenv_script}"
	printf "rootfs_%i_checksum %s\n" "${next_boot_part}" "${rootfs_md5}" >> "${setenv_script}"

	mkdir -p /var/lock
	fw_setenv -s "${setenv_script}" || {
		echo "failed to update U-Boot environment"
		return 1
	}
}
