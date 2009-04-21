#!/bin/sh
# Copyright (C) 2009 OpenWrt.org

setup_switch_dev() {
	swconfig dev "$1" load network
}

setup_switch() {
	config_load network
	config_foreach setup_switch_dev switch
}
