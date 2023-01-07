#!/bin/sh

setbackup() { 
	extn=$(uci -q get bwmon.general.external)
	if [ "$extn" = "0" ]; then
		backPath=/usr/lib/bwmon/data/
	else
		if [ -e "$extn""/" ]; then
			backPath=$extn"/data/"
		else
			backPath=/usr/lib/bwmon/data/
			uci set bwmon.general.external="0"
			uci commit bwmon
		fi
	fi
	if [ ! -e "$backpath" ]; then
		mkdir -p $backPath
	fi
}

LAN_TYPE=$(uci get network.lan.ipaddr | awk -F. ' { print $1"."$2 }')
LEASES_FILE=/tmp/dhcp.leases
lockDir=/tmp/WRTbmon

ifname="ifname"
source /etc/openwrt_release
twone=$(echo "$DISTRIB_RELEASE" | grep "21.02")
if [ ! -z "$twone" ]; then
	ifname="device"
fi
	

[ ! -d "$lockDir" ] && mkdir "$lockDir"
basePath="/tmp/bwmon/"
mkdir -p $basePath"data"
dataPath=$basePath"data/"
setbackup
lockDir1=/tmp/wrtbwmon1.lock
lockDir=/tmp/wrtbwmon.lock
mkdir -p "$lockDir"
pidFile=$lockDir/pid
STARTIMEX=$(date +%s)
STARTIMEY=$(date +%s)
STARTIMEZ=$(date +%s)
cYear=$(date +%Y)
cDay=$(date +%d)
cMonth=$(date +%m)
setup_time=60
update_time=60
bs=$(uci -q get bwmon.general.backup)
let "bs=$bs*60"
backup_time=$bs
pause=30
unlimited="peak"

networkFuncs=/lib/functions/network.sh
uci=`which uci 2>/dev/null`
nslookup=`which nslookup 2>/dev/null`
nvram=`which nvram 2>/dev/null`
binDir=/usr/sbin
chains='INPUT OUTPUT FORWARD'
DEBUG=
interfaces='eth0' # in addition to detected WAN
DB="/tmp/usage.db"
mode=

log() {
	logger -t "wrtbwmon" "$@"
}

header="#mac,ip,iface,in,out,total,first_date,last_date"

createDbIfMissing()
{
    [ ! -f "$DB" ] && rm -f $DB;echo $header > "$DB"
}

checkWAN()
{
    [ -z "$wan" ] && return
}

lookup()
{
    MAC=$1
    IP=$2
    userDB=$3
    for USERSFILE in $userDB /tmp/dhcp.leases /tmp/dnsmasq.conf /etc/dnsmasq.conf /etc/hosts; do
	[ -e "$USERSFILE" ] || continue
	case $USERSFILE in
	    /tmp/dhcp.leases )
		USER=$(grep -i "$MAC" $USERSFILE | cut -f4 -s -d' ')
		;;
	    /etc/hosts )
		USER=$(grep "^$IP " $USERSFILE | cut -f2 -s -d' ')
		;;
	    * )
		USER=$(grep -i "$MAC" "$USERSFILE" | cut -f2 -s -d,)
		;;
	esac
	[ "$USER" = "*" ] && USER=
	[ -n "$USER" ] && break
    done
    if [ -n "$DO_RDNS" -a -z "$USER" -a "$IP" != "NA" -a -n "$nslookup" ]; then
	USER=`$nslookup $IP $DNS | awk '!/server can/{if($4){print $4; exit}}' | sed -re 's/[.]$//'`
    fi
    [ -z "$USER" ] && USER=${MAC}
    echo $USER
}

