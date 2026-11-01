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
# if sw_tryactive == 2 // boot commited slot
#   if sw_commit == 0
#     sw_active = 0
#     run slot0
#   else
#     sw_active = 1
#     run slot1
# else
#   sw_tryactive = 2 // boot commited slot next time
#   if sw_tryactive == 0
#     sw_active = 0
#     run slot0
#   else
#     sw_active = 1
#     run slot1
#
# After successful boot, image in slot should mark in as working,
# i.e. do:
#  sw_commit = sw_active
# If image doesn't work (kernel panics for example), another slot
# will be booted
#
# We might reconsider successful boot criteria in future

rtl9607c_get_spare_part() {
	local active="$(fw_printenv -n sw_active)"
	if [[ "$active" == 0 ]]; then
		echo 1
	else
		echo 0
	fi
}

rtl9607c_set_boot_vol() {
	fw_setenv sw_tryactive "$1"
}

platform_do_upgrade_dualboot_rtl9607c() {
	local spare_num="$(rtl9607c_get_spare_part)"
	CI_KERNPART="ubi_k$spare_num"
	CI_ROOTPART="ubi_r$spare_num"
	nand_do_flash_file "$1" || nand_do_upgrade_failed
	rtl9607c_set_boot_vol "$spare_num"
	nand_do_upgrade_success
}

