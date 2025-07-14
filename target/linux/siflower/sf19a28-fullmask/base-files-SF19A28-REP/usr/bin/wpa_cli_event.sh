#!/bin/ash
#echo "~$1~$2~$3~" > /dev/ttyS0
. /lib/functions.sh

wds_enabled=0
wds_if=$1
check_wds_connection () {
	local cfg="$1"

	config_get ifname "$cfg" ifname

	if [[ "$ifname" == "$wds_if" ]]; then
		config_get disable "$cfg" disabled
		if [[ "$disable" == "0" ]]; then
			wds_enabled="1"
		fi
	fi
	return 0
}

check_wps(){
	wps_status=0
	[ -f /tmp/wps_status ] && {
		wps_status=`cat /tmp/wps_status`
	}
}

prepare_params() {
	case "$wds_if" in
		sfi0)
			radio_num=0
			band=24g
			;;
		sfi1)
			radio_num=1
			band=5g
			;;
	esac
}

set_channel() {
	#get_channel
	chan=`iwinfo $wds_if info | grep Chan|awk -F ' ' '{print $4}'`

	[ "$chan" -gt 0  ] && {
		uci set wireless.radio${radio_num}.channel="$chan"
		# set sfix which is not in use disabled = 1,and set 5g htmode
		if [ $radio_num = 1 ]; then
			uci set wireless.@wifi-iface[2].disabled='1'
			uci set wireless.radio1.htmode="VHT80"
			[ "$chan" = "165"  ] && uci set wireless.radio1.htmode="VHT20"
		else
			uci set wireless.@wifi-iface[3].disabled='1'
		fi
		uci commit wireless
		output=`wifi reload`
	}
}

set_uhttpd() {
	if [ "$1" = "index" ] ;then
		uci del_list uhttpd.main.index_page='cgi-bin/first.lua'
		uci del_list uhttpd.main.index_page='index.htm'
		uci set uhttpd.main.error_page='/index.htm'
		uci add_list uhttpd.main.index_page='index.htm'
		uci commit uhttpd
		/etc/init.d/uhttpd restart
	elif [ "$1" = "test" ] ;then
		uci del_list uhttpd.main.index_page='cgi-bin/first.lua'
		uci del_list uhttpd.main.index_page='index.htm'
		uci set uhttpd.main.error_page='/cgi-bin/first.lua'
		uci add_list uhttpd.main.index_page='cgi-bin/first.lua'
		uci commit uhttpd
		/etc/init.d/uhttpd restart
	else
		echo "set_uhttpd param error" > /dev/ttyS0
	fi
	#echo "$0 set_uhttpd end" > /dev/ttyS0
}

if [[ "$wds_if" == "sfi0" || "$wds_if" == "sfi1" ]]; then
	config_load wireless
	config_foreach check_wds_connection wifi-iface
	if [[ "$wds_enabled" != "1" ]]; then
		exit 0
	fi

	check_wps
	prepare_params

	if [ "$2" == "WPS-SUCCESS" ]; then
		[ "$wps_status" = 0 ] && {
			echo "$wds_if" > /tmp/wps_status
			#echo "wps~select~$wds_if~" > /dev/ttyS0
		}
	fi

	if [ "$2" == "CONNECTED" ]; then
		if [ "$wps_status" != 0 ]; then
			if [ "$wds_if" != "$wps_status" ]; then
				#echo "wps $wps_status now $wds_if so exit 0" > /dev/ttyS0
				exit 0
			fi
			#shall we change wps_status here to avoid setting more than once?
			/usr/bin/wps_config.sh $wds_if $band
			exit 0
		fi

		#echo "wpa_cli_evt: connected" > /dev/ttyS0
		local busy=`cat /tmp/wds_sta_status`
		while [ "$busy" == "b" ]
		do
			busy=`cat /tmp/wds_sta_status`
		done
		echo "b" > /tmp/wds_sta_status

		/bin/led-button -l 18
		dns_down=`uci get basic_setting.dnsmasq.down`
		echo "1" > /tmp/wds_connected
		# dns_down use to judge whether rep has been configured, 1 mean has been configured
		[ "$dns_down" = "1"  ] && {
			# enable relayd
			uci set network.stabridge.disabled='0'
			if [ "$wds_if" == "sfi0" ]; then
				uci set network.stabridge.network='lan wwan'
			elif [ "$wds_if" == "sfi1" ] ;then
				uci set network.stabridge.network='lan wwwan'
			else
				echo "wpa_cli_evt: connected wds_if=$wds_if" > /dev/ttyS0
			fi
			uci commit network
			/etc/init.d/relayd restart

			/etc/init.d/dnsmasq restart
			set_channel
			set_uhttpd "index"
		}

		echo "0" > /tmp/wds_sta_status
	fi

	if [ "$2" == "DISCONNECTED" ]; then
		[ "$wps_status" != "0" ] && exit 0
		#echo "wpa_cli_evt: disconnected" > /dev/ttyS0
		local busy=`cat /tmp/wds_sta_status`
		while [ "$busy" == "b" ]
		do
			busy=`cat /tmp/wds_sta_status`
		done
		echo "b" > /tmp/wds_sta_status
		# enable lan dhcp server
		echo "0" > /tmp/wds_connected
		# disable relay
		uci set network.stabridge.disabled='1'
		uci commit network
		/etc/init.d/relayd restart

		/etc/init.d/dnsmasq restart

		local dns_status=`ps|grep dnsmasq|grep -vc grep`
		while [ "$dns_status" != "2" ]
		do
			sleep 1
			dns_status=`ps|grep dnsmasq|grep -vc grep`
		done

		set_uhttpd "test"
		# kick out devices
		ubus call lepton.network net_restart

		/bin/led-button -l 17

		echo "1" > /tmp/wds_sta_status

	fi

	# This is called by wds_sta_is_disconnected() in wirelessnew.lua
	if [ "$2" == "RECONNECT" ]; then
		local busy=`cat /tmp/wds_sta_status`
		while [ "$busy" == "b" ]
		do
			busy=`cat /tmp/wds_sta_status`
		done
		echo "b" > /tmp/wds_sta_status

		local result=""
		wpa_cli reconfigure
		wpa_cli scan
		result=`wpa_cli scan_result | grep $3 -i`
		if [ "$result""x" != "x" ]; then
			# host exists, try again
			echo "1" > /tmp/wds_sta_status
		else
			# no host
			echo "2" > /tmp/wds_sta_status
		fi
		# don't exit immediately, wait sta reconnect
		sleep 4
	fi

fi
#echo "wpa_cli_event done!" > /dev/ttyS0
