append DRIVERS "broadcom"

scan_broadcom() {
	local device="$1"
	local wds
	local adhoc sta apmode mon disabled
	local adhoc_if sta_if ap_if mon_if
	local _c=0

	config_get vifs "$device" vifs
	for vif in $vifs; do
		config_get_bool disabled "$vif" disabled 0
		[ $disabled -eq 0 ] || continue

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
		config_set "$vif" ifname "${device}${_c:+-$_c}"
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
		;;
		::)
			radio=0
		;;
	esac
}

disable_broadcom() {
	local device="$1"
	set_wifi_down "$device"
	wlc ifname "$device" down
	wlc ifname "$device" bssid `wlc ifname "$device" default_bssid`
	(
		include /lib/network

		local pid_file=/var/run/nas.$device.pid
		[ -e $pid_file ] && start-stop-daemon -K -q -s SIGKILL -p $pid_file && rm $pid_file

		# make sure the interfaces are down and removed from all bridges
		for dev in $device ${device}-1 ${device}-2 ${device}-3; do
			ifconfig "$dev" down 2>/dev/null >/dev/null && {
				unbridge "$dev"
			}
		done
	)
	true
}

enable_broadcom() {
	local device="$1"
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
	config_get frag "$device" frag
	config_get rts "$device" rts
	config_get hwmode "$device" hwmode
	config_get htmode "$device" htmode
	local vif_pre_up vif_post_up vif_do_up vif_txpower
	local doth=0
	local wmm=1

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

	[ ${channel:-0} -ge 1 -a ${channel:-0} -le 14 ] && band=2
	[ ${channel:-0} -ge 36 ] && band=1

	case "$hwmode" in
		*na)	nmode=1; nreqd=0;;
		*a)	nmode=0;;
		*ng)	gmode=1; nmode=1; nreqd=0;;
		*n)	nmode=1; nreqd=1;;
		*b)	gmode=0; nmode=0;;
		*bg)	gmode=1; nmode=0;;
		*g)	gmode=2; nmode=0;;
		*gst)	gmode=4; nmode=0;;
		*lrs)	gmode=5; nmode=0;;
		*)      case "$band" in
				2) gmode=1; nmode=1; nreqd=0;;
				1) nmode=1; nreqd=0;;
				*) gmode=1; nmode=1; nreqd=0;;
			esac
			;;
	esac

        # Use 'nmode' for N-Phy only
	[ "$(wlc ifname $device phytype)" = 4 ] || nmode=

	# Use 'chanspec' instead of 'channel' for 'N' modes (See bcmwifi.h)
	[ ${nmode:-0} -ne 0 -a -n "$band" ] && {
		case "$htmode" in
			HT40-)	chanspec=$(printf 0x%x%x%02x $band 0xe $(($channel - 2))); channel=;;
			HT40+)	chanspec=$(printf 0x%x%x%02x $band 0xd $(($channel + 2))); channel=;;
			HT20)	chanspec=$(printf 0x%x%x%02x $band 0xb $channel); channel=;;
			*) ;;
		esac
	}

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
			*wep*)
				wsec_r=1
				wsec=1
				defkey=1
				config_get key "$vif" key
				case "$enc" in
					*shared*) append vif_do_up "wepauth 1" "$N";;
					*) append vif_do_up "wepauth 0" "$N";;
				esac
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
			*psk*)
				wsec_r=1
				config_get key "$vif" key

				# psk version + default cipher
				case "$enc" in
					*mixed*|*psk+psk2*) auth=132; wsec=6;;
					*psk2*) auth=128; wsec=4;;
					*) auth=4; wsec=2;;
				esac

				# cipher override
				case "$enc" in
					*tkip+aes*|*tkip+ccmp*|*aes+tkip*|*ccmp+tkip*) wsec=6;;
					*aes*|*ccmp*) wsec=4;;
					*tkip*) wsec=2;;
				esac

				# group rekey interval
				config_get rekey "$vif" wpa_group_rekey

				eval "${vif}_key=\"\$key\""
				nasopts="-k \"\$${vif}_key\"${rekey:+ -g $rekey}"
			;;
			*wpa*)
				wsec_r=1
				eap_r=1
				config_get auth_server "$vif" auth_server
				[ -z "$auth_server" ] && config_get auth_server "$vif" server
				config_get auth_port "$vif" auth_port
				[ -z "$auth_port" ] && config_get auth_port "$vif" port
				config_get auth_secret "$vif" auth_secret
				[ -z "$auth_secret" ] && config_get auth_secret "$vif" key

				# wpa version + default cipher
				case "$enc" in
					*mixed*|*wpa+wpa2*) auth=66; wsec=6;;
					*wpa2*) auth=64; wsec=4;;
					*) auth=2; wsec=2;;
				esac

				# cipher override
				case "$enc" in
					*tkip+aes*|*tkip+ccmp*|*aes+tkip*|*ccmp+tkip*) wsec=6;;
					*aes*|*ccmp*) wsec=4;;
					*tkip*) wsec=2;;
				esac

				# group rekey interval
				config_get rekey "$vif" wpa_group_rekey

				eval "${vif}_key=\"\$auth_secret\""
				nasopts="-r \"\$${vif}_key\" -h $auth_server -p ${auth_port:-1812}${rekey:+ -g $rekey}"
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
		}

		[ "$mode" = "adhoc" ] && {
			config_get bssid "$vif" bssid
			[ -n "$bssid" ] && {
				append vif_pre_up "bssid $bssid" "$N"
				append vif_pre_up "ibss_merge 0" "$N"
			} || {
				append vif_pre_up "ibss_merge 1" "$N"
			}
		}

		append vif_post_up "enabled 1" "$N"

		config_get ifname "$vif" ifname
		#append if_up "ifconfig $ifname up" ";$N"

		local net_cfg
		net_cfg="$(find_net_config "$vif")"
		[ -z "$net_cfg" ] || {
			append if_up "set_wifi_up '$vif' '$ifname'" ";$N"
			append if_up "start_net '$ifname' '$net_cfg'" ";$N"
		}
		[ -z "$nas" -o -z "$nasopts" ] || {
			eval "${vif}_ssid=\"\$ssid\""
			nas_mode="-A"
			[ "$mode" = "sta" ] && nas_mode="-S"
			[ -z "$nas_cmd" ] && {
				local pid_file=/var/run/nas.$device.pid
				nas_cmd="start-stop-daemon -S -b -p $pid_file -x $nas -- -P $pid_file -H 34954"
			}
			append nas_cmd "-i $ifname $nas_mode -m $auth -w $wsec -s \"\$${vif}_ssid\" -g 3600 -F $nasopts"
		}
		_c=$(($_c + 1))
	done
	wlc ifname "$device" stdin <<EOF
