#!/bin/ash

CFG=/etc/board.json

. /usr/share/libubox/jshn.sh

json_select_array() {
	local _json_no_warning=1

	json_select "$1"
	[ $? = 0 ] && return

	json_add_array $1
	json_close_array

        json_select "$1"
}

json_select_object() {
	local _json_no_warning=1

	json_select "$1"
	[ $? = 0 ] && return

	json_add_object $1
	json_close_object

        json_select "$1"
}

_ucidef_set_interface() {
	local name=$1
	local iface=$2

	json_select_object $name
	json_add_string ifname "${iface%%.*}"
	[ "$iface" == "${iface%%.*}" ] || json_add_boolean create_vlan 1 
	json_select ..
}

ucidef_set_interface_loopback()
{
	# stub
	local a=$1
}

ucidef_set_interface_lan() {
	local lan_if=$1

	json_select_object network
	_ucidef_set_interface lan $lan_if
	json_select ..
}

ucidef_set_interfaces_lan_wan() {
	local lan_if=$1
	local wan_if=$2

	json_select_object network
	_ucidef_set_interface lan $lan_if
	_ucidef_set_interface wan $wan_if
	json_select ..
}

ucidef_add_switch() {
	local name=$1
	local reset=$2
	local enable=$3

	json_select_object switch

        json_select_object $name
	[ "$enable" -eq 1 ] && json_add_boolean enable 1
	[ "$reset" -eq 1 ] && json_add_boolean reset 1
	json_select ..

	json_select ..
}

ucidef_add_switch_attr() {
	local name=$1
	local key=$2
	local val=$3

	json_select_object switch

        json_select_object $name
	json_add_string $key $val
	json_select ..

	json_select ..
}

ucidef_add_switch_vlan() {
	local name=$1
	local vlan=$2
	local ports=$3
	local cpu_port=''

	case $vlan in
	1)	vlan=lan;;
	2)	vlan=wan;;
	*)	vlan=vlan$vlan;;
	esac

	json_select_object switch
	json_select_object $name
	json_select_object vlans

	json_add_array $vlan
	for p in $ports; do
		if [ ${p%t} != $p ]; then
			cpu_port=$p
		else
			json_add_int "" $p
		fi
	done
	json_close_array

	json_select ..
	[ -n "$cpu_port" ] && json_add_int cpu_port $cpu_port
	json_select ..
	json_select ..
}

ucidef_set_interface_macaddr() {
	local network=$1
	local macaddr=$2

	json_select_object network

	json_select $network
	[ $? -eq 0 ] || {
		json_select ..
		return
	}

	json_add_string macaddr $macaddr
	json_select ..
	
	json_select ..
}

ucidef_set_led_netdev() {
	local cfg="led_$1"
	local name=$2
	local sysfs=$3
	local dev=$4

	json_select_object led
	
	json_select_object $1
	json_add_string name $name
	json_add_string type netdev
	json_add_string sysfs $sysfs
	json_add_string device $dev
	json_select ..

	json_select ..
}

ucidef_set_led_interface() {
	local name=$1
	local sysfs=$2

	json_select_object led
	
	json_select_object $1
	json_add_string name $name
	json_add_string type interface
	json_add_string sysfs $sysfs
	json_add_string interface $name
	json_select ..

	json_select ..
}

ucidef_set_led_usbdev() {
	local cfg="led_$1"
	local name=$2
	local sysfs=$3
	local dev=$4

	json_select_object led
	
	json_select_object $1
	json_add_string name $name	
	json_add_string type usb
	json_add_string sysfs $sysfs
	json_add_string device $dev
	json_select ..

	json_select ..
}

ucidef_set_led_wlan() {
	local cfg="led_$1"
	local name=$2
	local sysfs=$3
	local trigger=$4

	json_select_object led
	
	json_select_object $1
	json_add_string name $name
	json_add_string type trigger
	json_add_string sysfs $sysfs
	json_add_string trigger $trigger
	json_select ..

	json_select ..
}

ucidef_set_led_switch() {
	local cfg="led_$1"
	local name=$2
	local sysfs=$3
	local trigger=$4
	local port_mask=$5

	json_select_object led
	
	json_select_object $1
	json_add_string name $name
	json_add_string type switch
	json_add_string sysfs $sysfs
	json_add_string trigger $trigger
	json_add_string port_mask $port_mask
	json_select ..

	json_select ..
}

ucidef_set_led_default() {
	local cfg="led_$1"
	local name=$2
	local sysfs=$3
	local default=$4

	json_select_object led
	
	json_select_object $1
	json_add_string name $name
	json_add_string sysfs $sysfs
	json_add_string default $default
	json_select ..

	json_select ..
}

ucidef_set_led_rssi() {
	local cfg="led_$1"
	local name=$2
	local sysfs=$3
	local iface=$4
	local minq=$5
	local maxq=$6
	local offset=$7
	local factor=$8

	json_select_object led
	
	json_select_object rssi
	json_select_object $1
	json_add_string name $name
	json_add_string sysfs $sysfs
	json_add_string minq $minq
	json_add_string maxq $maxq
	json_add_string offset $offset
	json_add_string factor $factor
	json_select ..
	json_select ..

	json_select ..
}

ucidef_set_rssimon() {
	local dev="$1"
	local refresh="$2"
	local threshold="$3"

	json_select_object led
	
	json_select_object rssi
	json_add_string type rssi
	json_add_string dev $dev
	json_add_string threshold $threshold
	json_select ..

	json_select ..
	
}

board_config_update() {
	json_init
	[ -f ${CFG} ] && json_load "$(cat ${CFG})"
}

board_config_flush() {
	json_dump -i > /tmp/.board.json
	mv /tmp/.board.json ${CFG}
}
