wpa_supplicant_setup_vif() {
	local vif="$1"
	local driver="$2"
	
	# wpa_supplicant should use wext for mac80211 cards
	[ "$driver" = "mac80211" ] && driver='wext'

	case "$enc" in
		PSK|psk|psk2|PSK2)
			key_mgmt='WPA-PSK'
			config_get_bool usepassphrase "$vif" passphrase 1
			case "$enc" in
				psk|PSK)
					proto='WPA'
					if [ "$usepassphrase" = "1" ]; then
						passphrase="psk=\"${key}\""
					else
						passphrase="psk=${key}"
					fi
				;;
				psk2|PSK2)
					proto='RSN'
					if [ "$usepassphrase" = "1" ]; then
						passphrase="psk=\"${key}\""
					else
						passphrase="psk=${key}"
					fi
				;;
			esac
		;;
		WPA|wpa|WPA2|wpa2|8021x|8021X)
			proto='WPA2'
			key_mgmt='WPA-EAP'
			config_get ca_cert "$vif" ca_cert
			ca_cert=${ca_cert:+"ca_cert=\"$ca_cert\""}
			case "$eap_type" in
				tls|TLS)
					pairwise='pairwise=CCMP'
					group='group=CCMP'
					config_get priv_key "$vif" priv_key
					config_get priv_key_pwd "$vif" priv_key_pwd
					priv_key="private_key=\"$priv_key\""
					priv_key_pwd="private_key_passwd=\"$priv_key_pwd\""
				;;
				peap|PEAP|ttls|TTLS)
					config_get auth "$vif" auth
					config_get identity "$vif" identity
					config_get password "$vif" password
					phase2="phase2=\"auth=${auth:-MSCHAPV2}\""
					identity="identity=\"$identity\""
					password="password=\"$password\""
				;;
			esac
			eap_type="eap=$(echo $eap_type | tr 'a-z' 'A-Z')"
		;;
	esac
	config_get ifname "$vif" ifname
	config_get bridge "$vif" bridge
	config_get ssid "$vif" ssid
	config_get bssid "$vif" bssid
	bssid=${bssid:+"bssid=$bssid"}
	cat > /var/run/wpa_supplicant-$ifname.conf <<EOF
ctrl_interface=/var/run/wpa_supplicant-$ifname
network={
	scan_ssid=1
	ssid="$ssid"
	$bssid
	key_mgmt=$key_mgmt
	proto=$proto
	$passphrase
	$pairwise
	$group
	$eap_type
	$ca_cert
	$priv_key
	$priv_key_pwd
	$phase2
	$identity
	$password
}
EOF
	[ -z "$proto" ] || wpa_supplicant ${bridge:+ -b $bridge} -B -P "/var/run/wifi-${ifname}.pid" -D ${driver:-wext} -i "$ifname" -c /var/run/wpa_supplicant-$ifname.conf
}
