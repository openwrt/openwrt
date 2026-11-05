. /lib/functions.sh

preinit_set_poe_out() {
	case $(board_name) in
	arista,w-118)
		# lan3's PoE output only from 802.3at (poe-af low) or DC (dc-in high)
		if [ "$(cat /sys/class/gpio/dc-in/value)" = 1 ] ||
		   [ "$(cat /sys/class/gpio/poe-af/value)" = 0 ]; then
			echo enabled > /sys/devices/platform/output-poe/state
		fi
		;;
	esac
}

boot_hook_add preinit_main preinit_set_poe_out
