hostapd_add_rate() {
	local var="$1"
	local val="$(($2 / 1000))"
	local sub="$((($2 / 100) % 10))"
	append $var "$val" ","
	[ $sub -gt 0 ] && append $var "."
}

hostapd_add_basic_rate() {
	local var="$1"
	local val="$(($2 / 100))"
	append $var "$val" " "
}

hostapd_append_wep_key() {
	local var="$1"

	wep_keyidx=0
	set_default key 1
	case "$key" in
		[1234])
			for idx in 1 2 3 4; do
				local zidx
				zidx=$(($idx - 1))
				json_get_var ckey "key${idx}"
				[ -n "$ckey" ] && \
					append $var "wep_key${zidx}=$(prepare_key_wep "$ckey")" "$N$T"
			done
			wep_keyidx=$((key - 1))
		;;
		*)
			append $var "wep_key0=$(prepare_key_wep "$key")" "$N$T"
		;;
	esac
}

hostapd_add_log_config() {
	config_add_boolean \
		log_80211 \
		log_8021x \
		log_radius \
		log_wpa \
		log_driver \
		log_iapp \
		log_mlme

	config_add_int log_level
}

hostapd_common_add_device_config() {
	config_add_array basic_rate

	config_add_string country
	config_add_boolean country_ie

	hostapd_add_log_config
}

hostapd_prepare_device_config() {
	local config="$1"
	local driver="$2"

	local base="${config%%.conf}"
	local base_cfg=

	json_get_vars country country_ie beacon_int

	hostapd_set_log_options base_cfg

	set_default country_ie 1
	[ -n "$country" ] && {
		append base_cfg "country_code=$country" "$N"
		[ "$country_ie" -gt 0 ] && append base_cfg "ieee80211d=1" "$N"
	}
	[ -n "$hwmode" ] && append base_cfg "hw_mode=$hwmode" "$N"

	local brlist= br
	json_get_values basic_rate_list basic_rate
	for br in $basic_rate_list; do
		hostapd_add_basic_rate brlist "$br"
	done
	[ -n "$brlist" ] && append base_cfg "basic_rates=$brlist" "$N"
	[ -n "$beacon_int" ] && append base_cfg "beacon_int=$beacon_int" "$N"

	cat > "$config" <<EOF
driver=$driver
$base_cfg
EOF
}

hostapd_common_add_bss_config() {
	config_add_string bssid ssid
	config_add_boolean wds wmm hidden

	config_add_int maxassoc max_inactivity
	config_add_boolean disassoc_low_ack isolate short_preamble

	config_add_int \
		wep_rekey eap_reauth_period \
		wpa_group_rekey wpa_pair_rekey wpa_master_rekey

	config_add_boolean rsn_preauth auth_cache
	config_add_int ieee80211w

	config_add_string auth_server server
	config_add_string auth_secret
	config_add_int auth_port port

	config_add_string acct_server
	config_add_string acct_secret
	config_add_int acct_port

	config_add_string dae_client
	config_add_string dae_secret
	config_add_int dae_port

	config_add_string nasid
	config_add_string iapp_interface
	config_add_string eap_type ca_cert client_cert identity auth priv_key priv_key_pwd

	config_add_string key1 key2 key3 key4 password

	config_add_boolean wps_pushbutton wps_label ext_registrar
	config_add_string wps_device_type wps_device_name wps_manufacturer wps_pin

	config_add_int ieee80211w_max_timeout ieee80211w_retry_timeout

	config_add_string macfilter macfile
	config_add_array maclist

	config_add_int mcast_rate
	config_add_array basic_rate
}

