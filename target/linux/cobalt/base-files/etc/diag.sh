#!/bin/sh
# Copyright (C) 2009 OpenWrt.org

set_state() {
        case "$1" in
                preinit)
                        [ -d /sys/class/leds/qube-front ] && {
                                echo none > /sys/class/leds/qube-front/trigger
                                echo 255 > /sys/class/leds/qube-front/brightness
                        }
                ;;
                done)
                        [ -d /sys/class/leds/qube-front ] && {
				echo 0 > /sys/class/leds/qube-front/brightness
                        }
                ;;
        esac
}
