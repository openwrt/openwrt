#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "TEXTING" "$@"
}

handle_numbers() {
	let "NUMBERS = $NUMBERS + 1"
	if [ $NUMBERS -eq $nindex ]; then
		numch=$1
	fi
}

do_numbers() {
	local config=$1
	config_list_foreach "$config" number handle_numbers
}

handle_messages() {
	let "NUMBERS = $NUMBERS + 1"
	if [ $NUMBERS -eq $nindex ]; then
		numch=$1
	fi
}

do_messages() {
	local config=$1
	config_list_foreach "$config" message handle_messages
}

sendmsg() {
	nnum=$(uci -q get texting.texting.numbers)
	mnum=$(uci -q get texting.texting.messages)
	delay=$(uci -q get texting.texting.interval)
	NUMBERS=0
	RANDOM=$(date +%s%N | cut -b10-19)
	RAN=$RANDOM
	RAN1=${RAN:0:1}
	if [ $RAN1 = "0" ]; then
		RAN="1"$RAN
		rlen=${#RAN}
		if [ $rlen -gt 9 ]; then
			RAN=${RAN:0:8}
		fi
	fi
	nindex=$(( $RAN % $nnum + 1 ))
	config_load texting
	config_foreach do_numbers numbers
	Phone=$numch
	NUMBERS=0
	RANDOM=$(date +%s%N | cut -b10-19)
	RAN=$RANDOM
	RAN1=${RAN:0:1}
	if [ $RAN1 = "0" ]; then
		RAN="1"$RAN
		rlen=${#RAN}
		if [ $rlen -gt 9 ]; then
			RAN=${RAN:0:8}
		fi
	fi
	nindex=$(( $RAN % $mnum + 1 ))
	config_foreach do_messages messages
	Message=$numch
	/usr/lib/fullmenu/chksms.sh
	if [ -e /tmp/texting ]; then
		/usr/lib/sms/smsout.sh "$Phone" "$Message" 
	fi
}

checktime() {
	shour=$(uci -q get texting.texting.starthour)
	smin=$(uci -q get texting.texting.startmin)
	ehour=$(uci -q get texting.texting.endhour)
	emin=$(uci -q get texting.texting.endmin)
	chour=$(date +%H)
	cmin=$(date +%M)
	if [ $shour -gt $chour ]; then
		flag="0"
	else
		if [ $shour -eq $chour ]; then
			if [ $smin -le $cmin ]; then
				flag="1"
			else
				flag="0"
			fi
		else
			flag="1"
		fi
	fi

	if [ $flag = "1" ]; then
		if [ $ehour -lt $chour ]; then
			flag="0"
		else
			if [ $ehour -eq $chour ]; then
				if [ $emin -lt $cmin ]; then
					flag="0"
				else
					flag="1"
				fi
			else
				flag="1"
			fi
		fi
	fi

	echo $flag
}

while true
do
	running=$(checktime)
	if [ $running = "1" ]; then
		EN=$(uci -q get texting.setting.enabled)
		if [ $EN = "1" ]; then
			sendmsg
			sleep $delay
		fi
	else
		sleep 900
	fi
done