append DRIVERS "broadcom"

scan_broadcom() {
	local device="$1"
	local wds
	local adhoc sta apmode
	local adhoc_if sta_if ap_if

	config_get vifs "$device" vifs
	for vif in $vifs; do
		config_get mode "$vif" mode
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
				[ -z "$addr" ] || append wds "$addr"
			;;
			*) echo "$device($vif): Invalid mode";;
		esac
	done
	config_set "$device" wds "$wds"

	local _c=
	for vif in ${adhoc_if:-$sta_if $ap_if}; do
		config_set "$vif" ifname "wl0${_c:+.$_c}"
		_c=$((${_c:-0} + 1))
	done
	config_set "$device" vifs "${adhoc_if:-$sta_if $ap_if}"

	ifdown="down"
	for vif in 0 1 2 3; do
		append ifdown "vif $vif" "$N"
		append ifdown "enabled 0" "$N"
	done

	ap=1
	infra=1
	mssid=1
	apsta=0
	radio=1
	case "$adhoc:$sta:$apmode" in
		1*)
			ap=0
			mssid=0
			infra=0
		;;
		:1:1)
			apsta=1
			wet=1
		;;
		:1:)
			wet=1
			ap=0
			mssid=0
		;;
		::)
			radio=0
		;;
	esac
}

disable_broadcom() {
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
	local vif_pre_up vif_post_up vif_do_up

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

	for vif in $vifs; do
		config_get mode "$vif" mode
		append vif_pre_up "vif $_c" "$N"
		append vif_post_up "vif $_c" "$N"
		
		[ "$mode" = "sta" ] || {
			config_get_bool hidden "$vif" hidden 1
			append vif_pre_up "closed $hidden" "$N"
			config_get_bool isolate "$vif" isolate
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
							append vif_pre_up "wepkey $def$knr,$k" "$N"
						done
					;;
					"");;
					*) append vif_pre_up "wepkey =1,$key" "$N";;
				esac
			;;
			*psk*|*PSK*)
				wsec_r=1
				config_get key "$vif" key
				case "$enc" in
					wpa2*|WPA2*|PSK2*|psk2*) auth=128; wsec=4;;
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
					wpa2*|WPA2*) auth=64; wsec=4;;
					*) auth=2; wsec=2;;
				esac
				eval "${vif}_key=\"\$key\""
				nasopts="-r \"\$${vif}_key\" -h $server -p $port"
			;;
		esac
		append vif_post_up "wsec $wsec" "$N"
		append vif_post_up "wpa_auth $auth" "$N"
		append vif_post_up "wsec_restrict $wsec_r" "$N"
		append vif_post_up "eap_restrict $eap_r" "$N"
		
		config_get ssid "$vif" ssid
		append vif_post_up "vlan_mode 0"
		append vif_post_up "ssid $ssid" "$N"
		case "$mode" in
			sta|adhoc) append vif_do_up "ssid $ssid" "$N";;
		esac
		
		append vif_post_up "enabled 1" "$N"
		
		config_get ifname "$vif" ifname
		append if_up "ifconfig $ifname up" ";$N"
		local net_cfg bridge
		net_cfg="$(find_net_config "$vif")"
		[ -z "$net_cfg" ] || {
			bridge="$(bridge_interface "$net_cfg")"
			append if_up "start_net '$ifname' '$net_cfg'" ";$N"
		}
		[ -z "$nasopts" ] || {
			eval "${vif}_ssid=\"\$ssid\""
			mode="-A"
			[ "$vif" = "$sta_if" ] && mode="-S"
			[ -z "$nas" ] || nas_cmd="${nas_cmd:+$nas_cmd$N}$nas -P /var/run/nas.$ifname.pid -H 34954 ${bridge:+ -l $bridge} -i $ifname $mode -m $auth -w $wsec -s \"\$${vif}_ssid\" -g 3600 $nasopts &"
		}
		_c=$(($_c + 1))
	done
	killall -KILL nas >&- 2>&-
	wlc stdin <<EOF
$ifdown

ap $ap
mssid $mssid
apsta $apsta
infra $infra
${wet:+wet 1}

radio ${radio:-1}
macfilter 0
maclist none
wds ${wds:-none}
channel ${channel:-0}
country ${country:-IL0}
maxassoc ${maxassoc:-128}
slottime ${slottime:--1}

$vif_pre_up
up
$vif_post_up
EOF
	eval "$if_up"
	wlc stdin <<EOF
$vif_do_up
EOF
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

config wifi-iface
	option device   wl0
	option mode     ap
	option ssid     OpenWrt
	option hidden   0
	option encryption none

EOF
}
