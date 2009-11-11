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
	config_get wpa_group_rekey "$vif" wpa_group_rekey
	config_get ieee80211d "$vif" ieee80211d
	config_get_bool wds "$vif" wds 0
	[ "$wds" -gt 0 -a "$driver" = "nl80211" ] && wds="wds_sta=1" || wds=""
	case "$hwmode" in
		bg) hwmode=g;;
	esac
	config_get country "$device" country
	[ "$channel" = auto ] && channel=
	[ -n "$channel" -a -z "$hwmode" ] && wifi_fixup_hwmode "$device"
	[ -n "$hwmode" ] && {
		config_get hwmode_11n "$device" hwmode_11n
		[ -n "$hwmode_11n" ] && {
			hwmode="$hwmode_11n"
			config_get ht_capab "$device" ht_capab
			[ -n "$ht_capab" -a -n "${ht_capab%%\[*}" ] && {
				ht_capab=`echo "[$ht_capab]" | sed -e 's, ,][,g'`
			}
		}
	}
	cat > /var/run/hostapd-$ifname.conf <<EOF
ctrl_interface=/var/run/hostapd-$ifname
driver=$driver
interface=$ifname
${hwmode:+hw_mode=$hwmode}
${channel:+channel=$channel}
${bridge:+bridge=$bridge}
ssid=$ssid
wpa=$wpa
${crypto:+wpa_pairwise=$crypto}
${country:+country_code=$country}
${hwmode_11n:+ieee80211n=1}
${ht_capab:+ht_capab=$ht_capab}
${wpa_group_rekey:+wpa_group_rekey=$wpa_group_rekey}
${ieee80211d:+ieee80211d=$ieee80211d}
$wds
$hostapd_cfg
EOF
	case "$driver" in
		madwifi)
		;;
		*) 
			cat >> /var/run/hostapd-$ifname.conf <<EOF
wme_enabled=1
wme_ac_bk_cwmin=4
wme_ac_bk_cwmax=10
wme_ac_bk_aifs=7
wme_ac_bk_txop_limit=0
wme_ac_bk_acm=0
wme_ac_be_aifs=3
wme_ac_be_cwmin=4
wme_ac_be_cwmax=10
wme_ac_be_txop_limit=0
wme_ac_be_acm=0
wme_ac_vi_aifs=2
wme_ac_vi_cwmin=3
wme_ac_vi_cwmax=4
wme_ac_vi_txop_limit=94
wme_ac_vi_acm=0
wme_ac_vo_aifs=2
wme_ac_vo_cwmin=2
wme_ac_vo_cwmax=3
wme_ac_vo_txop_limit=47
wme_ac_vo_acm=0
tx_queue_data3_aifs=7
tx_queue_data3_cwmin=15
tx_queue_data3_cwmax=1023
tx_queue_data3_burst=0
tx_queue_data2_aifs=3
tx_queue_data2_cwmin=15
tx_queue_data2_cwmax=63
tx_queue_data2_burst=0
tx_queue_data1_aifs=1
tx_queue_data1_cwmin=7
tx_queue_data1_cwmax=15
tx_queue_data1_burst=3.0
tx_queue_data0_aifs=1
tx_queue_data0_cwmin=3
tx_queue_data0_cwmax=7
tx_queue_data0_burst=1.5
EOF
		;;
	esac
	hostapd -P /var/run/wifi-$ifname.pid -B /var/run/hostapd-$ifname.conf
}

