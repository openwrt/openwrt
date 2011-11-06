#!/bin/sh
append DRIVERS "atheros"

find_atheros_phy() {
	local device="$1"

	local macaddr="$(config_get "$device" macaddr | tr 'A-Z' 'a-z')"
	config_get phy "$device" phy
	[ -z "$phy" -a -n "$macaddr" ] && {
		cd /proc/sys/dev
		for phy in $(ls -d wifi* 2>&-); do
			[ "$macaddr" = "$(cat /sys/class/net/${phy}/address)" ] || continue
			config_set "$device" phy "$phy"
			break
		done
		config_get phy "$device" phy
	}
	[ -n "$phy" -a -d "/proc/sys/dev/$phy" ] || {
		echo "phy for wifi device $1 not found"
		return 1
	}
	[ -z "$macaddr" ] && {
		config_set "$device" macaddr "$(cat /sys/class/net/${phy}/address)"
	}
	return 0
}

scan_atheros() {
	local device="$1"
	local wds
	local adhoc ahdemo sta ap monitor disabled

	[ ${device%[0-9]} = "wifi" ] && config_set "$device" phy "$device"

	local ifidx=0
	
	config_get vifs "$device" vifs
	for vif in $vifs; do
		config_get_bool disabled "$vif" disabled 0
		[ $disabled = 0 ] || continue

		local vifname
		[ $ifidx -gt 0 ] && vifname="ath${device#radio}-$ifidx" || vifname="ath${device#radio}"

		config_get ifname "$vif" ifname
		config_set "$vif" ifname "${ifname:-$vifname}"
		
		config_get mode "$vif" mode
		case "$mode" in
			adhoc|ahdemo|sta|ap|monitor)
				append $mode "$vif"
			;;
			wds)
				config_get ssid "$vif" ssid
				[ -z "$ssid" ] && continue

				config_set "$vif" wds 1
				config_set "$vif" mode sta
				mode="sta"
				addr="$ssid"
				${addr:+append $mode "$vif"}
			;;
			*) echo "$device($vif): Invalid mode, ignored."; continue;;
		esac

		ifidx=$(($ifidx + 1))
	done

	case "${adhoc:+1}:${sta:+1}:${ap:+1}" in
		# valid mode combinations
		1::) wds="";;
		1::1);;
		:1:1)config_set "$device" nosbeacon 1;; # AP+STA, can't use beacon timers for STA
		:1:);;
		::1);;
		::);;
		*) echo "$device: Invalid mode combination in config"; return 1;;
	esac

	config_set "$device" vifs "${sta:+$sta }${ap:+$ap }${adhoc:+$adhoc }${ahdemo:+$ahdemo }${wds:+$wds }${monitor:+$monitor}"
}


disable_atheros() (
	local device="$1"

	find_atheros_phy "$device" || return 0
	config_get phy "$device" phy

	set_wifi_down "$device"

	include /lib/network
	cd /proc/sys/net
	for dev in *; do
		grep "$phy" "$dev/%parent" >/dev/null 2>/dev/null && {
			[ -f "/var/run/wifi-${dev}.pid" ] &&
				kill "$(cat "/var/run/wifi-${dev}.pid")"
			ifconfig "$dev" down
			unbridge "$dev"
			wlanconfig "$dev" destroy
		}
	done
	return 0
)

