#!/bin/sh

ROOTER=/usr/lib/rooter

CURRMODEM=$1

if [ -e /etc/config/mwan3 ]; then
	INTER=$(uci get modem.modeminfo$CURRMODEM.inter)
	ENB=$(uci get mwan3.wan$INTER.enabled)
	if [ ! -z $ENB ]; then
		uci set mwan3.wan$INTER.enabled=0
		uci commit mwan3
	fi
fi