#!/bin/sh

# Copyright 2019 Shun Li <riverscn@gmail.com>
# Licensed to the public under the GNU General Public License v3.

add_to_set() {
    local ip=$1
    local cfg=$2
    local subnet=$ip/24
    ping -W1 -c1 $ip &>/dev/null && return
    if ! ipset -q test iptvhelper_$cfg $subnet; then
        ipset add iptvhelper_$cfg $subnet
        echo added $subnet to set iptvhelper_$cfg
    fi
}

echo $1

logread -e "iptvhelper\.$1" -f |
    while read line; do
        ip=$(echo "$line" | sed -r 's|.*DST=([0-9.]+).*|\1|')
        echo requested $ip
        add_to_set $ip $1 &
    done
