#!/bin/sh
. /lib/netifd/netifd-wireless.sh
. /lib/netifd/hostapd.sh

init_wireless_driver "$@"

MP_CONFIG_INT="mesh_retry_timeout mesh_confirm_timeout mesh_holding_timeout mesh_max_peer_links
	       mesh_max_retries mesh_ttl mesh_element_ttl mesh_hwmp_max_preq_retries
	       mesh_path_refresh_time mesh_min_discovery_timeout mesh_hwmp_active_path_timeout
	       mesh_hwmp_preq_min_interval mesh_hwmp_net_diameter_traversal_time mesh_hwmp_rootmode
	       mesh_hwmp_rann_interval mesh_gate_announcements mesh_sync_offset_max_neighor
	       mesh_rssi_threshold mesh_hwmp_active_path_to_root_timeout mesh_hwmp_root_interval
	       mesh_hwmp_confirmation_interval mesh_awake_window mesh_plink_timeout"
MP_CONFIG_BOOL="mesh_auto_open_plinks mesh_fwding"
MP_CONFIG_STRING="mesh_power_mode"

drv_mac80211_init_device_config() {
	hostapd_common_add_device_config

	config_add_string path phy macaddr
	config_add_string hwmode
	config_add_int beacon_int chanbw frag rts
	config_add_int rxantenna txantenna antenna_gain txpower distance
	config_add_boolean noscan
	config_add_array ht_capab
	config_add_boolean \
		rxldpc \
		short_gi_80 \
		short_gi_160 \
		tx_stbc_2by1 \
		su_beamformer \
		su_beamformee \
		mu_beamformer \
		mu_beamformee \
		vht_txop_ps \
		htc_vht \
		rx_antenna_pattern \
		tx_antenna_pattern
	config_add_int vht_max_mpdu vht_max_rx_stbc vht_link_adapt vht160
}

drv_mac80211_init_iface_config() {
	hostapd_common_add_bss_config

	config_add_string macaddr ifname

	config_add_boolean wds powersave
	config_add_int maxassoc
	config_add_int max_listen_int
	config_add_int dtim_interval

	# mesh
	config_add_int $MP_CONFIG_INT
	config_add_boolean $MP_CONFIG_BOOL
	config_add_string $MP_CONFIG_STRING
}

