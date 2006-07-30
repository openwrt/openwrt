#!/bin/sh
# Copyright (C) 2006 OpenWrt.org

setup_switch_vlan() {
	DIR="/proc/switch/$CONFIG_SECTION/vlan/$1"
	[ -d "$DIR" ] || return 0
	
	config_get ports "$CONFIG_SECTION" "vlan$1"
	echo "$ports" > "$DIR/ports"
}

setup_switch() {
	config_cb() {
		case "$1" in
			switch)
				[ -n "$2" -a -d "/proc/switch/$2" ] && {
					echo 1 > "/proc/switch/$2/reset"
					echo 1 > "/proc/switch/$2/enable"
					echo 1 > "/proc/switch/$2/enable_vlan"
					option_cb() {
						case "$1" in
							vlan*) setup_switch_vlan "${1##vlan}";;
						esac
					}
				}
			;;
			*)
				option_cb() { return 0; }
			;;
		esac
	}
	config_load network
}
