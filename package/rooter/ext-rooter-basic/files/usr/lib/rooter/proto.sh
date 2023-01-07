#!/bin/sh

log() {
	modlog "Protofind $idV/$idP" "$@"
}

idV=$1
idP=$2
DEVICENAME=$3
path="/sys/bus/usb/devices/$DEVICENAME/"
cnfg=$(cat $path"bConfigurationValue")
ipath="$DEVICENAME:$cnfg."

cntr=$(cat $path"bNumInterfaces")
cntr=$(echo "$cntr" | sed 's/[[:space:]]//g')
if [ $4 -eq 1 ]; then
	log "Number Interfaces w/drivers : $cntr"
fi
serialcnt=0
retval=0
fcntr=0

while [ true ]; do
	if [ -e $path$ipath$fcntr ]; then
		DRIVER=""
		cat $path$ipath$fcntr"/uevent" > /tmp/uevent$DEVICENAME
		source /tmp/uevent$DEVICENAME
		rm -f /tmp/uevent$DEVICENAME
		if [ $4 -eq 1 ]; then
			log "Driver Name : $fcntr $DRIVER"
		fi
		if [ "$DRIVER" = "option" -o "$DRIVER" = "qcserial" -o "$DRIVER" = "usb_serial" -o drv[j] == "usb_serial" -o "$DRIVER" = "sierra" ]; then
			let serialcnt=$serialcnt+1
		fi
		let cntr=$cntr-1
		if [ "$cntr" -lt 1 ]; then
			break
		fi
	fi
	let fcntr=$fcntr+1
done
cntr=$(cat $path"bNumInterfaces")
cntr=$(echo "$cntr" | sed 's/[[:space:]]//g')
fcntr=0
while [ true ]; do
	if [ -e $path$ipath$fcntr ]; then
		cat $path$ipath$fcntr"/uevent" > /tmp/uevent$DEVICENAME
		source /tmp/uevent$DEVICENAME
		rm -f /tmp/uevent$DEVICENAME
		case $DRIVER in
			"sierra_net" )
				retval=1
				break
			;;
			"qmi_wwan" )
				retval=2
				break
			;;
			"cdc_mbim" )
				retval=3
				break
			;;
			"huawei_cdc_ncm" )
				retval=4
				break
			;;
			"cdc_ncm" )
				retval=24
				break
			;;
			"cdc_ether"|"rndis_host" )
				retval=5
				break
			;;
			"ipheth" )
				retval=9
				break
			;;
			"uvcvideo" )
				retval=99
				break
			;;
			"usblp" )
				retval=98
				break
			;;
			"usb-storage" )
				retval=97
				break
			;;
		esac
		let cntr=$cntr-1
		if [ "$cntr" -lt 1 ]; then
			break
		fi
	fi
	let fcntr=$fcntr+1
done
if [ $serialcnt -gt 0 -a $retval -eq 0 ]; then
	retval=11
fi
echo 'retval="'"$retval"'"' > /tmp/proto

