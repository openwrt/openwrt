#!/bin/sh
# Copyright (C) 2007 OpenWrt.org

# This setup gives us 3.5 distinguishable states:
#
# Solid OFF:  Bootloader running, or kernel hung (timer task stalled)
# Solid ON:   Kernel hung (timer task stalled)
# 5Hz blink:  preinit
# Heartbeat:  normal operation

set_state() {
        case "$1" in
                preinit)
                        [ -d /sys/class/leds/status ] && {
                                echo timer >/sys/class/leds/status/trigger
                                echo 100 >/sys/class/leds/status/delay_on
                                echo 100 >/sys/class/leds/status/delay_off
                        }
                ;;
                done)
                        [ -d /sys/class/leds/status ] && {
                                echo heartbeat >/sys/class/leds/status/trigger
                        }
                ;;
        esac
}
