#!/bin/sh
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
				config_get ssid "$vif" ssid
				[ -z "$addr" -a -n "$ssid" ] && {
					config_set "$vif" wds 1
					config_set "$vif" mode sta
					mode="sta"
					addr="$ssid"
				}
				${addr:+append $mode "$vif"}
			;;
			*) echo "$device($vif): Invalid mode, ignored."; continue;;
		esac
	done

	case "${adhoc:+1}:${sta:+1}:${ap+1}" in
		# valid mode combinations
		1::) wds="";;
		:1:1)config_set "$device" nosbeacon 1;; # AP+STA, can't use beacon timers for STA
		:1:);;
		::1);;
		::);;
		*) echo "$device: Invalid mode combination in config"; return 1;;
	esac

	config_set "$device" vifs "${ap:+$ap }${adhoc:+$adhoc }${sta:+$sta }${wds:+$wds }"
}


disable_atheros() (
	local device="$1"

	# kill all running hostapd and wpa_supplicant processes that
	# are running on atheros vifs 
	for pid in `pidof hostapd wpa_supplicant`; do
		grep ath /proc/$pid/cmdline >/dev/null && \
			kill $pid
	done
	
	include /lib/network
	cd /proc/sys/net
	for dev in *; do
		grep "$device" "$dev/%parent" >/dev/null 2>/dev/null && {
			ifconfig "$dev" down 
			unbridge "$dev"
			wlanconfig "$dev" destroy
		}
	done
	return 0
)

enable_atheros() {
	config_get channel "$device" channel
	config_get vifs "$device" vifs
	
	disable_atheros "$device"
	local first=1
	for vif in $vifs; do
		nosbeacon=
		config_get ifname "$vif" ifname
		config_get enc "$vif" encryption
		config_get mode "$vif" mode
		
		[ "$mode" = sta ] && config_get nosbeacon "$device" nosbeacon
		
		config_get ifname "$vif" ifname
		ifname=$(wlanconfig "$ifname" create wlandev "$device" wlanmode "$mode" ${nosbeacon:+nosbeacon})
		[ $? -ne 0 ] && {
			echo "enable_atheros($device): Failed to set up $mode vif $ifname" >&2
			continue
		}
		config_set "$vif" ifname "$ifname"

		[ "$first" = 1 ] && {
			# only need to change freq band and channel on the first vif
			config_get agmode "$device" mode
			pureg=0
			case "$agmode" in
				*b) agmode=11b;;
				*bg) agmode=11g;;
				*g) agmode=11g; pureg=1;;
				*a) agmode=11a;;
				*) agmode=11g;;
			esac
			iwconfig "$ifname" channel 0 
			iwpriv "$ifname" mode "$agmode"
			iwpriv "$ifname" pureg "$pureg"
			iwconfig "$ifname" channel "$channel"
		}
	
		config_get_bool hidden "$vif" hidden
		iwpriv "$ifname" hide_ssid "$hidden"

		config_get wds "$vif" wds
		case "$wds" in
			1|on|enabled) wds=1;;
			*) wds=0;;
		esac
		iwpriv "$ifname" wds "$wds"

		wpa=
		case "$enc" in
			WEP|wep)
				for idx in 1 2 3 4; do
					config_get key "$vif" "key${idx}"
					iwconfig "$ifname" enc "[$idx]" "${key:-off}"
				done
				config_get key "$vif" key
				iwconfig "$ifname" enc "${key:-1}"
			;;
			PSK|psk|PSK2|psk2)
				config_get key "$vif" key
			;;
		esac

		case "$mode" in
			wds)
				config_get addr "$vif" bssid
				iwpriv "$ifname" wds_add "$addr"
			;;
			*)
				config_get ssid "$vif" ssid
			;;
			adhoc)
				config_get addr "$vif" bssid
				[ -z "$addr" ] || { 
					iwconfig "$ifname" ap "$addr"
				}
			;;
		esac

		[ "$mode" = "sta" ] && {
			config_get_bool bgscan "$vif" bgscan 1
			iwpriv "$ifname" bgscan "$bgscan"
		}

		config_get_bool antdiv "$device" diversity 1
		sysctl -w dev."$device".diversity="$antdiv" >&-

		config_get antrx "$device" rxantenna
		if [ -n "$antrx" ]; then
			sysctl -w dev."$device".rxantenna="$antrx" >&-
		fi

		config_get anttx "$device" txantenna
		if [ -n "$anttx" ]; then
			sysctl -w dev."$device".txantenna="$anttx" >&-
		fi

		config_get distance "$device" distance
		if [ -n "$distance" ]; then
			athctrl -i "$device" -d "$distance" >&-
		fi

		config_get txpwr "$vif" txpower
		if [ -n "$txpwr" ]; then
			iwconfig "$ifname" txpower "${txpwr%%.*}"
		fi

		ifconfig "$ifname" up

		local net_cfg bridge
		net_cfg="$(find_net_config "$vif")"
		[ -z "$net_cfg" ] || {
			bridge="$(bridge_interface "$net_cfg")"
			config_set "$vif" bridge "$bridge"
			start_net "$ifname" "$net_cfg"
		}
		iwconfig "$ifname" essid "$ssid"
		case "$mode" in
			ap)
				hostapd_setup_vif "$vif" madwifi || {
					echo "enable_atheros($device): Failed to set up wpa for interface $ifname" >&2
					# make sure this wifi interface won't accidentally stay open without encryption
					ifconfig "$ifname" down
					wlanconfig "$ifname" destroy
					continue
				}
			;;
			wds|sta)
				case "$enc" in 
					PSK|psk|PSK2|psk2)
						case "$enc" in
							PSK|psk)
								proto='proto=WPA';;
							PSK2|psk2)
								proto='proto=RSN';;
						esac
						cat > /var/run/wpa_supplicant-$ifname.conf <<EOF
ctrl_interface=/var/run/wpa_supplicant
network={
	scan_ssid=1
	ssid="$ssid"
	key_mgmt=WPA-PSK
	$proto
	psk="$key"
}
EOF
					;;
					WPA|wpa|WPA2|wpa2)
						#add wpa_supplicant calls here
					;;
				esac
				wpa_supplicant ${bridge:+ -b $bridge} -Bw -D wext -i "$ifname" -c /var/run/wpa_supplicant-$ifname.conf
			;;
		esac
		first=0
	done
}


detect_atheros() {
	cd /proc/sys/dev
	[ -d ath ] || return
	for dev in $(ls wifi* 2>&-); do
		config_get type "$dev" type
		[ "$type" = atheros ] && return
		cat <<EOF
config wifi-device  $dev
	option type     atheros
	option channel  5
#       option diversity 1
#       option txantenna 0
#       option rxantenna 0
#       option distance  2000
# disable radio to prevent an open ap after reflashing:
	option disabled 1


config wifi-iface
	option device	$dev
	option network	lan
	option mode	ap
	option ssid	OpenWrt
	option hidden	0
#	option txpower	15
#	option bgscan	enable
	option encryption none

EOF
	done
}