mac80211_hostapd_setup_base() {
	local phy="$1"

	json_select config

	[ "$auto_channel" -gt 0 ] && channel=acs_survey

	[ "$enable_ht" -gt 0 ] && {
		json_get_vars noscan htmode
		json_get_values ht_capab_list ht_capab

		append base_cfg "ieee80211n=1" "$N"

		ht_capab=
		case "$htmode" in
			HT20|HT40-|HT40+) ht_capab="[$htmode]";;
			VHT40|VHT80|VHT160)
				case "$channel" in
					36|44|52|60|100|108|116|124|132|140|149|157) ht_capab="[HT40+]";;
					40|48|56|64|104|112|120|128|136|144|153|161) ht_capab="[HT40-]";;
				esac
				;;
		esac
		for cap in $ht_capab_list; do
			ht_capab="$ht_capab[$cap]"
		done

		[ -n "$ht_capab" ] && append base_cfg "ht_capab=$ht_capab" "$N"

		# 802.11ac
		enable_ac=0
		idx="$channel"
		case "$htmode" in
			VHT40)
				case "$channel" in
					36|40) idx=38;;
					44|48) idx=42;;
					52|56) idx=54;;
					60|64) idx=58;;
					100|104) idx=102;;
					108|112) idx=110;;
					116|120) idx=118;;
					124|128) idx=126;;
					132|136) idx=134;;
					140|144) idx=142;;
					149|153) idx=151;;
					157|161) idx=159;;
				esac
				enable_ac=1
				append base_cfg "vht_oper_chwidth=0" "$N"
				append base_cfg "vht_oper_centr_freq_seg0_idx=$idx" "$N"
			;;
			VHT80)
				case "$channel" in
					36|40|44|48) idx=42;;
					52|56|60|64) idx=58;;
					100|104|108|112) idx=106;;
					116|120|124|128) idx=122;;
					132|136|140|144) idx=138;;
					149|153|157|161) idx=155;;
				esac
				enable_ac=1
				append base_cfg "vht_oper_chwidth=1" "$N"
				append base_cfg "vht_oper_centr_freq_seg0_idx=$idx" "$N"
			;;
			VHT160)
				case "$channel" in
					36|40|44|48|52|56|60|64) idx=50;;
					100|104|108|112|116|120|124|128) idx=114;;
				esac
				enable_ac=1
				append base_cfg "vht_oper_chwidth=2" "$N"
				append base_cfg "vht_oper_centr_freq_seg0_idx=$idx" "$N"
			;;
		esac

		if [ "$enable_ac" != "0" ]; then
			json_get_vars \
				rxldpc:1 \
				short_gi_80:1 \
				short_gi_160:1 \
				tx_stbc_2by1:1 \
				su_beamformer:1 \
				su_beamformee:1 \
				mu_beamformer:1 \
				mu_beamformee:1 \
				vht_txop_ps:1 \
				htc_vht:1 \
				rx_antenna_pattern:1 \
				tx_antenna_pattern:1 \
				vht_max_mpdu:11454 \
				vht_max_rx_stbc:4 \
				vht_link_adapt:3 \
				vht160:2

			append base_cfg "ieee80211ac=1" "$N"
			vht_capab=""
			vht_cap=0
			for cap in $(iw phy "$phy" info | awk -F "[()]" '/VHT Capabilities/ { print $2 }'); do
				vht_cap="$(($vht_cap | $cap))"
			done

			# boolean
			[ "$((($vht_cap & 16) * $rxldpc))" -eq 16 ] && \
				vht_capab="$vht_capab[RXLDPC]"
			[ "$((($vht_cap & 32) * $short_gi_80))" -eq 32 ] && \
				vht_capab="$vht_capab[SHORT-GI-80]"
			[ "$((($vht_cap & 64) * $short_gi_160))" -eq 64 ] && \
				vht_capab="$vht_capab[SHORT-GI-160]"
			[ "$((($vht_cap & 128) * $tx_stbc_2by1))" -eq 128 ] && \
				vht_capab="$vht_capab[TX-STBC-2BY1]"
			[ "$((($vht_cap & 2048) * $su_beamformer))" -eq 2048 ] && \
				vht_capab="$vht_capab[SU-BEAMFORMER]"
			[ "$((($vht_cap & 4096) * $su_beamformee))" -eq 4096 ] && \
				vht_capab="$vht_capab[SU-BEAMFORMEE]"
			[ "$((($vht_cap & 524288) * $mu_beamformer))" -eq 524288 ] && \
				vht_capab="$vht_capab[MU-BEAMFORMER]"
			[ "$((($vht_cap & 1048576) * $mu_beamformee))" -eq 1048576 ] && \
				vht_capab="$vht_capab[MU-BEAMFORMEE]"
			[ "$((($vht_cap & 2097152) * $vht_txop_ps))" -eq 2097152 ] && \
				vht_capab="$vht_capab[VHT-TXOP-PS]"
			[ "$((($vht_cap & 4194304) * $htc_vht))" -eq 4194304 ] && \
				vht_capab="$vht_capab[HTC-VHT]"
			[ "$((($vht_cap & 268435456) * $rx_antenna_pattern))" -eq 268435456 ] && \
				vht_capab="$vht_capab[RX-ANTENNA-PATTERN]"
			[ "$((($vht_cap & 536870912) * $tx_antenna_pattern))" -eq 536870912 ] && \
				vht_capab="$vht_capab[TX-ANTENNA-PATTERN]"

			# supported Channel widths
			vht160_hw=0
			[ "$(($vht_cap & 12))" -eq 4 -a 1 -le "$vht160" ] && \
				vht160_hw=1
			[ "$(($vht_cap & 12))" -eq 8 -a 2 -le "$vht160" ] && \
				vht160_hw=2
			[ "$vht160_hw" = 1 ] && vht_capab="$vht_capab[VHT160]"
			[ "$vht160_hw" = 2 ] && vht_capab="$vht_capab[VHT160-80PLUS80]"

			# maximum MPDU length
			vht_max_mpdu_hw=3895
			[ "$(($vht_cap & 3))" -ge 1 -a 7991 -le "$vht_max_mpdu" ] && \
				vht_max_mpdu_hw=7991
			[ "$(($vht_cap & 3))" -ge 2 -a 11454 -le "$vht_max_mpdu" ] && \
				vht_max_mpdu_hw=11454
			[ "$vht_max_mpdu_hw" != 3895 ] && \
				vht_capab="$vht_capab[MAX-MPDU-$vht_max_mpdu_hw]"

			# support for the reception of PPDUs using STBC
			vht_max_rx_stbc_hw=0
			[ "$(($vht_cap & 1792))" -ge 256 -a 1 -le "$vht_max_rx_stbc" ] && \
				vht_max_rx_stbc_hw=1
			[ "$(($vht_cap & 1792))" -ge 512 -a 2 -le "$vht_max_rx_stbc" ] && \
				vht_max_rx_stbc_hw=2
			[ "$(($vht_cap & 1792))" -ge 768 -a 3 -le "$vht_max_rx_stbc" ] && \
				vht_max_rx_stbc_hw=3
			[ "$(($vht_cap & 1792))" -ge 1024 -a 4 -le "$vht_max_rx_stbc" ] && \
				vht_max_rx_stbc_hw=4
			[ "$vht_max_rx_stbc_hw" = 1 ] && vht_capab="$vht_capab[RX-STBC-1]"
			[ "$vht_max_rx_stbc_hw" = 2 ] && vht_capab="$vht_capab[RX-STBC-12]"
			[ "$vht_max_rx_stbc_hw" = 3 ] && vht_capab="$vht_capab[RX-STBC-123]"
			[ "$vht_max_rx_stbc_hw" = 4 ] && vht_capab="$vht_capab[RX-STBC-1234]"

			# whether or not the STA supports link adaptation using VHT variant
			vht_link_adapt_hw=0
			[ "$(($vht_cap & 201326592))" -ge 134217728 -a 2 -le "$vht_link_adapt" ] && \
				vht_link_adapt_hw=2
			[ "$(($vht_cap & 201326592))" -ge 201326592 -a 3 -le "$vht_link_adapt" ] && \
				vht_link_adapt_hw=3
			[ "$vht_link_adapt_hw" != 0 ] && \
				vht_capab="$vht_capab[VHT-LINK-ADAPT-$vht_link_adapt_hw]"

			[ -n "$vht_capab" ] && append base_cfg "vht_capab=$vht_capab" "$N"
		fi
	}

	hostapd_prepare_device_config "$hostapd_conf_file" nl80211
	cat >> "$hostapd_conf_file" <<EOF
