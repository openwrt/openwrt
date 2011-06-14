#!/bin/sh
# Copyright (C) 2009-2011 OpenWrt.org

front_led=/sys/class/leds/qube::front

set_state() {
        case "$1" in
                preinit)
                        [ -d $front_led ] && {
                                echo none > $front_led/trigger
                                echo 255 > $front_led/brightness
                        }
                ;;
                done)
                        [ -d $front_led ] && {
				echo 0 > $front_led/brightness
                        }
                ;;
        esac
}
