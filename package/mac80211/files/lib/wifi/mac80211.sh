#!/bin/sh
append DRIVERS "mac80211"

find_mac80211_phy() {
	config_get device "$1"

	local macaddr="$(config_get "$device" macaddr | tr 'A-Z' 'a-z')"
	config_get phy "$device" phy
	[ -z "$phy" -a -n "$macaddr" ] && {
		for phy in $(ls /sys/class/ieee80211 2>/dev/null); do
			[ "$macaddr" = "$(cat /sys/class/ieee80211/${phy}/macaddress)" ] || continue
			config_set "$device" phy "$phy"
			break
		done
		config_get phy "$device" phy
	}
	[ -n "$phy" -a -d "/sys/class/ieee80211/$phy" ] || {
		echo "PHY for wifi device $1 not found"
		return 1
	}
	[ -z "$macaddr" ] && {
		config_set "$device" macaddr "$(cat /sys/class/ieee80211/${phy}/macaddress)"
	}
	return 0
}

scan_mac80211() {
	local device="$1"
	local adhoc sta ap monitor mesh

	config_get vifs "$device" vifs
	for vif in $vifs; do
		config_get mode "$vif" mode
		case "$mode" in
			adhoc|sta|ap|monitor|mesh)
				append $mode "$vif"
			;;
			*) echo "$device($vif): Invalid mode, ignored."; continue;;
		esac
	done

	config_set "$device" vifs "${ap:+$ap }${adhoc:+$adhoc }${sta:+$sta }${monitor:+$monitor }${mesh:+$mesh}"
}


disable_mac80211() (
	local device="$1"

	find_mac80211_phy "$device" || return 0
	config_get phy "$device" phy

	set_wifi_down "$device"
	# kill all running hostapd and wpa_supplicant processes that
	# are running on atheros/mac80211 vifs
	for pid in `pidof hostapd wpa_supplicant`; do
		grep wlan /proc/$pid/cmdline >/dev/null && \
			kill $pid
	done

	include /lib/network
	for wdev in $(ls /sys/class/ieee80211/${phy}/device/net 2>/dev/null); do
		ifconfig "$wdev" down 2>/dev/null
		unbridge "$dev"
		iw dev "$wdev" del
	done

	return 0
)
get_freq() {
	local phy="$1"
	local chan="$2"
	iw "$phy" info | grep -E -m1 "(\* ${chan:-....} MHz${chan:+|\\[$chan\\]})" | grep MHz | awk '{print $2}'
}
enable_mac80211() {
	local device="$1"
	config_get channel "$device" channel
	config_get vifs "$device" vifs
	config_get txpower "$device" txpower
	config_get country "$device" country
	config_get distance "$device" distance
	find_mac80211_phy "$device" || return 0
	config_get phy "$device" phy
	local i=0
	fixed=""

	[ -n "$country" ] && iw reg set "$country"
	[ "$channel" = "auto" -o "$channel" = "0" ] || {
		fixed=1
	}

	[ -n "$distance" ] && iw phy "$phy" set distance "$distance"

	export channel fixed
	# convert channel to frequency
	local freq="$(get_freq "$phy" "${fixed:+$channel}")"

	wifi_fixup_hwmode "$device" "g"
	for vif in $vifs; do
		while [ -d "/sys/class/net/wlan$i" ]; do
			i=$(($i + 1))
		done

		config_get ifname "$vif" ifname
		[ -n "$ifname" ] || {
			ifname="wlan$i"
		}
		config_set "$vif" ifname "$ifname"

		config_get enc "$vif" encryption
		config_get mode "$vif" mode
		config_get ssid "$vif" ssid
		config_get_bool wds "$vif" wds 0

		# It is far easier to delete and create the desired interface
		case "$mode" in
			adhoc)
				iw phy "$phy" interface add "$ifname" type adhoc
			;;
			ap)
				# Hostapd will handle recreating the interface and
				# it's accompanying monitor
				iw phy "$phy" interface add "$ifname" type managed
			;;
			mesh)
				config_get mesh_id "$vif" mesh_id
				iw phy "$phy" interface add "$ifname" type mp mesh_id "$mesh_id"
			;;
			monitor)
				iw phy "$phy" interface add "$ifname" type monitor
			;;
			sta)
				local wdsflag
				[ "$wds" -gt 0 ] && wdsflag="4addr on"
				iw phy "$phy" interface add "$ifname" type managed $wdsflag
				config_get_bool powersave "$vif" powersave 0
				[ "$powersave" -gt 0 ] && powersave="on" || powersave="off"
				iwconfig "$ifname" power "$powersave"
			;;
		esac

		# All interfaces must have unique mac addresses
		# which can either be explicitly set in the device
		# section, or automatically generated
		config_get macaddr "$device" macaddr
		local mac_1="${macaddr%%:*}"
		local mac_2="${macaddr#*:}"

		config_get vif_mac "$vif" macaddr
		[ -n "$vif_mac" ] || {
			if [ "$i" -gt 0 ]; then
				offset="$(( 2 + $i * 4 ))"
			else
				offset="0"
			fi
			vif_mac="$( printf %02x $((0x$mac_1 + $offset)) ):$mac_2"
		}
		ifconfig "$ifname" hw ether "$vif_mac"

		# We attempt to set teh channel for all interfaces, although
		# mac80211 may not support it or the driver might not yet
		[ -n "$fixed" -a -n "$channel" ] && iw dev "$ifname" set channel "$channel"

		local key keystring

		# Valid values are:
		# wpa / wep / none
		#
		# !! ap !!
		#
		# ALL ap functionality will be passed to hostapd
		#
		# !! mesh / adhoc / station !!
		# none -> NO encryption
		#
		# wep + keymgmt = '' -> we use iw to connect to the
		# network.
		#
		# wep + keymgmt = 'NONE' -> wpa_supplicant will be
		# configured to handle the wep connection
		if [ ! "$mode" = "ap" ]; then
			case "$enc" in
				*wep*)
					config_get keymgmt "$vif" keymgmt
					if [ -z "$keymgmt" ]; then
						config_get key "$vif" key
						key="${key:-1}"
						case "$key" in
							[1234])
								for idx in 1 2 3 4; do
									local zidx
									zidx=$(($idx - 1))
									config_get ckey "$vif" "key${idx}"
									if [ -n "$ckey" ]; then
										[ $idx -eq $key ] && zidx="d:${zidx}"
										append keystring "${zidx}:$(prepare_key_wep "$ckey")"
									fi
								done
								;;
							*)
								keystring="d:0:$(prepare_key_wep "$key")"
								;;
						esac
					fi
				;;
				*psk*|*wpa*)
					config_get key "$vif" key
				;;
			esac
		fi

		# txpower is not yet implemented in iw
		config_get vif_txpower "$vif" txpower
		# use vif_txpower (from wifi-iface) to override txpower (from
		# wifi-device) if the latter doesn't exist
		txpower="${txpower:-$vif_txpower}"
		[ -z "$txpower" ] || iwconfig "$ifname" txpower "${txpower%%.*}"

		config_get frag "$vif" frag
		if [ -n "$frag" ]; then
			iw phy "$phy" set frag "${frag%%.*}"
		fi

		config_get rts "$vif" rts
		if [ -n "$rts" ]; then
			iw phy "$phy" set rts "${rts%%.*}"
		fi

		ifconfig "$ifname" up

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
				if eval "type hostapd_setup_vif" 2>/dev/null >/dev/null; then
					hostapd_setup_vif "$vif" nl80211 || {
						echo "enable_mac80211($device): Failed to set up wpa for interface $ifname" >&2
						# make sure this wifi interface won't accidentally stay open without encryption
						ifconfig "$ifname" down
						continue
					}
				fi
			;;
			adhoc)
				config_get bssid "$vif" bssid
				iw dev "$ifname" ibss join "$ssid" $freq ${fixed:+fixed-freq} $bssid
			;;
			sta|mesh)
				config_get bssid "$vif" bssid
				case "$enc" in
					*wep*)
						if [ -z "$keymgmt" ]; then
							[ -n "$keystring" ] &&
								iw dev "$ifname" connect "$ssid" ${fixed:+$freq} $bssid key $keystring
						else
							if eval "type wpa_supplicant_setup_vif" 2>/dev/null >/dev/null; then
								wpa_supplicant_setup_vif "$vif" wext || {
									echo "enable_mac80211($device): Failed to set up wpa_supplicant for interface $ifname" >&2
									# make sure this wifi interface won't accidentally stay open without encryption
									ifconfig "$ifname" down
									continue
								}
							fi
						fi
					;;
					*wpa*|*psk*)
						config_get key "$vif" key
						if eval "type wpa_supplicant_setup_vif" 2>/dev/null >/dev/null; then
							wpa_supplicant_setup_vif "$vif" wext || {
								echo "enable_mac80211($device): Failed to set up wpa_supplicant for interface $ifname" >&2
								# make sure this wifi interface won't accidentally stay open without encryption
								ifconfig "$ifname" down
								continue
							}
						fi
					;;
					*)
						iw dev "$ifname" connect "$ssid" ${fixed:+$freq} $bssid
					;;
				esac

			;;
		esac
	done
}


