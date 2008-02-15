#!/bin/sh
append DRIVERS "prism2"

scan_prism2() {
	local device="$1"
	
	config_get mode "$device" mode
	case "$mode" in
		ad-hoc|managed|master)
		;;
		*) echo "$device: Invalid mode, ignored."; continue;;
	esac
	
}

disable_prism2() (
	local device="$1"
	
	set_wifi_down "$device"

	include /lib/network
	ifconfig "$device" down 
	unbridge "$device"
	return 0
)

enable_prism2() {
	local irqdevs

	config_get prifw "$device" prifw
	config_get stafw "$device" stafw
	config_get mode "$device" mode
	config_get rate "$device" rate
	config_get channel "$device" channel
	config_get txpower "$device" txpower
	config_get ssid "$device" ssid
	config_get maclist "$device" maclist
	config_get macpolicy "$device" macpolicy
	[ -f "$prifw" ] || [ -f "$stafw" ] && [ -x /usr/sbin/prism2_srec ] && {
		irqdevs=$(cat /proc/interrupts | grep wifi${device##wlan} | cut -b 37- | tr -d ",")
		for dev in $irqdevs; do
			[ "$(config_get "$dev" type)" = "atheros" ] && wifi down "$dev"
		done
		[ -f "$prifw" ] && prism2_srec -g $device $prifw
		[ -f "$stafw" ] && prism2_srec -r $device $stafw
		for dev in $irqdevs; do
			[ "$(config_get "$dev" type)" = "atheros" ] && wifi up "$dev"
		done
	}
	[ -n "$mode" ] && iwconfig $device mode $mode
	[ -n "$rate" ] && iwconfig $device rate $rate
	[ -n "$channel" ] && iwconfig $device channel $channel
	[ -n "$ssid" ] && iwconfig $device essid $ssid
	ifconfig "$device" up
	[ -n "$txpower" ] && iwconfig $device txpower $txpower
	[ -n "$maclist" ] && {
		# flush MAC list
		iwpriv $device maccmd 3
		for mac in $maclist; do
			iwpriv $device addmac $mac
		done
	}
	case "$macpolicy" in
		open)
			iwpriv $device maccmd 0
		;;
		allow)
			iwpriv $device maccmd 1
		;;
		deny)
			iwpriv $device maccmd 2
		;;
		*)
			# default deny policy if mac list exists
			[ -n "$maclist" ] && iwpriv $device maccmd 2
		;;
	esac
	# kick all stations if we have policy explicitly set
	[ -n "$macpolicy" ] && iwpriv $device maccmd 4
	local net_cfg bridge
	net_cfg="$(find_net_config "$device")"
	[ -z "$net_cfg" ] || {
		bridge="$(bridge_interface "$net_cfg")"
		config_set "$device" bridge "$bridge"
		start_net "$device" "$net_cfg"
	}
	set_wifi_up "$device" "$device"
}


detect_prism2() {
	cd /proc/net/hostap
	[ -d wlan0 ] || return
	for dev in wlan*; do
		config_get type "$dev" type
		[ "$type" = prism2 ] && continue
		cat <<EOF

config wifi-device $dev
	option type	prism2
#	option rate	11M
	option channel	5
	option txpower	127
	option mode     managed
	option ssid     OpenWrt
#	option macpolicy deny
#	option maclist	'12:34:56:78:90:12
#			 09:87:65:43:21:09'

	# REMOVE THIS LINE TO ENABLE WIFI:
	option disabled 1
	
EOF
	done
}
