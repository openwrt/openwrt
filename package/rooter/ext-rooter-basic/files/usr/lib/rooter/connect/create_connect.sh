#!/bin/sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	modlog "Create Connection $CURRMODEM" "$@"
}

ifname1="ifname"
if [ -e /etc/newstyle ]; then
	ifname1="device"
fi

handle_timeout(){
	local wget_pid="$1"
	local count=0
	TIMEOUT=70
	res=1
	if [ -d /proc/${wget_pid} ]; then
		res=0
	fi
	while [ "$res" = 0 -a $count -lt "$((TIMEOUT))" ]; do
		sleep 1
		count=$((count+1))
		res=1
		if [ -d /proc/${wget_pid} ]; then
			res=0
		fi
	done

	if [ "$res" = 0 ]; then
		log "Killing process on timeout"
		kill "$wget_pid" 2> /dev/null
		res=1
		if [ -d /proc/${wget_pid} ]; then
			res=0
		fi
		if [ "$res" = 0 ]; then
			log "Killing process on timeout"
			kill -9 $wget_pid 2> /dev/null
		fi
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
		if [ $DHCP = 1 ]; then
			uci set network.wan$INTER.peerdns=0
			uci set network.wan$INTER.dns="$bDNS"
		fi
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
		log "Using Provider assigned DNS"
		pdns=0
		rm -f /tmp/v[46]dns$INTER
	fi
}

set_dns2() {
	local pDNS1=$(uci -q get modem.modeminfo$CURRMODEM.dns1)
	local pDNS2=$(uci -q get modem.modeminfo$CURRMODEM.dns2)
	local pDNS3=$(uci -q get modem.modeminfo$CURRMODEM.dns3)
	local pDNS4=$(uci -q get modem.modeminfo$CURRMODEM.dns4)

	local _DNS1 _DNS2 _DNS3 _DNS4 aDNS bDNS

	echo "$pDNS1 $pDNS2 $pDNS3 $pDNS4" | grep -o "[[:graph:]]" &>/dev/null
	if [ $? = 0 ]; then
		log "Using DNS settings from the Connection Profile"
		pdns=1
		_DNS1=$pDNS1
		_DNS2=$pDNS2
		_DNS3=$pDNS3
		_DNS4=$pDNS4
	else
		log "Using Provider assigned DNS"
		pdns=0
		_DNS1=$DNS1
		_DNS2=$DNS2
		_DNS3=$DNS3
		_DNS4=$DNS4
	fi

	aDNS="$_DNS1 $_DNS2 $_DNS3 $_DNS4"

	bDNS=""
	for DNSV in $(echo "$aDNS"); do
		if [ "$DNSV" != "0.0.0.0" ] && [ "$DNSV" != "0:0:0:0:0:0:0:0" ] && [ -z "$(echo "$bDNS" | grep -o "$DNSV")" ]; then
			[ -n "$(echo "$DNSV" | grep -o ":")" ] && [ -z "$ip6" ] && continue
			bDNS="$bDNS $DNSV"
		fi
	done

	bDNS=$(echo $bDNS)
	uci set network.wan$INTER.dns="$bDNS"
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
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	fi
	if `echo $OX | grep "+CGDCONT: $CID,\"$IPVAR\",\"$NAPN\"," 1>/dev/null 2>&1`
	then
		if [ -z "$(echo $OX | grep -o "+CFUN: 1")" ]; then
			OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "AT+CFUN=1")
		fi
	else
		ATCMDD="AT+CGDCONT=$CID,\"$IPVAR\",\"$NAPN\""
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "AT+CFUN=$CFUNOFF")
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "AT+CFUN=1")
		sleep 5
	fi
	if [ -e /etc/nocops ]; then
		rm -f /tmp/block
	fi
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
	NUSER=$(uci -q get modem.modeminfo$CURRMODEM.user)
	NPASS=$(uci -q get modem.modeminfo$CURRMODEM.passw)
	NAUTH=$(uci -q get modem.modeminfo$CURRMODEM.auth)
	spin=$(uci -q get custom.simpin.pin) # SIM Pin
	if [ -z "$spin" ]; then
		spin=$(uci -q get modem.modeminfo$CURRMODEM.pincode) # Profile Pin
		if [ -z "$spin" ]; then
			spin=$(uci -q get profile.simpin.pin) # Default profile Pin
		fi
	fi
	PINC=$spin
	uci set modem.modeminfo$CURRMODEM.pincode=$PINC
	uci commit modem
	PDPT=$(uci -q get modem.modeminfo$CURRMODEM.pdptype)
#
# QMI and MBIM can't handle nil
#
	case $PROT in
	"2"|"3"|"30" )
		if [ -z "$NUSER" ]; then
			NUSER="NIL"
		fi
		if [ -z "$NPASS" ]; then
			NPASS="NIL"
		fi
		;;
	esac

	uci set modem.modem$CURRMODEM.apn=$NAPN
	uci set modem.modem$CURRMODEM.user=$NUSER
	uci set modem.modem$CURRMODEM.passw=$NPASS
	uci set modem.modem$CURRMODEM.auth=$NAUTH
	uci set modem.modem$CURRMODEM.pin=$PINC
	uci commit modem
}

