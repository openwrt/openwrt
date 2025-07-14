#!/bin/sh

update_guest_lan() {
	guest_lan=`uci -q get network.guest`
	if [ "x$guest_lan" != "x" ]; then
		guest_lan_ip=`uci -q get network.guest.ipaddr`
		if [ "x$guest_lan_ip" != "x" ]; then
			echo addifname 0 7 br-guest > /sys/kernel/debug/hnat_debug
		fi
	fi
}

update_lan() {
	for i in $(seq 1 7)
	do
		lan_name="lan$i"
		if [ $i == 1 ]; then
			lan_name="lan"
		fi
		ifexist=`uci -q get network.$lan_name`
		if [ "x$ifexist" != "x" ]; then
			lan_ip=`uci -q get network.$lan_name.ipaddr`
			if [ "x$lan_ip" != "x" ]; then
				lan_type=`uci -q get network.$lan_name.type`
				index=$(expr $i - 1)
				if [ "$lan_type" == "bridge" ]; then
					echo addifname 0 $index br-$lan_name > /sys/kernel/debug/hnat_debug
				else
					ifname=`uci -q get network.$lan_name.ifname`
					if [ "x$ifname" != "x" ]; then
						echo addifname 0 $index $ifname > /sys/kernel/debug/hnat_debug
					fi
				fi
			fi
		fi
	done
}

update_wan() {
	for i in $(seq 1 7)
	do
		wan_name="wan$i"
		if [ $i == 1 ]; then
			wan_name="wan"
		fi
		ifexist=`uci -q get network.$wan_name`
		if [ "x$ifexist" != "x" -a $wan_name != "wan6" ]; then
			ifname=`uci -q get network.$wan_name.ifname`
			if [ "x$ifname" != "x" ]; then
				index=$(expr $i - 1)
				echo addifname 1 $index $ifname > /sys/kernel/debug/hnat_debug
			fi
		fi
	done

}

update_pppoe_wan() {
	if [ -f "/usr/sbin/pppd" ]; then
		if_wan_exist=`uci -q get network.wan`
		if [ "x$if_wan_exist" != "x" ]; then
			echo addifname 1 7 pppoe-wan > /sys/kernel/debug/hnat_debug
		else
			wan_name=`uci show network | grep interface | grep wan | grep -v wan_dev | grep -v wan6 | head -1 | awk -F "[.=]" '{print $2}' | tr -d '\n'`
			if [ "x$wan_name" != "x" ]; then
				echo addifname 1 7 pppoe-$wan_name > /sys/kernel/debug/hnat_debug
			fi
		fi
	fi
}

echo dellan 1 1 > /sys/kernel/debug/hnat_debug
echo delwan 1 1 > /sys/kernel/debug/hnat_debug

update_guest_lan
update_lan
update_wan
update_pppoe_wan
