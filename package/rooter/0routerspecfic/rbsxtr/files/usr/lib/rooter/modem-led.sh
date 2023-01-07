#!/bin/sh

log() {
	logger -t "modem-led " "$@"
}

CURRMODEM=$1
COMMD=$2

	case $COMMD in
		"0" )
			echo none > /sys/class/leds/green:lte/trigger
			echo 0  > /sys/class/leds/green:lte/brightness
			;;
		"1" )
			echo timer > /sys/class/leds/green:lte/trigger
			echo 500  > /sys/class/leds/green:lte/delay_on
			echo 500  > /sys/class/leds/green:lte/delay_off
			;;
		"2" )
			echo timer > /sys/class/leds/green:lte/trigger
			echo 200  > /sys/class/leds/green:lte/delay_on
			echo 200  > /sys/class/leds/green:lte/delay_off
			;;
		"3" )
			echo timer > /sys/class/leds/green:lte/trigger
			echo 1000  > /sys/class/leds/green:lte/delay_on
			echo 0  > /sys/class/leds/green:lte/delay_off
			;;
		"4" )
			echo none > /sys/class/leds/green:lte/trigger
			echo 1  > /sys/class/leds/green:lte/brightness
			;;
	esac