chksierra() {
	SIERRAID=0
	if [ $idV = 1199 ]; then
		case $idP in
			"68aa"|"68a2"|"68a3"|"68a9"|"68b0"|"68b1" )
				SIERRAID=1
			;;
			"68c0"|"9040"|"9041"|"9051"|"9054"|"9056"|"90d3" )
				SIERRAID=1
			;;
			"9070"|"907b"|"9071"|"9079"|"901c"|"9091"|"901f"|"90b1" )
				SIERRAID=1
			;;
		esac
	elif [ $idV = 114f -a $idP = 68a2 ]; then
		SIERRAID=1
	elif [ $idV = 413c -a $idP = 81a8 ]; then
		SIERRAID=1
	elif [ $idV = 413c -a $idP = 81b6 ]; then
		SIERRAID=1
	fi
}

chktelitmbim() {
	TELITMBIM=0
	if [ $idV = 1bc7 -a $idP = 0032 ]; then
		TELITMBIM=1
	fi
}

chkT77() {
	T77=0
	if [ $idV = 413c -a $idP = 81d7 ]; then
		T77=1
	elif [ $idV = 413c -a $idP = 81d8 ]; then
		T77=1
	elif [ $idV = 0489 -a $idP = e0b4 ]; then
		T77=1
	elif [ $idV = 0489 -a $idP = e0b5 ]; then
		T77=1
	elif [ $idV = 1bc7 -a $idP = 1910 ]; then
		T77=1
	fi
	if [ $T77 = 1 ]; then
		[ -n "$TTYDEVS" ] || T77=0
	fi
}

chkraw() {
	RAW=0
	if [ $idV = 03f0 -a $idP = 0857 ]; then
		RAW=1
	elif [ $idV = 1bc7 -a $idP = 1900 ]; then
		RAW=1
	elif [ $idV = 1bc7 -a $idP = 1910 ]; then
		RAW=1
	elif [ $idV = 19d2 -a $idP = 1432 ]; then
		RAW=1
	elif [ $idV = 1e0e -a $idP = 9001 ]; then
		RAW=1
	elif [ $idV = 2c7c ]; then
		RAW=1
	elif [ $idV = 05c6 -a $idP = 9025 ]; then
		[ $MAN = "Telit" ] || RAW=1
	elif [ $idV = 05c6 -a $idP = 90db ]; then
		RAW=1
	elif [ $idV = 05c6 -a $idP = f601 ]; then
		RAW=1
	elif [ $idV = 2cb7 -a $idP = 0104 ]; then
		RAW=1
	elif [ $idV = 413c -a $idP = 81d7 ]; then
		RAW=1
	elif [ $idV = 413c -a $idP = 81e0 ]; then
		RAW=1
	elif [ $idV = 12d1 -a $idP = 1506 ]; then
		RAW=1
	fi
}

chkreg() {
	local OX REGV REGST REGCMD
	local COMMPORT="/dev/ttyUSB"$CPORT
	ATCMDD="AT+CEREG?;+CREG?"
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	for REGCMD in +CEREG +CREG; do
		REGV=$(echo "$OX" | grep -o "$REGCMD: [0-3],[0-9]")
		if [ -n "$REGV" ]; then
			REGST=$(echo "$REGV" | cut -d, -f2)
			case $REGST in
			"0" )
				continue
				;;
			"1"|"5"|"6"|"7" )
				REGOK=1
				break
				;;
			* )
				REGOK=0
				;;
			esac
		fi
	done
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

get_tty() {
# $1 is bInterfaceNumber value
	local IFNUM OX
	IFNUM=$1
	for TTYD in $(echo "$TTYDEVS"); do
		if [ ! "$ACM" = 1 ]; then
			OX=$(cat /sys/class/tty/$TTYD/../../../bInterfaceNumber | grep "$IFNUM")
		else
			OX=$(cat /sys/class/tty/$TTYD/../../bInterfaceNumber | grep "$IFNUM")
		fi
		if [ $? = 0 ]; then
			CPORT=$(echo $TTYD | grep -o "[[:digit:]]\+")
			break
		else
			CPORT=""
		fi
	done
}

get_tty_fix() {
# $1 is fixed ttyUSB or ttyACM port number
	local POS
	POS=`expr 1 + $1`
	CPORT=$(echo "$TTYDEVS" | cut -d' ' -f"$POS" | grep -o "[[:digit:]]\+")
}

get_tty_ncm() {
	local IFPROT OX
	PROTS="12 62 02 2"	# PC UI interface bInterfaceProtocol value
	for IFPROT in $PROTS; do
		for TTYD in $(echo "$TTYDEVS"); do
			OX=$(cat /sys/class/tty/$TTYD/../../../bInterfaceProtocol | grep -w "$IFPROT")
			if [ $? = 0 ]; then
				CPORT=$(echo $TTYD | grep -o "[[:digit:]]\+")
				break 2
			else
				CPORT=""
			fi
		done
	done
}

mbimcport() {
	lua $ROOTER/common/modemchk.lua "$idV" "$idP" "$CPORT" "$CPORT"
	source /tmp/parmpass
	uci set modem.modem$CURRMODEM.commport=$CPORT
	uci set modem.modem$CURRMODEM.proto="30"
	log "MBIM Comm Port : /dev/ttyUSB$CPORT"
}

CURRMODEM=$1
RECON=$2
SIERRAID=0

MAN=$(uci -q get modem.modem$CURRMODEM.manuf)
MOD=$(uci -q get modem.modem$CURRMODEM.model)
PROT=$(uci -q get modem.modem$CURRMODEM.proto)
idV=$(uci -q get modem.modem$CURRMODEM.idV)
idP=$(uci -q get modem.modem$CURRMODEM.idP)

