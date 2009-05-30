#!/bin/sh
append DRIVERS "prism2"

scan_prism2() {
	local device="$1"
	local mainvif
	local wds
	
	config_get vifs "$device" vifs
	local _c=0
	for vif in $vifs; do
		config_get mode "$vif" mode
		case "$mode" in
			adhoc|sta|ap|monitor)
				# Only one vif is allowed on AP, station, Ad-hoc or monitor mode
				[ -z "$mainvif" ] && {
					mainvif="$vif"
					config_set "$vif" ifname "$device"
				}
			;;
			wds)
				config_get ssid "$vif" ssid
				[ -z "$ssid" ] && continue
				config_set "$vif" ifname "${device}wds${_c}"
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
	
	set_wifi_down "$device"

	include /lib/network
	while read line < /proc/net/hostap/${device}/wds; do
		set $line
		[ -f "/var/run/wifi-${1}.pid" ] &&
			kill "$(cat "/var/run/wifi-${1}.pid")"
		ifconfig "$1" down
		unbridge "$1"
		iwpriv "$device" wds_del "$2"
	done
	unbridge "$device"
	return 0
)

enable_prism2() {
	local device="$1"

	config_get rxantenna "$device" rxantenna
	[ -n "$rxantenna" ] && iwpriv $device antsel_rx "$rxantenna"

	config_get txantenna "$device" txantenna
	[ -n "$txantenna" ] && iwpriv $device antsel_tx "$txantenna"

	config_get channel "$device" channel
	[ -n "$channel" ] && iwconfig "$device" channel "$channel" >/dev/null 2>/dev/null

	config_get txpower "$device" txpower
	[ -n "$txpower" ] && iwconfig "$device" txpower "${txpower%%.*}"

	config_get vifs "$device" vifs
	local first=1
	for vif in $vifs; do
		config_get ifname "$vif" ifname
		config_get ssid "$vif" ssid
		config_get mode "$vif" mode

		[ "$mode" = "wds" ] || iwconfig "$device" essid "$ssid"

		case "$mode" in
			sta) iwconfig "$device" mode managed;;
			ap) iwconfig "$device" mode master;;
			wds) iwpriv "$device" wds_add "$ssid";;
			*) iwconfig "$device" mode "$mode";;
		esac

		[ "$first" = 1 ] && {
			config_get rate "$vif" rate
			[ -n "$rate" ] && iwconfig "$device" rate "${rate%%.*}"

			config_get_bool hidden "$vif" hidden 0
			iwpriv "$ifname" enh_sec "$hidden"

			[ -n "$maclist" ] && {
				# flush MAC list
				iwpriv "$device" maccmd 3
				for mac in $maclist; do
					iwpriv "$device" addmac "$mac"
				done
			}
			case "$macpolicy" in
				allow)
					iwpriv $device maccmd 2
				;;
				deny)
					iwpriv $device maccmd 1
				;;
				*)
					# default deny policy if mac list exists
					[ -n "$maclist" ] && iwpriv $device maccmd 1
				;;
			esac
			# kick all stations if we have policy explicitly set
			[ -n "$macpolicy" ] && iwpriv $device maccmd 4
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

detect_prism2() {
	cd /proc/net/hostap
	[ -d wlan* ] || return
	for dev in $(ls -d wlan* 2>&-); do
		config_get type "$dev" type
		[ "$type" = prism2 ] && continue
		cat <<EOF

config wifi-device $dev
	option type	prism2
	option channel  6

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
