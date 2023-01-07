#!/bin/sh

hostname=$1

HO=$(uci get system.@system[-1].hostname)
if [ $HO = "OpenWrt" -o $HO = "LEDE" ]; then
	uci set system.@system[-1].hostname="$1"
	echo "$1" > /proc/sys/kernel/hostname
	uci commit system
fi
