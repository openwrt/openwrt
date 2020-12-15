#!/bin/bash
passwd root<<EOF
changeme
changeme
EOF
sysctl -p
sh /boot/init.sh
ipaddr=$(ubus call network.interface.wan status | grep \"address\" | grep -oE '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}')
echo "My IP Address is: $ipaddr" 1>&2
rm -f /boot/init.sh
rm -f /etc/init.sh
