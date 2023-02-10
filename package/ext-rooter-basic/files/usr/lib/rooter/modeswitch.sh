#!/bin/sh
. /lib/functions/procd.sh

MODCNT=6

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

modeswitch="/usr/bin/usb_modeswitch"

log() {
	modlog "usb-modeswitch $CURRMODEM" "$@"
}

sanitize() {
	sed -e 's/[[:space:]]\+$//; s/[[:space:]]\+/_/g' "$@"
}

find_usb_attrs() {
	local usb_dir="/sys$DEVPATH"
	[ -f "$usb_dir/idVendor" ] || usb_dir="${usb_dir%/*}"

	uVid=$(cat "$usb_dir/idVendor")
	uPid=$(cat "$usb_dir/idProduct")
	uMa=$(sanitize "$usb_dir/manufacturer")
	uPr=$(sanitize "$usb_dir/product")
	uSe=$(sanitize "$usb_dir/serial")
}

display_top() {
	log "*****************************************************************"
	log "*"
}

display_bottom() {
	log "*****************************************************************"
}


display() {
	local line1=$1
	log "* $line1"
	log "*"
}

#
# Save Interface variables
#
save_variables() {
	echo 'MODSTART="'"$MODSTART"'"' > /tmp/variable.file
	echo 'WWAN="'"$WWAN"'"' >> /tmp/variable.file
	echo 'USBN="'"$USBN"'"' >> /tmp/variable.file
	echo 'ETHN="'"$ETHN"'"' >> /tmp/variable.file
	echo 'WDMN="'"$WDMN"'"' >> /tmp/variable.file
	echo 'BASEPORT="'"$BASEPORT"'"' >> /tmp/variable.file
}
#
# delay until ROOter Initialization done
#
bootdelay() {
	if [ ! -f /tmp/bootend.file ]; then
		log "Delay for boot up"
		sleep 10
		while [ ! -f /tmp/bootend.file ]; do
			sleep 1
		done
		sleep 10
	fi
}

#
# return modem number based on port number
# 0 is not found
#
find_device() {
	DEVN=$1
	COUNTER=1
	while [ $COUNTER -le $MODCNT ]; do
		EMPTY=$(uci get modem.modem$COUNTER.empty)
		if [ $EMPTY -eq 0 ]; then
			DEVS=$(uci get modem.modem$COUNTER.device)
			if [ $DEVN = $DEVS ]; then
				retresult=$COUNTER
				return
			fi
		fi
		let COUNTER=COUNTER+1
	done
	retresult=0
}

#
# check if all modems are inactive or empty
# delete all if nothing active
#
check_all_empty() {
	COUNTER=1
	while [ $COUNTER -le $MODCNT ]; do
		EMPTY=$(uci get modem.modem$COUNTER.empty)
		if [ $EMPTY -eq 0 ]; then
			ACTIVE=$(uci get modem.modem$COUNTER.active)
			if [ $ACTIVE -eq 1 ]; then
				return
			fi
		fi
		let COUNTER=COUNTER+1
	done
	COUNTER=1
	while [ $COUNTER -le $MODCNT ]; do
		uci delete modem.modem$COUNTER
		uci set modem.modem$COUNTER=modem
		uci set modem.modem$COUNTER.empty=1
		let COUNTER=COUNTER+1
	done
	uci set modem.general.modemnum=1
	uci commit modem
	MODSTART=1
	WWAN=0
	USBN=0
	ETHN=1
	WDMN=0
	BASEPORT=0
	if
		ifconfig eth1
	then
		if [ -e "/sys/class/net/eth1/device/bInterfaceProtocol" ]; then
			ETHN=1
		else
			ETHN=2
		fi
	fi
	save_variables
	display_top; display "No Modems present"; display_bottom
}

change_bconf() {
	local devname=$1
	local conf=$2
	local mode=$3
	local unconf=0
	log "Switching Modem at $DEVICENAME to $mode by selecting Cfg# $bestcfg"
	echo $unconf >/sys/bus/usb/devices/$devname/bConfigurationValue
	sleep 1
	echo $conf >/sys/bus/usb/devices/$devname/bConfigurationValue
}


