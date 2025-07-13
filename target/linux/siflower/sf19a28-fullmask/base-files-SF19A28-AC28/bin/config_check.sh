#!/bin/sh
force_link=`uci get network.wan.force_link`
[ $force_link == "1" ] && {
	uci set network.wan.force_link=0
	uci commit network
}
