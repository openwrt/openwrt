#!/bin/sh

log() {
	logger -t "sdcard" "$@"
}

h721() {
	if [ $1 = "add" ]; then
		echo "17" > /sys/class/gpio/export
		echo "out" > /sys/class/gpio/gpio17/direction
		echo 0 > /sys/class/gpio/gpio17/value
	else
		echo "17" > /sys/class/gpio/export
		echo "out" > /sys/class/gpio/gpio17/direction
		echo 1 > /sys/class/gpio/gpio17/value
fi
}

wg1608() {
	if [ $1 = "add" ]; then
		echo timer > /sys/class/leds/zbt-wg3526:green:signal/trigger
		echo 1000  > /sys/class/leds/zbt-wg3526:green:signal/delay_on
		echo 0  > /sys/class/leds/zbt-wg3526:green:signal/delay_off
	else
		echo timer > /sys/class/leds/zbt-wg3526:green:signal/trigger
		echo 0  > /sys/class/leds/zbt-wg3526:green:signal/delay_on
		echo 1000  > /sys/class/leds/zbt-wg3526:green:signal/delay_off
	fi
}

ws7915() {
	if [ $1 = "add" ]; then
		echo timer > /sys/class/leds/sys/trigger
		echo 1000  > /sys/class/leds/sys/delay_on
		echo 0  > /sys/class/leds/sys/delay_off
	else
		echo timer > /sys/class/leds/sys/trigger
		echo 0  > /sys/class/leds/sys/delay_on
		echo 1000  > /sys/class/leds/sys/delay_off
	fi
}

ws1688() {
	if [ $1 = "add" ]; then
		echo timer > /sys/class/leds/usb/trigger
		echo 1000  > /sys/class/leds/usb/delay_on
		echo 0  > /sys/class/leds/usb/delay_off
	else
		echo timer > /sys/class/leds/usb/trigger
		echo 0  > /sys/class/leds/usb/delay_on
		echo 1000  > /sys/class/leds/usb/delay_off
	fi
}

ACTION=$1
model=$(cat /tmp/sysinfo/model)

case $ACTION in
	"add"|"remove" )
		mod=$(echo $model | grep "H721")
		if [ $mod ]; then
			h721 $ACTION
		fi
		mod=$(echo $model | grep "WG1608")
		if [ $mod ]; then
			wg1608 $ACTION
		fi
		mod=$(echo $model | grep "WS7915")
		if [ $mod ]; then
			ws7915 $ACTION
		fi
		
		mod=$(echo $model | grep "WS1688")
		if [ $mod ]; then
			ws1688 $ACTION
		fi
		;;
	"detect" )
		mod=$(echo $model | grep "Raspberry")
		if [ $mod ]; then
			echo 'detect="'"1"'"' > /tmp/detect.file
		else
			echo 'detect="'"0"'"' > /tmp/detect.file
		fi
		;;
esac