if [ ! -z "$RECON" ]; then
	$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "ReConnecting"
	uci set modem.modem$CURRMODEM.active=1
	uci set modem.modem$CURRMODEM.connected=0
	uci commit modem
	INTER=$(uci -q get modem.modeminfo$CURRMODEM.inter)
	jkillall getsignal$CURRMODEM
	rm -f $ROOTER_LINK/getsignal$CURRMODEM
	jkillall con_monitor$CURRMODEM
	rm -f $ROOTER_LINK/con_monitor$CURRMODEM
	jkillall mbim_monitor$CURRMODEM
	rm -f $ROOTER_LINK/mbim_monitor$CURRMODEM
	ifdown wan$INTER
	CPORT=$(uci -q get modem.modem$CURRMODEM.commport)
	WWANX=$(uci -q get modem.modem$CURRMODEM.wwan)
	WDMNX=$(uci -q get modem.modem$CURRMODEM.wdm)
	if [ "$RECON" = "1" ]; then
		$ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "reset.gcom" "$CURRMODEM"
	fi
else

	DELAY=$(uci -q get modem.modem$CURRMODEM.delay)
	if [ -z "$DELAY" ]; then
		DELAY=5
	fi

rm -f /tmp/usbwait

MATCH="$(uci get modem.modem$CURRMODEM.maxcontrol | cut -d/ -f3- | xargs dirname)"

case $PROT in
	# Sierra Direct-IP data interface

	"1" )
	OX="$(for a in /sys/class/net/*; do readlink $a; done | grep "$MATCH")"
	ifname=$(basename $OX)
	WWANX=$(echo $ifname | grep -o "[[:digit:]]")
	log "Modem $CURRMODEM Sierra Direct-IP Device : $ifname"

	uci set modem.modem$CURRMODEM.wwan=$WWANX
	uci set modem.modem$CURRMODEM.interface=$ifname
	uci commit modem
	;;

	# QMI, NCM and MBIM use cdc-wdm

	"2"|"3"|"30"|"4"|"6"|"7" )
	OX="$(for a in /sys/class/usbmisc/*; do readlink $a; done | grep "$MATCH")"
	devname=$(basename $OX)
	log "Modem $CURRMODEM WDM Device : $devname"
	WDMNX=$(echo $devname | grep -o "[[:digit:]]")
	ifname="$(ls /sys/class/usbmisc/$devname/device/net/)"
	WWANX=$(echo $ifname | grep -o "[[:digit:]]")

	uci set modem.modem$CURRMODEM.wdm=$WDMNX
	uci set modem.modem$CURRMODEM.wwan=$WWANX
	uci set modem.modem$CURRMODEM.interface=$ifname
	uci commit modem
	;;
esac

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
	log "No Comm Ports"
fi

get_tty_fix 0
if [ -n "$CPORT" ]; then
	uci set modem.modem$CURRMODEM.baseport=$CPORT
else
	uci set modem.modem$CURRMODEM.baseport=""
fi
uci commit modem.modem$CURRMODEM

	case $PROT in
#
# Sierra Direct-IP modem comm port
#
	"1" )
		log "Start Direct-IP Connection"
		get_tty 03
		lua $ROOTER/common/modemchk.lua "$idV" "$idP" "$CPORT" "$CPORT"
		source /tmp/parmpass
		log "Sierra Comm Port : /dev/ttyUSB$CPORT"
		;;
#
# QMI modem comm port
#
	"2" )
		log "Start QMI Connection"
		sleep $DELAY

		chksierra
		if [ $SIERRAID -eq 1 ]; then
			get_tty 03
		elif [ $idV = 1bc7 ]; then
			get_tty 03
		else
			if [ $idV = 2c7c ]; then
				QUEIF2="0121 0125 0306 0296 0512 0620 0800 030b 0801 0900"
				if [[ $(echo "$QUEIF2" | grep -o -i "$idP") ]]; then
					TPORT=2
				fi
			elif [ $idV = 05c6 -a $idP = 9025 ]; then
				[ $MAN = "Telit" ] || TPORT=2
			elif [ $idV = 1e0e -a $idP = 9001 ]; then
				TPORT=2
			else
				TPORT=1
			fi
			get_tty_fix $TPORT
		fi

		lua $ROOTER/common/modemchk.lua "$idV" "$idP" "$CPORT" "$CPORT"
		source /tmp/parmpass

		log "Modem $CURRMODEM QMI Comm Port : /dev/ttyUSB$CPORT"
		chkraw
		;;
	"3"|"30" )
		log "Start MBIM Connection"
		sleep $DELAY

		chksierra
		if [ $SIERRAID -eq 1 ]; then
			SIERRAIF2='1199:90b1'
			if [[ $(echo $SIERRAIF2 | grep -o -i "$idV:$idP") ]]; then
				IFNUM=02
			else
				IFNUM=03
			fi
			get_tty $IFNUM
			if [ -z "$CPORT" ]; then
				if [ $idP = "90d3" ]; then
					get_tty_fix 0
					lua $ROOTER/common/modemchk.lua "$idV" "$idP" "$CPORT" "$CPORT"
					source /tmp/parmpass
					uci set modem.modem$CURRMODEM.commport=$CPORT
					if [ -n "$CPORT" ]; then
						uci set modem.modem$CURRMODEM.proto="30"
					fi
					log "Modem $CURRMODEM MBIM Comm Port : /dev/ttyUSB$CPORT"
				else
					uci set modem.modem$CURRMODEM.commport=""
					uci set modem.modem$CURRMODEM.proto="3"
					log "No MBIM Comm Port"
				fi
			else
				mbimcport
			fi
		else
			chktelitmbim
			if [ $TELITMBIM -eq 1 ]; then
				get_tty 00
				lua $ROOTER/common/modemchk.lua "$idV" "$idP" "$CPORT" "$CPORT"
				source /tmp/parmpass
				ACMPORT=$CPORT
				CPORT=9$ACMPORT
				ln -fs /dev/ttyACM$ACMPORT /dev/ttyUSB$CPORT
				uci set modem.modem$CURRMODEM.commport=$CPORT
				if [ -n "$CPORT" ]; then
					uci set modem.modem$CURRMODEM.proto="30"
				fi
				log "Modem $CURRMODEM MBIM Comm Port : /dev/ttyUSB$CPORT"
			else
				chkT77
				if [ $T77 -eq 1 ]; then
					get_tty 02
					mbimcport
				else
					case $idV in
						"2c7c"|"05c6" )
							get_tty_fix 2
							mbimcport
						;;
						"03f0" )
							get_tty 02
							mbimcport
						;;
						"1bc7" )
							if [ "$idP" = "1041" ]; then
								get_tty 07
							else
								get_tty 02
							fi
							mbimcport
						;;
						"1e0e" )
							get_tty 02
							mbimcport
						;; 
						"2cb7" )
							get_tty_fix 0
							lua $ROOTER/common/modemchk.lua "$idV" "$idP" "$CPORT" "$CPORT"
							source /tmp/parmpass
							ACMPORT=$CPORT
							CPORT="8$ACMPORT"
							ln -fs /dev/ttyACM$ACMPORT /dev/ttyUSB$CPORT

							uci set modem.modem$CURRMODEM.commport=$CPORT
							uci set modem.modem$CURRMODEM.proto="30"
							log "Modem $CURRMODEM MBIM Comm Port : /dev/ttyUSB$CPORT"
						;;
						* )
							uci set modem.modem$CURRMODEM.commport=""
							log "No MBIM Comm Port"
						;;
					esac
				fi
			fi
		fi
		uci commit modem
		;;
