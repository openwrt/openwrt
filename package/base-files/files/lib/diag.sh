#!/bin/sh
# Copyright (C) 2006-2019 OpenWrt.org

. /lib/functions.sh

include /lib/diag

set_state() {
	if type 'get_status_led_platform' >/dev/null 2>/dev/null; then
		get_status_led_platform "$1"
	else
		get_status_led_default "$1"
	fi

	if type 'set_state_platform' >/dev/null 2>/dev/null; then
		set_state_platform "$1"
	else
		set_state_default "$1"
	fi
}
