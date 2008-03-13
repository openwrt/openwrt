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
			adhoc|ahdemo|sta|ap)
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
		1::1);;
		:1:1)config_set "$device" nosbeacon 1;; # AP+STA, can't use beacon timers for STA
		:1:);;
		::1);;
		::);;
		*) echo "$device: Invalid mode combination in config"; return 1;;
	esac

	config_set "$device" vifs "${ap:+$ap }${adhoc:+$adhoc }${ahdemo:+$ahdemo }${sta:+$sta }${wds:+$wds }"
}


disable_atheros() (
	local device="$1"

	set_wifi_down "$device"
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
	local device="$1"
	config_get channel "$device" channel
	config_get vifs "$device" vifs

	[ auto = "$channel" ] && channel=0

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
			config_get agmode "$device" agmode
			pureg=0
			case "$agmode" in
				*b) agmode=11b;;
				*bg) agmode=11g;;
				*g) agmode=11g; pureg=1;;
				*a) agmode=11a;;
				*) agmode=auto;;
			esac
			iwconfig "$ifname" channel "$channel" >/dev/null 2>/dev/null 
			ifconfig "$ifname" up
			sleep 1
			iwpriv "$ifname" mode "$agmode"
			iwpriv "$ifname" pureg "$pureg"
			iwconfig "$ifname" channel "$channel" >/dev/null 2>/dev/null 
		}
	
		config_get_bool hidden "$vif" hidden 0
		iwpriv "$ifname" hide_ssid "$hidden"

		config_get ff "$vif" ff
		if [ -n "$ff" ]; then
			iwpriv "$ifname" ff "$ff"
		fi

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
			wds)
				config_get addr "$vif" bssid
				iwpriv "$ifname" wds_add "$addr"
			;;
			adhoc|ahdemo)
				config_get addr "$vif" bssid
				[ -z "$addr" ] || { 
					iwconfig "$ifname" ap "$addr"
				}
			;;
		esac
		config_get ssid "$vif" ssid

		config_get_bool bgscan "$vif" bgscan
		[ -n "$bgscan" ] && iwpriv "$ifname" bgscan "$bgscan"

		config_get_bool antdiv "$device" diversity
		[ -n "$antdiv" ] && sysctl -w dev."$device".diversity="$antdiv" >&-

		config_get antrx "$device" rxantenna
		[ -n "$antrx" ] && sysctl -w dev."$device".rxantenna="$antrx" >&-

		config_get anttx "$device" txantenna
		[ -n "$anttx" ] && sysctl -w dev."$device".txantenna="$anttx" >&-

		config_get distance "$device" distance
		[ -n "$distance" ] && athctrl -i "$device" -d "$distance" >&-

		config_get txpwr "$vif" txpower
		[ -n "$txpwr" ] && iwconfig "$ifname" txpower "${txpwr%%.*}"

		config_get rate "$vif" rate
		[ -n "$rate" ] && iwconfig "$ifname" rate "${rate%%.*}"

		config_get mcast_rate "$vif" mcast_rate
		[ -n "$mcast_rate" ] && iwpriv "$ifname" mcast_rate "${mcast_rate%%.*}"

		config_get frag "$vif" frag
		[ -n "$frag" ] && iwconfig "$ifname" frag "${frag%%.*}"

		config_get rts "$vif" rts
		[ -n "$rts" ] && iwconfig "$ifname" rts "${rts%%.*}"

		config_get_bool doth "$vif" 80211h
		[ -n "$doth" ] && iwpriv "$ifname" doth "$doth"

		config_get_bool comp "$vif" compression
		[ -n "$comp" ] && iwpriv "$ifname" compression "$comp"

		config_get_bool minrate "$vif" minrate
		[ -n "$minrate" ] && iwpriv "$ifname" minrate "$minrate"

		config_get_bool maxrate "$vif" maxrate
		[ -n "$maxrate" ] && iwpriv "$ifname" maxrate "$maxrate"

		config_get_bool burst "$vif" bursting
		[ -n "$burst" ] && iwpriv "$ifname" burst "$burst"

		config_get_bool wmm "$vif" wmm
		[ -n "$wmm" ] && iwpriv "$ifname" wmm "$wmm"

		config_get_bool xr "$vif" xr
		[ -n "$xr" ] && iwpriv "$ifname" xr "$xr"

		config_get_bool ar "$vif" ar
		[ -n "$ar" ] && iwpriv "$ifname" ar "$ar"

		config_get_bool turbo "$vif" turbo
		[ -n "$turbo" ] && iwpriv "$ifname" turbo "$turbo"

		config_get_bool doth "$vif" doth 0
		[ -n "$doth" ] && iwpriv "$ifname" doth "$doth"

		config_get maclist "$vif" maclist
		[ -n "$maclist" ] && {
			# flush MAC list
			iwpriv "$ifname" maccmd 3
			for mac in $maclist; do
				iwpriv "$ifname" addmac "$mac"
			done
		}

		config_get macpolicy "$vif" macpolicy
		case "$macpolicy" in
			allow)
				iwpriv "$ifname" maccmd 1
			;;
			deny)
				iwpriv "$ifname" maccmd 2
			;;
			*)
				# default deny policy if mac list exists
				[ -n "$maclist" ] && iwpriv "$ifname" maccmd 2
			;;
		esac

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
				config_get_bool isolate "$vif" isolate 0
				iwpriv "$ifname" ap_bridge "$((isolate^1))"

				if eval "type hostapd_setup_vif" 2>/dev/null >/dev/null; then
					hostapd_setup_vif "$vif" madwifi || {
						echo "enable_atheros($device): Failed to set up wpa for interface $ifname" >&2
						# make sure this wifi interface won't accidentally stay open without encryption
						ifconfig "$ifname" down
						wlanconfig "$ifname" destroy
						continue
					}
				fi
			;;
			wds|sta)
				config_get_bool usepassphrase "$vif" passphrase 1
				case "$enc" in
					PSK|psk|PSK2|psk2)
						case "$enc" in
							PSK|psk)
								proto='proto=WPA'
								if [ "$usepassphrase" = "1" ]; then
									passphrase="psk=\"${key}\""
								else
									passphrase="psk=${key}"
								fi
								;;
							PSK2|psk2)
								proto='proto=RSN'
                                                                if [ "$usepassphrase" = "1" ]; then
                                                                        passphrase="psk=\"${key}\""
                                                                else
                                                                        passphrase="psk=${key}"
                                                                fi
								;;
						esac
						cat > /var/run/wpa_supplicant-$ifname.conf <<EOF
network={
	scan_ssid=1
	ssid="$ssid"
	key_mgmt=WPA-PSK
	$proto
	$passphrase
}
EOF
					;;
					WPA|wpa|WPA2|wpa2)
						#add wpa_supplicant calls here
					;;
				esac
				[ -z "$proto" ] || wpa_supplicant ${bridge:+ -b $bridge} -B -D wext -i "$ifname" -c /var/run/wpa_supplicant-$ifname.conf
			;;
		esac
		first=0
	done
}


detect_atheros() {
	cd /proc/sys/dev
	[ -d ath ] || return
	for dev in $(ls -d wifi* 2>&-); do
		config_get type "$dev" type
		[ "$type" = atheros ] && return
		cat <<EOF
config wifi-device  $dev
	option type     atheros
	option channel  auto

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
