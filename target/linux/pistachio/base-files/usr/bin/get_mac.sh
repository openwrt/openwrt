#!/bin/sh

. /lib/functions/mac.sh

generate_mac $1
echo $MAC > /tmp/MAC