$ifdown

${nmode:+band ${band:-0}}
${nmode:+nmode $nmode}
${nmode:+${nreqd:+nreqd $nreqd}}
${gmode:+gmode $gmode}
apsta $apsta
ap $ap
${mssid:+mssid $mssid}
infra $infra
${wet:+wet 1}
802.11d 0
802.11h ${doth:-0}
wme ${wmm:-1}
rxant ${rxantenna:-3}
txant ${txantenna:-3}
fragthresh ${frag:-2346}
rtsthresh ${rts:-2347}
monitor ${monitor:-0}

radio ${radio:-1}
macfilter ${macfilter:-0}
maclist ${maclist:-none}
wds none
${wds:+wds $wds}
country ${country:-US}
${channel:+channel $channel}
${chanspec:+chanspec $chanspec}
maxassoc ${maxassoc:-128}
slottime ${slottime:--1}
${frameburst:+frameburst $frameburst}

$vif_pre_up
up
$vif_post_up
EOF
	eval "$if_up"
	wlc ifname "$device" stdin <<EOF
$vif_do_up
EOF

	# use vif_txpower (from last wifi-iface) instead of txpower (from
	# wifi-device) if the latter does not exist
	txpower=${txpower:-$vif_txpower}
	[ -z "$txpower" ] || iwconfig $device txpower ${txpower}dBm

	eval "$nas_cmd"
}


detect_broadcom() {
	local i=-1

	while grep -qs "^ *wl$((++i)):" /proc/net/dev; do
		local channel

		config_get type wl${i} type
		[ "$type" = broadcom ] && continue
		channel=`wlc ifname wl${i} channel`
		cat <<EOF
config wifi-device  wl${i}
	option type     broadcom
	option channel  ${channel:-11}

	# REMOVE THIS LINE TO ENABLE WIFI:
	option disabled 1

config wifi-iface
	option device   wl${i}
	option network	lan
	option mode     ap
	option ssid     OpenWrt${i#0}
	option encryption none

EOF
	done
}
