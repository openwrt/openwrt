#!/bin/sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	modlog "Create Hostless Connection $CURRMODEM" "$@"
}

ifname1="ifname"
if [ -e /etc/newstyle ]; then
	ifname1="device"
fi

handle_timeout(){
	local wget_pid="$1"
	local count=0
	ps | grep -v grep | grep $wget_pid
	res="$?"
	while [ "$res" = 0 -a $count -lt "$((TIMEOUT))" ]; do
		sleep 1
		count=$((count+1))
		ps | grep -v grep | grep $wget_pid
		res="$?"
	done

	if [ "$res" = 0 ]; then
		log "Killing process on timeout"
		kill "$wget_pid" 2> /dev/null
		ps | grep -v grep | grep $wget_pid
		res="$?"
		if [ "$res" = 0 ]; then
			log "Killing process on timeout"
			kill -9 $wget_pid 2> /dev/null
		fi
	fi
}

check_apn() {
	IPVAR="IP"
	local COMMPORT="/dev/ttyUSB"$CPORT
	if [ -e /etc/nocops ]; then
		echo "0" > /tmp/block
	fi
	ATCMDD="AT+CGDCONT=?"
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")

	[ "$PDPT" = "0" ] && PDPT=""
	for PDP in "$PDPT" IPV4V6; do
		if [[ "$(echo $OX | grep -o "$PDP")" ]]; then
			IPVAR="$PDP"
			break
		fi
	done

	uci set modem.modem$CURRMODEM.pdptype=$IPVAR
	uci commit modem

	log "PDP Type selected in the Connection Profile: \"$PDPT\", active: \"$IPVAR\""

	if [ "$idV" = "12d1" ]; then
		CFUNOFF="0"
	else
		CFUNOFF="4"
	fi
	ATCMDD="AT+CGDCONT?;+CFUN?"
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	CGDCONT2=$(echo $OX | grep "+CGDCONT: 2,")
	if [ -z "$CGDCONT2" ]; then
		ATCMDD="AT+CGDCONT=2,\"$IPVAR\",\"ims\""
		OXy=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	fi
	CGDCONT=$(echo $OX | grep -o "$CID,[^,]\+,[^,]\+,[^,]\+,0,0,1")
	IPCG=$(echo $CGDCONT | cut -d, -f4)
	if [ "$CGDCONT" == "$CID,\"$IPVAR\",\"$NAPN\",$IPCG,0,0,1" ]; then
		if [ -z "$(echo $OX | grep -o "+CFUN: 1")" ]; then
			OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "AT+CFUN=1")
		fi
	else
		ATCMDD="AT+CGDCONT=$CID,\"$IPVAR\",\"$NAPN\",,0,0,1"
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "AT+CFUN=$CFUNOFF")
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "AT+CFUN=1")
		sleep 5
	fi
}

set_dns() {
	local pDNS1=$(uci -q get modem.modeminfo$CURRMODEM.dns1)
	local pDNS2=$(uci -q get modem.modeminfo$CURRMODEM.dns2)
	local pDNS3=$(uci -q get modem.modeminfo$CURRMODEM.dns3)
	local pDNS4=$(uci -q get modem.modeminfo$CURRMODEM.dns4)

	local aDNS="$pDNS1 $pDNS2 $pDNS3 $pDNS4"
	local bDNS=""

	echo "$aDNS" | grep -o "[[:graph:]]" &>/dev/null
	if [ $? = 0 ]; then
		log "Using DNS settings from the Connection Profile"
		pdns=1
		for DNSV in $(echo "$aDNS"); do
			if [ "$DNSV" != "0.0.0.0" ] && [ -z "$(echo "$bDNS" | grep -o "$DNSV")" ]; then
				[ -n "$(echo "$DNSV" | grep -o ":")" ] && continue
				bDNS="$bDNS $DNSV"
			fi
		done

		bDNS=$(echo $bDNS)
		uci set network.wan$INTER.peerdns=0
		uci set network.wan$INTER.dns="$bDNS"
		echo "$bDNS" > /tmp/v4dns$INTER

		bDNS=""
		for DNSV in $(echo "$aDNS"); do
			if [ "$DNSV" != "0:0:0:0:0:0:0:0" ] && [ -z "$(echo "$bDNS" | grep -o "$DNSV")" ]; then
				[ -z "$(echo "$DNSV" | grep -o ":")" ] && continue
				bDNS="$bDNS $DNSV"
			fi
		done
		echo "$bDNS" > /tmp/v6dns$INTER
	else
		log "Using Hostless Modem as a DNS relay"
		pdns=0
		rm -f /tmp/v[46]dns$INTER
	fi
}

