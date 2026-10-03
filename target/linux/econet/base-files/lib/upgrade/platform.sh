platform_check_image() {
	local board=$(board_name)

	case "$board" in
	chinamobile,gs3101|\
	dasan,h660gm-a-airtel|\
	dasan,h660gm-a-generic|\
	jiofiber,jcow407|\
	jiofiber,jcow414)
		return 0
		;;
	zyxel,ex3301-t0)
		[ "$#" -gt 1 ] && return 1

		# Verify OpenWrt image metadata matches the current device
		if fwtool -q -i /dev/null "$1"; then
			local dev
			dev=$(fwtool -q -m - "$1" 2>/dev/null | \
				jsonfilter -e '@.metadata.supported_devices' 2>/dev/null)
			case "$dev" in
				*"$board"*) return 0 ;;
			esac
			echo "Firmware metadata does not match device ($board)"
			return 1
		fi

		# Only accept raw 2RDH TRX when forced with -F
		local magic=$(dd if="$1" bs=1 count=4 2>/dev/null)
		if [ "$magic" = "2RDH" ]; then
			[ "$FORCE" = "1" ] && return 0
			echo "Image is raw TRX without metadata; use sysupgrade -F to force."
			return 1
		fi

		echo "Invalid image: missing OpenWrt metadata or 2RDH header"
		return 1
		;;
	esac

	return 1
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	chinamobile,gs3101|\
	dasan,h660gm-a-airtel|\
	dasan,h660gm-a-generic|\
	jiofiber,jcow407|\
	jiofiber,jcow414)
		CI_KERNPART="tclinux_kernel"
		nand_do_upgrade "$1"
		;;
	zyxel,ex3301-t0)
		PART_NAME="tclinux"
		MTD_ARGS="-e rootfs_data"
		default_do_upgrade "$1"
		;;
	esac
}
