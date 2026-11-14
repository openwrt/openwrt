# RTL9607C comes with dualboot support, hardcoded in bootloader
# Dualboot is achieved by u-boot env variables. These variables
# are hardcoded and gets overwritten by bootloader on every boot.
# In other words, we will have to work with it
#
# Dualboot is controlled by following variables:
# - sw_tryactive - try to boot slot X
# - sw_active - currently running slot
# - sw_commit - slot is confirmed as working
# Boot process (in u-boot) could be described like this:
#
# if sw_tryactive == 2 // boot committed slot
#   if sw_commit == 0
#     sw_active = 0
#     run slot0
#   else
#     sw_active = 1
#     run slot1
# else
#   if sw_tryactive == 0
#     sw_tryactive = 2
#     sw_active = 0
#     run slot0
#   else
#     sw_tryactive = 2
#     sw_active = 1
#     run slot1
#
# After successful boot, image in slot should mark it as working,
# i.e. do:
#  sw_commit = sw_active
# If image doesn't work (kernel panics for example), another slot
# will be booted
#
# We might reconsider successful boot criteria in future

rtl9607c_get_spare_part() {
	local active="$(fw_printenv -n sw_active)"
	if [ "$active" == 0 ]; then
		echo 1
	elif [ "$active" == 1 ]; then
		echo 0
	fi
}

# Complete dualboot - unused for now because of issues with
# mounting correct root
platform_do_upgrade_rtl9607c_dualboot() {
	local spare_num="$(rtl9607c_get_spare_part)"

	if [ "$spare_num" != 0 -a "$spare_num" != 1 ]; then
		echo "$(board_name): can't read spare slot '$spare_num'"
		return 1
	fi

	CI_KERNPART="ubi_k$spare_num"
	CI_ROOTPART="ubi_r$spare_num"

	nand_do_flash_file "$1" || nand_do_upgrade_failed
	if nand_do_restore_config && sync && fw_setenv sw_tryactive "$spare_num"; then
		echo "sysupgrade successful"
		umount -a
		reboot -f
	fi
	nand_do_upgrade_failed
}

# Use slot 1 leaving slot 0 for OEM firmware
platform_do_upgrade_rtl9607c_oneslot() {
	# For now write OpenWrt to slot 1, preserve OEM in slot 0
	local spare_num="1"

	CI_KERNPART="ubi_k$spare_num"
	CI_ROOTPART="ubi_r$spare_num"

	# Two writes is the most failsafe way without actual dualboot
	fw_setenv sw_commit "0" || nand_do_upgrade_failed
	nand_do_flash_file "$1" || nand_do_upgrade_failed
	if nand_do_restore_config && sync && fw_setenv sw_tryactive "1"; then
		echo "sysupgrade successful"
		umount -a
		reboot -f
	fi
	nand_do_upgrade_failed
}

