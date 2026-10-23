# SPDX-License-Identifier: GPL-2.0-only

REQUIRE_IMAGE_METADATA=1

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
	zyxel,ex3301-t0|\
	zyxel,wx3100-t0)
		[ "$#" -eq 1 ] || return 1
		[ "$(get_magic_long "$1")" = "73797375" ] || {
			echo "Invalid image: expected a UBI sysupgrade archive"
			return 1
		}
		nand_do_platform_check "$board" "$1" || return 1

		local board_dir="sysupgrade-${board//,/_}"
		local kernel_magic root_magic
		kernel_magic=$(tar xOf "$1" "$board_dir/kernel" 2>/dev/null | \
			dd bs=4 count=1 2>/dev/null)
		root_magic=$(tar xOf "$1" "$board_dir/root" 2>/dev/null | \
			dd bs=4 count=1 2>/dev/null)
		[ "$kernel_magic" = "2RDH" ] && [ "$root_magic" = "hsqs" ] || {
			echo "Invalid image: expected TRX kernel and SquashFS rootfs"
			return 1
		}
		return 0
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
	jiofiber,jcow414|\
	zyxel,ex3301-t0|\
	zyxel,wx3100-t0)
		CI_KERNPART="tclinux_kernel"
		nand_do_upgrade "$1"
		;;
	esac
}
