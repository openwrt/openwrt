#!/bin/sh

log() {
	logger -t "edit email" "$@"
}

host=$(uci -q get custom.texting.smtp)
if [ -z $host ]; then
	exit 0
fi
user=$(uci -q get custom.texting.euser)
if [ -z $user ]; then
	exit 0
fi
pass=$(uci -q get custom.texting.epass)
if [ -z $pass ]; then
	exit 0
fi

STEMP="/tmp/eemail"
MSG="/usr/lib/bwmon/msmtprc"
DST="/etc/msmtprc"
rm -f $STEMP
cp $MSG $STEMP
sed -i -e "s!#HOST#!$host!g" $STEMP
sed -i -e "s!#USER#!$user!g" $STEMP
sed -i -e "s!#PASS#!$pass!g" $STEMP
mv $STEMP $DST
		