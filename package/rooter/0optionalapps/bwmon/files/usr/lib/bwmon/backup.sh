#!/bin/sh

log() {
	logger -t "wrtbwmon" "$@"
}

# parameters
#
btype=$1
cDay=$2
monthlyUsageDB=$3
dailyUsageDB=$4
monthlyUsageBack=$5
dailyUsageBack=$6
pDay=$7

/usr/lib/bwmon/backup-daily.lua $dailyUsageDB
/usr/lib/bwmon/backup-mon.lua $monthlyUsageDB
cp -f $monthlyUsageDB".bk" $monthlyUsageDB
cp -f $dailyUsageDB".bk" $dailyUsageDB

echo "start day $cDay" >> $monthlyUsageDB".bk"
cat $dailyUsageDB".bk" >> $monthlyUsageDB".bk"
echo "end day $cDay" >> $monthlyUsageDB".bk"

enb=$(uci -q get bwmon.general.enabled)

if [ $btype = "backup" ]; then
	if [ "$enb" = "1" ]; then
		cp -f $monthlyUsageDB".bk" $monthlyUsageBack
		cp -f $dailyUsageDB".bk" $dailyUsageBack 
	fi
else
	if [ $btype = "daily" ]; then
		cp -f $monthlyUsageDB".bk" $monthlyUsageDB
		if [ "$enb" = "1" ]; then
			cp -f $monthlyUsageDB".bk" $monthlyUsageBack
		fi
	fi
fi

rm -f $dailyUsageDB".bk"

bwday=$(uci -q get modem.modeminfo1.bwday)
if [ ! -z "$bwday" ]; then
	if [ $bwday = $pDay -a $bwday != "0" ]; then
		if [ -e /usr/lib/bwmon/sendsms ]; then
			/usr/lib/bwmon/sendsms.sh
		fi
	fi
fi