#!/bin/sh
append DRIVERS "prism2"

find_prism2_phy() {
	local device="$1"

	local macaddr="$(config_get "$device" macaddr | tr 'A-Z' 'a-z')"
	config_get phy "$device" phy
	[ -z "$phy" -a -n "$macaddr" ] && {
		cd /proc/net/hostap
		for phy in $(ls -d wlan* 2>&-); do
			[ "$macaddr" = "$(cat /sys/class/net/${phy}/address)" ] || continue
			config_set "$device" phy "$phy"
			break
		done
		config_get phy "$device" phy
	}
	[ -n "$phy" -a -d "/proc/net/hostap/$phy" ] || {
		echo "phy for wifi device $1 not found"
		return 1
	}
	[ -z "$macaddr" ] && {
		config_set "$device" macaddr "$(cat /sys/class/net/${phy}/address)"
	}
	return 0
}

scan_prism2() {
	local device="$1"
	local mainvif
	local wds

	[ ${device%[0-9]} = "wlan" ] && config_set "$device" phy "$device" || find_prism2_phy "$device" || {
		config_unset "$device" vifs
		return 0
	}
	config_get phy "$device" phy

	config_get vifs "$device" vifs
	local _c=0
	for vif in $vifs; do
		config_get mode "$vif" mode
		case "$mode" in
			adhoc|sta|ap|monitor)
				# Only one vif is allowed on AP, station, Ad-hoc or monitor mode
				[ -z "$mainvif" ] && {
					mainvif="$vif"
					config_set "$vif" ifname "$phy"
				}
			;;
			wds)
				config_get ssid "$vif" ssid
				[ -z "$ssid" ] && continue
				config_set "$vif" ifname "${phy}wds${_c}"
				_c=$(($_c + 1))
				addr="$ssid"
				${addr:+append wds "$vif"}
			;;
			*) echo "$device($vif): Invalid mode, ignored."; continue;;
		esac
	done
	config_set "$device" vifs "${mainvif:+$mainvif }${wds:+$wds}"
}

disable_prism2() (
	local device="$1"

	find_prism2_phy "$device" || return 0
	config_get phy "$device" phy

	set_wifi_down "$device"

	include /lib/network
	while read line < /proc/net/hostap/${phy}/wds; do
		set $line
		[ -f "/var/run/wifi-${1}.pid" ] &&
			kill "$(cat "/var/run/wifi-${1}.pid")"
		ifconfig "$1" down
		unbridge "$1"
		iwpriv "$phy" wds_del "$2"
	done
	unbridge "$phy"
	return 0
)

enable_prism2() {
	local device="$1"

	find_prism2_phy "$device" || return 0
	config_get phy "$device" phy

	config_get rxantenna "$device" rxantenna
	config_get txantenna "$device" txantenna
	config_get_bool diversity "$device" diversity
	[ -n "$diversity" ] && {
		rxantenna="1"
		txantenna="1"
	}
	[ -n "$rxantenna" ] && iwpriv "$phy" antsel_rx "$rxantenna"
	[ -n "$txantenna" ] && iwpriv "$phy" antsel_tx "$txantenna"

	config_get channel "$device" channel
	[ -n "$channel" ] && iwconfig "$phy" channel "$channel" >/dev/null 2>/dev/null

	config_get txpower "$device" txpower
	[ -n "$txpower" ] && iwconfig "$phy" txpower "${txpower%%.*}"

	config_get vifs "$device" vifs
	local first=1
	for vif in $vifs; do
		config_get ifname "$vif" ifname
		config_get ssid "$vif" ssid
		config_get mode "$vif" mode

		[ "$mode" = "wds" ] || iwconfig "$phy" essid -- "$ssid"

		case "$mode" in
			sta)
				iwconfig "$phy" mode managed
				config_get addr "$device" bssid
				[ -z "$addr" ] || { 
					iwconfig "$phy" ap "$addr"
				}
			;;
			ap) iwconfig "$phy" mode master;;
			wds) iwpriv "$phy" wds_add "$ssid";;
			*) iwconfig "$phy" mode "$mode";;
		esac

		[ "$first" = 1 ] && {
			config_get rate "$vif" rate
			[ -n "$rate" ] && iwconfig "$phy" rate "${rate%%.*}"

			config_get_bool hidden "$vif" hidden 0
			iwpriv "$phy" enh_sec "$hidden"

			config_get frag "$vif" frag
			[ -n "$frag" ] && iwconfig "$phy" frag "${frag%%.*}"

			config_get rts "$vif" rts
			[ -n "$rts" ] && iwconfig "$phy" rts "${rts%%.*}"

			config_get maclist "$vif" maclist
			[ -n "$maclist" ] && {
				# flush MAC list
				iwpriv "$phy" maccmd 3
				for mac in $maclist; do
					iwpriv "$phy" addmac "$mac"
				done
			}
			config_get macpolicy "$vif" macpolicy
			case "$macpolicy" in
				allow)
					iwpriv "$phy" maccmd 2
				;;
				deny)
					iwpriv "$phy" maccmd 1
				;;
				*)
					# default deny policy if mac list exists
					[ -n "$maclist" ] && iwpriv "$phy" maccmd 1
				;;
			esac
			# kick all stations if we have policy explicitly set
			[ -n "$macpolicy" ] && iwpriv "$phy" maccmd 4
		}

		config_get enc "$vif" encryption
		case "$enc" in
			WEP|wep)
				for idx in 1 2 3 4; do
					config_get key "$vif" "key${idx}"
					iwconfig "$ifname" enc "[$idx]" "${key:-off}"
				done
				config_get key "$vif" key
				key="${key:-1}"
				case "$key" in
					[1234]) iwconfig "$ifname" enc "[$key]";;
					*) iwconfig "$ifname" enc "$key";;
				esac
			;;
			psk*|wpa*)
				start_hostapd=1
				config_get key "$vif" key
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

		case "$mode" in
			ap)
				if [ -n "$start_hostapd" ] && eval "type hostapd_setup_vif" 2>/dev/null >/dev/null; then
					hostapd_setup_vif "$vif" hostap || {
						echo "enable_prism2($device): Failed to set up hostapd for interface $ifname" >&2
						# make sure this wifi interface won't accidentally stay open without encryption
						ifconfig "$ifname" down
						continue
					}
				fi
			;;
			wds|sta)
				if eval "type wpa_supplicant_setup_vif" 2>/dev/null >/dev/null; then
					wpa_supplicant_setup_vif "$vif" hostap || {
						echo "enable_prism2($device): Failed to set up wpa_supplicant for interface $ifname" >&2
						ifconfig "$ifname" down
						continue
					}
				fi
			;;
		esac
		first=0
	done

}

check_device() {
	[ ${1%[0-9]} = "wlan" ] && config_set "$1" phy "$1"
	config_get phy "$1" phy
	[ -z "$phy" ] && {
		find_prism2_phy "$1" >/dev/null || return 0
		config_get phy "$1" phy
	}
	[ "$phy" = "$dev" ] && found=1
}

detect_prism2() {
	devidx=0
	config_load wireless
	while :; do
		config_get type "radio$devidx" type
		[ -n "$type" ] || break
		devidx=$(($devidx + 1))
	done
	cd /proc/net/hostap
	[ -d wlan* ] || return
	for dev in $(ls -d wlan* 2>&-); do
		found=0
		config_foreach check_device wifi-device
		[ "$found" -gt 0 ] && continue
		cat <<EOF

config wifi-device radio$devidx
	option type	prism2
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
