#!/bin/sh

SM=$(uci get system.4g5g)
if [ -z $SM ]; then
	uci set system.4g5g=led
	uci set system.4g5g.name="4G5G"
	uci set system.4g5g.sysfs="green:4g"
	uci set system.4g5g.trigger="netdev"
	uci set system.4g5g.dev="wwan0"
	uci set system.4g5g.mode="link tx rx"
	uci set system.4g5g.default='0'

	uci commit system
	/etc/init.d/led restart
fi

uci set system.gpio2=gpio_switch
uci set system.gpio2.name='gpio2'
uci set system.gpio2.gpio_pin='2'
uci set system.gpio2.value='0'
uci commit system
/etc/init.d/system restart

