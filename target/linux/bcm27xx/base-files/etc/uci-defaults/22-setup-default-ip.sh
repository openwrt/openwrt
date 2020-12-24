#!/bin/sh
uci set network.lan.ipaddr='192.168.8.1'
uci commit network
exit 0
