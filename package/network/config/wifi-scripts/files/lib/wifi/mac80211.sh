#!/bin/sh

append DRIVERS "mac80211"

mac80211_get_board_phy_name() (
	local path="$1"
	local fallback_phy=""

	__check_phy() {
		local val="$1"
		local key="$2"
		local ref_path="$3"

		json_select "$key"
		json_get_vars path
		json_select ..

		[ "${ref_path%+*}" = "$path" ] && fallback_phy=$key
		[ "$ref_path" = "$path" ] || return

		echo "$key"
		exit
	}

	json_load_file /etc/board.json
	json_for_each_item __check_phy wlan "$path"
	[ -n "$fallback_phy" ] && echo "${fallback_phy}.${path##*+}"
)

mac80211_rename_board_phy_by_path() {
	local path="$1"

	local new_phy="$(mac80211_get_board_phy_name "$path")"
	[ -z "$new_phy" -o "$new_phy" = "$phy" ] && return

	iw "$phy" set name "$new_phy"
}

mac80211_rename_board_phy_by_name() (
	local phy="$1"
	local suffix="${phy##*.}"
	[ "$suffix" = "$phy" ] && suffix=

	json_load_file /etc/board.json
	json_select wlan
	json_select "${phy%.*}" || return
	json_get_vars path

	prev_phy="$(iwinfo nl80211 phyname "path=$path${suffix:++$suffix}")"
	[ -n "$prev_phy" ] || return

	[ "$prev_phy" = "$phy" ] && return

	iw "$prev_phy" set name "$phy"
)

mac80211_rename_phy() {
	local phy path macaddr

	config_get phy "$1" phy
	[ -n "$phy" ] && {
		[ -d /sys/class/ieee80211/$phy ] && return
		mac80211_rename_board_phy_by_name "$phy"
		[ -d /sys/class/ieee80211/$phy ] && return
	}
	config_get path "$1" path
	[ -n "$path" ] && {
		phy="$(iwinfo nl80211 phyname "path=$path")"
		[ -n "$phy" ] && {
			mac80211_rename_board_phy_by_path "$path"
			return
		}
	}
	config_get macaddr "$1" macaddr
	[ -n "$macaddr" ] && {
		for phy in $(ls /sys/class/ieee80211 2>/dev/null); do
			grep -i -q "$macaddr" "/sys/class/ieee80211/${phy}/macaddress" && {
				path="$(iwinfo nl80211 path "$phy")"
				mac80211_rename_board_phy_by_path "$path"
				return
			}
		done
	}
}

detect_mac80211() {
	config_load wireless
	config_foreach mac80211_rename_phy wifi-device
}
