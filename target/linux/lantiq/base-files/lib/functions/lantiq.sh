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

lantiq_is_dt_led_chosen() {
	[ -f "/sys/firmware/devicetree/base/chosen/leds/$1" ] && echo "true"
}

lantiq_get_dt_led_chosen() {
	local label
	local nodepath
	local basepath="/sys/firmware/devicetree/base"
	local chosenpath="$basepath/chosen/leds/$1"

	[ -f "$chosenpath" ] && nodepath=$(cat "$chosenpath")
	[ -n "$nodepath" ] && label=$(cat "$basepath$nodepath/label")

	echo "$label"
}
