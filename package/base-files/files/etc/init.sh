#!/bin/bash
passwd root<<EOF
changeme
changeme
EOF
sysctl -p
sh /boot/init.sh
rm -f /boot/init.sh
rm -f /etc/init.sh