set_network() {
	uci delete network.wan$INTER
	uci set network.wan$INTER=interface
	uci set network.wan$INTER.proto=dhcp
	uci set network.wan$INTER.${ifname1}=$1
	uci set network.wan$INTER.metric=$INTER"0"
	set_dns
	uci commit network
	sleep 5
}

save_variables() {
	echo 'MODSTART="'"$MODSTART"'"' > /tmp/variable.file
	echo 'WWAN="'"$WWAN"'"' >> /tmp/variable.file
	echo 'USBN="'"$USBN"'"' >> /tmp/variable.file
	echo 'ETHN="'"$ETHN"'"' >> /tmp/variable.file
	echo 'WDMN="'"$WDMN"'"' >> /tmp/variable.file
	echo 'BASEPORT="'"$BASEPORT"'"' >> /tmp/variable.file
}

chcklog() {
	OOX=$1
	CLOG=$(uci -q get modem.modeminfo$CURRMODEM.log)
	if [ $CLOG = "1" ]; then
		log "$OOX"
	fi
}

get_connect() {
	NAPN=$(uci -q get modem.modeminfo$CURRMODEM.apn)
	PDPT=$(uci -q get modem.modeminfo$CURRMODEM.pdptype)
	uci set modem.modem$CURRMODEM.apn="$NAPN"
	uci commit modem
}

get_tty_fix() {
# $1 is fixed ttyUSB or ttyACM port number
	local POS
	POS=`expr 1 + $1`
	CPORT=$(echo "$TTYDEVS" | cut -d' ' -f"$POS" | grep -o "[[:digit:]]\+")
}

get_ip() {
	ATCMDD="AT+CGPIAF=1,1,1,0;+CGPADDR"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	OX=$(echo "$OX" | grep "^+CGPADDR: $CID," | cut -d'"' -f2)
	ip4=$(echo $OX | cut -d, -f1 | grep "\.")
	ip6=$(echo $OX | cut -d, -f2 | grep ":")
	log "IP address(es) obtained: $ip4 $ip6"
}

check_ip() {
	if [[ $(echo "$ip6" | grep -o "^[23]") ]]; then
	# Global unicast IP acquired
		v6cap=1
	elif [[ $(echo "$ip6" | grep -o "^[0-9a-fA-F]\{1,4\}:") ]]; then
	# non-routable address
		v6cap=2
	else
		v6cap=0
	fi

	if [ -n "$ip6" -a -z "$ip4" ]; then
		log "Running IPv6-only mode"
		nat46=1
	fi
}

addv6() {
	. /lib/functions.sh
	. /lib/netifd/netifd-proto.sh
	local interface=wan$INTER
	local zone="$(fw3 -q network "$interface" 2>/dev/null)"

	log "Adding IPv6 dynamic interface"
	json_init
	json_add_string name "${interface}_6"
	json_add_string ${ifname1} "@$interface"
	json_add_string proto "dhcpv6"
	json_add_string extendprefix 1
	[ -n "$zone" ] && json_add_string zone "$zone"
	[ "$pdns" = 1 ] && json_add_boolean peerdns 0
	[ "$nat46" = 1 ] || json_add_string iface_464xlat 0
	proto_add_dynamic_defaults
	json_close_object
	ubus call network add_dynamic "$(json_dump)"
}

CURRMODEM=$1

MAN=$(uci -q get modem.modem$CURRMODEM.manuf)
MOD=$(uci -q get modem.modem$CURRMODEM.model)
$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Connecting"
$ROOTER/log/logger "Attempting to Connect Modem #$CURRMODEM ($MAN $MOD)"

BASEP=$(uci -q get modem.modem$CURRMODEM.baseport)
idV=$(uci -q get modem.modem$CURRMODEM.idV)
idP=$(uci -q get modem.modem$CURRMODEM.idP)
log " "
log "Hostless ID $idV:$idP"
log " "