#
# Add Modem and connect
#
if [ "$ACTION" = add ]; then
	bootdelay
	CNTR=0
	while [ -e /tmp/modgone ]; do
		sleep 1
		CNTR=`expr $CNTR + 1`
		if [ $CNTR -gt 10 ]; then
			rm -f /tmp/modgone
			break
		fi
	done
	find_usb_attrs

	if echo $DEVICENAME | grep -q ":" ; then
		exit 0
	fi

	if [ -z $uMa ]; then
		log "Ignoring Unnamed Hub"
		exit 0
	fi

	UPR=${uPr}
	CT=`echo $UPR | tr '[A-Z]' '[a-z]'`
	if echo $CT | grep -q "hub" ; then
		log "Ignoring Named Hub"
		exit 0
	fi

	if [ $uVid = 1d6b ]; then
		log "Ignoring Linux Hub"
		exit 0
	fi

#
# Ignore Ethernet adapters
#
	if [ $uVid = 13b1 -a $uPid = 0041 ]; then
		exit 0
	elif [ $uVid = 2357 -a $uPid = 0601 ]; then
		exit 0
	elif [ $uVid = 0b95 -a $uPid = 772b ]; then
		exit 0
	elif [ $uVid = 0b95 -a $uPid = 1790 ]; then
		exit 0
	elif [ $uVid = 0bda -a $uPid = 8152 ]; then
		exit 0
	fi

	bNumConfs=$(cat /sys/bus/usb/devices/$DEVICENAME/bNumConfigurations)
	bNumIfs=$(cat /sys/bus/usb/devices/$DEVICENAME/bNumInterfaces)

	# Uncomment the next line to ignore USB-Serial adapters and similar single-port devices
	# if [ $bNumConfs = 1 -a $bNumIfs = 1 ] && exit 0		

	$ROOTER/proto.sh $uVid $uPid $DEVICENAME 0
	source /tmp/proto
	rm -f /tmp/proto
	#cat /sys/kernel/debug/usb/devices > /tmp/wdrv
	#lua $ROOTER/protofind.lua $uVid $uPid 0
	#retval=$?

	if [ -e /etc/config/mjpg-streamer ]; then
		if [ $retval -eq 99 ]; then
			log "Start MJPEG Streamer $DEVICENAME"
			/etc/init.d/mjpg-streamer start
			uci delete mjpg-streamer.camera
			uci set mjpg-streamer.camera=mjpg-stream
			uci set mjpg-streamer.camera.idv=$DEVICENAME
			uci commit mjpg-streamer
			exit 0
		fi
	fi
	if [ -e /etc/config/p910nd ]; then
		if [ $retval -eq 98 ]; then
			# Check if lp device is plugged in and p910nd is not already started
			log "USB Printer device plugged in, starting p910nd"
			/etc/init.d/p910nd start
			uci delete p910nd.printer
			uci set p910nd.printer=printer
			uci set p910nd.printer.idv=$DEVICENAME
			uci commit p910nd
			exit 0
		fi
	fi
	if [ $retval -eq 97 ]; then
		if grep "$uVid:$uPid" /etc/usb-mode.json > /dev/null ; then
			log "Modem found"
		else
			log "Found USB Storage"
			exit 0
		fi
	fi

	DELAY=1
	if [ -f /tmp/usbwait ]; then
		log "Delay for previous modem"
		while [ -f /tmp/usbwait ]; do
			sleep 1
			let DELAY=$DELAY+1
			if [ $DELAY -gt 15 ]; then
				break
			fi
		done
	fi
	echo "1" > /tmp/usbwait
	
	source /tmp/variable.file
	source /tmp/modcnt
	MODCNT=$MODCNTX

	reinsert=0
	find_device $DEVICENAME
	if [ $retresult -gt 0 ]; then
		ACTIVE=$(uci get modem.modem$retresult.active)
		if [ $ACTIVE = 1 ]; then
			rm -f /tmp/usbwait
			exit 0
		else
			IDP=$(uci get modem.modem$retresult.uPid)
			IDV=$(uci get modem.modem$retresult.uVid)
			if [ $uVid = $IDV -a $uPid = $IDP ]; then
				reinsert=1
				CURRMODEM=$retresult
				MODSTART=$retresult
				WWANX=$(uci get modem.modem$CURRMODEM.wwan)
				if [ -n "$WWANX" ]; then
					WWAN=$WWANX
					save_variables
				fi
				WDMNX=$(uci get modem.modem$CURRMODEM.wdm)
				if [ -n "$WDMNX" ]; then
					WDMN=$WDMNX
					save_variables
				fi
			else
				display_top; display "Reinsert of different Modem not allowed"; display_bottom
				rm -f /tmp/usbwait
				exit 0
			fi
		fi
	fi

	log "Add : $DEVICENAME: Manufacturer=${uMa:-?} Product=${uPr:-?} Serial=${uSe:-?} $uVid $uPid"

	if [ $MODSTART -gt $MODCNT ]; then
		display_top; display "Exceeded Maximun Number of Modems"; display_bottom
		exit 0
	fi

	if [ $reinsert = 0 ]; then
		CURRMODEM=$MODSTART
	fi

	FILEN=$uVid:$uPid
	display_top; display "Start of Modem Detection and Connection Information"
	display "Product=${uPr:-?} $uVid $uPid"; display_bottom
	cat /sys/kernel/debug/usb/devices > /tmp/prembim
	lua $ROOTER/mbimfind.lua $uVid $uPid
	retval=$?
	rm -f /tmp/prembim
	if [ ! -e /sbin/umbim ]; then
		retval=0
	fi

	while : ; do
		bConfig=$(cat /sys/bus/usb/devices/$DEVICENAME/bConfigurationValue)
		if [ -n "$bConfig" -a -n "$bNumConfs" ]; then
			log "Found Modem at $DEVICENAME in Cfg#= $bConfig from $bNumConfs available"
			break
		else
			sleep 1
		fi
	done

	FORCEQMI='03f0:0857 1bc7:1900'
	if echo $FORCEQMI | grep -q -i "$FILEN"; then
		bestcfg=1
		if [ $bConfig -ne $bestcfg ]; then
			change_bconf $DEVICENAME $bestcfg QMI
		fi
	else
		if [ $retval -ne 0 ]; then
			display_top; display "Found MBIM Modem at $DEVICENAME with Config of $retval"; display_bottom
			if [ $FILEN = "12d1:15c1" ]; then
				bestcfg=2
				if [ $bConfig -ne $bestcfg ]; then
					change_bconf $DEVICENAME $bestcfg ECM
				fi
			elif [ $FILEN = "413c:81d7" -o $FILEN = "05c6:9025" ]; then
				bestcfg=1
				case $bNumConfs in
				"3" )
					change_bconf $DEVICENAME $bestcfg QMI
					;;
				"2" )
					if [ $bNumIfs -lt 4 ]; then
						change_bconf $DEVICENAME $bestcfg QMI
					fi
					;;
				esac
			elif [ $FILEN = "03f0:9d1d" -a $bNumConfs -eq 3 ]; then
				bestcfg=1
				change_bconf $DEVICENAME $bestcfg QMI
			else
				if [ $bConfig -ne $retval ]; then
					change_bconf $DEVICENAME $retval MBIM
				fi
			fi
		else
			if grep "$FILEN" /etc/usb-mode.json > /dev/null ; then
				procd_open_service "usbmode"
				procd_open_instance
				procd_set_param command "/sbin/usbmode" -s
				procd_close_instance
				procd_close_service
			else
				display_top; display "Device at $DEVICENAME does not have a switch data file"; display_bottom
			fi
		fi
	fi
	sleep 10
	usb_dir="/sys$DEVPATH"
	idV="$(sanitize "$usb_dir/idVendor")"
	idP="$(sanitize "$usb_dir/idProduct")"
	display_top; display "Modem at $DEVICENAME switched to : $idV:$idP"; display_bottom

	if [ $idV = 2357 -a $idP = 9000 ]; then
		sleep 10
	fi

	#cat /sys/kernel/debug/usb/devices > /tmp/wdrv
	#lua $ROOTER/protofind.lua $idV $idP 1
	$ROOTER/proto.sh $uVid $uPid $DEVICENAME 1
	source /tmp/proto
	rm -f /tmp/proto
	#retval=$?
	if [ $idV = 8087 -a $idP = 095a ]; then
		retval=28
	fi
	if [ $idV = 2cb7 -a $idP = 000b ]; then
		retval=28
	fi
	display_top; display "ProtoFind returns : $retval"; display_bottom
	rm -f /tmp/wdrv

	if [ $reinsert = 0 ]; then
		BASEP=$BASEPORT
		if [ -f /tmp/drv ]; then
			source /tmp/drv
			BASEPORT=`expr $PORTN + $BASEPORT`
		fi
	fi
	rm -f /tmp/drv

	if [ $retval -ne 0 ]; then
		log "Found Modem $CURRMODEM"
		if [ $reinsert = 0 ]; then
			uci set modem.modem$CURRMODEM.empty=0
			uci set modem.modem$CURRMODEM.uVid=$uVid
			uci set modem.modem$CURRMODEM.uPid=$uPid
			uci set modem.modem$CURRMODEM.idV=$idV
			uci set modem.modem$CURRMODEM.idP=$idP
			uci set modem.modem$CURRMODEM.device=$DEVICENAME
			uci set modem.modem$CURRMODEM.baseport=$BASEP
			uci set modem.modem$CURRMODEM.maxport=$BASEPORT
			uci set modem.modem$CURRMODEM.proto=$retval
			uci set modem.modem$CURRMODEM.maxcontrol=/sys$DEVPATH/descriptors
			find_usb_attrs
			uci set modem.modem$CURRMODEM.manuf=$uMa
			uci set modem.modem$CURRMODEM.model=$uPr
			uci set modem.modem$CURRMODEM.serial=$uSe
			uci set modem.modem$CURRMODEM.celltype="-"
		fi
		uci set modem.modem$CURRMODEM.active=1
		uci set modem.modem$CURRMODEM.connected=0
		uci commit modem
		if [ -e $ROOTER/modem-led.sh ]; then
			$ROOTER/modem-led.sh $CURRMODEM 1
		fi
	fi

	if [ $reinsert = 0 -a $retval != 0 ]; then
		MODSTART=`expr $MODSTART + 1`
		save_variables
	fi

