#!/bin/sh

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}
# DBG=-v

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	modlog "MBIM Connect $CURRMODEM" "$@"
}

enb=$(uci -q get custom.connect.ipv6)
if [ -z $enb ]; then
	enb="1"
fi

ifname1="ifname"
if [ -e /etc/newstyle ]; then
	ifname1="device"
fi

get_connect() {
	NAPN=$(uci -q get modem.modeminfo$CURRMODEM.apn)
	NAPN2=$(uci -q get modem.modeminfo$CURRMODEM.apn2)
	NUSER=$(uci -q get modem.modeminfo$CURRMODEM.user)
	NPASS=$(uci -q get modem.modeminfo$CURRMODEM.passw)
	NAUTH=$(uci -q get modem.modeminfo$CURRMODEM.auth)
	PINC=$(uci -q get modem.modeminfo$CURRMODEM.pincode)
	PDPT=$(uci -q get modem.modeminfo$CURRMODEM.pdptype)
	isplist=$(uci -q get modem.modeminfo$CURRMODEM.isplist)

	apn=$NAPN
	apn2=$NAPN2
	username="$NUSER"
	password="$NPASS"
	auth=$NAUTH
	pincode=$PINC

	if [ "$PDPT" = 0 ]; then
		ipt=""
	else
		IPVAR=$(uci -q get modem.modem$CURRMODEM.pdptype)
		case "$IPVAR" in
			"IP" )
				ipt="ipv4:"
			;;
			"IPV6" )
				ipt="ipv6:"
			;;
			"IPV4V6" )
				ipt="ipv4v6:"
			;;
		esac
	fi
}

get_sub() {
	log "Checking subscriber"
	tid=$((tid + 1))
	SUB=$(umbim $DBG -n -t $tid -d $device subscriber)
	retq=$?
	if [ $retq -ne 0 ]; then
		log "Subscriber init failed"
		proto_notify_error "$interface" NO_SUBSCRIBER
		return 1
	fi
	CNUM=$(echo "$SUB" | awk '/number:/ {print $2}')
	IMSI=$(echo "$SUB" | awk '/subscriberid:/ {print $2}')
	uci set modem.modem$CURRMODEM.imsi=$IMSI
	ICCID=$(echo "$SUB" | awk '/simiccid:/ {print $2}')
	uci set modem.modem$CURRMODEM.iccid=$ICCID
	uci commit modem
}

proto_mbim_init_config() {
	available=1
	no_device=1
	proto_config_add_string "device:device"
	proto_config_add_string apn
	proto_config_add_string apn2
	proto_config_add_string pincode
	proto_config_add_string delay
	proto_config_add_string auth
	proto_config_add_string username
	proto_config_add_string password
}

