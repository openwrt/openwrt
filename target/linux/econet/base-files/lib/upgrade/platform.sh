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
	nokia,g140w-c)
		g140wc_get_bootram /tmp/g140wc.bootram || {
			echo "Can't find the bootloader's second stage (bootram) in slot A or B"
			return 1
		}
		rm -f /tmp/g140wc.bootram
		return 0
		;;
	esac

	return 1
}

platform_pre_upgrade() {
	case "$(board_name)" in
	nokia,g140w-c)
		# Runs before the switch to the ramfs, with all tools at hand.
		# Boot slot A next, which matters when installing from a RAM
		# boot, once the image is ready to be written there.
		rm -f /tmp/g140wc-sysupgrade.tar /tmp/g140wc-cal.bin
		g140wc_add_bootram "$1" /tmp/g140wc-sysupgrade.tar || return
		g140wc_has_cal || g140wc_cal_from_stock /tmp/g140wc-cal.bin || \
			echo "No Wi-Fi calibration found, Wi-Fi will stay off"
		/sbin/en75_chboot openwrt
		;;
	esac
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
	nokia,g140w-c)
		# Prepared by platform_pre_upgrade
		[ -f /tmp/g140wc-sysupgrade.tar ] || {
			echo "The image could not be prepared, not writing it"
			return 1
		}
		CI_KERNPART="tclinux_kernel"
		if [ -f /tmp/g140wc-cal.bin ]; then
			g140wc_store_cal /tmp/g140wc-cal.bin || \
				echo "Could not store the Wi-Fi calibration"
		fi
		nand_do_upgrade /tmp/g140wc-sysupgrade.tar
		;;
	esac
}
