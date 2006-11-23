#!/bin/sh
set -x
append DRIVERS "atheros"

scan_atheros() {
	local device="$1"
	local wds
	local adhoc sta ap
	
	config_get vifs "$device" vifs
	for vif in $vifs; do
	
		config_get ifname "$vif" ifname
		config_set "$vif" ifname "${ifname:-ath}"
		
		config_get mode "$vif" mode
		case "$mode" in
			adhoc|sta|ap)
				append $mode "$vif"
			;;
			wds)
				config_get addr "$vif" bssid
				${addr:+append wds "$addr"}
			;;
			*) echo "$device($vif): Invalid mode, ignored."; continue;;
		esac
	done

	case "${adhoc:+1}:${sta:+}:${ap+1}" in
		# valid mode combinations
		1::);;
		:1:1)config_set "$device" nosbeacon 1;; # AP+STA, can't use beacon timers for STA
		:1:);;
		::1);;
		:::);;
		*) echo "$device: Invalid mode combination in config"; return 1;;
	esac

	config_set "$device" vifs "${ap:+$ap }${adhoc:+$adhoc }${sta:+$sta }"
	
	[ -z "$adhoc" ] && config_set "$device" wds "$wdsifs"
}

disable_atheros() (
	local device="$1"
	
	include /lib/network
	cd /proc/sys/net
	for dev in *; do
		grep "$device" "$dev/%parent" >/dev/null 2>/dev/null && {
			ifconfig "$dev" down 
			unbridge "$dev"
			wlanconfig $dev destroy
		}
	done
)

enable_atheros() {
	config_get channel "$device" channel
	config_get wds "$device" wds
	config_get vifs "$device" vifs

	for vif in $vifs; do
		nosbeacon=
		config_get ifname "$vif" ifname
		config_get enc "$vif" encryption
		config_get mode "$vif" mode
		
		[ "$mode" = sta ] && config_get nosbeacon "$device" nosbeacon
		
		config_get ifname "$vif" ifname
		ifname=$(wlanconfig "$ifname" create wlandev "$device" wlanmode "$mode" ${nosbeacon:+nosbeacon})
		config_set "$vif" ifname "$ifname"
		
		[ $? -ne 0 ] && {
			echo "enable_atheros($device): Failed to set up vif $ifname" >&2
			continue
		}
		wpa=
		case "$enc" in
			WEP|wep)
				for idx in 1 2 3 4; do
					config_get key "$vif" "key${idx}"
					iwconfig "$ifname" enc "[$idx]" "${key:-off}"
				done
				config_get key "$vif" key
				iwconfig "$ifname" enc "$key"
			;;
		esac
		case "$mode" in
			ap)
				local hostapd_cfg=
				case "$enc" in
					*psk*|*PSK*)
					# FIXME: wpa
					;;
					*wpa*|*WPA*)
					# FIXME: add wpa+radius here
					;;
				esac
			;;
			sta)
				# FIXME: implement wpa_supplicant calls here
			;;
		esac	
		
		config_get ssid "$vif" ssid
		append if_up "iwconfig $ifname essid $ssid channel $channel" ";$N"
		append if_up "sleep 1" ";$N"
		append if_up "ifconfig $ifname up" ";$N"
		
		local net_cfg bridge
		net_cfg="$(find_net_config "$vif")"
		[ -z "$net_cfg" ] || {
			bridge="$(bridge_interface "$net_cfg")"
			append if_up "start_net '$ifname' '$net_cfg'" ";$N"
		}
		# TODO: start hostapd
	done
	
	#killall -KILL $hostapd >&- 2>&-
	eval "$if_up"
}


detect_atheros() {
	cd /proc/sys/dev
	[ -d ath ] || return
	for dev in wifi*; do
		config_get type "$dev" type
		[ "$type" = atheros ] && return
		cat <<EOF
config wifi-device  $dev
	option type     atheros
	option channel  5

config wifi-iface
	option device   $dev
	option mode     ap
	option ssid     OpenWrt
	option hidden   0
	option encryption none

EOF
	done
}