_proto_mbim_setup() {
	local interface="$1"
	local tid=2
	local ret v6cap pdns v4dns v6dns

	if [ ! -f /tmp/bootend.file ]; then
		return 0
	fi

	CURRMODEM=$(uci -q get network.$interface.currmodem)
	uci set modem.modem$CURRMODEM.connected=0
	uci commit modem
	rm -f $ROOTER_LINK/reconnect$CURRMODEM
	jkillall getsignal$CURRMODEM
	rm -f $ROOTER_LINK/getsignal$CURRMODEM
	jkillall con_monitor$CURRMODEM
	rm -f $ROOTER_LINK/con_monitor$CURRMODEM
	jkillall mbim_monitor$CURRMODEM
	rm -f $ROOTER_LINK/mbim_monitor$CURRMODEM

	local device apn pincode delay
	json_get_vars device apn apn2 pincode delay auth username password

	case $auth in
		"0" )
			auth=
		;;
		"1" )
			auth="pap"
		;;
		"2" )
			auth="chap"
		;;
		"*" )
			auth=
		;;
	esac

	IMEI="Unknown"
	IMSI="Unknown"
	ICCID="Unknown"
	CNUM="*"
	CNUMx="*"

	[ -n "$ctl_device" ] && device=$ctl_device

	[ -n "$device" ] || {
		log "No control device specified"
		proto_notify_error "$interface" NO_DEVICE
		proto_set_available "$interface" 0
		return 1
	}
	[ -c "$device" ] || {
		log "The specified control device does not exist"
		proto_notify_error "$interface" NO_DEVICE
		proto_set_available "$interface" 0
		return 1
	}

	devname="$(basename "$device")"
	devpath="$(readlink -f /sys/class/usbmisc/$devname/device/)"
	ifname="$( ls "$devpath"/net )"

	[ -n "$ifname" ] || {
		log "Failed to find matching interface"
		proto_notify_error "$interface" NO_IFNAME
		proto_set_available "$interface" 0
		return 1
	}

	[ -n "$delay" ] && sleep "$delay"

	log "Query radio state"
	umbim $DBG -n -d $device radio | grep "off"
	STATUS=$?

	[ "$STATUS" -ne 0 ] || {
		sleep 1
		log "Setting FCC Auth"
		uqmi $DBG -s -m -d $device --fcc-auth
		sleep 1
	}

	log "Reading capabilities"
	tid=$((tid + 1))
	DCAPS=$(umbim $DBG -n -t $tid -d $device caps)
	retq=$?
	if [ $retq -ne 0 ]; then

		log "Failed to read modem caps"
		tid=$((tid + 1))
		umbim $DBG -t $tid -d "$device" disconnect
		proto_notify_error "$interface" PIN_FAILED
		return 1
	fi
	CUSTOM=$(echo "$DCAPS" | awk '/customdataclass:/ {print $2}')
	IMEI=$(echo "$DCAPS" | awk '/deviceid:/ {print $2}')
	uci set modem.modem$CURRMODEM.imei=$IMEI
	echo 'CUSTOM="'"$CUSTOM"'"' > /tmp/mbimcustom$CURRMODEM

	get_sub

	if [ ! -f /tmp/profile$CURRMODEM ]; then
		$ROOTER/connect/get_profile.sh $CURRMODEM
	fi

	get_connect

	log "Checking PIN state"
	tid=$((tid + 1))
	umbim $DBG -n -t $tid -d $device pinstate
	retq=$?
	if [ $retq -eq 2 ]; then
		log "PIN is required"
		if [ ! -z $pincode ]; then
			log "Sending PIN"
			tid=$((tid + 1))
			umbim $DBG -n -t $tid -d $device unlock "$pincode" 2>/dev/null
			retq=$?
			if [ $retq -ne 0 ]; then
				log "PIN unlock failed"
				exit 1
			else
				log "PIN unlocked"
				sleep 3
				CHKPORT=$(uci get modem.modem$CURRMODEM.commport)
				if [ ! -z $CHKPORT ]; then
					$ROOTER/common/gettype.sh $CURRMODEM
				else
					get_sub
				fi
			fi
		else
			log "PIN is missing in the profile"
			exit 1
		fi
	else
		log "PIN is not required"
	fi

	log "Register with network"
	for i in $(seq 30); do
		tid=$((tid + 1))
		REG=$(umbim $DBG -n -t $tid -d $device registration)
		retq=$?
		[ $retq -ne 2 ] && break
		sleep 2
	done
	if [ $retq != 0 ]; then
		if [ $retq != 4 ]; then
			log "Subscriber registration failed"
			proto_notify_error "$interface" NO_REGISTRATION
			return 1
		fi
	fi
	MCCMNC=$(echo "$REG" | awk '/provider_id:/ {print $2}')
	PROV=$(echo "$REG" | awk '/provider_name:/ {print $2}')
	MCC=${MCCMNC:0:3}
	MNC=${MCCMNC:3}

	tid=$((tid + 1))

	log "Attach to network"
	ATTACH=$(umbim $DBG -n -t $tid -d $device attach)
	retq=$?
	if [ $retq != 0 ]; then
		log "Failed to attach to network"
		proto_notify_error "$interface" ATTACH_FAILED
		return 1
	fi
	UP=$(echo "$ATTACH" | awk '/uplinkspeed:/ {print $2}')
	DOWN=$(echo "$ATTACH" | awk '/downlinkspeed:/ {print $2}')

	tid=$((tid + 1))

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
		apn=$NAPN
		username="$NUSER"
		password="$NPASS"
		auth=$NAUTH
		case $auth in
			"0" )
				auth="none"
			;;
			"1" )
				auth="pap"
			;;
			"2" )
				auth="chap"
			;;
			"*" )
				auth="none"
			;;
		esac
		
		if [ ! -e /etc/config/isp ]; then
			log "Connect to network using $apn"
		else
			log "Connect to network"
		fi
		
		if [ ! -e /etc/config/isp ]; then
			log "$ipt $apn $auth $username $password"
		fi
		
		tidd=0
		tcnt=4
		while ! umbim $DBG -n -t $tid -d $device connect "$ipt""$apn" "$auth" "$username" "$password"; do
			tid=$((tid + 1))
			sleep 1;
			tidd=$((tidd + 1))
			if [ $tidd -gt $tcnt ]; then
				break;
			fi
		done
		if [ $tidd -le $tcnt ]; then
			break
		fi
	done
	if [ $tidd -gt $tcnt ]; then
		log "Failed to connect to network"
		return 1
	fi
	log "Save Connect Data"
	uci set modem.modem$CURRMODEM.mdevice=$device
	uci set modem.modem$CURRMODEM.mapn=$apn
	uci set modem.modem$CURRMODEM.mipt=$itp
	uci set modem.modem$CURRMODEM.mauth=$auth
	uci set modem.modem$CURRMODEM.musername=$username
	uci set modem.modem$CURRMODEM.mpassword=$password
	uci commit modem
	
	tid=$((tid + 1))

	log "Get IP config"
	CONFIG=$(umbim $DBG -n -t $tid -d $device config) || {
		log "config failed"
		return 1
	}

	IP=$(echo -e "$CONFIG"|grep "ipv4address"|grep -E -o "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)")
	GATE=$(echo -e "$CONFIG"|grep "ipv4gateway"|grep -E -o "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)")
	DNS1=$(echo -e "$CONFIG"|grep "ipv4dnsserver"|grep -E -o "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)" |sed -n 1p)
	DNS2=$(echo -e "$CONFIG"|grep "ipv4dnsserver"|grep -E -o "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)" |sed -n 2p)
	if [ $enb = "1" ]; then
		IP6=$(echo "$CONFIG" | awk '/ipv6address:/ {print $2}' | cut -d / -f 1)
		DNS3=$(echo "$CONFIG" | awk '/ipv6dnsserver:/ {print $2}' | sed -n 1p)
		DNS4=$(echo "$CONFIG" | awk '/ipv6dnsserver:/ {print $2}' | sed -n 2p)
	fi
	echo "$GATE" > /tmp/mbimgateway

	[ -n "$IP" ] && echo "IP: $IP"
	[ -n "$DNS1" ] && echo "DNS1: $DNS1"
	[ -n "$DNS2" ] && echo "DNS2: $DNS2"
	if [ $enb = "1" ]; then
		[ -n "$IP6" ] && echo "IPv6: $IP6"
		[ -n "$DNS3" ] && echo "DNS3: $DNS3"
		[ -n "$DNS4" ] && echo "DNS4: $DNS4"
	fi

	log "Connected, setting IP"

	if [ $enb = "1" ]; then
		if [ -n "$IP6" -a -z "$IP" ]; then
			log "Running IPv6-only mode"
			nat46=1
		fi

		if [[ $(echo "$IP6" | grep -o "^[23]") ]]; then
			# Global unicast IP acquired
			v6cap=1
		elif
			[[ $(echo "$IP6" | grep -o "^[0-9a-fA-F]\{1,4\}:") ]]; then
			# non-routable address
			v6cap=2
		else
			v6cap=0
		fi
	fi

	INTER=$(uci get modem.modem$CURRMODEM.inter)
	if [ -e /tmp/v4dns$INTER -o -e /tmp/v6dns$INTER ]; then
		pdns=1
		if [ -e /tmp/v4dns$INTER ]; then
			v4dns=$(cat /tmp/v4dns$INTER 2>/dev/null)
		fi
		if [ $enb = "1" ]; then
			if [ -e /tmp/v6dns$INTER ]; then
				v6dns=$(cat /tmp/v6dns$INTER 2>/dev/null)
			fi
		fi
	else
		v4dns="$DNS1 $DNS2"
		if [ $enb = "1" ]; then
			v6dns="$DNS3 $DNS4"
		fi
	fi

	proto_init_update "$ifname" 1

	if [ -n "$IP" ]; then
		proto_add_ipv4_address $IP "255.255.255.255"
		proto_add_ipv4_route "0.0.0.0" 0
	fi

	for DNSV in $(echo "$v4dns"); do
		proto_add_dns_server "$DNSV"
	done

	if [ $enb = "1" ]; then
		if [ "$v6cap" -gt 0 ]; then
			# RFC 7278: Extend an IPv6 /64 Prefix to LAN
			proto_add_ipv6_address $IP6 128
			if [ "$v6cap" = 1 ]; then
				proto_add_ipv6_prefix $IP6/64
				proto_add_ipv6_route "::0" 0 "" "" "" $IP6/64
				for DNSV in $(echo "$v6dns"); do
					proto_add_dns_server "$DNSV"
				done
			fi
		fi
	fi

	proto_add_data
		json_add_string zone wan
	proto_close_data

	proto_send_update "$interface"

	if [ $enb = "1" ]; then
		if [ "$v6cap" -gt 0 ]; then
			local zone="$(fw3 -q network "$interface" 2>/dev/null)"
		fi
		if [ "$v6cap" = 2 ]; then
			log "Adding IPv6 dynamic interface"
			json_init
			json_add_string name "${interface}_6"
			json_add_string ${ifname1} "@$interface"
			json_add_string proto "dhcpv6"
			json_add_string extendprefix 1
			[ -n "$zone" ] && json_add_string zone "$zone"
			[ "$nat46" = 1 ] || json_add_string iface_464xlat 0
			json_add_boolean peerdns 0
			json_add_array dns
				for DNSV in $(echo "$v6dns"); do
					json_add_string "" "$DNSV"
				done
			json_close_array
			proto_add_dynamic_defaults
			json_close_object
			ubus call network add_dynamic "$(json_dump)"
		elif
			[ "$v6cap" = 1 -a "$nat46" = 1 ]; then
			log "Adding 464XLAT (CLAT) dynamic interface"
			json_init
			json_add_string name "CLAT$INTER"
			json_add_string proto "464xlat"
			json_add_string tunlink "${interface}"
			[ -n "$zone" ] && json_add_string zone "$zone"
			proto_add_dynamic_defaults
			json_close_object
			ubus call network add_dynamic "$(json_dump)"
		fi
	fi

	tid=$((tid + 1))
	uci_set_state network $interface tid "$tid"
