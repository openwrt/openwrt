#!/bin/sh

lantiq_board_detect() {
	name=`grep "^machine" /proc/cpuinfo | sed "s/machine.*: \(.*\)/\1/g" | sed "s/\(.*\) - .*/\1/g"`
	model=`grep "^machine" /proc/cpuinfo | sed "s/machine.*: \(.*\)/\1/g" | sed "s/.* - \(.*\)/\1/g"`
	[ -z "$name" ] && name="unknown"
	[ -z "$model" ] && model="unknown"
	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"
	echo $name > /tmp/sysinfo/board_name
	echo $model > /tmp/sysinfo/model
}

lantiq_board_model() {
	local model

	[ -f /tmp/sysinfo/model ] && model=$(cat /tmp/sysinfo/model)
	[ -z "$model" ] && model="unknown"

	echo "$model"
}

lantiq_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}

lantiq_get_dt_led() {
	local label
	local ledpath
	local basepath="/sys/firmware/devicetree/base"
	local nodepath="$basepath/aliases/led-$1"

	[ -f "$nodepath" ] && ledpath=$(cat "$nodepath")
	[ -n "$ledpath" ] && label=$(cat "$basepath$ledpath/label")

	echo "$label"
}

lantiq_is_vdsl_system() {
	grep -qE "system type.*: (VR9|xRX200)" /proc/cpuinfo
}
