#!/bin/sh
append DRIVERS "mac80211"

lookup_phy() {
	[ -n "$phy" ] && {
		[ -d /sys/class/ieee80211/$phy ] && return
	}

	local devpath
	config_get devpath "$device" path
	[ -n "$devpath" -a -d "/sys/devices/$devpath/ieee80211" ] && {
		phy="$(ls /sys/devices/$devpath/ieee80211 | grep -m 1 phy)"
		[ -n "$phy" ] && return
	}

	local macaddr="$(config_get "$device" macaddr | tr 'A-Z' 'a-z')"
	[ -n "$macaddr" ] && {
		for _phy in $(ls /sys/class/ieee80211 2>/dev/null); do
			[ "$macaddr" = "$(cat /sys/class/ieee80211/${_phy}/macaddress)" ] || continue
			phy="$_phy"
			return
		done
	}
	phy=
	return
}

find_mac80211_phy() {
	local device="$1"

	config_get phy "$device" phy
	lookup_phy
	[ -n "$phy" -a -d "/sys/class/ieee80211/$phy" ] || {
		echo "PHY for wifi device $1 not found"
		return 1
	}
	config_set "$device" phy "$phy"

	config_get macaddr "$device" macaddr
	[ -z "$macaddr" ] && {
		config_set "$device" macaddr "$(cat /sys/class/ieee80211/${phy}/macaddress)"
	}

	return 0
}

check_mac80211_device() {
	config_get phy "$1" phy
	[ -z "$phy" ] && {
		find_mac80211_phy "$1" >/dev/null || return 0
		config_get phy "$1" phy
	}
	[ "$phy" = "$dev" ] && found=1
}

detect_mac80211() {
	devidx=0
	config_load wireless
	while :; do
		config_get type "radio$devidx" type
		[ -n "$type" ] || break
		devidx=$(($devidx + 1))
	done
	for dev in $(ls /sys/class/ieee80211); do
		found=0
		config_foreach check_mac80211_device wifi-device
		[ "$found" -gt 0 ] && continue

		mode_11n=""
		mode_band="g"
		channel="11"
		htmode=""

		ht_cap=0
		for cap in $(iw phy "$dev" info | grep 'Capabilities:' | cut -d: -f2); do
			ht_cap="$(($ht_cap | $cap))"
		done
		ht_capab="";
		[ "$ht_cap" -gt 0 ] && {
			mode_11n="n"
			htmode="HT20"

			list="	list ht_capab"
			[ "$(($ht_cap & 1))" -eq 1 ] && append ht_capab "$list	LDPC" "$N"
			[ "$(($ht_cap & 16))" -eq 16 ] && append ht_capab "$list	GF" "$N"
			[ "$(($ht_cap & 32))" -eq 32 ] && append ht_capab "$list	SHORT-GI-20" "$N"
			[ "$(($ht_cap & 64))" -eq 64 ] && append ht_capab "$list	SHORT-GI-40" "$N"
			[ "$(($ht_cap & 128))" -eq 128 ] && append ht_capab "$list	TX-STBC" "$N"
			[ "$(($ht_cap & 768))" -eq 256 ] && append ht_capab "$list	RX-STBC1" "$N"
			[ "$(($ht_cap & 768))" -eq 512 ] && append ht_capab "$list	RX-STBC12" "$N"
			[ "$(($ht_cap & 768))" -eq 768 ] && append ht_capab "$list	RX-STBC123" "$N"
			[ "$(($ht_cap & 4096))" -eq 4096 ] && append ht_capab "$list	DSSS_CCK-40" "$N"
		}
		iw phy "$dev" info | grep -q '2412 MHz' || { mode_band="a"; channel="36"; }

		vht_cap=$(iw phy "$dev" info | grep -c 'VHT Capabilities')
		[ "$vht_cap" -gt 0 ] && {
			mode_band="a";
			channel="36"
			htmode="VHT80"
		}

		[ -n $htmode ] && append ht_capab "	option htmode	$htmode" "$N"

		if [ -x /usr/bin/readlink ]; then
			path="$(readlink -f /sys/class/ieee80211/${dev}/device)"
			path="${path##/sys/devices/}"
			dev_id="	option path	'$path'"
		else
			dev_id="	option macaddr	$(cat /sys/class/ieee80211/${dev}/macaddress)"
		fi

		cat <<EOF
config wifi-device  radio$devidx
	option type     mac80211
	option channel  ${channel}
	option hwmode	11${mode_11n}${mode_band}
$dev_id
$ht_capab
	# REMOVE THIS LINE TO ENABLE WIFI:
	option disabled 1

config wifi-iface
	option device   radio$devidx
	option network  lan
	option mode     ap
	option ssid     OpenWrt
	option encryption none

EOF
	devidx=$(($devidx + 1))
	done
}

