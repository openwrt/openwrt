#!/bin/sh
append DRIVERS "mac80211"

mac80211_hostapd_setup_base() {
	local phy="$1"
	local ifname="$2"

	cfgfile="/var/run/hostapd-$phy.conf"
	macfile="/var/run/hostapd-$phy.maclist"
	[ -e "$macfile" ] && rm -f "$macfile"

	config_get device "$vif" device
	config_get country "$device" country
	config_get hwmode "$device" hwmode
	config_get channel "$device" channel
	config_get_bool noscan "$device" noscan
	[ -n "$channel" -a -z "$hwmode" ] && wifi_fixup_hwmode "$device"
	[ "$channel" = auto ] && channel=
	[ -n "$hwmode" ] && {
		config_get hwmode_11n "$device" hwmode_11n
		[ -n "$hwmode_11n" ] && {
			hwmode="$hwmode_11n"
			append base_cfg "ieee80211n=1" "$N"
			config_get htmode "$device" htmode
			config_get ht_capab_list "$device" ht_capab
			case "$htmode" in
				HT20|HT40+|HT40-) ht_capab="[$htmode]";;
				*)ht_capab=;;
			esac
			for cap in $ht_capab_list; do
				ht_capab="$ht_capab[$cap]"
			done
			[ -n "$ht_capab" ] && append base_cfg "ht_capab=$ht_capab" "$N"
		}
	}

	config_get macfilter "$vif" macfilter
	case "$macfilter" in
		allow)
			append base_cfg "macaddr_acl=1" "$N"
			append base_cfg "accept_mac_file=$macfile" "$N"
			;;
		deny)
			append base_cfg "macaddr_acl=0" "$N"
			append base_cfg "deny_mac_file=$macfile" "$N"
			;;
	esac
	config_get maclist "$vif" maclist
	[ -n "$maclist" ] && {
		for mac in $maclist; do
			echo "$mac" >> $macfile
		done
	}
	cat >> "$cfgfile" <<EOF
ctrl_interface=/var/run/hostapd-$phy
driver=nl80211
wmm_ac_bk_cwmin=4
wmm_ac_bk_cwmax=10
wmm_ac_bk_aifs=7
wmm_ac_bk_txop_limit=0
wmm_ac_bk_acm=0
wmm_ac_be_aifs=3
wmm_ac_be_cwmin=4
wmm_ac_be_cwmax=10
wmm_ac_be_txop_limit=0
wmm_ac_be_acm=0
wmm_ac_vi_aifs=2
wmm_ac_vi_cwmin=3
wmm_ac_vi_cwmax=4
wmm_ac_vi_txop_limit=94
wmm_ac_vi_acm=0
wmm_ac_vo_aifs=2
wmm_ac_vo_cwmin=2
wmm_ac_vo_cwmax=3
wmm_ac_vo_txop_limit=47
wmm_ac_vo_acm=0
tx_queue_data3_aifs=7
tx_queue_data3_cwmin=15
tx_queue_data3_cwmax=1023
tx_queue_data3_burst=0
tx_queue_data2_aifs=3
tx_queue_data2_cwmin=15
tx_queue_data2_cwmax=63
tx_queue_data2_burst=0
tx_queue_data1_aifs=1
tx_queue_data1_cwmin=7
tx_queue_data1_cwmax=15
tx_queue_data1_burst=3.0
tx_queue_data0_aifs=1
tx_queue_data0_cwmin=3
tx_queue_data0_cwmax=7
tx_queue_data0_burst=1.5
${hwmode:+hw_mode=$hwmode}
${channel:+channel=$channel}
${country:+country_code=$country}
${noscan:+noscan=$noscan}
$base_cfg

EOF
}