check_device() {
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
		config_foreach check_device wifi-device
		[ "$found" -gt 0 ] && continue

		mode_11n=""
		mode_band="g"
		channel="5"
		ht_cap=0
		for cap in $(iw phy "$dev" info | grep 'HT capabilities' | cut -d: -f2); do
			ht_cap="$(($ht_cap | $cap))"
		done
		ht_capab="";
		[ "$ht_cap" -gt 0 ] && {
			mode_11n="n"
			list="	list ht_capab"
			[ "$(($ht_cap & 1))" -eq 1 ] && append ht_capab "$list	LDPC" "$N"
			[ "$(($ht_cap & 2))" -eq 2 ] && append ht_capab "$list	HT40-" "$N"
			[ "$(($ht_cap & 32))" -eq 32 ] && append ht_capab "$list	SHORT-GI-20" "$N"
			[ "$(($ht_cap & 64))" -eq 64 ] && append ht_capab "$list	SHORT-GI-40" "$N"
			[ "$(($ht_cap & 4096))" -eq 4096 ] && append ht_capab "$list	DSSS_CCK-40" "$N"
		}
		iw phy "$dev" info | grep -q '2412 MHz' || { mode_band="a"; channel="36"; }

		cat <<EOF
config wifi-device  radio$devidx
	option type     mac80211
	option channel  ${channel}
	option macaddr	$(cat /sys/class/ieee80211/${dev}/macaddress)
	option hwmode	11${mode_11n}${mode_band}
	# REMOVE THIS LINE TO ENABLE WIFI:
	option disabled 1
$ht_capab

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

