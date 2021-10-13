#!/bin/sh

. /lib/functions.sh

load() {
	INTERFACE=$1 ACTION=ifup . /etc/hotplug.d/iface/80-dataplane
}

config_load dataplane
config_foreach load interface