mac80211_hostapd_setup_bss() {
	local phy="$1"
	local vif="$2"

	hostapd_cfg=
	cfgfile="/var/run/hostapd-$phy.conf"
	config_get ifname "$vif" ifname

	if [ -f "$cfgfile" ]; then
		append hostapd_cfg "bss=$ifname" "$N"
	else
		mac80211_hostapd_setup_base "$phy" "$ifname"
		append hostapd_cfg "interface=$ifname" "$N"
	fi

	local net_cfg bridge
	net_cfg="$(find_net_config "$vif")"
	[ -z "$net_cfg" ] || bridge="$(bridge_interface "$net_cfg")"
	config_set "$vif" bridge "$bridge"

	hostapd_set_bss_options hostapd_cfg "$vif"

	config_get_bool wds "$vif" wds 0
	[ "$wds" -gt 0 ] && append hostapd_cfg "wds_sta=1" "$N"

	local macaddr hidden maxassoc wmm
	config_get macaddr "$vif" macaddr
	config_get maxassoc "$vif" maxassoc
	config_get_bool hidden "$vif" hidden 0
	config_get_bool wmm "$vif" wmm 1
	cat >> /var/run/hostapd-$phy.conf <<EOF
$hostapd_cfg
wmm_enabled=$wmm
bssid=$macaddr
ignore_broadcast_ssid=$hidden
${maxassoc:+max_num_sta=$maxassoc}
EOF
}

mac80211_start_vif() {
	local vif="$1"
	local ifname="$2"

	local net_cfg
	net_cfg="$(find_net_config "$vif")"
	[ -z "$net_cfg" ] || start_net "$ifname" "$net_cfg"

	set_wifi_up "$vif" "$ifname"
}

