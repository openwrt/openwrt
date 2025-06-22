#!/bin/sh /etc/rc.common

START=90
STOP=10

start() {
    echo "Applying custom TTL rules..."
    nft -f /etc/firewall.nft
}

stop() {
    echo "Flushing custom TTL rules..."
    nft flush table inet custom_ttl
    nft delete table inet custom_ttl
}

