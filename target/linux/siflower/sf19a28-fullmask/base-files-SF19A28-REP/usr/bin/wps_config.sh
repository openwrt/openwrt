#!/bin/sh
echo "~$wds_if~$band~ wps setting" > /dev/ttyS0
wds_if=$1
band=$2

connection_check()
{
	echo "~$wds_if~$band~ prepare_config" > /dev/ttyS0
	local check_time=0
	local n1="false"
	while [ -n $n1 -a $check_time -lt 60  ]
	do
		chan=`iwinfo $wds_if info | grep Chan|awk -F ' ' '{print $4}'`
		ssid=`iwinfo $wds_if info | grep ESSID|awk -F '"' '{print $2}'`
		n1=`echo $chan|sed 's/[0-9]//g'`
		sleep 1
		let "check_time ++"
	done

	[ $check_time -lt 60  ] || {
		echo "~wds~failed~exit~" > /dev/ttyS0
		[ -f /tmp/wps_status  ] && rm /tmp/wps_status
		exit 0
	}
}

prepare_config()
{
	psk=$(cat  /var/run/wpa_supplicant-$wds_if.conf | grep "$ssid" -A5 |grep psk |tail -1| awk -F '"' '{print $2}')
	encription=$(cat  /var/run/wpa_supplicant-$wds_if.conf | grep "$ssid" -A5 |grep key_mgmt | awk -F '=' '{print $2}')
	proto=$(cat  /var/run/wpa_supplicant-$wds_if.conf | grep "$ssid" -A5 |grep proto | awk -F '=' '{print $2}')
	case $encription in
		NONE)
			enc="open"
			;;
		*PSK*)
			enc="psk2+ccmp"
			[ "$proto" = "WPA" ] && enc="psk+ccmp"
			;;
		*)
			enc="psk2+ccmp"
			;;
	esac
	# freq : dualband one ssid.
	config={\"sfi_num\":\"$wds_if\",\"ssid\":\"$ssid\",\"encryption\":\"$enc\",\"key\":\"$psk\",\"bssid\":\"\"}
	last_config={\"freq\":\"1\",\"band\":\"$band\",\"channel\":\"$chan\",\"ssid_24g\":\"${ssid}\",\
\"encryption_24g\":\"$enc\",\"key_24g\":\"$psk\",\"ssid_5g\":\"${ssid}\",\"encryption_5g\":\"$enc\",\"key_5g\":\"$psk\"}

}

set_config()
{
	output=`ubus call lepton.network set_wireless "$config"`
	echo "~wps~done~" > /dev/ttyS0
	output=`ubus call lepton.network set_config_last "$last_config"`
	# set wds flag and light the led
	/bin/led-button -l 18
	echo "1" > /tmp/wds_connected
	[ -f /tmp/wps_status  ] && rm /tmp/wps_status
	exit 0
}

connection_check
prepare_config
set_config