#
# Huawei NCM
#
	"4"|"6"|"7"|"24"|"26"|"27" )
		if [ "$idV" = "2c7c" -a "$idP" = "0900" ]; then
			ATCMDD='AT+QCFG="usbnet",2'
			OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB2" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
			ATCMDD='AT+CFUN=1,1'
			OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB2" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
			reboot -f
		fi
		log "Start NCM Connection"
		sleep $DELAY

		get_tty_ncm
		lua $ROOTER/common/modemchk.lua "$idV" "$idP" "$CPORT" "$CPORT"
		source /tmp/parmpass

		log "NCM Comm Port : /dev/ttyUSB$CPORT"
		;;
	"28" )
		log "Start Fibocom NCM Connection"
		get_tty_fix 2
		lua $ROOTER/common/modemchk.lua "$idV" "$idP" "$CPORT" "$CPORT"
		source /tmp/parmpass
		ACMPORT=$CPORT
		CPORT="8$ACMPORT"
		ln -fs /dev/ttyACM$ACMPORT /dev/ttyUSB$CPORT
		log "Modem $CURRMODEM Fibocom NCM Comm Port : /dev/ttyUSB$CPORT"
		;;
	esac

	uci set modem.modem$CURRMODEM.commport=$CPORT
	uci commit modem

fi
if [ $PROT = "3" ]; then
# May have got changed to 30 above
	PROT=$(uci -q get modem.modem$CURRMODEM.proto)
fi
if [ -z "$idV" ]; then
	idV=$(uci -q get modem.modem$CURRMODEM.idV)
fi
QUECTEL=false
if [ "$idV" = "2c7c" ]; then
	QUECTEL=true
elif [ "$idV" = "05c6" ]; then
	QUELST="9090,9003,9215"
	if [[ $(echo "$QUELST" | grep -o "$idP") ]]; then
		QUECTEL=true
	fi
fi

if [ -e $ROOTER/connect/preconnect.sh ]; then
	if [ "$RECON" != "2" ]; then
		$ROOTER/connect/preconnect.sh $CURRMODEM
	fi
fi

if $QUECTEL; then
	if [ "$RECON" != "2" ]; then
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
	fi
	$ROOTER/connect/bandmask $CURRMODEM 1
	clck=$(uci -q get custom.bandlock.cenable$CURRMODEM)
	if [ $clck = "1" ]; then
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
		if [ $ear1 != "0" -a $pc1 != "0" ]; then
			earcnt=$earcnt","$ear1","$pc1
			let cnt=cnt+1
		fi
		if [ $ear2 != "0" -a $pc2 != "0" ]; then
			earcnt=$earcnt","$ear2","$pc2
			let cnt=cnt+1
		fi
		if [ $ear3 != "0" -a $pc3 != "0" ]; then
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
fi
$ROOTER/luci/celltype.sh $CURRMODEM
if [ $SIERRAID -eq 1 ]; then
	$ROOTER/connect/bandmask $CURRMODEM 0
	$ROOTER/luci/celltype.sh $CURRMODEM
fi
if [ $idV = "2dee" ]; then
	ATC="AT^MODE=0"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATC")
fi
if [ $idV = "2cb7" -o $idV = "8087" ]; then
	$ROOTER/connect/bandmask $CURRMODEM 2
fi

chkT77
if [ $T77 -eq 1 ]; then
	$ROOTER/connect/bandmask $CURRMODEM 3
