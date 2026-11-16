#!/bin/sh

if [ "$ACTION" = "add" ]; then
	read -r board_name < "/tmp/sysinfo/board_name"

	if [[ "$board_name" = "mellanox-technologies-ltd-msn*" ]]; then
		read -r port_name < "/sys/class/net/$DEVICENAME/phys_port_name"
		[ -n "$port_name" ] && ip link set "$DEVICENAME" name "sw$port_name"
	fi
fi
