#!/bin/sh

log() {
	logger -t "modem-led " "$@"
}

CURRMODEM=$1
COMMD=$2

DEV=$(uci get modem.modem$CURRMODEM.device)
DEVV=${DEV:0:1}

if [ $DEVV = "1" ]; then
	case $COMMD in
		"0" )
			echo timer > /sys/class/leds/h721:green:4g5g/trigger
			echo 0  > /sys/class/leds/h721:green:4g5g/delay_on
			echo 1000  > /sys/class/leds/h721:green:4g5g/delay_off
			;;
		"1" )
			echo timer > /sys/class/leds/h721:green:4g5g/trigger
			echo 500  > /sys/class/leds/h721:green:4g5g/delay_on
			echo 500  > /sys/class/leds/h721:green:4g5g/delay_off
			;;
		"2" )
			echo timer > /sys/class/leds/h721:green:4g5g/trigger
			echo 200  > /sys/class/leds/h721:green:4g5g/delay_on
			echo 200  > /sys/class/leds/h721:green:4g5g/delay_off
			;;
		"3" )
			echo timer > /sys/class/leds/h721:green:4g5g/trigger
			echo 1000  > /sys/class/leds/h721:green:4g5g/delay_on
			echo 0  > /sys/class/leds/h721:green:4g5g/delay_off
			;;
		"4" )
			sig2=$3
			echo timer > /sys/class/leds/h721:green:rssi_lte1/trigger
			if [ $sig2 -lt 18 -a $sig2 -gt 0 ] 2>/dev/null;then
				echo 500  > /sys/class/leds/h721:green:rssi_lte1/delay_on
				echo 500  > /sys/class/leds/h721:green:rssi_lte1/delay_off
			elif [ $sig2 -ge 18 -a $sig2 -lt 31 ] 2>/dev/null;then
				echo 150  > /sys/class/leds/h721:green:rssi_lte1/delay_on
				echo 150  > /sys/class/leds/h721:green:rssi_lte1/delay_off
			elif [ $sig2 -eq 31 ] 2>/dev/null;then
				echo 0  > /sys/class/leds/h721:green:rssi_lte1/delay_on
				echo 1000  > /sys/class/leds/h721:green:rssi_lte1/delay_off
			else
				echo 950  > /sys/class/leds/h721:green:rssi_lte1/delay_on
				echo 950  > /sys/class/leds/h721:green:rssi_lte1/delay_off
			fi
			;;
	esac
else
	case $COMMD in
		"0" )
			echo timer > /sys/class/leds/h721:green:4g/trigger
			echo 0  > /sys/class/leds/h721:green:4g/delay_on
			echo 1000  > /sys/class/leds/h721:green:4g/delay_off
			;;
		"1" )
			echo timer > /sys/class/leds/h721:green:4g/trigger
			echo 500  > /sys/class/leds/h721:green:4g/delay_on
			echo 500  > /sys/class/leds/h721:green:4g/delay_off
			;;
		"2" )
			echo timer > /sys/class/leds/h721:green:4g/trigger
			echo 200  > /sys/class/leds/h721:green:4g/delay_on
			echo 200  > /sys/class/leds/h721:green:4g/delay_off
			;;
		"3" )
			echo timer > /sys/class/leds/h721:green:4g/trigger
			echo 1000  > /sys/class/leds/h721:green:4g/delay_on
			echo 0  > /sys/class/leds/h721:green:4g/delay_off
			;;
		"4" )
			sig=$3
			echo timer > /sys/class/leds/h721:green:rssi_lte2/trigger
			if [ $sig -lt 18 -a $sig -gt 0 ] 2>/dev/null;then
				echo 500  > /sys/class/leds/h721:green:rssi_lte2/delay_on
				echo 500  > /sys/class/leds/h721:green:rssi_lte2/delay_off
			elif [ $sig -ge 18 -a $sig -lt 31 ] 2>/dev/null;then
				echo 150  > /sys/class/leds/h721:green:rssi_lte2/delay_on
				echo 150  > /sys/class/leds/h721:green:rssi_lte2/delay_off
			elif [ $sig -eq 31 ] 2>/dev/null;then
				echo 0  > /sys/class/leds/h721:green:rssi_lte2/delay_on
				echo 1000  > /sys/class/leds/h721:green:rssi_lte2/delay_off
			else
				echo 950  > /sys/class/leds/h721:green:rssi_lte2/delay_on
				echo 950  > /sys/class/leds/h721:green:rssi_lte2/delay_off
			fi
			;;
	esac

fi