fi

if [ $idV = "1bc7" ]; then
	if [ $idP = "1040" -o $idP = "1041" ]; then
		$ROOTER/connect/bandmask $CURRMODEM 4
	fi
fi

CHKPORT=$(uci -q get modem.modem$CURRMODEM.commport)
if [ -n "$CHKPORT" ]; then
	$ROOTER/common/gettype.sh $CURRMODEM
	$ROOTER/connect/get_profile.sh $CURRMODEM
	if [ -e $ROOTER/simlock.sh ]; then
		$ROOTER/simlock.sh $CURRMODEM
	fi

	if [ -e /tmp/simpin$CURRMODEM ]; then
		log " SIM Error"
		if [ -e $ROOTER/simerr.sh ]; then
			$ROOTER/simerr.sh $CURRMODEM
		fi
		exit 0
	fi
	if [ -e /usr/lib/gps/gps.sh ]; then
		/usr/lib/gps/gps.sh $CURRMODEM &
	fi
	INTER=$(uci -q get modem.modeminfo$CURRMODEM.inter)
	[ $INTER = 3 ] && log "Modem $CURRMODEM disabled in Connection Profile" && exit 1
	$ROOTER/sms/check_sms.sh $CURRMODEM &
	get_connect
	if [ -z "$INTER" ]; then
		INTER=$CURRMODEM
	else
		if [ $INTER = 0 ]; then
			INTER=$CURRMODEM
		fi
	fi
	log "Profile for Modem $CURRMODEM sets interface to WAN$INTER"
	OTHER=1
	if [ $CURRMODEM = 1 ]; then
		OTHER=2
	fi
	EMPTY=$(uci -q get modem.modem$OTHER.empty)
	if [ $EMPTY = 0 ]; then
		OINTER=$(uci -q get modem.modem$OTHER.inter)
		if [ ! -z "$OINTER" ]; then
			if [ $INTER = $OINTER ]; then
				INTER=1
				if [ $OINTER = 1 ]; then
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

	DHCP=1
	if [ $PROT = 28 ]; then
		DHCP=0
	elif [ $PROT = 2 -a $idV = 05c6 -a $idP = 9025 ]; then
		[ $MAN = "Telit" ] || DHCP=0
	fi
	NODHCP=$(uci -q get modem.modeminfo$CURRMODEM.nodhcp)
	if [ $idV = "2c7c" -a $idP = "0801" ]; then
		NODHCP="1"
	fi
	if [ "$NODHCP" = "1" ]; then
		DHCP=0
		log "Using QMI without DHCP"
	fi

	if [ $DHCP = 1 ]; then
		uci delete network.wan$INTER
		uci set network.wan$INTER=interface
		uci set network.wan$INTER.proto=dhcp
		uci set network.wan$INTER.${ifname1}=$ifname
		uci set network.wan$INTER._orig_bridge=false
		uci set network.wan$INTER.metric=$INTER"0"
		set_dns
		uci commit network
	else
		set_dns
	fi

	ttl=$(uci -q get modem.modeminfo$CURRMODEM.ttl)
	if [ -z "$ttl" ]; then
		ttl="0"
	fi
	$ROOTER/connect/handlettl.sh $CURRMODEM "$ttl" &

	if [ -e $ROOTER/changedevice.sh ]; then
		$ROOTER/changedevice.sh $ifname
	fi

	autoapn=$(uci -q get profile.disable.autoapn)
	imsi=$(uci -q get modem.modem$CURRMODEM.imsi)
	mcc6=${imsi:0:6}
	mcc5=${imsi:0:5}
	apd=0
	if [ -e /usr/lib/autoapn/apn.data ]; then
		apd=1
	fi
	if [ "$autoapn" = "1" -a $apd -eq 1 ]; then
		isplist=$(grep -F "$mcc6" '/usr/lib/autoapn/apn.data')
		if [ -z "$isplist" ]; then
			isplist=$(grep -F "$mcc5" '/usr/lib/autoapn/apn.data')
			if [ -z "$isplist" ]; then
				isplist="000000,$NAPN,Default,$NPASS,$CID,$NUSER,$NAUTH"
			fi
		fi
	else
		isplist="000000,$NAPN,Default,$NPASS,$CID,$NUSER,$NAUTH"
	fi

	uci set modem.modeminfo$CURRMODEM.isplist="$isplist"
	uci commit modem

	if [ $idV = 12d1 ]; then
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "curc.gcom" "$CURRMODEM")
		log "Huawei Unsolicited Responses Disabled"
		ATCMDD="AT^USSDMODE=0"
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	fi
	FORCE=$(uci -q get modem.modeminfo$CURRMODEM.ppp)
	if [ -n "$FORCE" ]; then
		if [ $FORCE = 1 ]; then
			log "Forcing PPP mode"
			case $idV in
			"12d1" )
				retval=10
				;;
			* )
				retval=11
				;;
			esac
			uci set modem.modem$CURRMODEM.proto=$retval
			rm -f $ROOTER_LINK/create_proto$CURRMODEM
			log "Forced Protcol Value : $retval"
			log "Connecting a PPP Modem"
			ln -fs $ROOTER/ppp/create_ppp.sh $ROOTER_LINK/create_proto$CURRMODEM
			$ROOTER_LINK/create_proto$CURRMODEM $CURRMODEM &
			exit 0
		fi
	fi
fi

if $QUECTEL; then
	ATCMDD="AT+QINDCFG=\"all\",1"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