enable_atheros() {
	local device="$1"

	find_atheros_phy "$device" || return 0
	config_get phy "$device" phy

	config_get regdomain "$device" regdomain
	[ -n "$regdomain" ] && echo "$regdomain" > /proc/sys/dev/$phy/regdomain

	config_get country "$device" country
	case "$country" in
		[A-Za-z]*) country=`grep -i "$country" /lib/wifi/madwifi_countrycodes.txt |cut -d " " -f 2`;;
		[0-9]*) ;;
		*) country="" ;;
	esac
	[ -n "$country" ] && echo "$country" > /proc/sys/dev/$phy/countrycode

	config_get_bool outdoor "$device" outdoor "0"
	echo "$outdoor" > /proc/sys/dev/$phy/outdoor

	config_get channel "$device" channel
	config_get vifs "$device" vifs
	config_get txpower "$device" txpower

	[ auto = "$channel" ] && channel=0

	config_get_bool antdiv "$device" diversity
	config_get antrx "$device" rxantenna
	config_get anttx "$device" txantenna
	config_get_bool softled "$device" softled
	config_get antenna "$device" antenna

	devname="$(cat /proc/sys/dev/$phy/dev_name)"
	local antgpio=
	local invert=
	case "$devname" in
		NanoStation2) antgpio=7; invert=1;;
		NanoStation5) antgpio=1; invert=1;;
		"NanoStation Loco2") antgpio=2;;
		"NanoStation Loco5")
			case "$antenna" in
				horizontal) antdiv=0; anttx=1; antrx=1;;
				vertical) antdiv=0; anttx=2; antrx=2;;
				*) antdiv=1; anttx=0; antrx=0;;
			esac
		;;
	esac
	if [ -n "$invert" ]; then
		_set="clear"
		_clear="set"
	else
		_set="set"
		_clear="clear"
	fi
	if [ -n "$antgpio" ]; then
		softled=0
		case "$devname" in
			"NanoStation Loco2")
				antdiv=0
				antrx=1
				anttx=1
				case "$antenna" in
					horizontal) gpioval=0;;
					*) gpioval=1;;
				esac
			;;
			*)
				case "$antenna" in
					external) antdiv=0; antrx=1; anttx=1; gpioval=1;;
					horizontal) antdiv=0; antrx=1; anttx=1; gpioval=0;;
					vertical) antdiv=0; antrx=2; anttx=2; gpioval=0;;
					auto) antdiv=1; antrx=0; anttx=0; gpioval=0;;
				esac
			;;
		esac

		[ -x "$(which gpioctl 2>/dev/null)" ] || antenna=
		gpioctl "dirout" "$antgpio" >/dev/null 2>&1
		case "$gpioval" in
			0)
				gpioctl "$_clear" "$antgpio" >/dev/null 2>&1
			;;
			1)
				gpioctl "$_set" "$antgpio" >/dev/null 2>&1
			;;
		esac
	fi

	[ -n "$antdiv" ] && sysctl -w dev."$phy".diversity="$antdiv" >&-
	[ -n "$antrx" ] && sysctl -w dev."$phy".rxantenna="$antrx" >&-
	[ -n "$anttx" ] && sysctl -w dev."$phy".txantenna="$anttx" >&-
	[ -n "$softled" ] && sysctl -w dev."$phy".softled="$softled" >&-

	config_get distance "$device" distance
	[ -n "$distance" ] && sysctl -w dev."$phy".distance="$distance" >&-

	for vif in $vifs; do
		local start_hostapd= vif_txpower= nosbeacon=
		config_get ifname "$vif" ifname
		config_get enc "$vif" encryption
		config_get eap_type "$vif" eap_type
		config_get mode "$vif" mode

		case "$mode" in
			sta) config_get_bool nosbeacon "$device" nosbeacon;;
			adhoc) config_get_bool nosbeacon "$vif" sw_merge 1;;
		esac

		[ "$nosbeacon" = 1 ] || nosbeacon=""
		ifname=$(wlanconfig "$ifname" create nounit wlandev "$phy" wlanmode "$mode" ${nosbeacon:+nosbeacon})
		[ $? -ne 0 ] && {
			echo "enable_atheros($device): Failed to set up $mode vif $ifname" >&2
			continue
		}
		config_set "$vif" ifname "$ifname"

		config_get hwmode "$device" hwmode
		[ -z "$hwmode" ] && config_get hwmode "$device" mode

		pureg=0
		case "$hwmode" in
			*b) hwmode=11b;;
			*bg) hwmode=11g;;
			*g) hwmode=11g; pureg=1;;
			*gdt) hwmode=11gdt;;
			*a) hwmode=11a;;
			*adt) hwmode=11adt;;
			*ast) hwmode=11ast;;
			*fh) hwmode=fh;;
			*) hwmode=auto;;
		esac
		iwpriv "$ifname" mode "$hwmode"
		iwpriv "$ifname" pureg "$pureg"

		iwconfig "$ifname" channel "$channel" >/dev/null 2>/dev/null 

		config_get_bool hidden "$vif" hidden 0
		iwpriv "$ifname" hide_ssid "$hidden"

		config_get ff "$vif" ff
		if [ -n "$ff" ]; then
			iwpriv "$ifname" ff "$ff"
		fi

		config_get wds "$vif" wds
		case "$wds" in
			1|on|enabled) wds=1;;
			*) wds=0;;
		esac
		iwpriv "$ifname" wds "$wds" >/dev/null 2>&1

		[ "$mode" = ap -a "$wds" = 1 ] && {
			config_get_bool wdssep "$vif" wdssep 1
			[ -n "$wdssep" ] && iwpriv "$ifname" wdssep "$wdssep"
		}

		case "$enc" in
			wep*)
				case "$enc" in
					*shared*) iwpriv "$ifname" authmode 2;;
					*)        iwpriv "$ifname" authmode 1;;
				esac
				for idx in 1 2 3 4; do
					config_get key "$vif" "key${idx}"
					iwconfig "$ifname" enc "[$idx]" "${key:-off}"
				done
				config_get key "$vif" key
				key="${key:-1}"
				case "$key" in
					[1234]) iwconfig "$ifname" enc "[$key]";;
					*) iwconfig "$ifname" enc "$key";;
				esac
			;;
			psk*|wpa*)
				start_hostapd=1
				config_get key "$vif" key
			;;
		esac

		case "$mode" in
			sta|adhoc|ahdemo)
				config_get addr "$vif" bssid
				[ -z "$addr" ] || { 
					iwconfig "$ifname" ap "$addr"
				}
			;;
		esac

		config_get_bool uapsd "$vif" uapsd 0
		iwpriv "$ifname" uapsd "$uapsd"

		config_get_bool bgscan "$vif" bgscan
		[ -n "$bgscan" ] && iwpriv "$ifname" bgscan "$bgscan"

		config_get rate "$vif" rate
		[ -n "$rate" ] && iwconfig "$ifname" rate "${rate%%.*}"

		config_get mcast_rate "$vif" mcast_rate
		[ -n "$mcast_rate" ] && iwpriv "$ifname" mcast_rate "${mcast_rate%%.*}"

		config_get frag "$vif" frag
		[ -n "$frag" ] && iwconfig "$ifname" frag "${frag%%.*}"

		config_get rts "$vif" rts
		[ -n "$rts" ] && iwconfig "$ifname" rts "${rts%%.*}"

		config_get_bool comp "$vif" compression 0
		iwpriv "$ifname" compression "$comp" >/dev/null 2>&1

		config_get minrate "$vif" minrate
		[ -n "$minrate" ] && iwpriv "$ifname" minrate "$minrate"

		config_get maxrate "$vif" maxrate
		[ -n "$maxrate" ] && iwpriv "$ifname" maxrate "$maxrate"

		config_get_bool burst "$vif" bursting
		[ -n "$burst" ] && iwpriv "$ifname" burst "$burst"

		config_get_bool wmm "$vif" wmm
		[ -n "$wmm" ] && iwpriv "$ifname" wmm "$wmm"

		config_get_bool xr "$vif" xr
		[ -n "$xr" ] && iwpriv "$ifname" xr "$xr"

		config_get_bool ar "$vif" ar
		[ -n "$ar" ] && iwpriv "$ifname" ar "$ar"

		config_get_bool beacon_power "$vif" beacon_power
		[ -n "$beacon_power" ] && iwpriv "$ifname" beacon_pwr "$beacon_power"

		config_get_bool doth "$vif" doth 0
		[ -n "$doth" ] && iwpriv "$ifname" doth "$doth"

		config_get_bool probereq "$vif" probereq
		[ -n "$probereq" ] && iwpriv "$ifname" probereq "$probereq"

		config_get maclist "$vif" maclist
		[ -n "$maclist" ] && {
			# flush MAC list
			iwpriv "$ifname" maccmd 3
			for mac in $maclist; do
				iwpriv "$ifname" addmac "$mac"
			done
		}

		config_get macpolicy "$vif" macpolicy
		case "$macpolicy" in
			allow)
				iwpriv "$ifname" maccmd 1
			;;
			deny)
				iwpriv "$ifname" maccmd 2
			;;
			*)
				# default deny policy if mac list exists
				[ -n "$maclist" ] && iwpriv "$ifname" maccmd 2
			;;
		esac

		ifconfig "$ifname" up

		local net_cfg bridge
		net_cfg="$(find_net_config "$vif")"
		[ -z "$net_cfg" ] || {
			bridge="$(bridge_interface "$net_cfg")"
			config_set "$vif" bridge "$bridge"
			start_net "$ifname" "$net_cfg"
		}

		config_get ssid "$vif" ssid
		[ -n "$ssid" ] && {
			iwconfig "$ifname" essid on
			iwconfig "$ifname" essid ${ssid:+-- }"$ssid"
		}

		set_wifi_up "$vif" "$ifname"

		# TXPower settings only work if device is up already
		# while atheros hardware theoretically is capable of per-vif (even per-packet) txpower
		# adjustment it does not work with the current atheros hal/madwifi driver

		config_get vif_txpower "$vif" txpower
		# use vif_txpower (from wifi-iface) instead of txpower (from wifi-device) if
		# the latter doesn't exist
		txpower="${txpower:-$vif_txpower}"
		[ -z "$txpower" ] || iwconfig "$ifname" txpower "${txpower%%.*}"

		case "$mode" in
			ap)
				config_get_bool isolate "$vif" isolate 0
				iwpriv "$ifname" ap_bridge "$((isolate^1))"

				if [ -n "$start_hostapd" ] && eval "type hostapd_setup_vif" 2>/dev/null >/dev/null; then
					hostapd_setup_vif "$vif" madwifi || {
						echo "enable_atheros($device): Failed to set up hostapd for interface $ifname" >&2
						# make sure this wifi interface won't accidentally stay open without encryption
						ifconfig "$ifname" down
						wlanconfig "$ifname" destroy
						continue
					}
				fi
			;;
			wds|sta)
				if eval "type wpa_supplicant_setup_vif" 2>/dev/null >/dev/null; then
					wpa_supplicant_setup_vif "$vif" madwifi || {
						echo "enable_atheros($device): Failed to set up wpa_supplicant for interface $ifname" >&2
						ifconfig "$ifname" down
						wlanconfig "$ifname" destroy
						continue
					}
				fi
			;;
		esac
	done
}

