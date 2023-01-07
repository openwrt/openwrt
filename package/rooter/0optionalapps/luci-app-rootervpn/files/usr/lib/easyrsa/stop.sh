#!/bin/sh

result=`ps | grep -i "generate.sh" | grep -v "grep" | wc -l`
if [ $result -ge 1 ]; then
	rkillall generate.sh
	rm -f /tmp/easyrsa
	PKI_DIR="/etc/openvpn/ssl"
	if [ -d "$PKI_DIR" ]; then
		rm -rfv "$PKI_DIR"
	fi
fi