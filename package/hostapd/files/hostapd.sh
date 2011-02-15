hostapd_set_bss_options() {
	local var="$1"
	local vif="$2"
	local enc wpa_group_rekey wps_possible

	config_get enc "$vif" encryption
	config_get wpa_group_rekey "$vif" wpa_group_rekey
	config_get_bool ap_isolate "$vif" isolate 0

	config_get device "$vif" device
	config_get hwmode "$device" hwmode
	config_get phy "$device" phy

	append "$var" "ctrl_interface=/var/run/hostapd-$phy" "$N"

	if [ "$ap_isolate" -gt 0 ]; then
		append "$var" "ap_isolate=$ap_isolate" "$N"
	fi

	# Examples:
	# psk-mixed/tkip 	=> WPA1+2 PSK, TKIP
	# wpa-psk2/tkip+aes	=> WPA2 PSK, CCMP+TKIP
	# wpa2/tkip+aes 	=> WPA2 RADIUS, CCMP+TKIP
	# ...

	# TODO: move this parsing function somewhere generic, so that
	# later it can be reused by drivers that don't use hostapd

	# crypto defaults: WPA2 vs WPA1
	case "$enc" in
		wpa2*|*psk2*)
			wpa=2
			crypto="CCMP"
		;;
		*mixed*)
			wpa=3
			crypto="CCMP TKIP"
		;;
		*)
			wpa=1
			crypto="TKIP"
		;;
	esac

	# explicit override for crypto setting
	case "$enc" in
		*tkip+aes|*tkip+ccmp|*aes+tkip|*ccmp+tkip) crypto="CCMP TKIP";;
		*aes|*ccmp) crypto="CCMP";;
		*tkip) crypto="TKIP";;
	esac

	# enforce CCMP for 11ng and 11na
	case "$hwmode:$crypto" in
		*ng:TKIP|*na:TKIP) crypto="CCMP TKIP";;
	esac

	# use crypto/auth settings for building the hostapd config
	case "$enc" in
		*psk*)
			config_get psk "$vif" key
			if [ ${#psk} -eq 64 ]; then
				append "$var" "wpa_psk=$psk" "$N"
			else
				append "$var" "wpa_passphrase=$psk" "$N"
			fi
			wps_possible=1
		;;
		*wpa*)
			# required fields? formats?
			# hostapd is particular, maybe a default configuration for failures
			config_get server "$vif" server
			append "$var" "auth_server_addr=$server" "$N"
			config_get port "$vif" port
			port=${port:-1812}
			append "$var" "auth_server_port=$port" "$N"
			config_get secret "$vif" key
			append "$var" "auth_server_shared_secret=$secret" "$N"
			config_get nasid "$vif" nasid
			append "$var" "nas_identifier=$nasid" "$N"
			append "$var" "eapol_key_index_workaround=1" "$N"
			append "$var" "radius_acct_interim_interval=300" "$N"
			append "$var" "ieee8021x=1" "$N"
			append "$var" "wpa_key_mgmt=WPA-EAP" "$N"
			append "$var" "wpa_group_rekey=300" "$N"
			append "$var" "wpa_gmk_rekey=640" "$N"
		;;
		*wep*)
			config_get key "$vif" key
			key="${key:-1}"
			case "$key" in
				[1234])
					for idx in 1 2 3 4; do
						local zidx
						zidx=$(($idx - 1))
						config_get ckey "$vif" "key${idx}"
						[ -n "$ckey" ] && \
							append "$var" "wep_key${zidx}=$(prepare_key_wep "$ckey")" "$N"
					done
					append "$var" "wep_default_key=$((key - 1))"  "$N"
				;;
				*)
					append "$var" "wep_key0=$(prepare_key_wep "$key")" "$N"
					append "$var" "wep_default_key=0" "$N"
				;;
			esac
			case "$enc" in
				*shared*)
					auth_algs=2
				;;
				*mixed*)
					auth_algs=3
				;;
			esac
			wpa=0
			crypto=
		;;
		*)
			wpa=0
			crypto=
		;;
	esac
	append "$var" "auth_algs=${auth_algs:-1}" "$N"
	append "$var" "wpa=$wpa" "$N"
	[ -n "$crypto" ] && append "$var" "wpa_pairwise=$crypto" "$N"
	[ -n "$wpa_group_rekey" ] && append "$var" "wpa_group_rekey=$wpa_group_rekey" "$N"

	config_get ssid "$vif" ssid
	config_get bridge "$vif" bridge
	config_get ieee80211d "$vif" ieee80211d
	config_get iapp_interface "$vif" iapp_interface

	config_get_bool wps_pbc "$vif" wps_pushbutton 0
	[ -n "$wps_possible" -a "$wps_pbc" -gt 0 ] && {
		append "$var" "eap_server=1" "$N"
		append "$var" "wps_state=2" "$N"
		append "$var" "ap_setup_locked=1" "$N"
		append "$var" "config_methods=push_button" "$N"
	}

	append "$var" "ssid=$ssid" "$N"
	[ -n "$bridge" ] && append "$var" "bridge=$bridge" "$N"
	[ -n "$ieee80211d" ] && append "$var" "ieee80211d=$ieee80211d" "$N"
	[ -n "$iapp_interface" ] && append "$var" iapp_interface=$(uci_get_state network "$iapp_interface" ifname "$iapp_interface") "$N"

	if [ "$wpa" -ge "2" ]
	then
		# RSN -> allow preauthentication
		config_get rsn_preauth "$vif" rsn_preauth
		if [ -n "$bridge" -a "$rsn_preauth" = 1 ]
		then
			append "$var" "rsn_preauth=1" "$N"
			append "$var" "rsn_preauth_interfaces=$bridge" "$N"
		fi

		# RSN -> allow management frame protection
		config_get ieee80211w "$vif" ieee80211w
		case "$ieee80211w" in
			[012])
				append "$var" "ieee80211w=$ieee80211w" "$N"
				[ "$ieee80211w" -gt "0" ] && {
					config_get ieee80211w_max_timeout "$vif" ieee80211w_max_timeout
					config_get ieee80211w_retry_timeout "$vif" ieee80211w_retry_timeout
					[ -n "$ieee80211w_max_timeout" ] && \
						append "$var" "assoc_sa_query_max_timeout=$ieee80211w_max_timeout" "$N"
					[ -n "$ieee80211w_retry_timeout" ] && \
						append "$var" "assoc_sa_query_retry_timeout=$ieee80211w_retry_timeout" "$N"
				}
			;;
		esac
	fi
}

hostapd_setup_vif() {
	local vif="$1"
	local driver="$2"
	hostapd_cfg=

	hostapd_set_bss_options hostapd_cfg "$vif"
	config_get ifname "$vif" ifname
	config_get device "$vif" device
	config_get channel "$device" channel
	config_get hwmode "$device" hwmode
	case "$hwmode" in
		*bg|*gdt|*gst|*fh) hwmode=g;;
		*adt|*ast) hwmode=a;;
	esac
	[ "$channel" = auto ] && channel=
	[ -n "$channel" -a -z "$hwmode" ] && wifi_fixup_hwmode "$device"
	cat > /var/run/hostapd-$ifname.conf <<EOF
driver=$driver
interface=$ifname
${hwmode:+hw_mode=${hwmode#11}}
${channel:+channel=$channel}
$hostapd_cfg
EOF
	hostapd -P /var/run/wifi-$ifname.pid -B /var/run/hostapd-$ifname.conf
}