#
# Handle specific modem models
#
	case $retval in
	"0" )
		#
		# ubox GPS module
		#
		if [ $idV = 1546 ]; then
			if echo $uPr | grep -q "GPS"; then
				SYMLINK="gps0"
				BASEX=`expr 1 + $BASEP`
				ln -s /dev/ttyUSB$BASEX /dev/${SYMLINK}
				display_top ; display "Hotplug Symlink from /dev/ttyUSB$BASEX to /dev/${SYMLINK} created"
				display_bottom
			fi
		fi
		rm -f /tmp/usbwait
		exit 0
		;;
	"1" )
		log "Connecting a Sierra Modem"
		ln -s $ROOTER/connect/create_connect.sh $ROOTER_LINK/create_proto$CURRMODEM
		$ROOTER_LINK/create_proto$CURRMODEM $CURRMODEM &
		;;
	"2" )
		log "Connecting a QMI Modem"
		ln -s $ROOTER/connect/create_connect.sh $ROOTER_LINK/create_proto$CURRMODEM
		$ROOTER_LINK/create_proto$CURRMODEM $CURRMODEM &
		;;
	"3" )
		log "Connecting a MBIM Modem"
		ln -s $ROOTER/connect/create_connect.sh $ROOTER_LINK/create_proto$CURRMODEM
		$ROOTER_LINK/create_proto$CURRMODEM $CURRMODEM &
		;;
	"6"|"4"|"7"|"24"|"26"|"27" )
		log "Connecting a Huawei NCM Modem"
		ln -s $ROOTER/connect/create_connect.sh $ROOTER_LINK/create_proto$CURRMODEM
		$ROOTER_LINK/create_proto$CURRMODEM $CURRMODEM &
		;;
	"28" )
		log "Connecting a Fibocom NCM Modem"
		ln -s $ROOTER/connect/create_connect.sh $ROOTER_LINK/create_proto$CURRMODEM
		$ROOTER_LINK/create_proto$CURRMODEM $CURRMODEM &
		;;
	"5" )
		log "Connecting a Hostless Modem or Phone"
		ln -s $ROOTER/connect/create_hostless.sh $ROOTER_LINK/create_proto$CURRMODEM
		$ROOTER_LINK/create_proto$CURRMODEM $CURRMODEM &
		;;
	"10"|"11"|"12"|"13"|"14"|"15"|"16" )
		log "Connecting a PPP Modem"
		ln -s $ROOTER/ppp/create_ppp.sh $ROOTER_LINK/create_proto$CURRMODEM
		$ROOTER_LINK/create_proto$CURRMODEM $CURRMODEM
		;;
	"9" )
		log "Connecting an iPhone"
		ln -s $ROOTER/connect/create_iphone.sh $ROOTER_LINK/create_proto$CURRMODEM
		$ROOTER_LINK/create_proto$CURRMODEM $CURRMODEM
		;;
	esac

