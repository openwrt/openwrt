#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "TEXTING" "$@"
}

sleep 3

handle_numbers() {
	let "NUMBERS = $NUMBERS + 1"
}

do_numbers() {
	local config=$1
	config_list_foreach "$config" number handle_numbers
}

handle_messages() {
	let "NUMBERS = $NUMBERS + 1"
}

do_messages() {
	local config=$1
	config_list_foreach "$config" message handle_numbers
}

SHOUR=$(uci -q get texting.setting.start)
HOUR=`expr $SHOUR / 4`
let "TH = $HOUR * 4"
let "TMP1 = $SHOUR - $TH"
let "MIN = $TMP1 * 15"
uci set texting.texting.starthour=$HOUR
uci set texting.texting.startmin=$MIN
EHOUR=$(uci -q get texting.setting.end)
HOUR=`expr $EHOUR / 4`
let "TH = $HOUR * 4"
let "TMP1 = $EHOUR - $TH"
let "MIN = $TMP1 * 15"
uci set texting.texting.endhour=$HOUR
uci set texting.texting.endmin=$MIN

NUMBERS=0
config_load texting
config_foreach do_numbers numbers
uci set texting.texting.numbers=$NUMBERS
NUMBERS=0
config_foreach do_messages messages
uci set texting.texting.messages=$NUMBERS

TIMES=$(uci -q get texting.setting.times)
let "TH = $EHOUR - $SHOUR"
let "TMIN = $TH * 15"
let "INTERVAL = $TMIN / $TIMES"
let "INTERVAL = $INTERVAL * 60"
uci set texting.texting.interval=$INTERVAL
uci commit texting

result=`ps | grep -i "dotext.sh" | grep -v "grep" | wc -l`
if [ $result -lt 1 ]; then
	/usr/lib/fullmenu/dotext.sh &
fi