detectIF()
{
    if [ -f "$networkFuncs" ]; then
	IF=`. $networkFuncs; network_get_device netdev $1; echo $netdev`
	[ -n "$IF" ] && echo $IF && return
    fi

    if [ -n "$uci" -a -x "$uci" ]; then
	IF=`$uci get network.${1}.$ifname 2>/dev/null`
	[ $? -eq 0 -a -n "$IF" ] && echo $IF && return
    fi

    if [ -n "$nvram" -a -x "$nvram" ]; then
	IF=`$nvram get ${1}_$ifname 2>/dev/null`
	[ $? -eq 0 -a -n "$IF" ] && echo $IF && return
    fi
}

detectLAN()
{
    [ -e /sys/class/net/br-lan ] && echo br-lan && return
    lan=$(detectIF lan)
    [ -n "$lan" ] && echo $lan && return
}

detectWAN()
{
    [ -n "$WAN_IF" ] && echo $WAN_IF && return
    wan=$(detectIF wan)
    [ -n "$wan" ] && echo $wan && return
    wan=$(ip route show 2>/dev/null | grep default | sed -re '/^default/ s/default.*dev +([^ ]+).*/\1/')
    [ -n "$wan" ] && echo $wan && return
    [ -f "$networkFuncs" ] && wan=$(. $networkFuncs; network_find_wan wan; echo $wan)
    [ -n "$wan" ] && echo $wan && return
}

lock()
{
    attempts=0
    while [ $attempts -lt 10 ]; do
	mkdir $lockDir1 2>/dev/null && break
	attempts=$((attempts+1))
	pid=`cat $pidFile 2>/dev/null`
	if [ -n "$pid" ]; then
	    if [ -d "/proc/$pid" ]; then
		[ -n "$DEBUG" ] && echo "WARNING: Lockfile detected but process $(cat $pidFile) does not exist !"
		rm -rf $lockDir1
	    else
		sleep 1
	    fi
	fi
    done
    mkdir $lockDir1 2>/dev/null
    echo $$ > $pidFile
    [ -n "$DEBUG" ] && echo $$ "got lock after $attempts attempts"
    trap '' INT
}

unlock()
{
    rm -rf $lockDir1
    [ -n "$DEBUG" ] && echo $$ "released lock"
    trap "rm -f /tmp/*_$$.tmp; kill $$" INT
}

# chain
newChain()
{
    chain=$1
    # Create the RRDIPT_$chain chain (it doesn't matter if it already exists).
    iptables -t mangle -N RRDIPT_$chain 2> /dev/null
    
    # Add the RRDIPT_$chain CHAIN to the $chain chain if not present
    iptables -t mangle -C $chain -j RRDIPT_$chain 2>/dev/null
    if [ $? -ne 0 ]; then
	[ -n "$DEBUG" ] && echo "DEBUG: iptables chain misplaced, recreating it..."
	iptables -t mangle -I $chain -j RRDIPT_$chain
    fi
}

# chain tun
newRuleIF()
{
    chain=$1
    IF=$2
    
    #!@todo test
    if [ "$chain" = "OUTPUT" ]; then
	cmd="iptables -t mangle -o $IF -j RETURN"
	eval $cmd " -C RRDIPT_$chain 2>/dev/null" || eval $cmd " -A RRDIPT_$chain"
    elif [ "$chain" = "INPUT" ]; then
	cmd="iptables -t mangle -i $IF -j RETURN"
	eval $cmd " -C RRDIPT_$chain 2>/dev/null" || eval $cmd " -A RRDIPT_$chain"
    fi
}

