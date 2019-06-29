#!/bin/sh

push_led_bit()
{
	local num=$1
	echo "1" >/sys/class/gpio/led_clk/value
	echo "$num" >/sys/class/gpio/led_b/value
	echo "0" >/sys/class/gpio/led_clk/value
}

led_ctrl() {
	local oldvalue=
	while :; do
		rssi5=0
		rssi4=0
		rssi3=0
		rssi2=0
		rssi1=0
		mobile_3g=0
		mobile_4g=0
		mobile_network=0

		lq=`iwinfo wlan0 info | grep "Link Quality:" | sed 's,.*Link Quality:\(.*\)/.*,\1,'`
		lq=$((lq+0))
		test $lq -ge 1  && rssi1=1
		test $lq -ge 14 && rssi2=1
		test $lq -ge 28 && rssi3=1
		test $lq -ge 42 && rssi4=1
		test $lq -ge 56 && rssi5=1

		newvalue="$rssi5$rssi4$rssi3$rssi2$rssi1$mobile_3g$mobile_4g$mobile_network"
		if [ "x$oldvalue" != "x$newvalue" ]; then
			for num in $mobile_network $mobile_4g $mobile_3g $rssi1 $rssi2 $rssi3 $rssi4 $rssi5; do
				push_led_bit $num
			done
		fi
		oldvalue=$newvalue
		sleep 5
		[ -d /tmp/led-ctrl.lck ] || break
	done
}

case $1 in
	start)
		mkdir /tmp/led-ctrl.lck 2>/dev/null && {
			led_ctrl &
		}
    ;;
	stop)
		rm -rf /tmp/led-ctrl.lck
    ;;
esac
