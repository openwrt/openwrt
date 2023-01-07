#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "hostless " "$@"
}

TMPFILE="/tmp/XXXXXX"
TMPFILE1="/tmp/XXXXXX1"

make_status() {
	echo "$IP" > /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "$CSQ" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "$MODEM" >> /tmp/status$CURRMODEM.file
	echo "$PROV" >> /tmp/status$CURRMODEM.file
	echo "$NETWORK" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo " " >> /tmp/status$CURRMODEM.file
	echo " " >> /tmp/status$CURRMODEM.file
	echo "$MONSTAT" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "$CONN" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "Hostless/Phone" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
}

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

get_zte_model() {
	uci set modem.modem$CURRMODEM.ztemodel=UNKNOWN
	uci commit modem
	SERIAL=$(uci get modem.modem$CURRMODEM.serial)
	if [ $PROD = 1405 ]; then
		if [ "`echo $SERIAL | grep P680A1ZTED`" != "" ]; then
			uci set modem.modem$CURRMODEM.ztemodel=MF669
			uci set modem.modem$CURRMODEM.model=MF669/MF70
			uci commit modem
		else
			uci set modem.modem$CURRMODEM.ztemodel=V5
			uci set modem.modem$CURRMODEM.model=MF823/MF825/MF93D
			uci commit modem
		fi
	fi
	if [ $PROD = 0349 ]; then
		uci set modem.modem$CURRMODEM.ztemodel=MF821D
		uci set modem.modem$CURRMODEM.model=MF821D
		uci commit modem
	fi
	if [ $PROD = 0166 ]; then
		if [ "`echo $MODEL | grep MF91`" != "" ]; then
			uci set modem.modem$CURRMODEM.ztemodel=MF91D
			uci set modem.modem$CURRMODEM.model=MF91D
			uci commit modem
		else
			if [ "`echo $MODEL | grep MF91`" != "" ]; then
				uci set modem.modem$CURRMODEM.ztemodel=MF91
				uci set modem.modem$CURRMODEM.model=MF91
				uci commit modem
			fi
		fi
	fi
	if [ $PROD = 1408 ]; then
		if [ "`echo $SERIAL | grep P680A1ZTED`" != "" ]; then
			uci set modem.modem$CURRMODEM.ztemodel=V5
			uci set modem.modem$CURRMODEM.model=MF93D
			uci commit modem
		fi
	fi
}

get_basic(){
	ZTEMOD=$(uci get modem.modem$CURRMODEM.ztemodel)
	TIMEOUT=3
	case $ZTEMOD in
	"MF669" )
		wget http://$IP/logo_data.asp -O $TMPFILE > /dev/null 2>&1 &
		handle_timeout "$!"
		LUCKNUM="`cat $TMPFILE | grep lucknum | cut -d \' -f 2`"
		;;
	"V5" )
		wget "http://$IP/goform/goform_get_cmd_process?isTest=false&cmd=network_type%2Cwan_ipaddr%2Cppp_status%2Cprefer_dns_auto%2Cstandby_dns_auto%2Csignalbar&multi_data=1&_=1376406501348" -O $TMPFILE > /dev/null 2>&1 &
		handle_timeout "$!"
		;;
	"MF821D"|"MF91D"|"MF91" )
		wget http://$IP/goform/status_update -O $TMPFILE > /dev/null 2>&1 &
		handle_timeout "$!"
		;;
	esac
	PROV="`cat $TMPFILE | grep network_provider | cut -d \' -f 2`"
	[ -z $PROV ] && PROV="-"
	rm -f $TMPFILE
}

get_signal(){
	TIMEOUT=3
	case $ZTEMOD in
	"MF669" )
		wget http://$IP/logo_data.asp -O $TMPFILE > /dev/null 2>&1 &
		handle_timeout "$!"
		SIGNAL="`cat $TMPFILE | grep signalbar | cut -d \' -f 2`"
		NETWORK="`cat $TMPFILE | grep network_type | cut -d \' -f 2`"
		;;
	"V5" )
		wget "http://$IP/goform/goform_get_cmd_process?isTest=false&cmd=signalbar,wan_csq,network_type,network_provider,ppp_status,modem_main_state,rmcc,rmnc,,domain_stat,cell_id,rssi,rscp,lte_rssi,lte_rsrq,lte_rsrp,lte_snr,ecio,sms_received_flag,sts_received_flag,simcard_roam&multi_data=1&sms_received_flag_flag=0&sts_received_flag_flag=0" -O $TMPFILE > /dev/null 2>&1 &
		handle_timeout "$!"
		SIGNAL="`cat $TMPFILE | grep signalbar'\"\:\"[^\"]*' -o | cut -d'"' -f 3`"
		NETWORK="`cat $TMPFILE | grep network_type'\"\:\"[^\"]*' -o | cut -d'"' -f 3`"
		;;
	"MF821D"|"MF91D"|"MF91" )
		wget http://$IP/goform/status_update -O $TMPFILE > /dev/null 2>&1 &
		handle_timeout "$!"
		SIGNAL="`cat $TMPFILE | cut -d \; -f 1-1`"
		net="`cat $TMPFILE | cut -d \; -f 4-4`"
		if [ -z $net ]; then
			NETWORK="-"
		else
			[ "$net" = "0" ] && NETWORK="No Service"
			[ "$net" = "1" ] && NETWORK="LTE"
			[ "$net" = "2" ] && NETWORK="EVDO"
			[ "$net" = "3" ] && NETWORK="CDMA"
			[ "$net" = "4" ] && NETWORK="WCDMA"
			[ "$net" = "5" ] && NETWORK="GSM"
			[ "$net" = "6" ] && NETWORK="HSDPA"
			[ "$net" = "7" ] && NETWORK="HUSPA"
			[ "$net" = "8" ] && NETWORK="HSPA+"
			[ "$net" = "10" ] && NETWORK="EDGE"
			[ "$net" = "11" ] && NETWORK="GPRS"
		fi
		;;
	esac
	[ -z $SIGNAL ] && SIGNAL=0
	[ -z $NETWORK ] && NETWORK="-"
	rm -f $TMPFILE
}

CURRMODEM=$1
PROTO=$2
CONN="Modem #"$CURRMODEM

PROD=$(uci get modem.modem$CURRMODEM.idP)
get_zte_model
MANUF=$(uci get modem.modem$CURRMODEM.manuf)
MODEL=$(uci get modem.modem$CURRMODEM.model)
MODEM=$MANUF" "$MODEL
IP=$(uci get modem.modem$CURRMODEM.ip)

get_basic

STARTIMEX=$(date +%s)
MONSTAT="Unknown"
rm -f /tmp/monstat$CURRMODEM

while [ 1 = 1 ]; do
	get_signal
	CSQ="$SIGNAL Bars"
	if [ -e /tmp/monstat$CURRMODEM ]; then
		source /tmp/monstat$CURRMODEM
	fi
	if [ -z $MONSTAT ]; then
		MONSTAT="Unknown"
	fi
	make_status
	CURRTIME=$(date +%s)
	let ELAPSE=CURRTIME-STARTIMEX
	while [ $ELAPSE -lt 10 ]; do
		sleep 2
		CURRTIME=$(date +%s)
		let ELAPSE=CURRTIME-STARTIMEX
	done
	STARTIMEX=$CURRTIME
done