check_atheros_device() {
	[ ${1%[0-9]} = "wifi" ] && config_set "$1" phy "$1"
	config_get phy "$1" phy
	[ -z "$phy" ] && {
		find_atheros_phy "$1" >/dev/null || return 0
		config_get phy "$1" phy
	}
	[ "$phy" = "$dev" ] && found=1
}


detect_atheros() {
	devidx=0
	config_load wireless
	while :; do
		config_get type "radio$devidx" type
		[ -n "$type" ] || break
		devidx=$(($devidx + 1))
	done
	cd /proc/sys/dev
	[ -d ath ] || return
	for dev in $(ls -d wifi* 2>&-); do
		found=0
		config_foreach check_atheros_device wifi-device
		[ "$found" -gt 0 ] && continue

		devname="$(cat /proc/sys/dev/$dev/dev_name)"
		case "$devname" in
			"NanoStation Loco2")
				EXTRA_DEV="
# Ubiquiti NanoStation Loco2 features
	option antenna	vertical # (horizontal|vertical)
"
			;;
			"NanoStation Loco5")
				EXTRA_DEV="
# Ubiquiti NanoStation Loco5 features
	option antenna	auto # (auto|horizontal|vertical)
"
			;;
			NanoStation*)
				EXTRA_DEV="
# Ubiquiti NanoStation features
	option antenna	auto # (auto|horizontal|vertical|external)
"
			;;
		esac

		cat <<EOF
config wifi-device  radio$devidx
	option type     atheros
	option channel  auto
	option macaddr	$(cat /sys/class/net/${dev}/address)
$EXTRA_DEV
	# REMOVE THIS LINE TO ENABLE WIFI:
	option disabled 1

config wifi-iface
	option device	radio$devidx
	option network	lan
	option mode	ap
	option ssid	OpenWrt
	option encryption none

EOF
	devidx=$(($devidx + 1))
	done
}
