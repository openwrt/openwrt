#!/bin/sh

log() {
	logger -t "modem-led " "$@"
}

CURRMODEM=$1
COMMD=$2

	case $COMMD in
		"0" )
			echo none > /sys/class/leds/green:globe/trigger
			echo 0  > /sys/class/leds/green:globe/brightness
			echo none > /sys/class/leds/green:signal/trigger
			echo 0  > /sys/class/leds/green:signal/brightness
			;;
		"1" )
			echo timer > /sys/class/leds/green:globe/trigger
			echo 500  > /sys/class/leds/green:globe/delay_on
			echo 500  > /sys/class/leds/green:globe/delay_off
			;;
		"2" )
			echo timer > /sys/class/leds/green:globe/trigger
			echo 200  > /sys/class/leds/green:globe/delay_on
			echo 200  > /sys/class/leds/green:globe/delay_off
			;;
		"3" )
			echo timer > /sys/class/leds/green:globe/trigger
			echo 1000  > /sys/class/leds/green:globe/delay_on
			echo 0  > /sys/class/leds/green:globe/delay_off
			;;
		"4" )
			echo none > /sys/class/leds/green:globe/trigger
			echo 1  > /sys/class/leds/green:globe/brightness
			sig2=$3
			echo timer > /sys/class/leds/green:signal/trigger
			if [ $sig2 -lt 18 -a $sig2 -gt 0 ] 2>/dev/null;then
				echo 500  > /sys/class/leds/green:signal/delay_on
				echo 500  > /sys/class/leds/green:signal/delay_off
			elif [ $sig2 -ge 18 -a $sig2 -lt 31 ] 2>/dev/null;then
				echo 150  > /sys/class/leds/green:signal/delay_on
				echo 150  > /sys/class/leds/green:signal/delay_off
			elif [ $sig2 -eq 31 ] 2>/dev/null;then
				echo 0  > /sys/class/leds/green:signal/delay_on
				echo 1000  > /sys/class/leds/green:signal/delay_off
			else
				echo 950  > /sys/class/leds/green:signal/delay_on
				echo 950  > /sys/class/leds/green:signal/delay_off
			fi
			;;
	esac
