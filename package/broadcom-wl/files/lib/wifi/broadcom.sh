append DRIVERS "broadcom"

scan_broadcom() {
	local device="$1"
	local wds
	local adhoc sta apmode mon
	local adhoc_if sta_if ap_if mon_if
	local _c=0

	config_get vifs "$device" vifs
	for vif in $vifs; do
		config_get mode "$vif" mode
		_c=$(($_c + 1))
		case "$mode" in
			adhoc)
				adhoc=1
				adhoc_if="$vif"
			;;
			sta)
				sta=1
				sta_if="$vif"
			;;
			ap)
				apmode=1
				ap_if="${ap_if:+$ap_if }$vif"
			;;
			wds)
				config_get addr "$vif" bssid
				[ -z "$addr" ] || {
					addr=$(echo "$addr" | tr 'A-F' 'a-f')
					append wds "$addr"
				}
			;;
			monitor)
				mon=1
				mon_if="$vif"
			;;
			*) echo "$device($vif): Invalid mode";;
		esac
	done
	config_set "$device" wds "$wds"

	local _c=
	for vif in ${adhoc_if:-$sta_if $ap_if $mon_if}; do
		config_set "$vif" ifname "wl0${_c:+.$_c}"
		_c=$((${_c:-0} + 1))
	done
	config_set "$device" vifs "${adhoc_if:-$sta_if $ap_if $mon_if}"

	ifdown="down"
	for vif in 0 1 2 3; do
		append ifdown "vif $vif" "$N"
		append ifdown "enabled 0" "$N"
	done

	ap=1
	infra=1
	if [ "$_c" -gt 1 ]; then
		mssid=1
	else
		mssid=
	fi
	apsta=0
	radio=1
	monitor=0
	passive=0
	case "$adhoc:$sta:$apmode:$mon" in
		1*)
			ap=0
			mssid=
			infra=0
		;;
		:1:1:)
			apsta=1
			wet=1
		;;
		:1::)
			wet=1
			ap=0
			mssid=
		;;
		:::1)
			wet=1
			ap=0
			mssid=
			monitor=1
			passive=1
		;;
		::)
			radio=0
		;;
	esac
}

disable_broadcom() {
	set_wifi_down "$1"
	wlc down
	(
		include /lib/network

		# make sure the interfaces are down and removed from all bridges
		for dev in wl0 wl0.1 wl0.2 wl0.3; do
			ifconfig "$dev" down 2>/dev/null >/dev/null && {
				unbridge "$dev"
			}
		done
	)
	true
}

