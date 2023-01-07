#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "excede BW " "$@"
}

do_throttle() {
	local config=$1
	local limit

	config_get name $1 name
	config_get limit $1 limit
	config_get throttle $1 throttle
	let kamt=limit*1000000
	if [ $amt -gt $kamt ]; then
		if [ $limit -gt $baselimit ]; then
			speed=$throttle
			level=$name
			baselimit=$limit
		fi
	fi
}

bb="$(uci -q get custom.bwallocate.manual)"
if [ "$bb" != "1" ]; then
	lock=$(uci -q get custom.bwallocate.lock)
	if [ $lock = "1" ]; then
		enb=$(uci -q get custom.bwallocate.enabled)
		if [ $enb = '1' ]; then
			allocate=$2
			total=$1
			/usr/lib/bwmon/block 0
			action=$(uci -q get custom.bwallocate.action)
			if [ -z $action ]; then
				action=0
			fi
			if [ ! -e /usr/lib/throttle/throttle.sh ]; then
				action=0
			fi
			uci set custom.bwallocate.status='0'
			uci commit custom
			if [ $action != "2" ]; then
				if [ $total -gt $allocate ]; then
					if [ $action = "0" ]; then
						if [ -e /etc/nodogsplash/control ]; then
							/etc/nodogsplash/control block
						else
							/usr/lib/bwmon/block 1
						fi
						uci set custom.bwallocate.status='1'
						uci commit custom
					else
						down=$(uci -q get custom.bwallocate.down)
						if [ -z $down ]; then
							down=5
						fi
						up=$(uci -q get custom.bwallocate.up)
						if [ -z $up ]; then
							up=2
						fi
						/usr/lib/throttle/throttle.sh start $down $up
						uci set custom.bwallocate.status='2'
						uci commit custom
					fi
				else
					if [ -e /usr/lib/throttle/throttle.sh ]; then
						/usr/lib/throttle/throttle.sh stop
					fi
					if [ -e /etc/nodogsplash/control ]; then
						/etc/nodogsplash/control unblock
					fi
					/usr/lib/bwmon/block 0
				fi
			else
				meth=$(uci -q get custom.bwallocate.meth)
				if [ -z $meth ]; then
					meth="0"
				fi
				if [ $meth = "0" ]; then
					amt=$total
				else
					amt=$3
				fi
				speed="0"
				baselimit="0"
				config_load custom
				config_foreach do_throttle throttle
				if [ $speed != "0" ]; then
					/usr/lib/bwmon/float.lua "$speed"
					source /tmp/float
					/usr/lib/throttle/throttle.sh start $SPEED $SPEED 1
					log "Throttled to $speed Mbps"
					uci set custom.bwallocate.status='2'
					uci commit custom
				else
					if [ -e /usr/lib/throttle/throttle.sh ]; then
						/usr/lib/throttle/throttle.sh stop
					fi
				fi
				
			fi
		fi
	fi
fi