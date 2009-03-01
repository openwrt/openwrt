hostapd_setup_vif() {
	local vif="$1"
	local driver="$2"
	local hostapd_cfg=

	# Examples:
	# psk-mixed/tkip 	=> WPA1+2 PSK, TKIP
	# wpa-psk2/tkip+aes	=> WPA2 PSK, CCMP+TKIP
	# wpa2/tkip+aes 	=> WPA2 RADIUS, CCMP+TKIP
	# ...

	# TODO: move this parsing function somewhere generic, so that
	# later it can be reused by drivers that don't use hostapd

	# crypto defaults: WPA2 vs WPA1
	case "$enc" in
		wpa2*|WPA2*|*PSK2*|*psk2*)
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
		*tkip+aes|*TKIP+AES|*tkip+ccmp|*TKIP+CCMP) crypto="CCMP TKIP";;
		*tkip|*TKIP) crypto="TKIP";;
		*aes|*AES|*ccmp|*CCMP) crypto="CCMP";;
	esac

	# use crypto/auth settings for building the hostapd config
	case "$enc" in
		*psk*|*PSK*)
			config_get psk "$vif" key
			if [ ${#psk} -eq 64 ]; then
				append hostapd_cfg "wpa_psk=$psk" "$N"
			else
				append hostapd_cfg "wpa_passphrase=$psk" "$N"
			fi
		;;
		*wpa*|*WPA*)
		        # required fields? formats?
		        # hostapd is particular, maybe a default configuration for failures
			config_get server "$vif" server
			append hostapd_cfg "auth_server_addr=$server" "$N"
			config_get port "$vif" port
			port=${port:-1812}
			append hostapd_cfg "auth_server_port=$port" "$N"
			config_get secret "$vif" key
			append hostapd_cfg "auth_server_shared_secret=$secret" "$N"
			config_get nasid "$vif" nasid
			append hostapd_cfg "nas_identifier=$nasid" "$N"
			append hostapd_cfg "eapol_key_index_workaround=1" "$N"
			append hostapd_cfg "radius_acct_interim_interval=300" "$N"
			append hostapd_cfg "ieee8021x=1" "$N"
			append hostapd_cfg "auth_algs=1" "$N"
			append hostapd_cfg "wpa_key_mgmt=WPA-EAP" "$N"
			append hostapd_cfg "wpa_group_rekey=300" "$N"
			append hostapd_cfg "wpa_gmk_rekey=640" "$N"
		;;
		*)
			wpa=0
			crypto=
		;;
	esac
	config_get ifname "$vif" ifname
	config_get bridge "$vif" bridge
	config_get ssid "$vif" ssid
	config_get device "$vif" device
	config_get channel "$device" channel
	config_get hwmode "$device" hwmode
	config_get country "$device" country
	case "$hwmode" in
		11a) hwmode=a;;
		11b) hwmode=b;;
		11g) hwmode=g;;
		11n)
			hwmode=g
			append hostapd_cfg "ieee80211n=1" "$N"
			config_get ht_capab "$device" ht_capab
		;;
		*)
			hwmode=
			[ "$channel" -gt 14 ] && hwmode=a
		;;
	esac
	cat > /var/run/hostapd-$ifname.conf <<EOF
ctrl_interface=/var/run/hostapd-$ifname
driver=$driver
interface=$ifname
hw_mode=${hwmode:-g}
channel=$channel
${bridge:+bridge=$bridge}
ssid=$ssid
debug=0
wpa=$wpa
${crypto:+wpa_pairwise=$crypto}
${country:+country_code=$country}
${ht_capab:+ht_capab=$ht_capab}
$hostapd_cfg
EOF
	hostapd -P /var/run/wifi-$ifname.pid -B /var/run/hostapd-$ifname.conf
}