${channel:+channel=$channel}
${noscan:+noscan=$noscan}
$base_cfg

EOF
	json_select ..
}

mac80211_hostapd_setup_bss() {
	local phy="$1"
	local ifname="$2"
	local macaddr="$3"
	local type="$4"

	hostapd_cfg=
	append hostapd_cfg "$type=$ifname" "$N"

	hostapd_set_bss_options hostapd_cfg "$vif" || return 1
	json_get_vars wds dtim_period max_listen_int

	set_default wds 0

	[ "$wds" -gt 0 ] && append hostapd_cfg "wds_sta=1" "$N"
	[ "$staidx" -gt 0 ] && append hostapd_cfg "start_disabled=1" "$N"

	cat >> /var/run/hostapd-$phy.conf <<EOF
$hostapd_cfg
bssid=$macaddr
${dtim_period:+dtim_period=$dtim_period}
${max_listen_int:+max_listen_interval=$max_listen_int}
EOF
}

mac80211_generate_mac() {
	local phy="$1"
	local id="${macidx:-0}"

	local ref="$(cat /sys/class/ieee80211/${phy}/macaddress)"
	local mask="$(cat /sys/class/ieee80211/${phy}/address_mask)"

	[ "$mask" = "00:00:00:00:00:00" ] && mask="ff:ff:ff:ff:ff:ff";
	local oIFS="$IFS"; IFS=":"; set -- $mask; IFS="$oIFS"

	local mask1=$1
	local mask6=$6

	local oIFS="$IFS"; IFS=":"; set -- $ref; IFS="$oIFS"

	macidx=$(($id + 1))
	[ "$((0x$mask1))" -gt 0 ] && {
		b1="0x$1"
		[ "$id" -gt 0 ] && \
			b1=$(($b1 ^ ((($id - 1) << 2) | 0x2)))
		printf "%02x:%s:%s:%s:%s:%s" $b1 $2 $3 $4 $5 $6
		return
	}

	[ "$((0x$mask6))" -lt 255 ] && {
		printf "%s:%s:%s:%s:%s:%02x" $1 $2 $3 $4 $5 $(( 0x$6 ^ $id ))
		return
	}

	off2=$(( (0x$6 + $id) / 0x100 ))
	printf "%s:%s:%s:%s:%02x:%02x" \
		$1 $2 $3 $4 \
		$(( (0x$5 + $off2) % 0x100 )) \
		$(( (0x$6 + $id) % 0x100 ))
}

find_phy() {
	[ -n "$phy" -a -d /sys/class/ieee80211/$phy ] && return 0
	[ -n "$path" -a -d "/sys/devices/$path/ieee80211" ] && {
		phy="$(ls /sys/devices/$path/ieee80211 | grep -m 1 phy)"
		[ -n "$phy" ] && return 0
	}
	[ -n "$macaddr" ] && {
		for phy in $(ls /sys/class/ieee80211 2>/dev/null); do
			grep -i -q "$macaddr" "/sys/class/ieee80211/${phy}/macaddress" && return 0
		done
	}
	return 1
}

