bridge_interface() {
	(
		. /etc/functions.sh
		include network
		scan_interfaces
		cfg="$(find_config "$1")"
		[ -z "$cfg" ] && return 0
		config_get type "$cfg" type
		[ "$type" = bridge ] && config_get "$type" bridge
	)
}

scan_broadcom() {
	local device="$1"

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
				ap=1
				ap_if="${ap_if:+$ap_if }$vif"
			;;
			*) echo "$device($vif): Invalid mode";;
		esac
	done

	local _c=
	for vif in ${adhoc_if:-$sta_if $ap_if}; do
		config_set "$vif" ifname "wl0${_c:+.$_c}"
		_c=$((${_c:-0} + 1))
	done

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
	case "$adhoc:$sta:$ap" in
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


setup_broadcom() {
	local _c
	config_get channel "$device" channel
	config_get country "$device" country
	config_get maxassoc "$device" maxassoc

	_c=0
	nas="$(which nas)"
	nas_cmd=
	if_up=
	for vif in ${adhoc_if:-$sta_if $ap_if}; do
		append vif_pre_up "vif $_c" "$N"
		append vif_post_up "vif $_c" "$N"
		
		[ "$vif" = "$sta_if" ] || {
			config_get_bool hidden "$vif" hidden 1
			append vif_pre_up "closed $hidden" "$N"
			config_get_bool isolate "$vif" isolate
			append vif_pre_up "ap_isolate $hidden" "$N"
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
			;;
			*psk*|*PSK*)
				wsec_r=1
				config_get key "$vif" key
				case "$enc" in
					wpa2*|WPA2*|PSK2*|psk2*) auth=128; wsec=4;;
					*) auth=4; crypto=2;;
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
					*) auth=2; crypto=2;;
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
		append vif_post_up "ssid $ssid" "$N"
		append vif_post_up "enabled 1" "$N"
		
		config_get ifname "$vif" ifname
		append if_up "ifconfig $ifname up" ";$N"
		[ -z "$nasopts" ] || {
			bridge="$(bridge_interface "$ifname")"
			eval "${vif}_ssid=\"\$ssid\""
			mode="-A"
			[ "$vif" = "$sta_if" ] && mode="-S"
			[ -z "$nas" ] || nas_cmd="${nas_cmd:+$nas_cmd$N}$nas -P /var/run/nas.$ifname.pid -H 34954 ${bridge:+ -l $bridge} -i $ifname $mode -m $auth -w $crypto -s \"\$${vif}_ssid\" -g 3600 $nasopts &"
		}
		_c=$(($_c + 1))
	done
	killall -KILL nas >&- 2>&-
	wlc stdin <<EOF
$ifdown

mssid $mssid
ap $ap
apsta $apsta
infra $infra
${wet:+wet 1}

radio ${radio:-1}
macfilter 0
maclist none
wds none
channel ${channel:-0}
country ${country:-IL0}
maxassoc ${maxassoc:-128}

$vif_pre_up
up
$vif_post_up
EOF
	eval "$nas_cmd"
	eval "$if_up"
}