enable_broadcom() {
	local _c
	config_get channel "$device" channel
	config_get country "$device" country
	config_get maxassoc "$device" maxassoc
	config_get wds "$device" wds
	config_get vifs "$device" vifs
	config_get distance "$device" distance
	config_get slottime "$device" slottime
	config_get rxantenna "$device" rxantenna
	config_get txantenna "$device" txantenna
	config_get_bool frameburst "$device" frameburst
	config_get macfilter "$device" macfilter
	config_get maclist "$device" maclist
	config_get macaddr "$device" macaddr
	config_get txpower "$device" txpower
	local vif_pre_up vif_post_up vif_do_up vif_txpower
	local doth=0
	local wmm=0

	_c=0
	nas="$(which nas)"
	nas_cmd=
	if_up=

	[ -z "$slottime" ] && {
		[ -n "$distance" ] && {
			# slottime = 9 + (distance / 150) + (distance % 150 ? 1 : 0)
			slottime="$((9 + ($distance / 150) + 1 - (150 - ($distance % 150)) / 150 ))"
		}
	} || {
		slottime="${slottime:--1}"
	}

	case "$macfilter" in
		allow|2)
			macfilter=2;
		;;
		deny|1)
			macfilter=1;
		;;
		disable|none|0)
			macfilter=0;
		;;
	esac

	for vif in $vifs; do
		config_get vif_txpower "$vif" txpower

		config_get mode "$vif" mode
		append vif_pre_up "vif $_c" "$N"
		append vif_post_up "vif $_c" "$N"
		append vif_do_up "vif $_c" "$N"

		config_get_bool wmm "$vif" wmm "$wmm"
		config_get_bool doth "$vif" doth "$doth"

		[ "$mode" = "sta" ] || {
			config_get_bool hidden "$vif" hidden 0
			append vif_pre_up "closed $hidden" "$N"
			config_get_bool isolate "$vif" isolate 0
			append vif_pre_up "ap_isolate $isolate" "$N"
		}

		wsec_r=0
		eap_r=0
		wsec=0
		auth=0
		nasopts=
		config_get enc "$vif" encryption
		case "$enc" in
			WEP|wep)
				wsec_r=1
				wsec=1
				defkey=1
				config_get key "$vif" key
				case "$key" in
					[1234])
						defkey="$key"
						for knr in 1 2 3 4; do
							config_get k "$vif" key$knr
							[ -n "$k" ] || continue
							[ "$defkey" = "$knr" ] && def="=" || def=""
							append vif_do_up "wepkey $def$knr,$k" "$N"
						done
					;;
					"");;
					*) append vif_do_up "wepkey =1,$key" "$N";;
				esac
			;;
			*psk*|*PSK*)
				wsec_r=1
				config_get key "$vif" key
				case "$enc" in
					wpa*+wpa2*|WPA*+WPA2*|*psk+*psk2|*PSK+*PSK2) auth=132; wsec=6;;
					wpa2*|WPA2*|*PSK2|*psk2) auth=128; wsec=4;;
					*aes|*AES) auth=4; wsec=4;;
					*) auth=4; wsec=2;;
				esac
				eval "${vif}_key=\"\$key\""
				nasopts="-k \"\$${vif}_key\""
			;;
			*wpa*|*WPA*)
				wsec_r=1
				eap_r=1
				config_get key "$vif" key
				config_get server "$vif" server
				config_get port "$vif" port
				case "$enc" in
					wpa*+wpa2*|WPA*+WPA2*) auth=66; wsec=6;;
					wpa2*|WPA2*) auth=64; wsec=4;;
					*) auth=2; wsec=2;;
				esac
				eval "${vif}_key=\"\$key\""
				nasopts="-r \"\$${vif}_key\" -h $server -p ${port:-1812}"
			;;
		esac
		append vif_do_up "wsec $wsec" "$N"
		append vif_do_up "wpa_auth $auth" "$N"
		append vif_do_up "wsec_restrict $wsec_r" "$N"
		append vif_do_up "eap_restrict $eap_r" "$N"
		
		config_get ssid "$vif" ssid
		append vif_post_up "vlan_mode 0" "$N"
		append vif_post_up "ssid $ssid" "$N"
		append vif_do_up "ssid $ssid" "$N"

		[ "$mode" = "monitor" ] && {
			append vif_post_up "monitor $monitor" "$N"
			append vif_post_up "passive $passive" "$N"
		}

		[ "$mode" = "adhoc" ] && {
			config_get bssid "$vif" bssid
			[ -n "$bssid" ] && {
				append vif_pre_up "des_bssid $bssid" "$N"
				append vif_pre_up "allow_mode 1" "$N"
			}
		} || append vif_pre_up "allow_mode 0" "$N"
		
		append vif_post_up "enabled 1" "$N"
		
		config_get ifname "$vif" ifname
		#append if_up "ifconfig $ifname up" ";$N"

		local net_cfg bridge
		net_cfg="$(find_net_config "$vif")"
		[ -z "$net_cfg" ] || {
			bridge="$(bridge_interface "$net_cfg")"
			append if_up "set_wifi_up '$vif' '$ifname'" ";$N"
			append if_up "start_net '$ifname' '$net_cfg' \$(wlc ifname '$ifname' bssid)" ";$N"
		}
		[ -z "$nasopts" ] || {
			eval "${vif}_ssid=\"\$ssid\""
			nas_mode="-A"
			use_nas=1
			[ "$mode" = "sta" ] && {
				nas_mode="-S"
				[ -z "$bridge" ] || {
					append vif_post_up "supplicant 1" "$N"
					append vif_post_up "passphrase $key" "$N"
					
					use_nas=0
				}
			}
			[ -z "$nas" -o "$use_nas" = "0" ] || nas_cmd="${nas_cmd:+$nas_cmd$N}$nas -P /var/run/nas.$ifname.pid -H 34954 ${bridge:+ -l $bridge} -i $ifname $nas_mode -m $auth -w $wsec -s \"\$${vif}_ssid\" -g 3600 $nasopts &"
		}
		_c=$(($_c + 1))
	done
	killall -KILL nas >&- 2>&-
	wlc stdin <<EOF
$ifdown

apsta $apsta
ap $ap
${mssid:+mssid $mssid}
infra $infra
${wet:+wet 1}
802.11d 0
802.11h ${doth:-0}
wme ${wmm:-0}
rxant ${rxantenna:-3}
txant ${txantenna:-3}
monitor ${monitor:-0}
passive ${passive:-0}

radio ${radio:-1}
macfilter ${macfilter:-0}
maclist ${maclist:-none}
wds none
${wds:+wds $wds}
country ${country:-IL0}
${channel:+channel $channel}
maxassoc ${maxassoc:-128}
slottime ${slottime:--1}
${frameburst:+frameburst $frameburst}

$vif_pre_up
up
$vif_post_up
EOF
	eval "$if_up"
	wlc stdin <<EOF
$vif_do_up
EOF

	# use vif_txpower (from last wifi-iface) instead of txpower (from
	# wifi-device) if the latter does not exist
	txpower=${txpower:-$vif_txpower}
	[ -z "$txpower" ] || iwconfig $device txpower ${txpower}dBm

	eval "$nas_cmd"
}


detect_broadcom() {
	[ -f /proc/net/wl0 ] || return
	config_get type wl0 type
	[ "$type" = broadcom ] && return
	cat <<EOF
config wifi-device  wl0
	option type     broadcom
	option channel  5

	# REMOVE THIS LINE TO ENABLE WIFI:
	option disabled 1

config wifi-iface
	option device   wl0
	option network	lan
	option mode     ap
	option ssid     OpenWrt
	option encryption none
EOF
}