mac80211_check_ap() {
	has_ap=1
}

mac80211_prepare_vif() {
	json_select config

	json_get_vars ifname mode ssid wds powersave macaddr

	[ -n "$ifname" ] || ifname="wlan${phy#phy}${if_idx:+-$if_idx}"
	if_idx=$((${if_idx:-0} + 1))

	set_default wds 0
	set_default powersave 0

	json_select ..

	[ -n "$macaddr" ] || {
		macaddr="$(mac80211_generate_mac $phy)"
		macidx="$(($macidx + 1))"
	}

	json_add_object data
	json_add_string ifname "$ifname"
	json_close_object
	json_select config

	# It is far easier to delete and create the desired interface
	case "$mode" in
		adhoc)
			iw phy "$phy" interface add "$ifname" type adhoc
		;;
		ap)
			# Hostapd will handle recreating the interface and
			# subsequent virtual APs belonging to the same PHY
			if [ -n "$hostapd_ctrl" ]; then
				type=bss
			else
				type=interface
			fi

			mac80211_hostapd_setup_bss "$phy" "$ifname" "$macaddr" "$type" || return

			[ -n "$hostapd_ctrl" ] || {
				iw phy "$phy" interface add "$ifname" type managed
				hostapd_ctrl="${hostapd_ctrl:-/var/run/hostapd/$ifname}"
			}
		;;
		mesh)
			json_get_vars key mesh_id
			if [ -n "$key" ]; then
				iw phy "$phy" interface add "$ifname" type mp
			else
				iw phy "$phy" interface add "$ifname" type mp mesh_id "$mesh_id"
			fi
		;;
		monitor)
			iw phy "$phy" interface add "$ifname" type monitor
		;;
		sta)
			local wdsflag=
			staidx="$(($staidx + 1))"
			[ "$wds" -gt 0 ] && wdsflag="4addr on"
			iw phy "$phy" interface add "$ifname" type managed $wdsflag
			[ "$powersave" -gt 0 ] && powersave="on" || powersave="off"
			iw "$ifname" set power_save "$powersave"
		;;
	esac

	case "$mode" in
		monitor|mesh)
			[ "$auto_channel" -gt 0 ] || iw dev "$ifname" set channel "$channel" $htmode
		;;
	esac

	if [ "$mode" != "ap" ]; then
		# ALL ap functionality will be passed to hostapd
		# All interfaces must have unique mac addresses
		# which can either be explicitly set in the device
		# section, or automatically generated
		ifconfig "$ifname" hw ether "$macaddr"
	fi

	json_select ..
}

mac80211_setup_supplicant() {
	wpa_supplicant_prepare_interface "$ifname" nl80211 || return 1
	wpa_supplicant_add_network "$ifname"
	wpa_supplicant_run "$ifname" ${hostapd_ctrl:+-H $hostapd_ctrl}
}

mac80211_setup_adhoc() {
	json_get_vars bssid ssid key mcast_rate

	keyspec=
	[ "$auth_type" == "wep" ] && {
		set_default key 1
		case "$key" in
			[1234])
				local idx
				for idx in 1 2 3 4; do
					json_get_var ikey "key$idx"

					[ -n "$ikey" ] && {
						ikey="$(($idx - 1)):$(prepare_key_wep "$ikey")"
						[ $idx -eq $key ] && ikey="d:$ikey"
						append keyspec "$ikey"
					}
				done
			;;
			*)
				append keyspec "d:0:$(prepare_key_wep "$key")"
			;;
		esac
	}

	brstr=
	for br in $basic_rate_list; do
		hostapd_add_rate brstr "$br"
	done

	mcval=
	[ -n "$mcast_rate" ] && hostapd_add_rate mcval "$mcast_rate"

	iw dev "$ifname" ibss join "$ssid" $freq $htmode fixed-freq $bssid \
		${beacon_int:+beacon-interval $beacon_int} \
		${brstr:+basic-rates $brstr} \
		${mcval:+mcast-rate $mcval} \
		${keyspec:+keys $keyspec}
}