accounting(){
	LAN_IFACE="br-lan"
	WAN_IFACE=$1
	LAN_IP=$(uci -q get network.lan.ipaddr)
	SERVER_IP=$(echo $LAN_IP  | cut -d . -f 1,2,3).0
	INTERNAL_NETMASK="$SERVER_IP/24"

	# create the ACCOUNTING chains
	iptables -w -N ACCOUNTING_BLOCK 2> /dev/null
	iptables -w -N ACCOUNTING_IN 2> /dev/null
	iptables -w -N ACCOUNTING_OUT 2> /dev/null
	check=0
	# check if accounting rule for ethernet wan not exist, add it
	checks=$(iptables -w -L FORWARD -v -n | grep "ACCOUNTING" | grep "$WAN_IFACE")
	[ -z "$checks" ] && check=1
	
	# check if jumps to the ACCOUNTING chains are still at the start of the FORWARD chain
	iptables -w -L FORWARD --line-numbers -n | grep "ACCOUNTING" | grep "^1 "
	if [ $? -ne 0 -o "$check" = "1" ]; then
		# remove old jump rules
		iptables -w -D FORWARD $(iptables -w -L FORWARD --line-numbers | grep ACCOUNTING | grep -m 1 -o "[0-9]*")
		while [ $? -eq 0 ]; do
			iptables -w -D FORWARD $(iptables -w -L FORWARD --line-numbers | grep ACCOUNTING | grep -m 1 -o "[0-9]*")
		done
		# insert new jump rules at start of FORWARD chain
		if [ -n "$WAN_IFACE" ]
		then
			iptables -w -I FORWARD -i ${WAN_IFACE} -j ACCOUNTING_IN
			iptables -w -I FORWARD -o ${WAN_IFACE} -j ACCOUNTING_OUT
		fi
		iptables -w -I FORWARD -j ACCOUNTING_BLOCK
	fi

	#For each host in the ARP table
	grep ${LAN_IFACE} /proc/net/arp | while read IP TYPE FLAGS MAC MASK IFACE
	do
		#Add iptables rules (if non existing).
		iptables -w -nL ACCOUNTING_IN | grep "${IP} " > /dev/null
		if [ $? -ne 0 ]; then
			iptables -w -I ACCOUNTING_IN -d ${IP} -s ${INTERNAL_NETMASK} -j RETURN
			iptables -w -I ACCOUNTING_IN -d ${IP} ! -s ${INTERNAL_NETMASK} -j RETURN
		fi

		iptables -w -nL ACCOUNTING_OUT | grep "${IP} " > /dev/null
		if [ $? -ne 0 ]; then
			iptables -w -I ACCOUNTING_OUT -s ${IP} -d ${INTERNAL_NETMASK} -j RETURN
			iptables -w -I ACCOUNTING_OUT -s ${IP} ! -d ${INTERNAL_NETMASK} -j RETURN
		fi
	done
}

