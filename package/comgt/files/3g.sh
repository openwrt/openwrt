set_3g_led() {
	# set on WRT54G3G only
	[ -d /proc/diag ] || return 0
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
		for device in /dev/ttyUSB0 /dev/ttyUSB1 /dev/ttyUSB2 /dev/tts/2 /dev/usb/tts/0 /dev/noz0; do
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
	local chat="/etc/chatscripts/3g.chat"
	
	config_get device "$config" device
	config_get maxwait "$config" maxwait
	maxwait=${maxwait:-20}
	while [ ! -e "$device" -a $maxwait -gt 0 ];do # wait for driver loading to catch up
		maxwait=$(($maxwait - 1))
		sleep 1
	done

	for module in slhc ppp_generic ppp_async; do
		/sbin/insmod $module 2>&- >&-
	done

	config_get apn "$config" apn
	config_get service "$config" service
	config_get pincode "$config" pincode
	config_get mtu "$config" mtu

	set_3g_led 1 0 1

	# figure out hardware specific commands for the card
	case "$service" in
		cdma|evdo) chat="/etc/chatscripts/evdo.chat";;
	*)
		cardinfo=$(gcom -d "$device" -s /etc/gcom/getcardinfo.gcom)
		if echo "$cardinfo" | grep Novatel; then
			case "$service" in
				umts_only) CODE=2;;
				gprs_only) CODE=1;;
				*) CODE=0;;
			esac
			mode="AT\$NWRAT=${CODE},2"
		elif echo "$cardinfo" | grep Option; then
			case "$service" in
				umts_only) CODE=1;;
				gprs_only) CODE=0;;
				*) CODE=3;;
			esac
			mode="AT_OPSYS=${CODE}"
		fi
		# Don't assume Option to be default as it breaks with Huawei Cards/Sticks
		
		PINCODE="$pincode" gcom -d "$device" -s /etc/gcom/setpin.gcom || {
			echo "$config(3g): Failed to set the PIN code."
			set_3g_led 0 0 0
			return 1
		}
		test -z "$mode" || {
			MODE="$mode" gcom -d "$device" -s /etc/gcom/setmode.gcom
		}
	esac
	set_3g_led 1 0 0

	config_set "$config" "connect" "${apn:+USE_APN=$apn }/usr/sbin/chat -t5 -v -E -f $chat"
	start_pppd "$config" \
		noaccomp \
		nopcomp \
		novj \
		nobsdcomp \
		noauth \
		lock \
		crtscts \
		${mtu:+mtu $mtu mru $mtu} \
		115200 "$device"
}
