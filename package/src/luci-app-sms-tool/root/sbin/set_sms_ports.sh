#!/bin/sh
# Copyright 2020-2022 Rafał Wabik (IceG) - From eko.one.pl forum
# Licensed to the GNU General Public License v3.0.

work=false
for port in /dev/ttyUSB*
do
    [[ -e $port ]] || continue
    gcom -d $port info &> /tmp/testusb
    testUSB=`cat /tmp/testusb | grep "Error\|Can't"`
    if [ -z "$testUSB" ]; then 
        work=$port
        break
    fi
done
rm -rf /tmp/testusb

if [ $work != false ]; then
uci set sms_tool.@sms_tool[0].readport=$work
uci set sms_tool.@sms_tool[0].sendport=$work
uci set sms_tool.@sms_tool[0].ussdport=$work
uci set sms_tool.@sms_tool[0].atport=$work
uci commit sms_tool
fi
