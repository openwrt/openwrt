wpa_supplicant_setup_vif() {
	local vif="$1"
	local driver="$2"
	local key="$key"
	local options="$3"
	local freq=""
	local ht="$5"
	local ap_scan=""
	local scan_ssid="1"
	[ -n "$4" ] && freq="frequency=$4"

	config_get enc "$vif" encryption
	config_get key "$vif" key

	local net_cfg bridge
	config_get bridge "$vif" bridge
	[ -z "$bridge" ] && {
		net_cfg="$(find_net_config "$vif")"
		[ -z "$net_cfg" ] || bridge="$(bridge_interface "$net_cfg")"
		config_set "$vif" bridge "$bridge"
	}

	local mode ifname wds modestr=""
	config_get mode "$vif" mode
	config_get ifname "$vif" ifname
	config_get_bool wds "$vif" wds 0
	[ -z "$bridge" ] || [ "$mode" = ap ] || [ "$mode" = sta -a $wds -eq 1 ] || {
		echo "wpa_supplicant_setup_vif($ifname): Refusing to bridge $mode mode interface"
		return 1
	}
	[ "$mode" = "adhoc" ] && {
		modestr="mode=1"
		scan_ssid="0"
		ap_scan="ap_scan=2"
	}

	key_mgmt='NONE'
	case "$enc" in
		*none*) ;;
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
							append "wep_key${zidx}" "wep_key${zidx}=$(prepare_key_wep "$ckey")"
					done
					wep_tx_keyidx="wep_tx_keyidx=$((key - 1))"
				;;
				*)
					wep_key0="wep_key0=$(prepare_key_wep "$key")"
					wep_tx_keyidx="wep_tx_keyidx=0"
				;;
			esac
		;;
		*psk*)
			key_mgmt='WPA-PSK'
			# if you want to use PSK with a non-nl80211 driver you
			# have to use WPA-NONE and wext driver for wpa_s
			[ "$mode" = "adhoc" -a "$driver" != "nl80211" ] && {
				key_mgmt='WPA-NONE'
				driver='wext'
			}
			if [ ${#key} -eq 64 ]; then
				passphrase="psk=${key}"
			else
				passphrase="psk=\"${key}\""
			fi
			case "$enc" in
				*psk2*)
					proto='proto=RSN'
					config_get ieee80211w "$vif" ieee80211w
				;;
				*psk*)
					proto='proto=WPA'
				;;
			esac
		;;
		*wpa*|*8021x*)
			proto='proto=WPA2'
			key_mgmt='WPA-EAP'
			config_get ieee80211w "$vif" ieee80211w
			config_get ca_cert "$vif" ca_cert
			config_get eap_type "$vif" eap_type
			ca_cert=${ca_cert:+"ca_cert=\"$ca_cert\""}
			case "$eap_type" in
				tls)
					pairwise='pairwise=CCMP'
					group='group=CCMP'
					config_get identity "$vif" identity
					config_get client_cert "$vif" client_cert
					config_get priv_key "$vif" priv_key
					config_get priv_key_pwd "$vif" priv_key_pwd
					identity="identity=\"$identity\""
					client_cert="client_cert=\"$client_cert\""
					priv_key="private_key=\"$priv_key\""
					priv_key_pwd="private_key_passwd=\"$priv_key_pwd\""
				;;
				peap|ttls)
					config_get auth "$vif" auth
					config_get identity "$vif" identity
					config_get password "$vif" password
					phase2="phase2=\"auth=${auth:-MSCHAPV2}\""
					identity="identity=\"$identity\""
					password="${password:+password=\"$password\"}"
				;;
			esac
			eap_type="eap=$(echo $eap_type | tr 'a-z' 'A-Z')"
		;;
	esac

	case "$ieee80211w" in
		[012])
			ieee80211w="ieee80211w=$ieee80211w"
		;;
	esac

	local fixed_freq bssid1 beacon_int brates mrate
	config_get ifname "$vif" ifname
	config_get bridge "$vif" bridge
	config_get ssid "$vif" ssid
	config_get bssid "$vif" bssid
	bssid1=${bssid:+"bssid=$bssid"}
	beacon_int=${beacon_int:+"beacon_int=$beacon_int"}

	local br brval brsub brstr
	[ -n "$basic_rate_list" ] && {
		for br in $basic_rate_list; do
			brval="$(($br / 1000))"
			brsub="$((($br / 100) % 10))"
			[ "$brsub" -gt 0 ] && brval="$brval.$brsub"
			[ -n "$brstr" ] && brstr="$brstr,"
			brstr="$brstr$brval"
		done
		brates=${basic_rate_list:+"rates=$brstr"}
	}

	local mcval=""
	[ -n "$mcast_rate" ] && {
		mcval="$(($mcast_rate / 1000))"
		mcsub="$(( ($mcast_rate / 100) % 10 ))"
		[ "$mcsub" -gt 0 ] && mcval="$mcval.$mcsub"
		mrate=${mcast_rate:+"mcast_rate=$mcval"}
	}

	local ht_str
	[ -n "$ht" ] && ht_str="htmode=$ht"

	rm -rf /var/run/wpa_supplicant-$ifname
	cat > /var/run/wpa_supplicant-$ifname.conf <<EOF
ctrl_interface=/var/run/wpa_supplicant-$ifname
$ap_scan
network={
	$modestr
	scan_ssid=$scan_ssid
	ssid="$ssid"
	$bssid1
	key_mgmt=$key_mgmt
	$proto
	$freq
	${fixed:+"fixed_freq=1"}
	$beacon_int
	$brates
	$mrate
	$ht_str
	$ieee80211w
	$passphrase
	$pairwise
	$group
	$eap_type
	$ca_cert
	$client_cert
	$priv_key
	$priv_key_pwd
	$phase2
	$identity
	$password
	$wep_key0
	$wep_key1
	$wep_key2
	$wep_key3
	$wep_tx_keyidx
}
EOF
	if [ -n "$proto" -o "$key_mgmt" = "NONE" ]; then
		wpa_supplicant ${bridge:+ -b $bridge} -B -P "/var/run/wifi-${ifname}.pid" -D ${driver:-wext} -i "$ifname" -c /var/run/wpa_supplicant-$ifname.conf $options
	else
		return 0
	fi
}