hostapd_set_bss_options() {
	local var="$1"
	local phy="$2"
	local vif="$3"

	wireless_vif_parse_encryption

	local bss_conf
	local wep_rekey wpa_group_rekey wpa_pair_rekey wpa_master_rekey

	json_get_vars \
		wep_rekey wpa_group_rekey wpa_pair_rekey wpa_master_rekey \
		maxassoc max_inactivity disassoc_low_ack isolate auth_cache \
		wps_pushbutton wps_label ext_registrar \
		wps_device_type wps_device_name wps_manufacturer wps_pin \
		macfilter ssid wmm hidden short_preamble

	set_default isolate 0
	set_default maxassoc 0
	set_default max_inactivity 0
	set_default short_preamble 1
	set_default disassoc_low_ack 1
	set_default hidden 0
	set_default wmm 1

	append bss_conf "ctrl_interface=/var/run/hostapd"
	if [ "$isolate" -gt 0 ]; then
		append bss_conf "ap_isolate=$isolate" "$N"
	fi
	if [ "$maxassoc" -gt 0 ]; then
		append bss_conf "max_num_sta=$maxassoc" "$N"
	fi
	if [ "$max_inactivity" -gt 0 ]; then
		append bss_conf "ap_max_inactivity=$max_inactivity" "$N"
	fi

	append bss_conf "disassoc_low_ack=$disassoc_low_ack" "$N"
	append bss_conf "preamble=$short_preamble" "$N"
	append bss_conf "wmm_enabled=$wmm" "$N"
	append bss_conf "ignore_broadcast_ssid=$hidden" "$N"

	[ "$wpa" -gt 0 ] && {
		[ -n "$wpa_group_rekey"  ] && append bss_conf "wpa_group_rekey=$wpa_group_rekey" "$N"
		[ -n "$wpa_pair_rekey"   ] && append bss_conf "wpa_ptk_rekey=$wpa_pair_rekey"    "$N"
		[ -n "$wpa_master_rekey" ] && append bss_conf "wpa_gmk_rekey=$wpa_master_rekey"  "$N"
	}

	case "$auth_type" in
		none)
			wps_possible=1
			# Here we make the assumption that if we're in open mode
			# with WPS enabled, we got to be in unconfigured state.
			wps_not_configured=1
		;;
		psk)
			json_get_vars key
			if [ ${#key} -lt 8 ]; then
				wireless_setup_vif_failed INVALID_WPA_PSK
				return 1
			elif [ ${#key} -eq 64 ]; then
				append bss_conf "wpa_psk=$key" "$N"
			else
				append bss_conf "wpa_passphrase=$key" "$N"
			fi
			wps_possible=1
		;;
		eap)
			json_get_vars \
				auth_server auth_secret auth_port \
				acct_server acct_secret acct_port \
				dae_client dae_secret dae_port \
				nasid rsn_preauth iapp_interface \
				eap_reauth_period

			# legacy compatibility
			[ -n "$auth_server" ] || json_get_var auth_server server
			[ -n "$auth_port" ] || json_get_var auth_port port
			[ -n "$auth_secret" ] || json_get_var auth_secret key

			set_default auth_port 1812
			set_default acct_port 1813
			set_default dae_port 3799

			append bss_conf "auth_server_addr=$auth_server" "$N"
			append bss_conf "auth_server_port=$auth_port" "$N"
			append bss_conf "auth_server_shared_secret=$auth_secret" "$N"

			[ -n "$acct_server" ] && {
				append bss_conf "acct_server_addr=$acct_server" "$N"
				append bss_conf "acct_server_port=$acct_port" "$N"
				[ -n "$acct_secret" ] && \
					append bss_conf "acct_server_shared_secret=$acct_secret" "$N"
			}

			[ -n "$eap_reauth_period" ] && append bss_conf "eap_reauth_period=$eap_reauth_period" "$N"

			[ -n "$dae_client" -a -n "$dae_secret" ] && {
				append bss_conf "radius_das_port=$dae_port" "$N"
				append bss_conf "radius_das_client=$dae_client $dae_secret" "$N"
			}

			append bss_conf "nas_identifier=$nasid" "$N"
			append bss_conf "eapol_key_index_workaround=1" "$N"
			append bss_conf "ieee8021x=1" "$N"
			append bss_conf "wpa_key_mgmt=WPA-EAP" "$N"
		;;
		wep)
			local wep_keyidx=0
			json_get_vars key
			hostapd_append_wep_key bss_conf
			append bss_conf "wep_default_key=$wep_keyidx" "$N"
			[ -n "$wep_rekey" ] && append bss_conf "wep_rekey_period=$wep_rekey" "$N"
		;;
	esac

	local auth_algs=$((($auth_mode_shared << 1) | $auth_mode_open))
	append bss_conf "auth_algs=${auth_algs:-1}" "$N"
	append bss_conf "wpa=$wpa" "$N"
	[ -n "$wpa_pairwise" ] && append bss_conf "wpa_pairwise=$wpa_pairwise" "$N"

	set_default wps_pushbutton 0
	set_default wps_label 0

	config_methods=
	[ "$wps_pushbutton" -gt 0 ] && append config_methods push_button
	[ "$wps_label" -gt 0 ] && append config_methods label

	[ -n "$wps_possible" -a -n "$config_methods" ] && {
		set_default ext_registrar 0
		set_default wps_device_type "6-0050F204-1"
		set_default wps_device_name "OpenWrt AP"
		set_default wps_manufacturer "openwrt.org"
		set_default wps_pin "12345670"

		wps_state=2
		[ -n "$wps_configured" ] && wps_state=1

		[ "$ext_registrar" -gt 0 -a -n "$bridge" ] && append bss_conf "upnp_iface=$bridge" "$N"

		append bss_conf "eap_server=1" "$N"
		append bss_conf "ap_pin=$wps_pin" "$N"
		append bss_conf "wps_state=$wps_state" "$N"
		append bss_conf "ap_setup_locked=0" "$N"
		append bss_conf "device_type=$wps_device_type" "$N"
		append bss_conf "device_name=$wps_device_name" "$N"
		append bss_conf "manufacturer=$wps_manufacturer" "$N"
		append bss_conf "config_methods=$config_methods" "$N"
	}

	append bss_conf "ssid=$ssid" "$N"
	[ -n "$network_bridge" ] && append bss_conf "bridge=$network_bridge" "$N"
	[ -n "$iapp_interface" ] &&  {
		iapp_interface="$(uci_get_state network "$iapp_interface" ifname "$iapp_interface")"
		[ -n "$iapp_interface" ] && append bss_conf "iapp_interface=$iapp_interface" "$N"
	}

	if [ "$wpa" -ge "2" ]; then
		if [ -n "$bridge" -a "$rsn_preauth" = 1 ]; then
			set_default auth_cache 1
			append bss_conf "rsn_preauth=1" "$N"
			append bss_conf "rsn_preauth_interfaces=$bridge" "$N"
		else
			set_default auth_cache 0
		fi

		append bss_conf "okc=$auth_cache" "$N"
		[ "$auth_cache" = 0 ] && append bss_conf "disable_pmksa_caching=1" "$N"

		# RSN -> allow management frame protection
		json_get_var ieee80211w ieee80211w
		case "$ieee80211w" in
			[012])
				json_get_vars ieee80211w_max_timeout ieee80211w_retry_timeout
				append bss_conf "ieee80211w=$ieee80211w" "$N"
				[ "$ieee80211w" -gt "0" ] && {
					[ -n "$ieee80211w_max_timeout" ] && \
						append bss_conf "assoc_sa_query_max_timeout=$ieee80211w_max_timeout" "$N"
					[ -n "$ieee80211w_retry_timeout" ] && \
						append bss_conf "assoc_sa_query_retry_timeout=$ieee80211w_retry_timeout" "$N"
				}
			;;
		esac
	fi

	_macfile="/var/run/hostapd-$ifname.maclist"
	case "$macfilter" in
		allow)
			append bss_conf "macaddr_acl=1" "$N"
			append bss_conf "accept_mac_file=$_macfile" "$N"
		;;
		deny)
			append bss_conf "macaddr_acl=0" "$N"
			append bss_conf "deny_mac_file=$_macfile" "$N"
		;;
		*)
			_macfile=""
		;;
	esac

	[ -n "$_macfile" ] && {
		json_get_vars macfile
		json_get_values maclist maclist

		rm -f "$_macfile"
		(
			for mac in $maclist; do
				echo "$mac"
			done
			[ -n "$macfile" -a -f "$macfile" ] && cat "$macfile"
		) > "$_macfile"
	}

	append "$var" "$bss_conf" "$N"
	return 0
}