MATCH="$(uci -q get modem.modem$CURRMODEM.maxcontrol | cut -d/ -f3- | xargs dirname)"
OX=$(for a in /sys/class/tty/*; do readlink $a; done | grep "$MATCH" | tr '\n' ' ' | xargs -r -n1 basename)
TTYDEVS=$(echo "$OX" | grep -o ttyUSB[0-9])
if [ $? -ne 0 ]; then
	TTYDEVS=$(echo "$OX" | grep -o ttyACM[0-9])
	[ $? -eq 0 ] && ACM=1
fi
TTYDEVS=$(echo "$TTYDEVS" | tr '\n' ' ')
TTYDEVS=$(echo $TTYDEVS)
if [ -n "$TTYDEVS" ]; then
	log Modem $CURRMODEM is a parent of $TTYDEVS
else
	log "No ECM Comm Port"
fi

if [ $idV = 1546 -a $idP = 1146 ]; then
	SP=1
elif [ $idV = 19d2 -a $idP = 1476 ]; then
	SP=2
elif [ $idV = 1410 -a $idP = 9022 ]; then
	SP=3
elif [ $idV = 1410 -a $idP = 9032 ]; then
	SP=3
elif [ $idV = 2cb7 -o $idV = 1508 ]; then
	sleep 5
	log "Fibocom ECM"
	SP=4
elif [ $idV = 2c7c ]; then
	SP=5
elif [ $idV = 12d1 -a $idP = 15c1 ]; then
	SP=6
elif [ $idV = 2cd2 ]; then
	log "MikroTik R11e ECM"
	SP=7
elif [ $idV = 0e8d -a $idP = 7127  ]; then
	log "RM350 ECM"
	SP=8
elif [ $idV = 0e8d -a $idP = 7126  ]; then
	log "RM350 ECM"
	SP=9
else
	SP=0
fi

log " "
log "Modem Type $SP"
log " "
if [ $SP -gt 0 ]; then
	if [ $SP -eq 3 ]; then
		PORTN=0
	elif [ $SP -eq 4 ]; then
		PORTN=2
	elif [ $SP -eq 5 ]; then
		[ $idP = 6026 ] && PORTN=1 || PORTN=2
	elif [ $SP -eq 6 ]; then
		PORTN=2
	elif [ $SP -eq 7 ]; then
		PORTN=0
	elif [ $SP -eq 8 ]; then
		PORTN=3
	elif [ $SP -eq 9 ]; then
		PORTN=1
	else
		PORTN=1
	fi
	get_tty_fix $PORTN
	lua $ROOTER/common/modemchk.lua "$idV" "$idP" "$CPORT" "$CPORT"
	source /tmp/parmpass

	if [ "$ACM" = 1 ]; then
		ACMPORT=$CPORT
		CPORT="7$ACMPORT"
		ln -fs /dev/ttyACM$ACMPORT /dev/ttyUSB$CPORT
	fi

	log "Modem $CURRMODEM ECM Comm Port : /dev/ttyUSB$CPORT"
	uci set modem.modem$CURRMODEM.commport=$CPORT
	uci commit modem

	$ROOTER/sms/check_sms.sh $CURRMODEM &

	if [ -e $ROOTER/connect/preconnect.sh ]; then
		$ROOTER/connect/preconnect.sh $CURRMODEM
	fi

	if [ $SP = 5 ]; then
		clck=$(uci -q get custom.bandlock.cenable$CURRMODEM)
		if [ "$clck" = "1" ]; then
			ear=$(uci -q get custom.bandlock.earfcn$CURRMODEM)
			pc=$(uci -q get custom.bandlock.pci$CURRMODEM)
			ear1=$(uci -q get custom.bandlock.earfcn1$CURRMODEM)
			pc1=$(uci -q get custom.bandlock.pci1$CURRMODEM)
			ear2=$(uci -q get custom.bandlock.earfcn2$CURRMODEM)
			pc2=$(uci -q get custom.bandlock.pci2$CURRMODEM)
			ear3=$(uci -q get custom.bandlock.earfcn3$CURRMODEM)
			pc3=$(uci -q get custom.bandlock.pci3$CURRMODEM)
			cnt=1
			earcnt=$ear","$pc
			if [ "$ear1" != "0" -a $pc1 != "0" ]; then
				earcnt=$earcnt","$ear1","$pc1
				let cnt=cnt+1
			fi
			if [ "$ear2" != "0" -a $pc2 != "0" ]; then
				earcnt=$earcnt","$ear2","$pc2
				let cnt=cnt+1
			fi
			if [ "$ear3" != "0" -a $pc3 != "0" ]; then
				earcnt=$earcnt","$ear3","$pc3
				let cnt=cnt+1
			fi
			earcnt=$cnt","$earcnt
			ATCMDD="at+qnwlock=\"common/4g\""
			OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
			log "$OX"
			if `echo $OX | grep "ERROR" 1>/dev/null 2>&1`
			then
				ATCMDD="at+qnwlock=\"common/lte\",2,$ear,$pc"
			else
				ATCMDD=$ATCMDD","$earcnt
			fi
			OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
			log "Cell Lock $OX"
			sleep 10
		fi
		$ROOTER/connect/bandmask $CURRMODEM 1
		uci commit modem
	fi


	if [ $SP = 4 ]; then
		if [ -e /etc/interwave ]; then
			idP=$(uci -q get modem.modem$CURRMODEM.idP)
			idPP=${idP:1:1}
			if [ "$idPP" = "1" ]; then
				ATC="AT+GTRAT=17"
			else
				ATC="AT+XACT=4,2"
			fi
			OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATC")
		fi
		$ROOTER/connect/bandmask $CURRMODEM 2
		uci commit modem
	fi
fi
	
if [ -e $ROOTER/modem-led.sh ]; then
	$ROOTER/modem-led.sh $CURRMODEM 2
fi

$ROOTER/common/gettype.sh $CURRMODEM
$ROOTER/connect/get_profile.sh $CURRMODEM
if [ $SP -gt 0 ]; then
	if [ -e $ROOTER/simlock.sh ]; then
		$ROOTER/simlock.sh $CURRMODEM
	fi

	if [ -e /tmp/simpin$CURRMODEM ]; then
		log " SIM Error"
		if [ -e $ROOTER/simerr.sh ]; then
			$ROOTER/simerr.sh $CURRMODEM
		fi
		#exit 0
	fi
	if [ -e /usr/lib/gps/gps.sh ]; then
		/usr/lib/gps/gps.sh $CURRMODEM &
	fi
fi

INTER=$(uci -q get modem.modeminfo$CURRMODEM.inter)
if [ -z "$INTER" ]; then
	INTER=$CURRMODEM
else
	if [ "$INTER" = 0 ]; then
		INTER=$CURRMODEM
	fi
fi
log "Profile for Modem $CURRMODEM sets interface to WAN$INTER"
OTHER=1
if [ $CURRMODEM = 1 ]; then
	OTHER=2
fi
EMPTY=$(uci -q get modem.modem$OTHER.empty)
if [ "$EMPTY" = 0 ]; then
	OINTER=$(uci -q get modem.modem$OTHER.inter)
	if [ ! -z "$OINTER" ]; then
		if [ $INTER = $OINTER ]; then
			INTER=1
			if [ "$OINTER" = 1 ]; then
				INTER=2
			fi
			log "Switched Modem $CURRMODEM to WAN$INTER as Modem $OTHER is using WAN$OINTER"
		fi
	fi
fi
uci set modem.modem$CURRMODEM.inter=$INTER
uci commit modem
log "Modem $CURRMODEM is using WAN$INTER"

CID=$(uci -q get modem.modeminfo$CURRMODEM.context)
[ -z "$CID" ] && CID=1

log "Checking Network Interface"
ifname="$(if [ "$MATCH" ]; then for a in /sys/class/net/*; do readlink $a; done | grep "$MATCH"; fi | xargs -r basename)"

if [ "$ifname" ]; then
	log "Modem $CURRMODEM ECM Data Port : $ifname"
	set_network "$ifname"
	uci set modem.modem$CURRMODEM.interface=$ifname
	if [ -e $ROOTER/changedevice.sh ]; then
		$ROOTER/changedevice.sh $ifname
	fi
else
	log "Modem $CURRMODEM - No ECM Data Port found"
fi
uci commit modem

ttl=$(uci -q get modem.modeminfo$CURRMODEM.ttl)
if [ -z "$ttl" ]; then
	ttl="0"
fi
hostless=$(uci -q get modem.modeminfo$CURRMODEM.hostless)
if [ "$ttl" != "0" -a "$ttl" != "1" -a "$ttl" != "TTL-INC 1" -a "$hostless" = "1" ]; then
	let "ttl=$ttl+1"
fi
$ROOTER/connect/handlettl.sh $CURRMODEM "$ttl"

if [ $SP -eq 2 ]; then
	get_connect
	export SETAPN=$NAPN
	BRK=1

	while [ $BRK -eq 1 ]; do
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "connect-zecm.gcom" "$CURRMODEM")
		chcklog "$OX"
		ERROR="ERROR"
		if `echo ${OX} | grep "${ERROR}" 1>/dev/null 2>&1`
		then
			$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Failed to Connect : Retrying"
		else
			BRK=0
		fi
	done
fi

if [ $SP -eq 4 ]; then
	get_connect
	export SETAPN=$NAPN
	BRK=1

	while [ $BRK -eq 1 ]; do
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "connect-fecm.gcom" "$CURRMODEM")
		chcklog "$OX"
		log " "
		log "Fibocom Connect : $OX"
		log " "
		ERROR="ERROR"
		if `echo ${OX} | grep "${ERROR}" 1>/dev/null 2>&1`
		then
			$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Failed to Connect : Retrying"
		else
			BRK=0
			get_ip
		fi
	done
fi
	
if [ $SP = 8 -o  $SP = 9 ]; then
	log "FM350 Connection Command"
	$ROOTER/connect/bandmask $CURRMODEM 2
	uci commit modem
	get_connect
	export SETAPN=$NAPN
	BRK=1
	
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "connect-fecm.gcom" "$CURRMODEM")
		chcklog "$OX"
		log " "
		log "Fibocom Connect : $OX"
		log " "
		ERROR="ERROR"
		if `echo ${OX} | grep "${ERROR}" 1>/dev/null 2>&1`
		then
			$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Failed to Connect : Retrying"
			log "Failed to Connect"
		else
			BRK=0
			get_ip
			check_ip
		fi
fi

if [ $SP = 5 ]; then
	get_connect
	if [ -n "$NAPN" ]; then
		$ROOTER/common/lockchk.sh $CURRMODEM
		if [ $idP = 6026 ]; then
			IPN=1
			case "$PDPT" in
			"IPV6" )
				IPN=2
				;;
			"IPV4V6" )
				IPN=3
				;;
			esac
			ATCMDD="AT+QICSGP=$CID,$IPN,\"$NAPN\""
			OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
			ATCMDD="AT+QNETDEVCTL=2,$CID,1"
			OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		else
			check_apn
		fi
	fi
	ATCMDD="AT+CNMI?"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	if `echo $OX | grep -o "+CNMI: [0-3],2," >/dev/null 2>&1`; then
		ATCMDD="AT+CNMI=0,0,0,0,0"
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	fi
	ATCMDD="AT+QINDCFG=\"smsincoming\""
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	if `echo $OX | grep -o ",1" >/dev/null 2>&1`; then
		ATCMDD="AT+QINDCFG=\"smsincoming\",0,1"
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	fi
	ATCMDD="AT+QINDCFG=\"all\""
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	if `echo $OX | grep -o ",1" >/dev/null 2>&1`; then
		ATCMDD="AT+QINDCFG=\"all\",0,1"
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	fi
	log "Quectel Unsolicited Responses Disabled"
	$ROOTER/luci/celltype.sh $CURRMODEM
	ATCMDD="AT+QINDCFG=\"all\",1"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")

	get_ip
	if [ -n "$ip6" ]; then
		check_ip
		if [ "$v6cap" -gt 0 ]; then
			addv6
		fi
	fi
fi

if [ $SP -eq 6 ]; then
	get_connect
	export SETAPN=$NAPN
	BRK=1

	while [ $BRK -eq 1 ]; do
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "connect-ncm.gcom" "$CURRMODEM")
		chcklog "$OX"
		ERROR="ERROR"
		if `echo ${OX} | grep "${ERROR}" 1>/dev/null 2>&1`
		then
			$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Failed to Connect : Retrying"
		else
			BRK=0
		fi
	done
fi

if [ $SP -eq 7 ]; then
	get_connect
	export SETAPN=$NAPN
	BRK=1

	if [ -n "$NAPN" ]; then
		check_apn
	fi

	while [ $BRK -eq 1 ]; do
		ATCMDD="AT\$ECMCALL=1"
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		chcklog "$OX"
		ERROR="ERROR"
		if `echo ${OX} | grep "${ERROR}" 1>/dev/null 2>&1`
		then
			$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Failed to Connect : Retrying"
		else
			BRK=0
			get_ip
			if [ -n "$ip6" ]; then
				check_ip
				if [ "$v6cap" -gt 0 ]; then
					addv6
				fi
			fi
		fi
	done
fi

rm -f /tmp/usbwait

ifup wan$INTER
while `ifstatus wan$INTER | grep -q '"up": false\|"pending": true'`; do
	sleep 1
done
wan_ip=$(expr "`ifstatus wan$INTER | grep '"nexthop":'`" : '.*"nexthop": "\(.*\)"')
if [ $? -ne 0 ] ; then
	wan_ip=192.168.0.1
fi
uci set modem.modem$CURRMODEM.ip=$wan_ip
uci commit modem

if [ -e $ROOTER/modem-led.sh ]; then
	$ROOTER/modem-led.sh $CURRMODEM 3
fi
		
$ROOTER/log/logger "HostlessModem #$CURRMODEM Connected with IP $wan_ip"

PROT=5

if [ $SP -gt 1 ]; then
	ln -s $ROOTER/signal/modemsignal.sh $ROOTER_LINK/getsignal$CURRMODEM
	$ROOTER_LINK/getsignal$CURRMODEM $CURRMODEM $PROT &
else
	VENDOR=$(uci -q get modem.modem$CURRMODEM.idV)
	case $VENDOR in
	"19d2" )
		TIMEOUT=3
		wget -O /tmp/connect.file http://$wan_ip/goform/goform_set_cmd_process?goformId=CONNECT_NETWORK &
		handle_timeout "$!"
		ln -s $ROOTER/signal/ztehostless.sh $ROOTER_LINK/getsignal$CURRMODEM
		$ROOTER_LINK/getsignal$CURRMODEM $CURRMODEM $PROT &
		;;
	"12d1" )
		log "Huawei Hostless"
		ln -s $ROOTER/signal/huaweihostless.sh $ROOTER_LINK/getsignal$CURRMODEM
		$ROOTER_LINK/getsignal$CURRMODEM $CURRMODEM $PROT &
		;;
	* )
		log "Other Hostless"
		ln -s $ROOTER/signal/otherhostless.sh $ROOTER_LINK/getsignal$CURRMODEM
		$ROOTER_LINK/getsignal$CURRMODEM $CURRMODEM $PROT &
		;;
esac
fi

ln -s $ROOTER/connect/conmon.sh $ROOTER_LINK/con_monitor$CURRMODEM
$ROOTER_LINK/con_monitor$CURRMODEM $CURRMODEM &
uci set modem.modem$CURRMODEM.connected=1
uci commit modem

if [ $SP -gt 0 ]; then
	if [ -e $ROOTER/connect/postconnect.sh ]; then
		$ROOTER/connect/postconnect.sh $CURRMODEM
	fi
	if [ $(uci -q get modem.modeminfo$CURRMODEM.at) -eq "1" ]; then
		ATCMDD=$(uci -q get modem.modeminfo$CURRMODEM.atc)
		if [ -n "$ATCMDD" ]; then
			OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
			OX=$($ROOTER/common/processat.sh "$OX")
			ERROR="ERROR"
			if `echo $OX | grep "$ERROR" 1>/dev/null 2>&1`
			then
				log "Error sending custom AT command: $ATCMDD with result: $OX"
			else
				log "Sent custom AT command: $ATCMDD with result: $OX"
			fi
		fi
	fi

	if [ -e $ROOTER/timezone.sh ]; then
		TZ=$(uci -q get modem.modeminfo$CURRMODEM.tzone)
		if [ "$TZ" = "1" ]; then
			log "Set TimeZone"
			$ROOTER/timezone.sh &
		fi
	fi
fi

#CLB=$(uci -q get modem.modeminfo$CURRMODEM.lb)
CLB=1
if [ -e /etc/config/mwan3 ]; then
	ENB=$(uci -q get mwan3.wan$INTER.enabled)
	if [ ! -z "$ENB" ]; then
		if [ "$CLB" = "1" ]; then
			uci set mwan3.wan$INTER.enabled=1
		else
			uci set mwan3.wan$INTER.enabled=0
		fi
		uci commit mwan3
		/usr/sbin/mwan3 restart
	fi
fi