setup()
{
	for chain in $chains; do
	    newChain $chain
	done

	wan=$(detectWAN)
	checkWAN
	wan1=$(detectIF wan1)
	wan2=$(detectIF wan2)
	C1=$(uci -q get modem.modem1.connected)
	C2=$(uci -q get modem.modem2.connected)$C1
	if [ ! -z $C2 ]; then
		interfaces="$wan1 $wan2"
		WW=$(uci -q get bwmon.bwwan.wan)
		if [ "$WW" = "1" ]; then
			interfaces=$interfaces $wan" wwan"	
		fi
	else
		WW=$(uci -q get bwmon.bwwan.wan)
		if [ "$WW" = "1" ]; then
			interfaces="$wan wwan"
		else
			return
		fi
	fi

	# track local data
	for chain in INPUT OUTPUT; do
	    for interface in $interfaces; do
		[ -n "$interface" ] && [ -e "/sys/class/net/$interface" ] && newRuleIF $chain $interface
		#if [ -e /etc/bwlock ]; then
			accounting $interface
		#fi
	    done
	done

	# this will add rules for hosts in arp table
	update $dailyUsageDB

	rm -f /tmp/*_$$.tmp
}

update()
{
	createDbIfMissing
    checkWAN
	PERTOTAL=0

    > /tmp/iptables_$$.tmp
    lock
    # only zero our own chains
    for chain in $chains; do
	iptables -nvxL RRDIPT_$chain -t mangle -Z >> /tmp/iptables_$$.tmp
    done
    # the iptables and readDB commands have to be separate. Otherwise,
    # they will fight over iptables locks
    awk -v mode="$mode" -v interfaces=\""$interfaces"\" -f $binDir/readDB.awk \
	$DB \
	/proc/net/arp \
	/tmp/iptables_$$.tmp
	
	while read L1
	do
	  MAC=$(echo ${L1} | cut -f1 -d, )
	  if [ $MAC != "#mac" ]; then
		MAC=$(echo ${L1} | cut -f1 -d, )
		IP=$(echo ${L1} | cut -f2 -d, )
		IN=$(echo ${L1} | cut -f4 -d, )
		IN=$((${IN}/1000))
		OUT=$(echo ${L1} | cut -f5 -d, )
		OUT=$((${OUT}/1000))
		TOTAL=$(echo ${L1} | cut -f6 -d, )
		TOTAL=$((${TOTAL}/1000))
		let PERTOTAL=PERTOTAL+TOTAL
		if [ $TOTAL -gt 0 -a $IP != "NA" ]; then
			for USERSFILE in /tmp/dhcp.leases /tmp/dnsmasq.conf /etc/dnsmasq.conf /etc/hosts; do
				[ -e "$USERSFILE" ] || continue
				case $USERSFILE in
						/tmp/dhcp.leases )
						NAME=$(grep -i "$MAC" $USERSFILE | cut -f4 -s -d' ')
					;;
						/etc/hosts )
						NAME=$(grep "^$IP " $USERSFILE | cut -f2 -s -d' ')
					;;
						* )
						NAME=$(grep -i "$MAC" "$USERSFILE" | cut -f2 -s -d,)
					;;
				esac
				[ "$NAME" = "*" ] && NAME=
				[ -n "$NAME" ] && break
			done
			if [ -z $NAME ]; then
				NAME="*"
			fi
		
			echo "\"mac\":\""${MAC}"\"","\"down\":\""${IN}"\"","\"up\":\""${OUT}"\"","\"offdown\":\""0"\"","\"offup\":\""0"\"","\"ip\":\""${IP}"\"","\"name\":\""${NAME}"\"" >> ${1}
		fi
	  fi
	done < $DB
	if [ -e /usr/lib/bwmon/period.sh ]; then
		/usr/lib/bwmon/period.sh "$PERTOTAL"
	fi
    unlock
}


createFiles() 
{
	while [ -e /tmp/lockbw ]; do
		sleep 1
	done
	echo "0" > /tmp/lockbw
	while [ ! -e $backPath*"mac_data.js" ]; do
		valid=$(cat /var/state/dnsmasqsec)
		st=$(echo "$valid" | grep "ntpd says time is valid")
		if [ ! -z "$st" ]; then
			break
		fi
		sleep 10
	done
	dailyUsageDB="$dataPath$cYear-$cMonth-$cDay-daily_data.js"
	dailyUsageBack="$backPath$cYear-$cMonth-$cDay-daily_data.js"
	if [ ! -f $dailyUsageBack ]; then
		rm -f $backPath*"daily_data.js"
		touch $dailyUsageDB
		touch $dailyUsageBack
	else
		cp -f $dailyUsageBack $dailyUsageDB
	fi
	monthlyUsageDB="$dataPath$cYear-$cMonth-mac_data.js"
	monthlyUsageBack="$backPath$cYear-$cMonth-mac_data.js"
	if [ -f $monthlyUsageBack ]; then
		cp -f $monthlyUsageBack $monthlyUsageDB".bk"
		sed "/start day $cDay/,/end day $cDay/d" $monthlyUsageDB".bk" > $monthlyUsageDB 
		cp -f $monthlyUsageBack $monthlyUsageDB".bk"
	else
		rm -f $backPath*"mac_data.js"
		touch $monthlyUsageDB
		touch $monthlyUsageBack
		/usr/lib/bwmon/backup.sh "backup" $cDay $monthlyUsageDB $dailyUsageDB $monthlyUsageBack $dailyUsageBack
	fi
	rm -f /tmp/lockbw
}

shutDown() 
{
	while [ -e /tmplockbw ]; do
		sleep 1
	done
	echo "0" > /tmp/lockbw
	/usr/lib/bwmon/backup.sh "backup" $cDay $monthlyUsageDB $dailyUsageDB $monthlyUsageBack $dailyUsageBack
	lua /usr/lib/bwmon/cleanup.lua
	rm -f /tmp/lockbw
}

checkSetup() 
{
	CURRTIME=$(date +%s)
	let ELAPSE=CURRTIME-STARTIMEX
	if [ $ELAPSE -gt $setup_time ]; then
		STARTIMEX=$CURRTIME
		setup
		/usr/lib/bwmon/backup.sh "setup" $cDay $monthlyUsageDB $dailyUsageDB $monthlyUsageBack $dailyUsageBack
	fi
}

checkUpdate() 
{
	CURRTIME=$(date +%s)
	let ELAPSE=CURRTIME-STARTIMEY
	if [ $ELAPSE -gt $update_time ]; then
		STARTIMEY=$CURRTIME
		update $dailyUsageDB $unlimited
	fi
}

checkBackup() 
{
	CURRTIME=$(date +%s)
	let ELAPSE=CURRTIME-STARTIMEZ
	bs=$(uci -q get bwmon.general.backup)
	let "bs=$bs*60"
	backup_time=$bs
	if [ $ELAPSE -gt $backup_time ]; then
		STARTIMEZ=$CURRTIME
		shutDown
	fi
}

checkTime() 
{
	while [ -e /tmplockbw ]; do
		sleep 1
	done
	echo "0" > /tmp/lockbw
	pDay=$(date +%d)
	pYear=$(date +%Y)
	pMonth=$(date +%m)
	if [ "$cDay" -ne "$pDay" ]; then
		/usr/lib/bwmon/backup.sh "daily" $cDay $monthlyUsageDB $dailyUsageDB $monthlyUsageBack $dailyUsageBack
		
		cDay=$pDay
		cMonth=$pMonth
		cYear=$pYear
		monthlyUsageBack="$backPath$cYear-$cMonth-mac_data.js"
		if [ ! -e $monthlyUsageBack ]; then
			rm -f $backPath*"mac_data.js"
		fi
		rm -f $dataPath[[:digit:]][[:digit:]][[:digit:]][[:digit:]]"-"[[:digit:]][[:digit:]]"-"[[:digit:]][[:digit:]]-daily_data.js
		rm -f $backPath[[:digit:]][[:digit:]][[:digit:]][[:digit:]]"-"[[:digit:]][[:digit:]]"-"[[:digit:]][[:digit:]]-daily_data.js
		roll=$(uci -q get custom.bwallocate.rollover)
		[ -z $roll ] && roll=1
		if [ "$roll" -eq "$pDay" ]; then
			rm -f $monthlyUsageDB
			rm -f $backPath*"mac_data.js"
			monthlyUsageDB="$dataPath$cYear-$cMonth-mac_data.js"
			monthlyUsageBack="$backPath$cYear-$cMonth-mac_data.js"
			touch $monthlyUsageDB
			uci set custom.texting.used='0'
			uci commit custom
			if [ -e /usr/lib/bwmon/periodreset.sh ]; then
				/usr/lib/bwmon/periodreset.sh
			fi
		fi
		rm -f $dailyUsageDB
		rm -f $backPath*"daily_data.js"
		dailyUsageDB="$dataPath$cYear-$cMonth-$cDay-daily_data.js"
		touch $dailyUsageDB
		dailyUsageBack="$backPath$cYear-$cMonth-$cDay-daily_data.js"
	fi 
	rm -f /tmp/lockbw
}

createFiles
setup
while [ -d $lockDir ]; do
	checkSetup
	checkTime
	checkBackup
	n=0
	while [ true ] ; do
		n=$(($n + 1))
		if [ ! -d "$lockDir" ]; then
			shutDown
			exit 0
		fi
		[ "$n" -gt "$pause" ] && break;
		sleep 1
	done
done
