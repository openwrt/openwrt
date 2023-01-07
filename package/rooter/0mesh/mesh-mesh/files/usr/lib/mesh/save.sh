#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Save" "$@"
}

state=$1

state=$(echo "$state" | tr "|" ",")
uci set mesh.radio.radionumber=$(echo "$state" | cut -d, -f1)
uci set mesh.radio.channelindex=$(echo "$state" | cut -d, -f2)
uci set mesh.radio.channellist=$(echo "$state" | cut -d, -f3)
uci set mesh.radio.usedfs=$(echo "$state" | cut -d, -f4)
uci set mesh.radio.channelwidth=$(echo "$state" | cut -d, -f5)
uci set mesh.radio.dedicated=$(echo "$state" | cut -d, -f6)

uci set mesh.network.networkid=$(echo "$state" | cut -d, -f7)
uci set mesh.network.netencrypted=$(echo "$state" | cut -d, -f8)
uci set mesh.network.netpassword=$(echo "$state" | cut -d, -f9)

uci set mesh.roam.signalenable=$(echo "$state" | cut -d, -f10)
uci set mesh.roam.signalid=$(echo "$state" | cut -d, -f11)

uci commit mesh
