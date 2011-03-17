#!/bin/sh
append DRIVERS "acx"

scan_acx() {
	local device="$1"
	local adhoc sta ap
	
	config_get vifs "$device" vifs
	for vif in $vifs; do
	
		config_get ifname "$vif" ifname
		config_set "$vif" ifname "${ifname:-$device}"
		
		config_get mode "$vif" mode
		case "$mode" in
			adhoc|sta|ap|monitor)
				append $mode "$vif"
			;;
			*) echo "$device($vif): Invalid mode, ignored."; continue;;
		esac
	done

	config_set "$device" vifs "${ap:+$ap }${adhoc:+$adhoc }${ahdemo:+$ahdemo }${sta:+$sta }${wds:+$wds }${monitor:+$monitor}"
}


disable_acx() (
	local device="$1"

	set_wifi_down "$device"
	# kill all running hostapd and wpa_supplicant processes that
	# are running on atheros/mac80211 vifs 
	for pid in `pidof hostapd wpa_supplicant`; do
		grep wlan /proc/$pid/cmdline >/dev/null && \
			kill $pid
	done
	
	include /lib/network
	cd /proc/sys/net
	for dev in *; do
		grep "$device" "$dev/%parent" >/dev/null 2>/dev/null && {
			ifconfig "$dev" down 
			unbridge "$dev"
		}
	done
	return 0
)

enable_acx() {
	local device="$1"
	config_get channel "$device" channel
	config_get vifs "$device" vifs
	config_get country "$device" country
	
	local first=1
	for vif in $vifs; do
		config_get ifname "$vif" ifname
		ifconfig "$ifname" down
		config_get enc "$vif" encryption
		config_get eap_type "$vif" eap_type
		config_get mode "$vif" mode
		
		config_get ifname "$vif" ifname
		[ $? -ne 0 ] && {
			echo "enable_acx($device): Failed to set up $mode vif $ifname" >&2
			continue
		}
		config_set "$vif" ifname "$ifname"

		[ "$first" = 1 ] && {
			# only need to change freq band and channel on the first vif
			case $country in
				us)
					iwpriv "$device" SetRegDomain 1
					;;
				ca)
					iwpriv "$device" SetRegDomain 2
					;;
				de|uk|be|hu|nl|pt|pl|se|dk)
					iwpriv "$device" SetRegDomain 3
					;;
				es)
					iwpriv "$device" SetRegDomain 4
					;;
				fr)
					iwpriv "$device" SetRegDomain 5
					;;
				jp)
					iwpriv "$device" SetRegDomain 7
					;;
				il)
					iwpriv "$device" SetRegDomain 8
			esac
					
			iwconfig "$ifname" channel "$channel" >/dev/null 2>/dev/null
			if [ "$mode" = adhoc ]; then
				iwlist "$ifname" scan >/dev/null 2>/dev/null
				sleep 1
				iwconfig "$ifname" mode ad-hoc >/dev/null 2>/dev/null
			fi
			sleep 1
			iwconfig "$ifname" channel "$channel" >/dev/null 2>/dev/null
		}

		case "$mode" in
			sta)
				iwconfig "$ifname" mode managed >/dev/null 2>/dev/null
				;;
			ap)
				iwconfig "$ifname" mode master >/dev/null 2>/dev/null
				;;
			*)
				iwconfig "$ifname" mode $mode >/dev/null 2>/dev/null
		esac
	
		wpa=
		case "$enc" in
			WEP|wep)
				for idx in 1 2 3 4; do
					config_get key "$vif" "key${idx}"
					iwconfig "$ifname" enc "[$idx]" "${key:-off}"
				done
				config_get key "$vif" key
				key="${key:-1}"
				case "$key" in
					[1234]) iwconfig "$ifname" enc restricted "[$key]";;
					*) iwconfig "$ifname" enc restricted "$key";;
				esac
			;;
			PSK|psk|PSK2|psk2)
				echo "WARNING WPA / WPA2 not supported by acx driver"
				config_get key "$vif" key
			;;
		esac

		case "$mode" in
			adhoc)
				config_get addr "$vif" bssid
				[ -z "$addr" ] || { 
					iwconfig "$ifname" ap "$addr"
				}
			;;
		esac
		config_get ssid "$vif" ssid

		config_get txpwr "$vif" txpower
		if [ -n "$txpwr" ]; then
			iwconfig "$ifname" txpower "${txpwr%%.*}"
		fi

		config_get frag "$vif" frag
		if [ -n "$frag" ]; then
			iwconfig "$ifname" frag "${frag%%.*}"
		fi

		config_get rts "$vif" rts
		if [ -n "$rts" ]; then
			iwconfig "$ifname" rts "${rts%%.*}"
		fi

		ifconfig "$ifname" up
		iwconfig "$ifname" channel "$channel" >/dev/null 2>/dev/null 

		local net_cfg bridge
		net_cfg="$(find_net_config "$vif")"
		[ -z "$net_cfg" ] || {
			bridge="$(bridge_interface "$net_cfg")"
			config_set "$vif" bridge "$bridge"
			start_net "$ifname" "$net_cfg"
		}
		iwconfig "$ifname" essid "$ssid"
		set_wifi_up "$vif" "$ifname"
		first=0
	done
	
	echo 1 >/sys/class/leds/wifi/brightness || :
}


detect_acx() {
	cd /sys/class/net
	for dev in $(ls -d wlan* 2>&-); do
		config_get type "$dev" type
		[ "$type" = acx ] && continue
		cat <<EOF
config wifi-device  $dev
	option type     acx
	option channel  11

	# REMOVE THIS LINE TO ENABLE WIFI:
	option disabled 1

config wifi-iface
	option device	$dev
	option network	lan
	option mode	ap
	option ssid	OpenWrt
	option encryption none

EOF
	done
}
