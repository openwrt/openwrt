#!/bin/sh
# Copyright (C) 2006 OpenWrt.org

sfwifi_ate remove
sleep 1

modify=0
if [ $1 == "lb1" ]
then
modify=1
sed -i "/TRX*/d" /lib/firmware/rf_trx_path.ini
echo "TRX_PATH_CFG=0xF3" > /lib/firmware/rf_trx_path.ini
fi

if [ $1 == "lb2" ]
then
modify=1
sed -i "/TRX*/d" /lib/firmware/rf_trx_path.ini
echo "TRX_PATH_CFG=0xFC" > /lib/firmware/rf_trx_path.ini
fi


if [ $1 == "hb1" ]
then
modify=2
sed -i "/TRX*/d" /lib/firmware/rf_trx_path.ini
echo "TRX_PATH_CFG=0x3F" > /lib/firmware/rf_trx_path.ini
fi


if [ $1 == "hb2" ]
then
modify=2
sed -i "/TRX*/d" /lib/firmware/rf_trx_path.ini
echo "TRX_PATH_CFG=0xCF" > /lib/firmware/rf_trx_path.ini
fi

if [ "$modify" == "0" ]
then
sed -i "/TRX*/d" /lib/firmware/rf_trx_path.ini
echo "TRX_PATH_CFG=0xFF" > /lib/firmware/rf_trx_path.ini
fi

if [ $1 == "lb" ]
then
sfwifi_ate reload $1 $2
sleep 2

ifconfig wlan0 up
ate_cmd wlan0 set ATE = ATESTART
fi

if [ $1 == "hb" ]
then
sfwifi_ate reload $1 $2
sleep 2

ifconfig wlan0 up
ate_cmd wlan1 set ATE = ATESTART
fi

if [ $1 == "hp" ]
then
sfwifi_ate reload
sleep 2

ifconfig wlan0 up
ifconfig wlan1 up
ate_cmd wlan0 set ATE = ATESTART
ate_cmd wlan1 set ATE = ATESTART
devmem 0x1150b100 32 0x007F7F7F
devmem 0x1110b100 32 0x007F7F7F

devmem 0x11c00cfe 16 0x0800
devmem 0x11c01cfe 16 0x0800
fi

if [ "$modify" == "1"  ]
then
sfwifi_ate reload
sleep 2

ifconfig wlan0 up
ifconfig wlan1 down
ate_cmd wlan0 set ATE = ATESTART
#ate_cmd wlan1 set ATE = ATESTART
elif [ "$modify" == "2" ]
then
sfwifi_ate reload
sleep 2

ifconfig wlan1 up
ifconfig wlan0 down
ate_cmd wlan1 set ATE = ATESTART
else
sfwifi_ate reload
sleep 2

ifconfig wlan0 up
ifconfig wlan1 up
ate_cmd wlan0 set ATE = ATESTART
ate_cmd wlan1 set ATE = ATESTART
fi

killall ate_server
if [ -w "/sys/kernel/debug/aetnensis/recalibrate" ];then
echo 200 > /sys/kernel/debug/aetnensis/recalibrate
fi
if [ -w "/sys/kernel/debug/aetnensis/cooling_temp" ];then
echo 0 > /sys/kernel/debug/aetnensis/cooling_temp
fi

ate_server
