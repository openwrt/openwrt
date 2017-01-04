#!/bin/sh

sunxi_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name="$(cat /tmp/sysinfo/board_name)"
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
