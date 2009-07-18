#!/bin/sh
# Copyright (C) 2009 OpenWrt.org

set_state() {
        case "$1" in
                done)
                        [ -d /sys/class/leds/power ] && {
                                echo 1 >/sys/class/leds/power/brightness
                        }
                ;;
        esac
}