fi

modis=$(uci -q get basic.basic.modem)
if [ ! -z $modis ]; then
	if [ $modis = "0" ]; then
		log "Modem Disabled"
		exit 0
	fi
fi

for isp in $isplist 
do
	NAPN=$(echo $isp | cut -d, -f2)
	NPASS=$(echo $isp | cut -d, -f4)
	CID=$(echo $isp | cut -d, -f5)
	NUSER=$(echo $isp | cut -d, -f6)
	NAUTH=$(echo $isp | cut -d, -f7)
	if [ "$NPASS" = "nil" ]; then
		NPASS="NIL"
	fi
	if [ "$NUSER" = "nil" ]; then
		NUSER="NIL"
	fi
	if [ "$NAUTH" = "nil" ]; then
		NAUTH="0"
	fi
	export SETAPN=$NAPN
	export SETUSER=$NUSER
	export SETPASS=$NPASS
	export SETAUTH=$NAUTH
	export PINCODE=$PINC
	
	uci set modem.modem$CURRMODEM.apn=$NAPN
	uci set modem.modem$CURRMODEM.user=$NUSER
	uci set modem.modem$CURRMODEM.passw=$NPASS
	uci set modem.modem$CURRMODEM.auth=$NAUTH
	uci set modem.modem$CURRMODEM.pin=$PINC
	uci commit modem

	concount=1
	while [ "$concount" -lt 3 ]; do
		case $PROT in
		"1" )
			OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "auto.gcom" "$CURRMODEM")
			chcklog "$OX"
			M7=$(echo "$OX" | sed -e "s/SCPROF:/SCPROF: /;s!  ! !g")
			AU=$(echo "$M7" | awk -F[,\ ] '/^\!SCPROF:/ {print $4}')
			if [ $AU = "1" ]; then
				AUTO="1"
				log "Autoconnect is Enabled"
			else
				AUTO="0"
				log "Autoconnect is not Enabled"
			fi
			;;
		esac
		uci set modem.modem$CURRMODEM.auto=$AUTO
		uci commit modem

		case $PROT in
	#
	# Check provider Lock
	#
		"1"|"2"|"4"|"6"|"7"|"24"|"26"|"27"|"30"|"28" )
			$ROOTER/common/lockchk.sh $CURRMODEM
			;;
		* )
			log "No Provider Lock Done"
			;;
		esac

		case $PROT in
	#
	# Sierra and NCM uses separate Pincode setting
	#
		"1"|"4"|"6"|"7"|"24"|"26"|"27"|"28" )
			if [ -n "$PINC" ]; then
				OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "setpin.gcom" "$CURRMODEM")
				chcklog "$OX"
				ERROR="ERROR"
				if `echo $OX | grep "$ERROR" 1>/dev/null 2>&1`
				then
					log "Modem $CURRMODEM Failed to Unlock SIM Pin"
					$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Failed to Connect : Pin Locked"
					exit 0
				fi
			fi
			;;
		* )
			log "Pincode in script"
			;;
		esac
		$ROOTER/log/logger "Attempting to Connect Modem #$CURRMODEM"
		log "Attempting to Connect Modem $CURRMODEM"

		if [ -e $ROOTER/modem-led.sh ]; then
			$ROOTER/modem-led.sh $CURRMODEM 2
		fi
		
		if [ -e $ROOTER/connect/chkconn.sh ]; then
			jkillall chkconn.sh
			$ROOTER/connect/chkconn.sh $CURRMODEM &
		fi

		BRK=0
		case $PROT in
	#
	# Sierra connect script
	#
		"1" )
			if [ $AUTO = "0" ]; then
				OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "connect-directip.gcom" "$CURRMODEM")
				chcklog "$OX"
				ERROR="ERROR"
				if `echo $OX | grep "$ERROR" 1>/dev/null 2>&1`
				then
					BRK=1
					$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Failed to Connect : Retrying"
				fi
				M7=$(echo "$OX" | sed -e "s/SCACT:/SCACT: /;s!  ! !g")
				SCACT="!SCACT: 1,1"
				if `echo ${M7} | grep "$SCACT" 1>/dev/null 2>&1`
				then
					BRK=0
					ifup wan$INTER
					sleep 20
				else
					BRK=1
					$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Failed to Connect : Retrying"
				fi
			else
				ifup wan$INTER
				sleep 20
			fi
			;;
	#
	# QMI connect script
	#
		"2" )
			check_apn
			$ROOTER/qmi/connectqmi.sh $CURRMODEM cdc-wdm$WDMNX $NAUTH $NAPN $NUSER $NPASS $RAW $DHCP $PINC
			if [ $? = 0 ]; then
				ifup wan$INTER
				[ -f /tmp/ipv6supp$INTER ] && addv6
			else
				#log "Restart Modem"
				#/usr/lib/rooter/luci/restart.sh $CURRMODEM
				exit 0
			fi
			;;
	#
	# NCM connect script
	#
		"4"|"6"|"7"|"24"|"26"|"27" )
			OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "ati")
			E5372=$(echo $OX | grep "E5372")
			R215=$(echo $OX | grep "R215")
			E5787=$(echo $OX | grep "E5787")
			check_apn
			if [ -n "$E5372" -o -n "$R215" -o -n "$E5787" ]; then
				ifup wan$INTER
				BRK=0
			else
				OX=$($ROOTER/gcom/gcom-locked "/dev/cdc-wdm$WDMNX" "connect-ncm.gcom" "$CURRMODEM")
				chcklog "$OX"
				ERROR="ERROR"
				if `echo $OX | grep "$ERROR" 1>/dev/null 2>&1`
				then
					OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "connect-ncm.gcom" "$CURRMODEM")
					chcklog "$OX"
				fi
				ERROR="ERROR"
				if `echo $OX | grep "$ERROR" 1>/dev/null 2>&1`
				then
					BRK=1
					$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Failed to Connect : Retrying"
				else
					ifup wan$INTER
					sleep 25
					OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "cgpaddr.gcom" "$CURRMODEM")
					chcklog "$OX"
					OX=$($ROOTER/common/processat.sh "$OX")
					STATUS=$(echo "$OX" | awk -F[,\ ] '/^\^SYSINFOEX:/ {print $2}' | sed 's/"//g')
					DOMAIN=$(echo "$OX" | awk -F[,\ ] '/^\^SYSINFOEX:/ {print $3}' | sed 's/"//g')
					if [ "x$STATUS" = "x" ]; then
						STATUS=$(echo "$OX" | awk -F[,\ ] '/^\^SYSINFO:/ {print $2}')
						DOMAIN=$(echo "$OX" | awk -F[,\ ] '/^\^SYSINFO:/ {print $3}')
					fi
					CGPADDR="+CGPADDR:"
					if `echo $OX | grep "$CGPADDR" 1>/dev/null 2>&1`
					then
						if [ $STATUS = "2" ]; then
							if [ $DOMAIN = "1" ]; then
								BRK=0
							else
								if [ $DOMAIN = "2" ]; then
									BRK=0
								else
									if [ $DOMAIN = "3" ]; then
										BRK=0
									else
										BRK=1
										$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Network Error : Retrying"
									fi
								fi
							fi
						else
							BRK=1
							$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Network Error : Retrying"
						fi
					else
						BRK=1
						$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "No IP Address : Retrying"
					fi
				fi
			fi
			[ $BRK = 0 ] && addv6
			;;
	#
	# Fibocom NCM connect
	#
		"28" )
			OX="$(for a in /sys/class/net/*; do readlink $a; done | grep "$MATCH" | grep ".6/net/")"
			ifname=$(basename $OX)
			log "Modem $CURRMODEM Fibocom NCM Data Port : $ifname"
			COMMPORT="/dev/ttyUSB"$CPORT
			ATCMDD="AT+CGACT=0,$CID"
			OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
			check_apn
			ATCMDD="AT+CGPIAF=1,0,0,0;+XDNS=$CID,1;+XDNS=$CID,2"
			OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
			ATCMDD="AT+CGACT=1,$CID"
			OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
			ERROR="ERROR"
			if [ -e /tmp/simerr$CURRMODEM ]; then
				SIMFAIL=1
				log "SIM card error"
			else
				chkreg
				[ "$REGOK" != 1 ] && log "Subscriber registration failed"
			fi
			if [ "$SIMFAIL" = 1 -o "$REGOK" != 1 ]; then
				BRK=1
				$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Failed to Connect : Retrying"
			elif `echo "$OX" | grep -q "$ERROR"`; then
				BRK=1
				$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Failed to Connect : Retrying"
			else
				ATCMDD="AT+CGCONTRDP=$CID"
				OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
				if `echo "$OX" | grep -q "$ERROR"`; then
					log "Failed to get IP information for context $CID"
					BRK=1
					$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Failed to get IP information : Retrying"
				else
					OX=$(echo "${OX//[\" ]/}")
					ip=$(echo $OX | cut -d, -f4 | grep -o "[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}")
					ip=$(echo $ip | cut -d' ' -f1)
					DNS1=$(echo $OX | cut -d, -f6)
					DNS2=$(echo $OX | cut -d, -f7)
					OX6=$(echo $OX | grep -o "+CGCONTRDP:$CID,[0-9]\+,[^,]\+,[0-9A-F]\{1,4\}:[0-9A-F]\{1,4\}.\+")
					ip6=$(echo $OX6 | grep -o "[0-9A-F]\{1,4\}:[0-9A-F]\{1,4\}:[0-9A-F]\{1,4\}:[0-9A-F]\{1,4\}:[0-9A-F]\{1,4\}:[0-9A-F]\{1,4\}:[0-9A-F]\{1,4\}:[0-9A-F]\{1,4\}")
					ip6=$(echo $ip6 | cut -d' ' -f1)
					DNS3=$(echo "$OX6" | cut -d, -f6)
					DNS4=$(echo "$OX6" | cut -d, -f7)

					log "IP address(es): $ip $ip6"
					log "DNS servers 1&2: $DNS1 $DNS2"
					log "DNS servers 3&4: $DNS3 $DNS4"

					if [[ $(echo "$ip6" | grep -o "^[23]") ]]; then
						# Global unicast IP acquired
						v6cap=1
					elif [[ $(echo "$ip6" | grep -o "^[0-9a-fA-F]\{1,4\}:") ]]; then
						# non-routable address
						v6cap=2
					else
						v6cap=0
					fi

					if [ -n "$ip6" -a -z "$ip" ]; then
						log "Running IPv6-only mode"
						nat46=1
					fi

					ATCMDD="AT+XDATACHANNEL=1,1,\"/USBCDC/2\",\"/USBHS/NCM/0\",2,$CID"
					OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
					RDNS=$(uci -q get network.wan$INTER.dns)

					log "Applying IP settings to wan$INTER"
					uci delete network.wan$INTER
					uci set network.wan$INTER=interface
					uci set network.wan$INTER.proto=static
					uci set network.wan$INTER.${ifname1}=$ifname
					uci set network.wan$INTER.metric=$INTER"0"
					if [ -n "$ip" ]; then
						uci set network.wan$INTER.ipaddr=$ip/32
						uci set network.wan$INTER.gateway='0.0.0.0'
					fi
					if [ "$v6cap" -gt 0 ]; then
						uci set network.wan$INTER.ip6addr=$ip6/128
					fi

					if [ -n "$RDNS" ]; then
						uci set network.wan$INTER.dns="$RDNS"
					else
						set_dns2
					fi

					uci commit network
					uci set modem.modem$CURRMODEM.interface=$ifname
					uci commit modem
					ip link set dev $ifname arp off
					ATCMDD="AT+CGDATA=\"M-RAW_IP\",$CID"
					OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "raw-ip.gcom" "$CURRMODEM" "$ATCMDD")
					RESP=$(echo $OX | sed "s/AT+CGDATA=\"M-RAW_IP\",$CID //")
					log "Final Modem $CURRMODEM result code is \"$RESP\""
					if [ "$RESP" = "OK CONNECT" ]; then
						ifup wan$INTER
						if [ -e /sys/class/net/$ifname/cdc_ncm/tx_timer_usecs ]; then
							echo "0" > /sys/class/net/$ifname/cdc_ncm/tx_timer_usecs
						fi
						[ $v6cap = 2 ] && addv6
						sleep 2
						BRK=0
					else
						BRK=1
						$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Failed to Connect : Retrying"
					fi
				fi
			fi
			;;
	#
	# MBIM connect script
	#
		"3"|"30" )
			if [ -n "$CPORT" ]; then
				check_apn
			fi
			log "Using Netifd Method"
			uci delete network.wan$INTER
			uci set network.wan$INTER=interface
			uci set network.wan$INTER.proto=mbim
			uci set network.wan$INTER.device=/dev/cdc-wdm$WDMNX
			uci set network.wan$INTER.metric=$INTER"0"
			uci set network.wan$INTER.currmodem=$CURRMODEM
			uci -q commit network
			rm -f /tmp/usbwait
			ifup wan$INTER
			MIFACE=$(uci -q get modem.modem$CURRMODEM.interface)
			if [ -e /sys/class/net/$MIFACE/cdc_ncm/tx_timer_usecs ]; then
				echo "0" > /sys/class/net/$MIFACE/cdc_ncm/tx_timer_usecs
			fi
			exit 0
			;;
		esac

		if [ $BRK = 1 ]; then
			ATCMDD="AT+COPS=0"
			OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
			$ROOTER/log/logger "Retry Connection with Modem #$CURRMODEM"
			log "Retry Connection"
			sleep 10
			concount=$((concount+1))
		else
			$ROOTER/log/logger "Modem #$CURRMODEM Connected"
			log "Modem $CURRMODEM Connected"
			break
		fi
	done
	if [ $BRK = 0 ]; then
		break
	fi
