#!/bin/sh
append DRIVERS "mac80211"

scan_mac80211() {
	local device="$1"
	local adhoc sta ap monitor mesh

	config_get vifs "$device" vifs
	for vif in $vifs; do

		config_get ifname "$vif" ifname
		config_set "$vif" ifname "${ifname:-$device}"

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

	set_wifi_down "$device"
	# kill all running hostapd and wpa_supplicant processes that
	# are running on atheros/mac80211 vifs
	for pid in `pidof hostapd wpa_supplicant`; do
		grep wlan /proc/$pid/cmdline >/dev/null && \
			kill $pid
	done

	include /lib/network
	cd /proc/sys/net
	for dev in *; do
		grep "$device" "$dev/%parent" >/dev/null 2>/dev/null && {
			ifconfig "$dev" down
			unbridge "$dev"
		}
	done
	return 0
)

enable_mac80211() {
	local device="$1"
	config_get channel "$device" channel
	config_get vifs "$device" vifs
	config_get txpower "$device" txpower

	local first=1
	local mesh_idx=0
	wifi_fixup_hwmode "$device" "g"
	for vif in $vifs; do
		ifconfig "$ifname" down 2>/dev/null
		config_get ifname "$vif" ifname
		config_get enc "$vif" encryption
		config_get eap_type "$vif" eap_type
		config_get mode "$vif" mode

		config_get ifname "$vif" ifname
		[ $? -ne 0 ] && {
			echo "enable_mac80211($device): Failed to set up $mode vif $ifname" >&2
			continue
		}
		config_set "$vif" ifname "$ifname"

		[ "$first" = 1 ] && {
			# only need to change freq band and channel on the first vif
			iwconfig "$ifname" channel "$channel" >/dev/null 2>/dev/null
			if [ "$mode" = adhoc ]; then
				iwlist "$ifname" scan >/dev/null 2>/dev/null
				sleep 1
				iwconfig "$ifname" mode ad-hoc >/dev/null 2>/dev/null
			fi
			# mesh interface should be created only for the first interface
			if [ "$mode" = mesh ]; then
				config_get mesh_id "$vif" mesh_id
				if [ -n "$mesh_id" ]; then
					iw dev "$ifname" interface add msh$mesh_idx type mp mesh_id $mesh_id
				fi
			fi
			sleep 1
			iwconfig "$ifname" channel "$channel" >/dev/null 2>/dev/null
		}
		if [ "$mode" = sta ]; then
			iwconfig "$ifname" mode managed >/dev/null 2>/dev/null
		else
			iwconfig "$ifname" mode $mode >/dev/null 2>/dev/null
		fi

		wpa=
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
			PSK|psk|PSK2|psk2)
				config_get key "$vif" key
			;;
		esac

		case "$mode" in
			adhoc)
				config_get addr "$vif" bssid
				[ -z "$addr" ] || {
					iwconfig "$ifname" ap "$addr"
				}
			;;
		esac
		config_get ssid "$vif" ssid

		config_get vif_txpower "$vif" txpower
		# use vif_txpower (from wifi-iface) to override txpower (from
		# wifi-device) if the latter doesn't exist
		txpower="${txpower:-$vif_txpower}"
		[ -z "$txpower" ] || iwconfig "$ifname" txpower "${txpower%%.*}"

		config_get frag "$vif" frag
		if [ -n "$frag" ]; then
			iwconfig "$ifname" frag "${frag%%.*}"
		fi

		config_get rts "$vif" rts
		if [ -n "$rts" ]; then
			iwconfig "$ifname" rts "${rts%%.*}"
		fi

		ifconfig "$ifname" up
		iwconfig "$ifname" channel "$channel" >/dev/null 2>/dev/null

		local net_cfg bridge
		net_cfg="$(find_net_config "$vif")"
		[ -z "$net_cfg" ] || {
			bridge="$(bridge_interface "$net_cfg")"
			config_set "$vif" bridge "$bridge"
			start_net "$ifname" "$net_cfg"
		}
		iwconfig "$ifname" essid "$ssid"
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
			sta)
				if eval "type wpa_supplicant_setup_vif" 2>/dev/null >/dev/null; then
					wpa_supplicant_setup_vif "$vif" wext || {
						echo "enable_mac80211($device): Failed to set up wpa_supplicant for interface $ifname" >&2
						# make sure this wifi interface won't accidentally stay open without encryption
						ifconfig "$ifname" down
						continue
					}
				fi
			;;
			mesh)
				# special case where physical interface should be down for mesh to work
				ifconfig "$ifname" down
				ifconfig "msh$mesh_idx" up
				iwlist msh$mesh_idx scan 2>/dev/null >/dev/null
			;;
		esac
		first=0
		mesh_idx=$(expr $mesh_idx + 1)
	done
}


detect_mac80211() {
	cd /sys/class/net
	for dev in $(ls -d wlan* 2>&-); do
		config_get type "$dev" type
		[ "$type" = mac80211 ] && return
		cat <<EOF
config wifi-device  $dev
	option type     mac80211
	option channel  5

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
