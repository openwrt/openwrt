#!/bin/sh

log() {
	logger -t "OpenVpn Vpn" "$@"
}

/etc/init.d/openvpn stop $1
sleep 5

