#!/bin/sh
append DRIVERS "acx"

find_acx_phy() {
	local device="$1"

	local macaddr="$(config_get "$device" macaddr | tr 'A-Z' 'a-z')"
	config_get phy "$device" phy
	[ -z "$phy" -a -n "$macaddr" ] && {
		cd /proc/driver
		for phy in $(ls acx_wlan[0-9] 2>&-); do
			phy=${phy#acx_}
			[ "$macaddr" = "$(cat /sys/class/net/${phy}/address)" ] || continue
			config_set "$device" phy "$phy"
			break
		done
		config_get phy "$device" phy
	}
	[ -n "$phy" -a -f "/proc/driver/acx_$phy" ] || {
		echo "phy for wifi device $1 not found"
		return 1
	}
	[ -z "$macaddr" ] && {
		config_set "$device" macaddr "$(cat /sys/class/net/${phy}/address)"
	}
	return 0
}

scan_acx() {
	local device="$1"

	[ ${device%[0-9]} = "wlan" ] && config_set "$device" phy "$device" || find_acx_phy "$device" || {
		config_unset "$device" vifs
		return 0
	}
	config_get phy "$device" phy

	config_get vifs "$device" vifs
	for vif in $vifs; do
		config_get mode "$vif" mode
		case "$mode" in
			adhoc|sta|ap|monitor)
				# Only one vif is allowed
				config_set "$vif" ifname "$phy"
				break
			;;
			*) echo "$device($vif): Invalid mode, ignored."; continue;;
		esac
	done
	config_set "$device" vifs "$vif"
}

disable_acx() (
	local device="$1"

	find_acx_phy "$device" || return 0
	config_get phy "$device" phy

	set_wifi_down "$device"

	include /lib/network
	unbridge "$phy"
	return 0
)

enable_acx() {
	local device="$1"

	find_acx_phy "$device" || return 0
	config_get phy "$device" phy

	config_get regdomain "$device" regdomain
	[ -n "$regdomain" ] && iwpriv "$device" SetRegDomain "$regdomain"

	config_get rxantenna "$device" rxantenna
	config_get txantenna "$device" txantenna
	config_get_bool diversity "$device" diversity
	[ -n "$diversity" ] && {
		rxantenna="2"
	}
	[ -n "$rxantenna" ] && iwpriv "$phy" SetRxAnt "$rxantenna"
	[ -n "$txantenna" ] && iwpriv "$phy" SetTxAnt "$txantenna"

	config_get channel "$device" channel
	[ -n "$channel" ] && iwconfig "$phy" channel "$channel" >/dev/null 2>/dev/null

	config_get txpower "$device" txpower
	[ -n "$txpower" ] && iwconfig "$phy" txpower "${txpower%%.*}"

	config_get vif "$device" vifs

	config_get ifname "$vif" ifname
	config_get ssid "$vif" ssid
	config_get mode "$vif" mode

	iwconfig "$phy" essid ${ssid:+-- }"${ssid:-any}"

	case "$mode" in
		sta)
			iwconfig "$phy" mode managed
			config_get addr "$device" bssid
			[ -z "$addr" ] || { 
				iwconfig "$phy" ap "$addr"
			}
		;;
		ap) iwconfig "$phy" mode master;;
		adhoc) iwconfig "$phy" mode ad-hoc;;
		*) iwconfig "$phy" mode "$mode";;
	esac

	config_get frag "$vif" frag
	[ -n "$frag" ] && iwconfig "$phy" frag "${frag%%.*}"

	config_get rts "$vif" rts
	[ -n "$rts" ] && iwconfig "$phy" rts "${rts%%.*}"

	config_get enc "$vif" encryption
	case "$enc" in
		wep)
			for idx in 1 2 3 4; do
				config_get key "$vif" "key${idx}"
				iwconfig "$ifname" enc restricted "[$idx]" "${key:-off}"
			done
			config_get key "$vif" key
			key="${key:-1}"
			case "$key" in
				[1234]) iwconfig "$ifname" enc restricted "[$key]";;
				*) iwconfig "$ifname" enc restricted "$key";;
			esac
		;;
		psk*|wpa*)
			echo "$device($vif): WPA/WPA2 not supported by acx driver"
			return 1
		;;
	esac

	local net_cfg bridge
	net_cfg="$(find_net_config "$vif")"
	[ -z "$net_cfg" ] || {
		bridge="$(bridge_interface "$net_cfg")"
		config_set "$vif" bridge "$bridge"
		start_net "$ifname" "$net_cfg"
	}
	set_wifi_up "$vif" "$ifname"

}

check_acx_device() {
	[ ${1%[0-9]} = "wlan" ] && config_set "$1" phy "$1"
	config_get phy "$1" phy
	[ -z "$phy" ] && {
		find_acx_phy "$1" >/dev/null || return 0
		config_get phy "$1" phy
	}
	[ "$phy" = "$dev" ] && found=1
}

detect_acx() {
	devidx=0
	config_load wireless
	while :; do
		config_get type "radio$devidx" type
		[ -n "$type" ] || break
		devidx=$(($devidx + 1))
	done
	cd /proc/driver
	for dev in $(ls acx_wlan[0-9] 2>&-); do
		dev=${dev#acx_}
		found=0
		config_foreach check_acx_device wifi-device
		[ "$found" -gt 0 ] && continue
		cat <<EOF
config wifi-device radio$devidx
	option type     acx
	option channel  11
	option macaddr	$(cat /sys/class/net/${dev}/address)

	# REMOVE THIS LINE TO ENABLE WIFI:
	option disabled 1

config wifi-iface
	option device	radio$devidx
	option network	lan
	option mode	ap
	option ssid	OpenWrt
	option encryption none

EOF
	devidx=$(($devidx + 1))
	done
}