#	SIGNAL=$(umbim $DBG -n -t $tid -d $device signal)
#	CSQ=$(echo "$SIGNAL" | awk '/rssi:/ {print $2}')

	if [ -e $ROOTER/modem-led.sh ]; then
		$ROOTER/modem-led.sh $CURRMODEM 3
	fi

	$ROOTER/log/logger "Modem #$CURRMODEM Connected"
	log "Modem $CURRMODEM Connected"

	IDP=$(uci get modem.modem$CURRMODEM.idP)
	IDV=$(uci get modem.modem$CURRMODEM.idV)

	if [ ! -s /tmp/msimdata$CURRMODEM ]; then
		echo $IDV" : "$IDP > /tmp/msimdatax$CURRMODEM
		echo "$IMEI" >> /tmp/msimdatax$CURRMODEM
		echo "$IMSI" >> /tmp/msimdatax$CURRMODEM
		echo "$ICCID" >> /tmp/msimdatax$CURRMODEM
		echo "1" >> /tmp/msimdatax$CURRMODEM
		mv -f /tmp/msimdatax$CURRMODEM /tmp/msimdata$CURRMODEM
	fi

	if [ ! -s /tmp/msimnum$CURRMODEM ]; then
		echo "$CNUM" > /tmp/msimnumx$CURRMODEM
		echo "$CNUMx" >> /tmp/msimnumx$CURRMODEM
		mv -f /tmp/msimnumx$CURRMODEM /tmp/msimnum$CURRMODEM
	fi

	uci set modem.modem$CURRMODEM.custom=$CUSTOM
	uci set modem.modem$CURRMODEM.provider=$PROV
	uci set modem.modem$CURRMODEM.down=$DOWN" kbps Down | "
	uci set modem.modem$CURRMODEM.up=$UP" kbps Up"
	uci set modem.modem$CURRMODEM.mcc=$MCC
	uci set modem.modem$CURRMODEM.mnc=" "$MNC
	uci set modem.modem$CURRMODEM.sig="--"
	uci set modem.modem$CURRMODEM.sms=0
	uci commit modem

	COMMPORT=$(uci get modem.modem$CURRMODEM.commport)
	if [ -z $COMMPORT ]; then
		ln -s $ROOTER/mbim/mbimdata.sh $ROOTER_LINK/getsignal$CURRMODEM
	else
		$ROOTER/sms/check_sms.sh $CURRMODEM &
		ln -s $ROOTER/signal/modemsignal.sh $ROOTER_LINK/getsignal$CURRMODEM
		# send custom AT startup command
		if [ $(uci -q get modem.modeminfo$CURRMODEM.at) -eq "1" ]; then
			ATCMDD=$(uci -q get modem.modeminfo$CURRMODEM.atc)
			if [ ! -z "${ATCMDD}" ]; then
				OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
				OX=$($ROOTER/common/processat.sh "$OX")
				ERROR="ERROR"
				if `echo ${OX} | grep "${ERROR}" 1>/dev/null 2>&1`
				then
					log "Error sending custom AT command: $ATCMDD with result: $OX"
				else
					log "Sent custom AT command: $ATCMDD with result: $OX"
				fi
			fi
		fi
	fi
	ln -s $ROOTER/connect/reconnect.sh $ROOTER_LINK/reconnect$CURRMODEM
	$ROOTER_LINK/getsignal$CURRMODEM $CURRMODEM $PROT &
	ln -s $ROOTER/connect/conmon.sh $ROOTER_LINK/con_monitor$CURRMODEM
	$ROOTER_LINK/con_monitor$CURRMODEM $CURRMODEM &
	#ln -s $ROOTER/mbim/monitor.sh $ROOTER_LINK/mbim_monitor$CURRMODEM
	#$ROOTER_LINK/mbim_monitor$CURRMODEM $CURRMODEM $device &

	uci set modem.modem$CURRMODEM.connected=1
	uci commit modem
	
	if [ -e $ROOTER/connect/postconnect.sh ]; then
		$ROOTER/connect/postconnect.sh $CURRMODEM
	fi
	
	if [ -e $ROOTER/timezone.sh ]; then
		TZ=$(uci -q get modem.modeminfo$CURRMODEM.tzone)
		if [ "$TZ" = "1" ]; then
			$ROOTER/timezone.sh &
		fi
	fi
	#CLB=$(uci -q get modem.modeminfo$CURRMODEM.lb)
	CLB=1
	if [ -e /etc/config/mwan3 ]; then
		INTER=$(uci get modem.modeminfo$CURRMODEM.inter)
		if [ -z $INTER ]; then
			INTER=0
		else
			if [ $INTER = 0 ]; then
				INTER=$CURRMODEM
			fi
		fi
		ENB=$(uci -q get mwan3.wan$CURRMODEM.enabled)
		if [ ! -z $ENB ]; then
			if [ $CLB = "1" ]; then
				uci set mwan3.wan$INTER.enabled=1
			else
				uci set mwan3.wan$INTER.enabled=0
			fi
			uci commit mwan3
			/usr/sbin/mwan3 restart
		fi
	fi
	rm -f /tmp/usbwait

	return 0
}

proto_mbim_setup() {

	local ret
	_proto_mbim_setup $@
	ret=$?

	[ "$ret" = 0 ] || {
		log "MBIM bringup failed, retry in 5s"
		CPORT=$(uci get modem.modem$CURRMODEM.commport)
		ATCMDD="AT+COPS=0"
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		#log "Restart Modem"
		#/usr/lib/rooter/luci/restart.sh $CURRMODEM
		sleep 5
	}

	exit 0
	return $ret
}

proto_mbim_teardown() {
	local interface="$1"

	local device
	json_get_vars device
	local tid=$(uci_get_state network $interface tid)

	[ -n "$ctl_device" ] && device=$ctl_device

	if [ -n "$device" ]; then
		log "Stopping network"
		if [ -n "$tid" ]; then
			tid=$((tid + 1))
			umbim $DBG -t $tid -d "$device" disconnect
			uci_revert_state network $interface tid
		else
			umbim $DBG -d "$device" disconnect
		fi
	fi

	proto_init_update "*" 0
	proto_send_update "$interface"

}

[ -n "$INCLUDE_ONLY" ] || add_protocol mbim
