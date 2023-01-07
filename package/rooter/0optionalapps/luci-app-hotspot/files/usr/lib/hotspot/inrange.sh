#!/bin/sh
. "/lib/functions.sh"

log() {
	wifilog "In Range" "$@"
}

rm -f /tmp/hot1
cnt=0
trm_ifstatus="false"
while [ ${cnt} -lt 20 ]
do
	trm_ifstatus="$(ubus -S call network.wireless status | jsonfilter -l1 -e '@.*.up')"
	if [ "${trm_ifstatus}" = "true" ]
        then
            break
        fi
        cnt=$((cnt+1))
        sleep 1
done

wif=$(uci -q get travelmate.global.freq)
RADIO=$(uci get wireless.wwan$wif.device)
ap_list="$(ubus -S call network.wireless status | jsonfilter -e "@.$RADIO.interfaces[@.config.mode=\"ap\"].ifname")"

trm_scanner="$(which iw)"
for ap in ${ap_list}
do
	ssid_list="$(${trm_scanner} dev "${ap}" scan 2>/dev/null > /tmp/scan
       cat /tmp/scan | awk '/SSID: /{if(!seen[$0]++){printf "\"";for(i=2; i<=NF; i++)if(i==2)printf $i;else printf " "$i;printf "\" "}}')"

	if [ -n "${ssid_list}" ]
       then
		if [ -f "/etc/hotspot" ]; then
                    	while IFS='|' read -r ssid encrypt key
                   	do
				ssidq="\"$ssid\""
                        	if [ -n "$(printf "${ssid_list}" | grep -Fo "${ssidq}")" ]
                       	then
					echo $ssid"|"$encrypt"|"$key"|1" >> /tmp/hot1
				else
					echo $ssid"|"$encrypt"|"$key"|0" >> /tmp/hot1
				fi
			done <"/etc/hotspot"
		fi
	else
		if [ -f "/etc/hotspot" ]; then
                    	while IFS='|' read -r ssid encrypt key
                   	do
				echo $ssid"|"$encrypt"|"$key"|0" >> /tmp/hot1
			done <"/etc/hotspot"
		fi
	fi
done
if [ -f "/tmp/hot1" ]; then
	mv -f /tmp/hot1 /tmp/hot
fi