mac80211_setup_vif() {
	local name="$1"

	json_select data
	json_get_vars ifname
	json_select ..

	json_select config
	json_get_vars mode
	json_get_var vif_txpower txpower

	ifconfig "$ifname" up || {
		wireless_setup_vif_failed IFUP_ERROR
		json_select ..
		return
	}

	set_default vif_txpower "$txpower"
	[ -z "$vif_txpower" ] || iw dev "$ifname" set txpower fixed "${vif_txpower%%.*}00"

	case "$mode" in
		mesh)
			for var in $MP_CONFIG_INT $MP_CONFIG_BOOL $MP_CONFIG_STRING; do
				json_get_var mp_val "$var"
				[ -n "$mp_val" ] && iw dev "$ifname" set mesh_param "$var" "$mp_val"
			done
			# todo: authsae
		;;
		adhoc)
			wireless_vif_parse_encryption
			if [ "$wpa" -gt 0 -o "$auto_channel" -gt 0 ]; then
				mac80211_setup_supplicant || failed=1
			else
				mac80211_setup_adhoc
			fi
		;;
		sta)
			mac80211_setup_supplicant || failed=1
		;;
	esac

	json_select ..
	[ -n "$failed" ] || wireless_add_vif "$name" "$ifname"
}

get_freq() {
	local phy="$1"
	local chan="$2"
	iw "$phy" info | grep -E -m1 "(\* ${chan:-....} MHz${chan:+|\\[$chan\\]})" | grep MHz | awk '{print $2}'
}

mac80211_interface_cleanup() {
	local phy="$1"

	for wdev in $(list_phy_interfaces "$phy"); do
		ifconfig "$wdev" down 2>/dev/null
		iw dev "$wdev" del
	done
}

drv_mac80211_cleanup() {
	hostapd_common_cleanup
}

drv_mac80211_setup() {
	json_select config
	json_get_vars \
		phy macaddr path \
		country chanbw distance \
		txpower antenna_gain \
		rxantenna txantenna \
		frag rts beacon_int
	json_get_values basic_rate_list basic_rate
	json_select ..

	find_phy || {
		echo "Could not find PHY for device '$1'"
		wireless_set_retry 0
		return 1
	}

	wireless_set_data phy="$phy"
	mac80211_interface_cleanup "$phy"

	# convert channel to frequency
	[ "$auto_channel" -gt 0 ] || freq="$(get_freq "$phy" "$channel")"

	[ -n "$country" ] && {
		iw reg get | grep -q "^country $country:" || {
			iw reg set "$country"
			sleep 1
		}
	}

	hostapd_conf_file="/var/run/hostapd-$phy.conf"

	no_ap=1
	macidx=0
	staidx=0

	[ -n "$chanbw" ] && {
		for file in /sys/kernel/debug/ieee80211/$phy/ath9k/chanbw /sys/kernel/debug/ieee80211/$phy/ath5k/bwmode; do
			[ -f "$file" ] && echo "$chanbw" > "$file"
		done
	}

	set_default rxantenna all
	set_default txantenna all
	set_default distance 0
	set_default antenna_gain 0

	iw phy "$phy" set antenna $txantenna $rxantenna >/dev/null 2>&1
	iw phy "$phy" set antenna_gain $antenna_gain
	iw phy "$phy" set distance "$distance"

	[ -n "$frag" ] && iw phy "$phy" set frag "${frag%%.*}"
	[ -n "$rts" ] && iw phy "$phy" set rts "${rts%%.*}"

	has_ap=
	hostapd_ctrl=
	for_each_interface "ap" mac80211_check_ap

	rm -f "$hostapd_conf_file"
	[ -n "$has_ap" ] && mac80211_hostapd_setup_base "$phy"

	for_each_interface "sta adhoc mesh monitor" mac80211_prepare_vif
	for_each_interface "ap" mac80211_prepare_vif

	[ -n "$hostapd_ctrl" ] && {
		/usr/sbin/hostapd -P /var/run/wifi-$phy.pid -B "$hostapd_conf_file"
		ret="$?"
		wireless_add_process "$(cat /var/run/wifi-$phy.pid)" "/usr/sbin/hostapd" 1
		[ "$ret" != 0 ] && {
			wireless_setup_failed HOSTAPD_START_FAILED
			return
		}
	}

	for_each_interface "ap sta adhoc mesh monitor" mac80211_setup_vif

	wireless_set_up
}

list_phy_interfaces() {
	local phy="$1"
	if [ -d "/sys/class/ieee80211/${phy}/device/net" ]; then
		ls "/sys/class/ieee80211/${phy}/device/net" 2>/dev/null;
	else
		ls "/sys/class/ieee80211/${phy}/device" 2>/dev/null | grep net: | sed -e 's,net:,,g'
	fi
}

drv_mac80211_teardown() {
	wireless_process_kill_all

	json_select data
	json_get_vars phy
	json_select ..

	mac80211_interface_cleanup "$phy"
}

add_driver mac80211