fi

#
# Remove Modem
#
if [ "$ACTION" = remove ]; then
	find_usb_attrs

	if echo $DEVICENAME | grep -q ":" ; then
		exit 0
	fi
	find_device $DEVICENAME
	if [ $retresult -gt 0 ]; then
		IDP=$(uci get modem.modem$retresult.idP)
		IDV=$(uci get modem.modem$retresult.idV)
		if [ $uVid = $IDV ]; then
			exit 0
		else
			INTER=$(uci get modem.modem$retresult.inter)
			if [ -z $INTER ]; then
				INTER=$retresult
			fi
			if [ -e $ROOTER/modem-led.sh ]; then
				$ROOTER/modem-led.sh $retresult 0
			fi
			uci set modem.modem$retresult.active=0
			uci set modem.modem$retresult.connected=0
			uci commit modem
			if [ -e /etc/config/mwan3 ]; then
				ENB=$(uci get mwan3.wan$retresult.enabled)
				if [ ! -z $ENB ]; then
					uci set mwan3.wan$INTER.enabled=0
					uci commit mwan3
				fi
			fi
			SMS=$(uci get modem.modem$CURRMODEM.sms)
			if [ $SMS = 1 ]; then
				if [ -e /usr/lib/sms/stopsms ]; then
					/usr/lib/sms/stopsms $CURRMODEM
				fi
			fi
			ifdown wan$INTER
			uci delete network.wan$INTER
			uci set network.wan$INTER=interface
			uci set network.wan$INTER.proto=dhcp
			ifname1="ifname"
			if [ -e /etc/newstyle ]; then
				ifname1="device"
			fi
			uci set network.wan$INTER.${ifname1}=" "
			uci set network.wan$INTER.metric=$INTER"0"
			uci commit network
			/etc/init.d/network reload
			ifdown wan$INTER
			PID=$(ps |grep "getsignal$retresult" | grep -v grep |head -n 1 | awk '{print $1}')
			kill -9 $PID
			rm -f $ROOTER_LINK/getsignal$retresult
			PID=$(ps |grep "reconnect$retresult" | grep -v grep |head -n 1 | awk '{print $1}')
			kill -9 $PID
			rm -f $ROOTER_LINK/reconnect$retresult
			PID=$(ps |grep "create_proto$retresult" | grep -v grep |head -n 1 | awk '{print $1}')
			kill -9 $PID
			rm -f $ROOTER_LINK/create_proto$retresult
			PID=$(ps |grep "processsms$retresult" | grep -v grep |head -n 1 | awk '{print $1}')
			kill -9 $PID
			rm -f $ROOTER_LINK/processsms$retresult
			PID=$(ps |grep "con_monitor$retresult" | grep -v grep |head -n 1 | awk '{print $1}')
			kill -9 $PID
			rm -f $ROOTER_LINK/con_monitor$retresult
			PID=$(ps |grep "mbim_monitor$retresult" | grep -v grep |head -n 1 | awk '{print $1}')
			kill -9 $PID
			rm -f $ROOTER_LINK/mbim_monitor$retresult
			if [ -e /usr/lib/gps/gpskill.sh ]; then
				/usr/lib/gps/gpskill.sh $retresult
			fi
			if [ -e $ROOTER/connect/chkconn.sh ]; then
				jkillall chkconn.sh
			fi
			$ROOTER/signal/status.sh $retresult "No Modem Present"
			$ROOTER/log/logger "Disconnect (Removed) Modem #$retresult"
			display_top; display "Remove : $DEVICENAME : Modem $retresult"; display_bottom
			check_all_empty
			rm -f /tmp/usbwait
			rm -f /tmp/mdown$retresult
			rm -f /tmp/msimdata$retresult
			rm -f /tmp/msimnum$retresult
			rm -f /tmp/modgone
			rm -f /tmp/bmask
			rm -f /tmp/simpin$retresult
		fi
	else
		IDV=$(uci get mjpg-streamer.camera.idv)
		if [ ! -z $IDV ]; then
			if [ $DEVICENAME = $IDV ]; then
				uci delete mjpg-streamer.camera
				uci commit mjpg-streamer
				/etc/init.d/mjpg-streamer stop
				log "Stop MJPEG-Streamer"
			fi
		fi
		IDV=$(uci get p910nd.printer.idv)
		if [ ! -z $IDV ]; then
			if [ $DEVICENAME = $IDV ]; then
				uci delete p910nd.printer
				uci commit p910nd
				if [ ! -d /sys$DEVPATH/*/lp0 -a -f /var/run/p9100d.pid ]; then
					log "USB Printer device unplugged, stopping p910nd"
					/etc/init.d/p910nd stop
					# p910nd does not seem to remove .pid file when stopped, removing it manually
					rm /var/run/p9100d.pid
				fi
			fi
		fi
	fi
fi

if [ "$ACTION" = "motion" ]; then
	logger webcam motion event
fi
