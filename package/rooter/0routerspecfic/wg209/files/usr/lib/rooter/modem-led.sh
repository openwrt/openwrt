#!/bin/sh 

log() {
	logger -t "modem-led " "$@"
}

CURRMODEM=$1
COMMD=$2

	case $COMMD in
		"0" )
			echo none > /sys/class/leds/green:signal1/trigger
			echo 0  > /sys/class/leds/green:signal1/brightness
			echo none > /sys/class/leds/green:signal2/trigger
			echo 0  > /sys/class/leds/green:signal2/brightness
			echo none > /sys/class/leds/green:signal3/trigger
			echo 0  > /sys/class/leds/green:signal3/brightness
			echo none > /sys/class/leds/green:internet/trigger
			echo 0  > /sys/class/leds/green:internet/brightness
			;;
		"1" )
			echo timer > /sys/class/leds/green:internet/trigger
			echo 500  > /sys/class/leds/green:internet/delay_on
			echo 500  > /sys/class/leds/green:internet/delay_off
			;;
		"2" )
			echo timer > /sys/class/leds/green:internet/trigger
			echo 200  > /sys/class/leds/green:internet/delay_on
			echo 200  > /sys/class/leds/green:internet/delay_off
			;;
		"3" )
			echo timer > /sys/class/leds/green:internet/trigger
			echo 1000  > /sys/class/leds/green:internet/delay_on
			echo 0  > /sys/class/leds/green:internet/delay_off
			;;
		"4" )
			sig2=$3
			if [ $sig2 -lt 8 -a $sig2 -gt 0 ] 2>/dev/null;then
				echo none > /sys/class/leds/green:internet/trigger
				echo 1  > /sys/class/leds/green:internet/brightness
				echo none > /sys/class/leds/green:signal3/trigger
				echo 0  > /sys/class/leds/green:signal3/brightness
				echo none > /sys/class/leds/green:signal2/trigger
				echo 0  > /sys/class/leds/green:signal2/brightness
				echo none > /sys/class/leds/green:signal1/trigger
				echo 0  > /sys/class/leds/green:signal1/brightness
			elif [ $sig2 -ge 8 -a $sig2 -lt 16 ] 2>/dev/null;then
				echo none > /sys/class/leds/green:internet/trigger
				echo 1  > /sys/class/leds/green:internet/brightness
				echo none > /sys/class/leds/green:signal3/trigger
				echo 1  > /sys/class/leds/green:signal3/brightness
				echo none > /sys/class/leds/green:signal2/trigger
				echo 0  > /sys/class/leds/green:signal2/brightness
				echo none > /sys/class/leds/green:signal1/trigger
				echo 0  > /sys/class/leds/green:signal1/brightness
			elif [ $sig2 -ge 16 -a $sig2 -lt 24 ] 2>/dev/null;then
				echo none > /sys/class/leds/green:internet/trigger
				echo 1  > /sys/class/leds/green:internet/brightness
				echo none > /sys/class/leds/green:signal3/trigger
				echo 1  > /sys/class/leds/green:signal3/brightness
				echo none > /sys/class/leds/green:signal2/trigger
				echo 1  > /sys/class/leds/green:signal2/brightness
				echo none > /sys/class/leds/green:signal1/trigger
				echo 0  > /sys/class/leds/green:signal1/brightness
			elif [ $sig2 -ge 24 ] 2>/dev/null;then
				echo none > /sys/class/leds/green:internet/trigger
				echo 1  > /sys/class/leds/green:internet/brightness
				echo none > /sys/class/leds/green:signal3/trigger
				echo 1  > /sys/class/leds/green:signal3/brightness
				echo none > /sys/class/leds/green:signal2/trigger
				echo 1  > /sys/class/leds/green:signal2/brightness
				echo none > /sys/class/leds/green:signal1/trigger
				echo 1  > /sys/class/leds/green:signal1/brightness
			fi
			;;
	esac
