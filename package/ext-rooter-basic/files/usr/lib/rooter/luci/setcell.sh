#!/bin/sh
 
ROOTER=/usr/lib/rooter

log() {
	modlog "Lock Cell $CURRMODEM" "$@"
}

dat="$1"

CURRMODEM=$(uci -q get modem.general.miscnum)

dat1=$(echo $dat | tr "|" ",")
dat2=$(echo $dat1 | cut -d, -f1)
if [ $dat2 = "0" ]; then
	uci set custom.bandlock.cenable$CURRMODEM='0'
else
	ear=$(echo $dat1 | cut -d, -f2)
	pc=$(echo $dat1 | cut -d, -f3)
	ear1=$(echo $dat1 | cut -d, -f4)
	pc1=$(echo $dat1 | cut -d, -f5)
	ear2=$(echo $dat1 | cut -d, -f6)
	pc2=$(echo $dat1 | cut -d, -f7)
	ear3=$(echo $dat1 | cut -d, -f8)
	pc3=$(echo $dat1 | cut -d, -f9)
	uci set custom.bandlock.cenable$CURRMODEM='1'
	uci set custom.bandlock.earfcn$CURRMODEM=$ear
	uci set custom.bandlock.pci$CURRMODEM=$pc
	uci set custom.bandlock.earfcn1$CURRMODEM=$ear1
	uci set custom.bandlock.pci1$CURRMODEM=$pc1
	uci set custom.bandlock.earfcn2$CURRMODEM=$ear2
	uci set custom.bandlock.pci2$CURRMODEM=$pc2
	uci set custom.bandlock.earfcn3$CURRMODEM=$ear3
	uci set custom.bandlock.pci3$CURRMODEM=$pc3
fi
uci commit custom

ifname1="ifname"
if [ -e /etc/newstyle ]; then
	ifname1="device"
fi


CURRMODEM=$(uci get modem.general.miscnum)
COMMPORT="/dev/ttyUSB"$(uci get modem.modem$CURRMODEM.commport)
CPORT=$(uci -q get modem.modem$CURRMODEM.commport)

ATCMDD="at+qnwlock=\"common/4g\""
OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
if `echo $OX | grep "ERROR" 1>/dev/null 2>&1`
then
	ATCMDD="at+qnwlock=\"common/lte\",0"
else
	ATCMDD=$ATCMDD",0"
fi
OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
log "$OX"
sleep 5		
/usr/lib/rooter/luci/restart.sh $CURRMODEM "9"