done

if [ $BRK = 1 ]; then
	exit 0
fi

if [ -e $ROOTER/modem-led.sh ]; then
	$ROOTER/modem-led.sh $CURRMODEM 3
fi

case $PROT in
#
# Sierra, NCM and QMI use modemsignal.sh and reconnect.sh
#
	"1"|"2"|"4"|"6"|"7"|"24"|"26"|"27"|"28" )
		ln -fs $ROOTER/signal/modemsignal.sh $ROOTER_LINK/getsignal$CURRMODEM
		ln -fs $ROOTER/connect/reconnect.sh $ROOTER_LINK/reconnect$CURRMODEM
		# send custom AT startup command
		if [ $(uci -q get modem.modeminfo$CURRMODEM.at) -eq "1" ]; then
			ATCMDD=$(uci -q get modem.modeminfo$CURRMODEM.atc)
			if [ ! -z "$ATCMDD" ]; then
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
		;;
esac

	$ROOTER_LINK/getsignal$CURRMODEM $CURRMODEM $PROT &
	ln -fs $ROOTER/connect/conmon.sh $ROOTER_LINK/con_monitor$CURRMODEM
	$ROOTER_LINK/con_monitor$CURRMODEM $CURRMODEM &
	uci set modem.modem$CURRMODEM.connected=1
	uci commit modem

	if [ -e $ROOTER/connect/postconnect.sh ]; then
		$ROOTER/connect/postconnect.sh $CURRMODEM
	fi

	if [ -e /etc/bandlock ]; then
		M1='AT+COPS=?'
		export TIMEOUT="120"
		#OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$M1")
		export TIMEOUT="5"
	fi

	if [ -e $ROOTER/timezone.sh ]; then
		TZ=$(uci -q get modem.modeminfo$CURRMODEM.tzone)
		if [ "$TZ" = "1" ]; then
			log "Set TimeZone"
			$ROOTER/timezone.sh &
		fi
	fi

	CLB=$(uci -q get modem.modeminfo$CURRMODEM.lb)
	CLB=1
	if [ -e /etc/config/mwan3 ]; then
		ENB=$(uci -q get mwan3.wan$INTER.enabled)
		if [ ! -z "$ENB" ]; then
			if [ $CLB = "1" ]; then
				uci set mwan3.wan$INTER.enabled=1
			else
				uci set mwan3.wan$INTER.enabled=0
			fi
			uci commit mwan3
			/usr/sbin/mwan3 restart
		fi
	fi
