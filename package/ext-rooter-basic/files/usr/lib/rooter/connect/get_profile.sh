#!/bin/sh
 
. /lib/functions.sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	modlog "Get Profile $CURRMODEM" "$@"
}

CURRMODEM=$1

MODEL=$(uci get modem.modem$CURRMODEM.model)
MANUF=$(uci get modem.modem$CURRMODEM.manuf)
idV=$(uci get modem.modem$CURRMODEM.idV)
idP=$(uci get modem.modem$CURRMODEM.idP)
IMEI=$(uci get modem.modem$CURRMODEM.imei)
IMSI=$(uci -q get modem.modem$CURRMODEM.imsi)
ICCID=$(uci get modem.modem$CURRMODEM.iccid)

log "Modem $CURRMODEM is $MANUF $MODEL"

rm -f /tmp/profile$CURRMODEM

if [ "$IMSI" = "Unknown" ]; then
	log "Warning: IMSI cannot be read - SIM card missing or locked?"
	touch /tmp/simerr$CURRMODEM
else
	rm -f /tmp/simerr$CURRMODEM
fi

MATCH=0

do_custom() {
	local config=$1
	local select name enabled select1
	local model
	local imsi
	local vid
	local pid

	if [ $MATCH -eq 0 ]; then
		config_get select $1 select
		config_get name $1 name
		config_get enabled $1 enabled
		if [ -z "$enabled" ]; then
			enabled=1
		fi
		if [ -z "$name" ]; then
			name="Not Named"
		fi
		if [ $enabled -eq 1 ]; then
			case $select in
			"0" )
				config_get vid $1 vid
				config_get pid $1 pid
				if [ $idV = $vid -a $idP = $pid ]; then
					MATCH=1
					log "Modem ID Profile - "$name""
				fi
				;;
			"1" )
				config_get imei $1 imei
				case $IMEI in
				"$imei"*)
					MATCH=1
					log "SIM IMEI Profile - "$name""
					;;
				esac
				;;
			"2" )
				config_get model $1 model
				if [ "$MODEL" == "$model" ]; then
					MATCH=1
					log "Modem Model Profile - "$name""
				fi
				;;
			"3" )
				config_get imsi $1 imsi
				case $IMSI in
				"$imsi"*)
					MATCH=1
					log "SIM IMSI Profile - "$name""
					;;
				esac
				;;
			"4" )
				config_get iccid $1 iccid
				case $ICCID in
				"$iccid"*)
					MATCH=1
					log "SIM ICCID Profile - "$name""
					;;
				esac
				;;
			esac
			if [ $MATCH = 1 ]; then
				config_get select1 $1 select1
				if [ $select1 -ne 10 ]; then
					MATCH=0
					case $select1 in
					"0" )
						config_get vid1 $1 vid1
						config_get pid1 $1 pid1
						if [ $idV = $vid1 -a $idP = $pid1 ]; then
							MATCH=1
							log "Modem ID Profile - "$name""
						fi
						;;
					"1" )
						config_get imei1 $1 imei1
						case $IMEI in
							"$imei"*)
								MATCH=1
								log "SIM IMEI Profile - "$name""
								;;
							esac
						;;
					"2" )
						config_get model1 $1 model1
						if [ "$MODEL" == "$model1" ]; then
							MATCH=1
							log "Modem Model Profile - "$name""
						fi
						;;
					"3" )
						config_get imsi1 $1 imsi1
						case $IMSI in
						"$imsi1"*)
							MATCH=1
							log "SIM IMSI Profile - "$name""
							;;
						esac
						;;
					"4" )
						config_get iccid1 $1 iccid1
						case $ICCID in
						"$iccid1"*)
							MATCH=1
							log "SIM ICCID Profile - "$name""
							;;
						esac
						;;
					esac
				fi
				if [ $MATCH = 1 ]; then
					local apn user passw pincode auth ppp delay lock mcc mnc
					local dns1 dns2 dns3 dns4 log lb at atc
					config_get apn $1 apn
					dapn=$(echo "$apn" | grep "|")
					if [ -z $dapn ]; then
						apn2=""
					else
						fapn=$apn"|"
						fapn=$(echo $fapn" " | tr "|" ",")
						apn=$(echo $fapn | cut -d, -f1)
						apn2=$(echo $fapn | cut -d, -f2)
					fi
					uci set modem.modeminfo$CURRMODEM.apn=$apn
					uci set modem.modeminfo$CURRMODEM.apn2=$apn2
					config_get mtu $1 mtu
					uci set modem.modeminfo$CURRMODEM.mtu=$mtu
					config_get context $1 context
					uci set modem.modeminfo$CURRMODEM.context=$context
					config_get user $1 user
					uci set modem.modeminfo$CURRMODEM.user=$user
					config_get passw $1 passw
					uci set modem.modeminfo$CURRMODEM.passw=$passw
					config_get pincode $1 pincode
					uci set modem.modeminfo$CURRMODEM.pincode=$pincode
					config_get auth $1 auth
					uci set modem.modeminfo$CURRMODEM.auth=$auth
					config_get ppp $1 ppp
					uci set modem.modeminfo$CURRMODEM.ppp=$ppp
					config_get inter $1 inter
					uci set modem.modeminfo$CURRMODEM.inter=$inter
					config_get delay $1 delay
					uci set modem.modeminfo$CURRMODEM.delay=$delay
					config_get lock $1 lock
					uci set modem.modeminfo$CURRMODEM.lock=$lock
					config_get mcc $1 mcc
					uci set modem.modeminfo$CURRMODEM.mcc=$mcc
					config_get mnc $1 mnc
					uci set modem.modeminfo$CURRMODEM.mnc=$mnc
					config_get dns1 $1 dns1
					uci set modem.modeminfo$CURRMODEM.dns1=$dns1
					config_get dns2 $1 dns2
					uci set modem.modeminfo$CURRMODEM.dns2=$dns2
					config_get dns3 $1 dns3
					uci set modem.modeminfo$CURRMODEM.dns3=$dns3
					config_get dns4 $1 dns4
					uci set modem.modeminfo$CURRMODEM.dns4=$dns4
					config_get log $1 log
					uci set modem.modeminfo$CURRMODEM.log=$log
					config_get lb $1 lb
					uci set modem.modeminfo$CURRMODEM.lb=$lb
					config_get at $1 at
					uci set modem.modeminfo$CURRMODEM.at=$at
					config_get atc $1 atc
					uci set modem.modeminfo$CURRMODEM.atc=$atc
					config_get tzone $1 tzone
					uci set modem.modeminfo$CURRMODEM.tzone=$tzone
					config_get nodhcp $1 nodhcp
					uci set modem.modeminfo$CURRMODEM.nodhcp=$nodhcp
					config_get pdptype $1 pdptype
					if [ $pdptype = "0" ]; then
						pdptype=""
					fi
					uci set modem.modeminfo$CURRMODEM.pdptype=$pdptype
					config_get ttl $1 ttl
					if [ -z "$ttl" ]; then
						ttl="0"
					fi
					uci set modem.modeminfo$CURRMODEM.ttl="$ttl"
					config_get ttloption $1 ttloption
					if [ -z "$ttloption" ]; then
						ttloption="0"
					fi
					uci set modem.modeminfo$CURRMODEM.ttloption="$ttloption"
					config_get hostless $1 hostless
					if [ -z "$hostless" ]; then
						hostless="0"
					fi
					uci set modem.modeminfo$CURRMODEM.hostless="$hostless"
					config_get bwday $1 bwday
					if [ -z $bwday ]; then
						bwday="0"
					fi
					uci set modem.modeminfo$CURRMODEM.bwday=$bwday
					config_get phone $1 phone
					if [ -z $phone ]; then
						phone="0"
					fi
					uci set modem.modeminfo$CURRMODEM.bwphone=$phone
					config_get bwdelay $1 bwdelay
					if [ -z $bwdelay ]; then
						bwdelay="0"
					fi
					uci set modem.modeminfo$CURRMODEM.bwdelay=$bwdelay

					[ -n "$apn" ] || log "This profile has no APN configured !!!"

					config_get alive $1 alive
					uci delete modem.pinginfo$CURRMODEM
					uci set modem.pinginfo$CURRMODEM=pinfo$CURRMODEM
					uci set modem.pinginfo$CURRMODEM.alive=$alive
					if [ $alive -ne 0 ]; then
						local reliability count pingtime pingwait packetsize down up

						handle_trackip() {
							local value="$1"
							uci add_list modem.pinginfo$CURRMODEM.trackip=$value
						}
						config_list_foreach "$config" trackip handle_trackip
						TIP=$(uci get modem.pinginfo$CURRMODEM.trackip)
						if [ -z "$TIP" ]; then
							uci add_list modem.pinginfo$CURRMODEM.trackip="1.1.1.1"
						fi
						config_get reliability $1 reliability
						uci set modem.pinginfo$CURRMODEM.reliability=$reliability
						config_get count $1 count
						uci set modem.pinginfo$CURRMODEM.count=$count
						config_get pingtime $1 pingtime
						uci set modem.pinginfo$CURRMODEM.pingtime=$pingtime
						config_get pingwait $1 pingwait
						uci set modem.pinginfo$CURRMODEM.pingwait=$pingwait
						config_get packetsize $1 packetsize
						uci set modem.pinginfo$CURRMODEM.packetsize=$packetsize
						config_get down $1 down
						uci set modem.pinginfo$CURRMODEM.down=$down
						config_get up $1 up
						uci set modem.pinginfo$CURRMODEM.up=$up
					fi

					uci commit modem
				fi
			fi
		fi
	fi
}

