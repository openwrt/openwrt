#!/bin/sh
. /lib/netifd/mac80211.sh

append DRIVERS "mac80211"

. /lib/functions/system.sh

lookup_phy() {
	[ -n "$phy" ] && {
		[ -d /sys/class/ieee80211/$phy ] && return
	}

	local devpath
	config_get devpath "$device" path
	[ -n "$devpath" ] && {
		phy="$(mac80211_path_to_phy "$devpath")"
		[ -n "$phy" ] && return
	}

	local macaddr="$(config_get "$device" macaddr | tr 'A-Z' 'a-z')"
	[ -n "$macaddr" ] && {
		for _phy in /sys/class/ieee80211/*; do
			[ -e "$_phy" ] || continue

			[ "$macaddr" = "$(cat ${_phy}/macaddress)" ] || continue
			phy="${_phy##*/}"
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
	CFG=/etc/board.json

	[ -s "$CFG" ] || touch "/tmp/wifi.update"
	json_load "$(cat ${CFG})"
	if json_is_a system object; then
		json_select system
			local wifi_ssid wifi_key wifi_country wifi_txpower wifi_default
			json_get_vars wifi_ssid wifi_key wifi_country wifi_txpower wifi_default
		json_select ..
	fi

	encryption="psk2"
	key=$(< /dev/urandom tr -dc ".,+:;%_A-Z-0-9" | head -c${1:-40};echo;)

	while :; do
		config_get type "radio$devidx" type
		[ -n "$type" ] || break
		devidx=$(($devidx + 1))
	done

	for _dev in /sys/class/ieee80211/*; do
		[ -e "$_dev" ] || continue

		dev="${_dev##*/}"

		found=0
		config_foreach check_mac80211_device wifi-device
		[ "$found" -gt 0 ] && continue

		mode_band="g"
		channel="1"
		htmode=""
		ht_capab=""
		[ -n "$wifi_txpower" ] && tx_power="set wireless.radio${devidx}.txpower=$wifi_txpower" \
			|| [ -n "$wifi_country" ] || tx_power="set wireless.radio${devidx}.txpower=5"
		[ -n "$wifi_country" ] && country="$wifi_country" || country="00"
		[ -n "$wifi_ssid" ] && ssid=$wifi_ssid || ssid=OpenWrt-$(hexdump -ve '"%02X"' -n4 /dev/urandom)

		iw phy "$dev" info | grep -q 'Capabilities:' && htmode=HT20

		iw phy "$dev" info | grep -q '5180 MHz' && {
			mode_band="a"
			channel="36"
			iw phy "$dev" info | grep -q 'VHT Capabilities' && htmode="VHT80"
		}

		[ -n "$htmode" ] && ht_capab="set wireless.radio${devidx}.htmode=$htmode"
		disable_legacy=$( [ "$mode_band" = "g" ] && echo "set wireless.radio${devidx}.legacy_rates='0'" )

		disabled=1
		grep -q $'^\xf7$' $(find /sys/firmware/devicetree -name linux,code) || \
			if [ -n "$wifi_default" ] ; then
				[ "$wifi_default" = "$mode_band" ] && disabled="0"
			else
				[[ -n "$wifi_key" && -n "$wifi_country" || "$mode_band" = "g" ]] && disabled=0
			fi

		path="$(mac80211_phy_to_path "$dev")"
		if [ -n "$path" ]; then
			dev_id="set wireless.radio${devidx}.path='$path'"
		else
			dev_id="set wireless.radio${devidx}.macaddr=$(cat /sys/class/ieee80211/${dev}/macaddress)"
		fi

		uci -q batch <<-EOF
			set wireless.radio${devidx}=wifi-device
			set wireless.radio${devidx}.type=mac80211
			set wireless.radio${devidx}.channel=${channel}
			set wireless.radio${devidx}.hwmode=11${mode_band}
			${disable_legacy}
			${tx_power}
			${dev_id}
			${ht_capab}
			set wireless.radio${devidx}.country=${country}
			set wireless.radio${devidx}.disabled=${disabled}

			set wireless.default_radio${devidx}=wifi-iface
			set wireless.default_radio${devidx}.device=radio${devidx}
			set wireless.default_radio${devidx}.network=lan
			set wireless.default_radio${devidx}.mode=ap
			set wireless.default_radio${devidx}.ssid=${ssid}
			set wireless.default_radio${devidx}.encryption=${encryption}
			set wireless.default_radio${devidx}.key=${key}
EOF
		uci -q commit wireless

		devidx=$(($devidx + 1))
	done

	uci -q set system.@system[0].default_key=$key && uci -q commit system
}
