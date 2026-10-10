REQUIRE_IMAGE_METADATA=1

herospeed_ab_check_image() {
	local members

	[ "$#" -gt 1 ] && return 1
	members="$(tar tf "$1" 2>/dev/null)" || return 1
	echo "$members" | grep -q '^sysupgrade-[^/]*/kernel$' || return 1
	echo "$members" | grep -q '^sysupgrade-[^/]*/root$' || return 1
	return 0
}

herospeed_ab_do_upgrade() {
	local slot other

	slot="$(rkab_slot_current)"
	other="$(rkab_slot_other "$slot")"
	[ -n "$other" ] || other=a
	CI_ROOTDEV="$RKAB_DISK"
	CI_KERNPART="boot_$other"
	CI_ROOTPART="system_$other"
	emmc_do_upgrade "$1"
	rkab_mark_active "$other"
}

platform_check_image() {
	local board=$(board_name)

	case "$board" in
	herospeed,rv1126-imx415)
		herospeed_ab_check_image "$@"
		return $?
		;;
	*)
		return 1
		;;
	esac
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	herospeed,rv1126-imx415)
		herospeed_ab_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}

platform_copy_config() {
	local board=$(board_name)

	case "$board" in
	herospeed,rv1126-imx415)
		emmc_copy_config
		;;
	esac
}