hostapd_set_log_options() {
	local var="$1"

	local log_level log_80211 log_8021x log_radius log_wpa log_driver log_iapp log_mlme
	json_get_vars log_level log_80211 log_8021x log_radius log_wpa log_driver log_iapp log_mlme

	set_default log_level 2
	set_default log_80211  1
	set_default log_8021x  1
	set_default log_radius 1
	set_default log_wpa    1
	set_default log_driver 1
	set_default log_iapp   1
	set_default log_mlme   1

	local log_mask=$((       \
		($log_80211  << 0) | \
		($log_8021x  << 1) | \
		($log_radius << 2) | \
		($log_wpa    << 3) | \
		($log_driver << 4) | \
		($log_iapp   << 5) | \
		($log_mlme   << 6)   \
	))

	append "$var" "logger_syslog=$log_mask" "$N"
	append "$var" "logger_syslog_level=$log_level" "$N"
	append "$var" "logger_stdout=$log_mask" "$N"
	append "$var" "logger_stdout_level=$log_level" "$N"

	return 0
}

_wpa_supplicant_common() {
	local ifname="$1"

	_rpath="/var/run/wpa_supplicant"
	_config="${_rpath}-$ifname.conf"
}

wpa_supplicant_teardown_interface() {
	_wpa_supplicant_common "$1"
	rm -rf "$_rpath" "$_config"
}

wpa_supplicant_prepare_interface() {
	local ifname="$1"
	_w_driver="$2"

	_wpa_supplicant_common "$1"

	json_get_vars mode wds

	[ -n "$network_bridge" ] && {
		fail=
		case "$mode" in
			adhoc)
				fail=1
			;;
			sta)
				[ "$wds" = 1 ] || fail=1
			;;
		esac

		[ -n "$fail" ] && {
			wireless_setup_vif_failed BRIDGE_NOT_ALLOWED
			return 1
		}
	}

	local ap_scan=

	_w_mode="$mode"
	_w_modestr=

	[[ "$mode" = adhoc ]] && {
		ap_scan="ap_scan=2"

		_w_modestr="mode=1"
	}

	wpa_supplicant_teardown_interface "$ifname"
	cat > "$_config" <<EOF