find_mac80211_phy() {
	local device="$1"

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

list_phy_interfaces() {
	local phy="$1"
	if [ -d "/sys/class/ieee80211/${phy}/device/net" ]; then
		ls "/sys/class/ieee80211/${phy}/device/net" 2>/dev/null;
	else
		ls "/sys/class/ieee80211/${phy}/device" 2>/dev/null | grep net: | sed -e 's,net:,,g'
	fi
}

disable_mac80211() (
	local device="$1"

	find_mac80211_phy "$device" || return 0
	config_get phy "$device" phy

	set_wifi_down "$device"
	# kill all running hostapd and wpa_supplicant processes that
	# are running on atheros/mac80211 vifs
	for pid in `pidof hostapd`; do
		grep -E "$phy" /proc/$pid/cmdline >/dev/null && \
			kill $pid
	done

	include /lib/network
	for wdev in $(list_phy_interfaces "$phy"); do
		[ -f "/var/run/$wdev.pid" ] && kill $(cat /var/run/$wdev.pid) >&/dev/null 2>&1
		for pid in `pidof wpa_supplicant`; do
			grep "$wdev" /proc/$pid/cmdline >/dev/null && \
				kill $pid
		done
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
	config_get txantenna "$device" txantenna all
	config_get rxantenna "$device" rxantenna all
	config_get frag "$device" frag
	config_get rts "$device" rts
	find_mac80211_phy "$device" || return 0
	config_get phy "$device" phy
	local i=0
	local macidx=0
	local apidx=0
	fixed=""
	local hostapd_ctrl=""

	[ -n "$country" ] && iw reg set "$country"
	[ "$channel" = "auto" -o "$channel" = "0" ] || {
		fixed=1
	}

	iw phy "$phy" set antenna "$txantenna $rxantenna"

	[ -n "$distance" ] && iw phy "$phy" set distance "$distance"
	[ -n "$frag" ] && iw phy "$phy" set frag "${frag%%.*}"
	[ -n "$rts" ] && iw phy "$phy" set rts "${rts%%.*}"

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

		config_get mode "$vif" mode
		config_get ssid "$vif" ssid

		# It is far easier to delete and create the desired interface
		case "$mode" in
			adhoc)
				iw phy "$phy" interface add "$ifname" type adhoc
			;;
			ap)
				# Hostapd will handle recreating the interface and
				# it's accompanying monitor
				apidx="$(($apidx + 1))"
				i=$(($i + 1))
				[ "$apidx" -gt 1 ] || iw phy "$phy" interface add "$ifname" type managed
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
				config_get_bool wds "$vif" wds 0
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
			if [ "$macidx" -gt 0 ]; then
				offset="$(( 2 + $macidx * 4 ))"
			else
				offset="0"
			fi
			vif_mac="$( printf %02x $((0x$mac_1 + $offset)) ):$mac_2"
			macidx="$(($macidx + 1))"
		}
		[ "$mode" = "ap" ] || ifconfig "$ifname" hw ether "$vif_mac"
		config_set "$vif" macaddr "$vif_mac"

		# !! ap !!
		#
		# ALL ap functionality will be passed to hostapd
		#
		# !! station !!
		#
		# ALL station functionality will be passed to wpa_supplicant
		#
		if [ ! "$mode" = "ap" ]; then
			# We attempt to set the channel for all interfaces, although
			# mac80211 may not support it or the driver might not yet
			# for ap mode this is handled by hostapd
			[ -n "$fixed" -a -n "$channel" ] && iw dev "$ifname" set channel "$channel"
		fi

		config_get vif_txpower "$vif" txpower
		# use vif_txpower (from wifi-iface) to override txpower (from
		# wifi-device) if the latter doesn't exist
		txpower="${txpower:-$vif_txpower}"
		[ -z "$txpower" ] || iw dev "$ifname" set txpower fixed "${txpower%%.*}00"
	done

	local start_hostapd=
	rm -f /var/run/hostapd-$phy.conf
	for vif in $vifs; do
		config_get mode "$vif" mode
		[ "$mode" = "ap" ] || continue
		mac80211_hostapd_setup_bss "$phy" "$vif"
		start_hostapd=1
	done

	[ -n "$start_hostapd" ] && {
		hostapd -P /var/run/wifi-$phy.pid -B /var/run/hostapd-$phy.conf || {
			echo "Failed to start hostapd for $phy"
			return
		}
		sleep 2

		for vif in $vifs; do
			config_get mode "$vif" mode
			config_get ifname "$vif" ifname
			[ "$mode" = "ap" ] || continue
			hostapd_ctrl="${hostapd_ctrl:-/var/run/hostapd-$phy/$ifname}"
			mac80211_start_vif "$vif" "$ifname"
		done
	}

	for vif in $vifs; do
		config_get mode "$vif" mode
		config_get ifname "$vif" ifname
		[ ! "$mode" = "ap" ] || continue
		ifconfig "$ifname" up

		if [ ! "$mode" = "ap" ]; then
			ifconfig "$ifname" up
			case "$mode" in
				adhoc)
					config_get bssid "$vif" bssid
					config_get ssid "$vif" ssid
					config_get bintval "$vif" bintval
					config_get basicrates "$vif" basicrates
					config_get encryption "$vif" encryption
					config_get key "$vif" key 1
					config_get mcast_rate "$vif" mcast_rate

					local keyspec=""
					[ "$encryption" == "wep" ] && {
						case "$key" in
							[1234])
								local idx
								for idx in 1 2 3 4; do
									local ikey
									config_get ikey "$vif" "key$idx"

									[ -n "$ikey" ] && {
										ikey="$(($idx - 1)):$(prepare_key_wep "$ikey")"
										[ $idx -eq $key ] && ikey="d:$ikey"
										append keyspec "$ikey"
									}
								done
							;;
							*) append keyspec "d:0:$(prepare_key_wep "$key")" ;;
						esac
					}

					local mcval=""
					[ -n "$mcast_rate" ] && {
						mcval="$(($mcast_rate / 1000))"
						mcsub="$(( ($mcast_rate / 100) % 10 ))"
						[ "$mcsub" -gt 0 ] && mcval="$mcval.$mcsub"
					}

					iw dev "$ifname" ibss join "$ssid" $freq \
						${fixed:+fixed-freq} $bssid \
						${mcval:+mcast-rate $mcval} \
						${bintval:+beacon-interval $bintval} \
						${basicrates:+basic-rates $basicrates} \
						${keyspec:+keys $keyspec}
				;;
				sta)
					if eval "type wpa_supplicant_setup_vif" 2>/dev/null >/dev/null; then
						wpa_supplicant_setup_vif "$vif" nl80211 "${hostapd_ctrl:+-H $hostapd_ctrl}" || {
							echo "enable_mac80211($device): Failed to set up wpa_supplicant for interface $ifname" >&2
							# make sure this wifi interface won't accidentally stay open without encryption
							ifconfig "$ifname" down
							continue
						}
					fi
				;;
			esac
			mac80211_start_vif "$vif" "$ifname"
		fi
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
		channel="11"
		ht_cap=0
		for cap in $(iw phy "$dev" info | grep 'Capabilities:' | cut -d: -f2); do
			ht_cap="$(($ht_cap | $cap))"
		done
		ht_capab="";
		[ "$ht_cap" -gt 0 ] && {
			mode_11n="n"
			append ht_capab "	option htmode	HT20" "$N"

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

		cat <<EOF
config wifi-device  radio$devidx
	option type     mac80211
	option channel  ${channel}
	option macaddr	$(cat /sys/class/ieee80211/${dev}/macaddress)
	option hwmode	11${mode_11n}${mode_band}
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

