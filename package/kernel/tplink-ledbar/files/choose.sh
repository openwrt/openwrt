# Effect selection for the TP-Link Archer BE550 front LED bar, as on the
# stock firmware: LEDs off > WPS > internet/Wi-Fi table.
# Usage: ledbar_choose <internet> <wifi> <wps> <leds_off>
# Each argument is 1 for true, anything else for false.
# Prints one of: off wps on partial breathing partial-breathing
ledbar_choose() {
	if [ "$4" = 1 ]; then
		echo off
	elif [ "$3" = 1 ]; then
		echo wps
	elif [ "$1" = 1 ] && [ "$2" = 1 ]; then
		echo on
	elif [ "$2" = 1 ]; then
		echo partial
	elif [ "$1" = 1 ]; then
		echo breathing
	else
		echo partial-breathing
	fi
}
