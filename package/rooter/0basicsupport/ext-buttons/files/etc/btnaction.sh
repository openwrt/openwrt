#!/bin/sh

FUNC=$1

reset_short() {
	passwd -d root
	reboot -f
}

reset_long() {
	mtd -r erase rootfs_data
}

wifi() {
	STATEFILE="/tmp/wifionoff.state"

	if [ $# -eq 1 ]; then
 	 case $1 in
	    "up"|"on")
	      STATE=off
	      ;;
	    "down"|"off")
	      STATE=on
	      ;;
	  esac
	else
	  if [ ! -e ${STATEFILE} ]; then
	    STATE=on
	  else
	    . ${STATEFILE}
	  fi
	fi
	if [ -z ${STATE} ]; then
	  STATE=on
	fi

	if [ ${STATE} == "on" ]; then
	  /sbin/wifi down
	  STATE=off
	else
	  /sbin/wifi up
	  STATE=on
	fi
  
	echo "STATE=${STATE}" > ${STATEFILE}
}

if [ $FUNC = "reset_short" ]; then
	reset_short
fi
if [ $FUNC = "reset_long" ]; then
	reset_long
fi
if [ $FUNC = "wifi" ]; then
	wifi $2
fi
