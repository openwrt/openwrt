#!/bin/sh /etc/rc.common
#
# start me with /bin/sh leds.sh & if run from shell directly
#
# copyrihgt professor_jonny Professor_jonnny@hotmail.com
#
# this file has been created as there is no factory mechinism in OpenWrt to handle RGB led's
# and they cant be controlled by the standard mechinism as there is no cycle function or
# priorioty given to individual LED's and mixing of differenr colours will result
# if different statuses are displayed simutaniously.

# The Deco line of TPlink products they only have a single Multifunction led that
# shows the status of the deco unit by cycling through rgb colours as follows:
#
# Yellow pulse	Deco is resetting
# Yellow solid	Deco is starting up
# Blue pulse	Deco is ready for setup, or WPS is in progress.
# Blue solid	Deco is setting up
# Red pulse	Deco is disconnected from main Deco
# Red solid		Deco has an issue
# Green pulse Deco is upgrading firmware.
# Green solid Deco is all set up and connected to 4G / WAN.
# White solid Deco is all set up and connected to 5G.
#
#
#
# many of these have simular functions in openwrt but some do not.
# the user may alter by the $state($) to display a specific colour as one sees fit.
# if someone comes up with a better solution than this I'm open to suggestion.
# I'm not trying to recreate the factory LED's functions but at least make it usable as a 
# status led in openwrt in a simular way That the oem has made use of it.


# setup easy led drive functions
set_rgb_leds() {
   echo "$1" > /sys/class/leds/red:/brightness
   echo "$2" > /sys/class/leds/green:/brightness
   echo "$3" > /sys/class/leds/blue:/brightness
}

while :
  do
    state0=0 # some function make led red
    state1=0 # some function make led green
    state2=0 # some function make led blue
    state3=1 # show power led yellow
    state4=0 # some function make led magenta
    state5=0 # some function make led cyan
    state6=0 # some function make led white

	if [ "$state0" = "1" ]; then
		set_rgb_leds 255 0 0     # red
    sleep 2
    set_rgb_leds 0 0 0
    sleep 1
	else
		set_rgb_leds 0 0 0     # off
	fi

	if [ "$state1" = "1" ]; then
		set_rgb_leds 0 255 0     # green
    sleep 2
    set_rgb_leds 0 0 0
    sleep 1
	else
		set_rgb_leds 0 0 0     # off
	fi

	if [ "$state2" = "1" ]; then
		set_rgb_leds 0 0 255     # blue
    sleep 2
    set_rgb_leds 0 0 0
    sleep 1
	else
		set_rgb_leds 0 0 0     # off
	fi

	if [ "$state3" = "1" ]; then
		set_rgb_leds 255 255 0   # yellow
    sleep 2
    set_rgb_leds 0 0 0
    sleep 1
	else
		set_rgb_leds 0 0 0     # off
	fi

	if [ "$state4" = "1" ]; then
		set_rgb_leds 255 0 255   # magneta
    sleep 2
    set_rgb_leds 0 0 0
    sleep 1
	else
		set_rgb_leds 0 0 0     # off
	fi

	if [ "$state5" = "1" ]; then
		set_rgb_leds 0 255 255   # cyan
    sleep 2
    set_rgb_leds 0 0 0
    sleep 1
	else
		set_rgb_leds 0 0 0     # off
	fi

	if [ "$state6" = "1" ]; then
		set_rgb_leds 255 255 255 # white
    sleep 2
    set_rgb_leds 0 0 0
    sleep 1
	else
		set_rgb_leds 0 0 0     # off
	fi

done
