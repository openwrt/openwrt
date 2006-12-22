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
			append hostapd_cfg "wpa_passphrase=$psk" "$N"
		;;
		*wpa*|*WPA*)
		# FIXME: add wpa+radius here
		;;
		*)
			return 0;
		;;
	esac
	config_get ifname "$vif" ifname
	config_get bridge "$vif" bridge
	config_get ssid "$vif" ssid
	cat > /var/run/hostapd-$ifname.conf <<EOF
driver=$driver
interface=$ifname
${bridge:+bridge=$bridge}
ssid=$ssid
debug=0
wpa=$wpa
wpa_pairwise=$crypto
$hostapd_cfg
EOF
	hostapd -B /var/run/hostapd-$ifname.conf
}

