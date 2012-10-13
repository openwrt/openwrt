#!/bin/sh
# Copyright (C) 2007-2012 OpenWrt.org

# This setup gives us 4.5 distinguishable states:
#
# (1-LED) Solid OFF:    Bootloader running, or kernel hung (timer task stalled)
# (1-LED) Solid ON:     Kernel hung (timer task stalled)
# (2-LED) Solid RED:    Bootloader running, or kernel hung (timer task stalled)
# (2-LED) Solid YELLOW: Kernel hung (timer task stalled)
# 5Hz blink:            preinit
# 10Hz blink:           failsafe
# (1-LED) Heartbeat:    normal operation
# (2-LED) Solid GREEN:  normal operation

set_state() {
        case "$1" in
                preinit)
                        [ -d /sys/class/leds/status ] && {
                                echo timer >/sys/class/leds/status/trigger
                                echo 100 >/sys/class/leds/status/delay_on
                                echo 100 >/sys/class/leds/status/delay_off
                        :; } || [ -d /sys/class/leds/power\:green ] && {
                                echo timer >/sys/class/leds/power\:green/trigger
                                echo 100 >/sys/class/leds/power\:green/delay_on
                                echo 100 >/sys/class/leds/power\:green/delay_off
                        }
                ;;
                failsafe)
                        [ -d /sys/class/leds/status ] && {
                                echo timer >/sys/class/leds/status/trigger
                                echo 50 >/sys/class/leds/status/delay_on
                                echo 50 >/sys/class/leds/status/delay_off
                        :; } || [ -d /sys/class/leds/power\:green ] && {
                                echo timer >/sys/class/leds/power\:green/trigger
                                echo 50 >/sys/class/leds/power\:green/delay_on
                                echo 50 >/sys/class/leds/power\:green/delay_off
                        }
                ;;
                done)
                        [ -d /sys/class/leds/status ] && {
                                echo heartbeat >/sys/class/leds/status/trigger
                        :; } || [ -d /sys/class/leds/power\:green ] && {
                                echo default-on >/sys/class/leds/power\:green/trigger
                                echo none >/sys/class/leds/power\:red/trigger
                        }
                ;;
        esac
}
