[ ! -e /etc/fw_env.config ] && exit 0

. /lib/functions/system.sh

case "$(board_name)" in
dna,valokuitu-plus-ex400)
	macaddr=$(/usr/sbin/fw_printenv -n ethaddr 2>/dev/null)
	mac2addr=$(/usr/sbin/fw_printenv -n eth1addr 2>/dev/null)

	[[ -n "$macaddr" && -z "$mac2addr" ]] &&
		/usr/sbin/fw_setenv eth1addr "$(macaddr_add $macaddr 1)"
	;;
esac

exit 0