$ap_scan
EOF
	return 0
}

wpa_supplicant_add_network() {
	local ifname="$1"

	_wpa_supplicant_common "$1"
	wireless_vif_parse_encryption

	json_get_vars \
		ssid bssid key \
		basic_rate mcast_rate \
		ieee80211w

	local key_mgmt='NONE'
	local enc_str=
	local network_data=
	local T="	"

	local wpa_key_mgmt="WPA-PSK"
	local scan_ssid="1"
	local freq

	[[ "$_w_mode" = "adhoc" ]] && {
		append network_data "mode=1" "$N$T"
		[ -n "$channel" ] && {
			freq="$(get_freq "$phy" "$channel")"
			append network_data "fixed_freq=1" "$N$T"
			append network_data "frequency=$freq" "$N$T"
		}

		scan_ssid=0

		[ "$_w_driver" = "nl80211" ] ||	wpa_key_mgmt="WPA-NONE"
	}

	[[ "$_w_mode" = adhoc ]] && append network_data "$_w_modestr" "$N$T"

	case "$auth_type" in
		none) ;;
		wep)
			local wep_keyidx=0
			hostapd_append_wep_key network_data
			append network_data "wep_tx_keyidx=$wep_keyidx" "$N$T"
		;;
		psk)
			local passphrase

			key_mgmt="$wpa_key_mgmt"
			if [ ${#key} -eq 64 ]; then
				passphrase="psk=${key}"
			else
				passphrase="psk=\"${key}\""
			fi
			append network_data "$passphrase" "$N$T"
		;;
		eap)
			key_mgmt='WPA-EAP'

			json_get_vars eap_type identity ca_cert
			[ -n "$ca_cert" ] && append network_data "ca_cert=\"$ca_cert\"" "$N$T"
			[ -n "$identity" ] && append network_data "identity=\"$identity\"" "$N$T"
			case "$eap_type" in
				tls)
					json_get_vars client_cert priv_key priv_key_pwd
					append network_data "client_cert=\"$client_cert\"" "$N$T"
					append network_data "private_key=\"$priv_key\"" "$N$T"
					append network_data "private_key_passwd=\"$priv_key_pwd\"" "$N$T"
				;;
				peap|ttls)
					json_get_vars auth password
					set_default auth MSCHAPV2
					append network_data "phase2=\"$auth\"" "$N$T"
					append network_data "password=\"$password\"" "$N$T"
				;;
			esac
			append network_data "eap=$(echo $eap_type | tr 'a-z' 'A-Z')" "$N$T"
		;;
	esac

	case "$wpa" in
		1)
			append network_data "proto=WPA" "$N$T"
		;;
		2)
			append network_data "proto=RSN" "$N$T"
		;;
	esac

	case "$ieee80211w" in
		[012])
			[ "$wpa" -ge 2 ] && append network_data "ieee80211w=$ieee80211w" "$N$T"
		;;
	esac

	local beacon_int brates mrate
	[ -n "$bssid" ] && append network_data "bssid=$bssid" "$N$T"
	[ -n "$beacon_int" ] && append network_data "beacon_int=$beacon_int" "$N$T"


	[ -n "$basic_rate" ] && {
		local br rate_list=
		for br in $basic_rate; do
			hostapd_add_rate rate_list "$br"
		done
		[ -n "$rate_list" ] && append network_data "rates=$rate_list" "$N$T"
	}

	[ -n "$mcast_rate" ] && {
		local mc_rate=
		hostapd_add_rate mc_rate "$mcast_rate"
		append network_data "mcast_rate=$mc_rate" "$N$T"
	}

	local ht_str
	[ -n "$ht" ] && append network_data "htmode=$ht" "$N$T"

	cat >> "$_config" <<EOF
network={
	scan_ssid=$scan_ssid
	ssid="$ssid"
	key_mgmt=$key_mgmt
	$network_data
}
EOF
	return 0
}

wpa_supplicant_run() {
	local ifname="$1"; shift

	_wpa_supplicant_common "$ifname"

	/usr/sbin/wpa_supplicant -B \
		${network_bridge:+-b $network_bridge} \
		-P "/var/run/wpa_supplicant-${ifname}.pid" \
		-D ${_w_driver:-wext} \
		-i "$ifname" \
		-c "$_config" \
		-C "$_rpath" \
		"$@"

	ret="$?"
	wireless_add_process "$(cat "/var/run/wpa_supplicant-${ifname}.pid")" /usr/sbin/wpa_supplicant 1

	[ "$ret" != 0 ] && wireless_setup_vif_failed WPA_SUPPLICANT_FAILED

	return $ret
}

hostapd_common_cleanup() {
	killall hostapd wpa_supplicant
}
