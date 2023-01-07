#!/bin/sh

log() {
	logger -t "Schedule Reboot" "$@"
}

PARM=$1

if [ $PARM = "0" ]; then
	HO=$(uci get system.@system[-1].zonename)
	if [ -z $HO ]; then
		HO="UTC"
	fi
	uci set schedule.timezone.zonename="$HO"
	uci commit schedule
fi

if [ $PARM = "1" ]; then
	sleep 5
	EN=$(uci get schedule.reboot.enable)
	if [ $EN = "1" ]; then
		SDHOUR=$(uci get schedule.reboot.sdhour)
		HOUR=`expr $SDHOUR / 4`
		let "TH = $HOUR * 4"
		let "TMP1 = $SDHOUR - $TH"
		let "MIN = $TMP1 * 15"
		echo "$MIN $HOUR * * * sleep 70 && touch /etc/banner && reboot -f" > /etc/cronbase
	else
		rm -f /etc/cronbase
	fi
	/usr/lib/rooter/luci/croncat.sh
fi