autoapn=$(uci -q get profile.disable.autoapn)
apd=0
if [ -e /usr/lib/autoapn/apn.data ]; then
	apd=1
fi

if [ $autoapn = "1" -a $apd -eq 1 ]; then
	MATCH=0
else
	autod=$(uci -q get profile.disable.enabled)
	if [ $autod = "1" ]; then
		MATCH=0
	else
		config_load profile
		config_foreach do_custom custom
	fi
fi

if [ $MATCH = 0 ]; then
	if [ $autod = "1" ]; then
		if [ -e /etc/config/isp ]; then
			MATCH=1
		fi
	fi
	if [ $MATCH = 1 ]; then
		isp=$(uci -q get isp.general.current)
		apn=$(uci -q get isp.$isp.apn)
		apn2=""
	else
		apn=$(uci -q get profile.default.apn)
		dapn=$(echo "$apn" | grep "|")
		if [ -z $dapn ]; then
			apn2=""
		else
			fapn=$apn"|"
			fapn=$(echo $fapn" " | tr "|" ",")
			apn=$(echo $fapn | cut -d, -f1)
			apn2=$(echo $fapn | cut -d, -f2)
		fi
	fi
	uci set modem.modeminfo$CURRMODEM.apn=$apn
	uci set modem.modeminfo$CURRMODEM.apn2=$apn2
	if [ -n "$ICCID" ]; then
		iccid="891490"
		case $ICCID in
		"$iccid"*)
			uci set modem.modeminfo$CURRMODEM.apn2=""
			uci set modem.modeminfo$CURRMODEM.apn="internet.freedommobile.ca"
			;;
		esac
	fi

	uci set modem.modeminfo$CURRMODEM.user=$(uci -q get profile.default.user)
	uci set modem.modeminfo$CURRMODEM.passw=$(uci -q get profile.default.passw)
	uci set modem.modeminfo$CURRMODEM.pincode=$(uci -q get profile.default.pincode)
	uci set modem.modeminfo$CURRMODEM.context=$(uci -q get profile.default.context)
	uci set modem.modeminfo$CURRMODEM.auth=$(uci get profile.default.auth)
	uci set modem.modeminfo$CURRMODEM.ppp=$(uci get profile.default.ppp)
	uci set modem.modeminfo$CURRMODEM.inter=0
	uci set modem.modeminfo$CURRMODEM.delay=$(uci get profile.default.delay)
	uci set modem.modeminfo$CURRMODEM.lock=$(uci get profile.default.lock)
	uci set modem.modeminfo$CURRMODEM.mcc=$(uci -q get profile.default.mcc)
	uci set modem.modeminfo$CURRMODEM.mnc=$(uci -q get profile.default.mnc)
	uci set modem.modeminfo$CURRMODEM.dns1=$(uci -q get profile.default.dns1)
	uci set modem.modeminfo$CURRMODEM.dns2=$(uci -q get profile.default.dns2)
	uci set modem.modeminfo$CURRMODEM.dns3=$(uci -q get profile.default.dns3)
	uci set modem.modeminfo$CURRMODEM.dns4=$(uci -q get profile.default.dns4)
	uci set modem.modeminfo$CURRMODEM.log=$(uci get profile.default.log)
	uci set modem.modeminfo$CURRMODEM.lb=$(uci get profile.default.lb)
	uci set modem.modeminfo$CURRMODEM.at=$(uci -q get profile.default.at)
	uci set modem.modeminfo$CURRMODEM.atc=$(uci -q get profile.default.atc)
	uci set modem.modeminfo$CURRMODEM.tzone=$(uci -q get profile.default.tzone)
	uci set modem.modeminfo$CURRMODEM.mtu=$(uci -q get profile.default.mtu)	
	uci set modem.modeminfo$CURRMODEM.nodhcp=$(uci -q get profile.default.nodhcp)
	pdp=$(uci -q get profile.default.pdptype)
	if [ $pdp = "0" ]; then
		pdp=""
	fi
	uci set modem.modeminfo$CURRMODEM.pdptype=$pdp
	ttl=$(uci -q get profile.default.ttl)
	if [ -z "$ttl" ]; then
		ttl="0"
	fi
	uci set modem.modeminfo$CURRMODEM.ttl="$ttl"
	ttloption=$(uci -q get profile.default.ttloption)
	if [ -z "$ttloption" ]; then
		ttloption="0"
	fi
	uci set modem.modeminfo$CURRMODEM.ttloption="$ttloption"
	hostless=$(uci -q get profile.default.hostless)
	if [ -z "$hostless" ]; then
		hostless="0"
	fi
	uci set modem.modeminfo$CURRMODEM.hostless="$hostless"
	bwday=$(uci -q get profile.default.bwday)
	if [ -z $bwday ]; then
		bwday="0"
	fi
	uci set modem.modeminfo$CURRMODEM.bwday=$bwday
	phone=$(uci -q get profile.default.phone)
	if [ -z $phone ]; then
		phone="0"
	fi
	uci set modem.modeminfo$CURRMODEM.bwphone=$phone
	bwdelay=$(uci -q get profile.default.bwdelay)
	if [ -z $bwdelay ]; then
		bwdelay="0"
	fi
	uci set modem.modeminfo$CURRMODEM.bwdelay=$bwdelay

	alive=$(uci get profile.default.alive)
	uci delete modem.pinginfo$CURRMODEM
	uci set modem.pinginfo$CURRMODEM=pinfo$CURRMODEM
	uci set modem.pinginfo$CURRMODEM.alive=$alive
	if [ $alive -ne 0 ]; then

		handle_trackip1() {
			local value="$1"
			uci add_list modem.pinginfo$CURRMODEM.trackip=$value
		}
		config_list_foreach "default" trackip handle_trackip1
		TIP=$(uci get modem.pinginfo$CURRMODEM.trackip)
		if [ -z "$TIP" ]; then
			uci add_list modem.pinginfo$CURRMODEM.trackip="1.1.1.1"
		fi
		uci set modem.pinginfo$CURRMODEM.reliability=$(uci get profile.default.reliability)
		uci set modem.pinginfo$CURRMODEM.count=$(uci get profile.default.count)
		uci set modem.pinginfo$CURRMODEM.pingtime=$(uci get profile.default.pingtime)
		uci set modem.pinginfo$CURRMODEM.pingwait=$(uci get profile.default.pingwait)
		uci set modem.pinginfo$CURRMODEM.packetsize=$(uci get profile.default.packetsize)
		uci set modem.pinginfo$CURRMODEM.down=$(uci get profile.default.down)
		uci set modem.pinginfo$CURRMODEM.up=$(uci get profile.default.up)
	fi

	uci commit modem
	if [ "$autoapn" = "1" -a $apd -eq 1 ]; then
		log "Automatic APN Used"
	else
		log "Default Profile Used"
		[ -n "$(uci -q get profile.default.apn)" ] || log "Default profile has no APN configured"
	fi
fi

if [ ! -e /etc/config/isp ]; then
	if [ "$autoapn" != "1" -a $apd -eq 1 ]; then
		APN=$(uci -q get modem.modeminfo$CURRMODEM.apn)
		log "APN of profile used is $APN"
	fi
fi

touch /tmp/profile$CURRMODEM
