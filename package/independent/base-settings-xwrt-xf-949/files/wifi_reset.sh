#!/bin/sh

mkdir /tmp/wifi_reset_lck || exit 0
(
echo 1 > /sys/class/leds/blue:usb/brightness
echo 1 > /sys/class/leds/blue:wlan/brightness
sleep 1
echo 0 > /sys/class/leds/blue:usb/brightness
echo 0 > /sys/class/leds/blue:wlan/brightness
sleep 1
echo 1 > /sys/class/leds/blue:usb/brightness
echo 0 > /sys/class/leds/blue:wlan/brightness
sleep 1
echo 0 > /sys/class/leds/blue:usb/brightness
echo 1 > /sys/class/leds/blue:wlan/brightness
sleep 1
/etc/init.d/led restart
) &

uci set wireless.wifinet1.disabled='1'
uci commit wireless
/etc/init.d/network reload

rmdir /tmp/wifi_reset_lck
