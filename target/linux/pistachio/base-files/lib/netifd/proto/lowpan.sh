#!/bin/sh
# netifd script for configuring 6lowpan interfaces
##

[ -n "$INCLUDE_ONLY" ] || {
        . /lib/functions.sh
        . /lib/functions/network.sh
        . /lib/functions/mac.sh
        . ../netifd-proto.sh
        init_proto "$@"
}

proto_lowpan_setup() {
        local cfg="$1" #wpan.

        local ip6addr pan_id channel ifname
        json_get_vars ip6addr pan_id channel ifname

        local id=${ifname##*[[:alpha:]]}
        local interface="$cfg$id" # i.e wpan0

        generate_mac $WPAN_OTP_REG0
        local WPAN_MAC=$(cat /etc/config/MAC_$WPAN_OTP_REG0)
        local WPAN_EUI=$(echo $WPAN_MAC | cut -d: -f1,2,3):ff:fe:$(echo $WPAN_MAC | cut -d: -f4,5,6)

        proto_init_update "$ifname" 1

        ifconfig "$interface" down
        ip link set $interface address $WPAN_EUI
        iwpan dev "$interface" set pan_id "$pan_id"
        iwpan phy phy$id set channel 0 $channel
        ifconfig "$interface" up

        ip link add link "$interface" name "$ifname" type lowpan

        proto_add_ipv6_address "$ip6addr" "128"
        proto_send_update "$cfg"
}

proto_lowpan_teardown() {
        local cfg="$1"
        local iface="$2"

        local ifname
        json_get_var ifname

        local id=${ifname##*[[:alpha:]]}
        local interface="$cfg$id" # i.e wpan0

        ifconfig "$interface" down
        ip link del "$ifname"

        proto_kill_command "$cfg"
}

proto_lowpan_init_config() {
        no_device=1
        available=1
        proto_config_add_string "ip6addr"
        proto_config_add_string "pan_id"
        proto_config_add_string "channel"
}

[ -n "$INCLUDE_ONLY" ] || {
        add_protocol lowpan
}
