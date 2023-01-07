#!/bin/sh 
. /lib/functions.sh

mkdir /tmp/sysinfo
echo -n "PC Engines Alix.2D" > /tmp/sysinfo/model

echo 'SDA="'"sda"'"' > /etc/sda_drop