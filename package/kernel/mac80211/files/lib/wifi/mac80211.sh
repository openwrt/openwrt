#!/bin/sh

append DRIVERS "mac80211"

check_mac80211_device() {
	local device="$1"
	local path="$2"
	local macaddr="$3"

	[ -n "$found" ] && return 0

	phy_path=
	config_get phy "$device" phy
	json_select wlan
	[ -n "$phy" ] && case "$phy" in
		phy*)
			[ -d /sys/class/ieee80211/$phy ] && \
				phy_path="$(iwinfo nl80211 path "$dev")"
		;;
		*)
			if json_is_a "$phy" object; then
				json_select "$phy"
				json_get_var phy_path path
				json_select ..
			elif json_is_a "${phy%.*}" object; then
				json_select "${phy%.*}"
				json_get_var phy_path path
				json_select ..
				phy_path="$phy_path+${phy##*.}"
			fi
		;;
	esac
	json_select ..
	[ -n "$phy_path" ] || config_get phy_path "$device" path
	[ -n "$path" -a "$phy_path" = "$path" ] && {
		found=1
		return 0
	}

	config_get dev_macaddr "$device" macaddr

	[ -n "$macaddr" -a "$dev_macaddr" = "$macaddr" ] && found=1

	return 0
}


__get_band_defaults() {
	local phy="$1"

	( iw phy "$phy" info; echo ) | awk '
BEGIN {
        bands = ""
}

($1 == "Band" || $1 == "") && band {
        if (channel) {
		mode="NOHT"
		if (ht) mode="HT20"
		if (vht && band != "1:") mode="VHT80"
		if (he) mode="HE80"
		if (he && band == "1:") mode="HE20"
                sub("\\[", "", channel)
                sub("\\]", "", channel)
                bands = bands band channel ":" mode " "
        }
        band=""
}

$1 == "Band" {
        band = $2
        channel = ""
	vht = ""
	ht = ""
	he = ""
}

$0 ~ "Capabilities:" {
	ht=1
}

$0 ~ "VHT Capabilities" {
	vht=1
}

$0 ~ "HE Iftypes" {
	he=1
}

$1 == "*" && $3 == "MHz" && $0 !~ /disabled/ && band && !channel {
        channel = $4
}

END {
        print bands
}'
}

get_band_defaults() {
	local phy="$1"

	for c in $(__get_band_defaults "$phy"); do
		local band="${c%%:*}"
		c="${c#*:}"
		local chan="${c%%:*}"
		c="${c#*:}"
		local mode="${c%%:*}"

		case "$band" in
			1) band=2g;;
			2) band=5g;;
			3) band=60g;;
			4) band=6g;;
			*) band="";;
		esac

		[ -n "$band" ] || continue
		[ -n "$mode_band" -a "$band" = "6g" ] && return

		mode_band="$band"
		channel="$chan"
		htmode="$mode"
	done
}

check_devidx() {
	case "$1" in
	radio[0-9]*)
		local idx="${1#radio}"
		[ "$devidx" -ge "${1#radio}" ] && devidx=$((idx + 1))
		;;
	esac
}

check_board_phy() {
	local name="$2"

	json_select "$name"
	json_get_var phy_path path
	json_select ..

	if [ "$path" = "$phy_path" ]; then
		board_dev="$name"
	elif [ "${path%+*}" = "$phy_path" ]; then
		fallback_board_dev="$name.${path#*+}"
	fi
}

detect_mac80211() {
	devidx=0
	config_load wireless
	config_foreach check_devidx wifi-device

	json_load_file /etc/board.json

	for _dev in /sys/class/ieee80211/*; do
		[ -e "$_dev" ] || continue

		dev="${_dev##*/}"

		mode_band=""
		channel=""
		htmode=""
		ht_capab=""

		get_band_defaults "$dev"

		path="$(iwinfo nl80211 path "$dev")"
		macaddr="$(cat /sys/class/ieee80211/${dev}/macaddress)"

		# work around phy rename related race condition
		[ -n "$path" -o -n "$macaddr" ] || continue

		board_dev=
		fallback_board_dev=
		json_for_each_item check_board_phy wlan
		[ -n "$board_dev" ] || board_dev="$fallback_board_dev"
		[ -n "$board_dev" ] && dev="$board_dev"

		found=
		config_foreach check_mac80211_device wifi-device "$path" "$macaddr"
		[ -n "$found" ] && continue

		name="radio${devidx}"
		devidx=$(($devidx + 1))
		case "$dev" in
			phy*)
				if [ -n "$path" ]; then
					dev_id="set wireless.${name}.path='$path'"
				else
					dev_id="set wireless.${name}.macaddr='$macaddr'"
				fi
				;;
			*)
				dev_id="set wireless.${name}.phy='$dev'"
				;;
		esac

		uci -q batch <<-EOF
			set wireless.${name}=wifi-device
			set wireless.${name}.type=mac80211
			${dev_id}
			set wireless.${name}.channel=${channel}
			set wireless.${name}.band=${mode_band}
			set wireless.${name}.htmode=$htmode
			set wireless.${name}.disabled=1

			set wireless.default_${name}=wifi-iface
			set wireless.default_${name}.device=${name}
			set wireless.default_${name}.network=lan
			set wireless.default_${name}.mode=ap
			set wireless.default_${name}.ssid=OpenWrt
			set wireless.default_${name}.encryption=none
EOF
		uci -q commit wireless
	done
}
