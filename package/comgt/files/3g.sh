set_3g_led() {
	grep WRT54G3G /proc/diag/model >/dev/null || return 0
	echo "$1" > /proc/diag/led/3g_green
	echo "$2" > /proc/diag/led/3g_blue
	echo "$3" > /proc/diag/led/3g_blink
}

scan_3g() {
	local device

	scan_ppp "$@"
	config_get device "$1" device

	# try to figure out the device if it's invalid
	[ -n "$device" -a -e "$device" ] || {
		for device in /dev/tts/2 /dev/usb/tts/0 /dev/noz0; do
			[ -e "$device" ] && {
				config_set "$1" device "$device"
				break
			}
		done
	}

	# enable 3G with the 3G button by default
	config_get button "$1" button
	[ -z "$button" ] && {
		config_set "$1" button 1
	}
}

stop_interface_3g() {
	set_3g_led 0 0 0
	killall gcom >/dev/null 2>/dev/null
}

setup_interface_3g() {
	local iface="$1"
	local config="$2"
	
	config_get device "$config" device

	for module in slhc ppp_generic ppp_async; do
		/sbin/insmod $module 2>&- >&-
	done

	config_get apn "$cfg" apn
	config_get service "$cfg" service
	config_get pincode "$cfg" pincode
	config_get mtu "$cfg" mtu

	set_3g_led 1 0 1

	# figure out hardware specific commands for the card
	if gcom -d "$device" -s /etc/gcom/getcardinfo.gcom | grep Novatel 2>/dev/null >/dev/null; then
		case "$service" in
			umts_only) CODE=2;;
			gprs_only) CODE=1;;
			*) CODE=0;;
		esac
		mode="AT\$NWRAT=${CODE},2"
	else
		case "$service" in
			umts_only) CODE=1;;
			gprs_only) CODE=0;;
			*) CODE=3;;
		esac
		mode="AT_OPSYS=${CODE}"
	fi
	
	PINCODE="$pincode" gcom -d "$device" -s /etc/gcom/setpin.gcom || {
		echo "$cfg(3g): Failed to set the PIN code."
		set_3g_led 0 0 0
		return 1
	}
	MODE="$mode" gcom -d "$device" -s /etc/gcom/setmode.gcom
	set_3g_led 1 0 0

	start_pppd "$config" \
		noaccomp \
		nopcomp \
		novj \
		nobsdcomp \
		noauth \
		lock \
		crtscts \
		connect "USE_APN=\"$apn\" /etc/ppp/3g.connect" \
		${mtu:+mtu $mtu mru $mtu} \
		460800 "$device"
}
