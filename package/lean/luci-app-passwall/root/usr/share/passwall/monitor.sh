#!/bin/sh

CONFIG=passwall
CONFIG_PATH=/var/etc/$CONFIG

uci_get_by_name() {
	local ret=$(uci get $CONFIG.$1.$2 2>/dev/null)
	echo ${ret:=$3}
}

uci_get_by_type() {
	local ret=$(uci get $CONFIG.@$1[0].$2 2>/dev/null)
	echo ${ret:=$3}
}

TCP_REDIR_SERVER=$(uci_get_by_type global tcp_redir_server nil)
UDP_REDIR_SERVER=$(uci_get_by_type global udp_redir_server nil)
[ "$UDP_REDIR_SERVER" == "default" ] && UDP_REDIR_SERVER=$TCP_REDIR_SERVER
SOCKS5_PROXY_SERVER=$(uci_get_by_type global socks5_proxy_server nil)
dns_mode=$(uci_get_by_type global dns_mode)
use_haproxy=$(uci_get_by_type global_haproxy balancing_enable 0)
haproxy_port=$(uci_get_by_type global_haproxy haproxy_port)
use_kcp=$(uci_get_by_name $TCP_REDIR_SERVER use_kcp 0)
kcp_port=$(uci_get_by_type global_proxy kcptun_port 11183)

#tcp
if [ $TCP_REDIR_SERVER != "nil" ] ;then
	icount=`ps -w | grep -E "ss-redir|ssr-redir|v2ray" | grep $CONFIG_PATH/TCP.json | grep -v grep | wc -l`
	if [ $icount = 0 ] ;then
		/etc/init.d/passwall restart
		exit 0
	fi
fi

#udp
if [ $UDP_REDIR_SERVER != "nil" ] ;then
	icount=`ps -w | grep -E "ss-redir|ssr-redir|v2ray" | grep $CONFIG_PATH/UDP.json | grep -v grep | wc -l`
	if [ $icount = 0 ] ;then
		/etc/init.d/passwall restart
		exit 0
	fi
fi

#socks5
if [ $SOCKS5_PROXY_SERVER != "nil" ] ;then
	icount=`ps -w | grep -E "ss-redir|ssr-redir|v2ray" | grep $CONFIG_PATH/SOCKS5.json | grep -v grep | wc -l`
	if [ $icount = 0 ] ;then
		/etc/init.d/passwall restart
		exit 0
	fi
fi

#dns
icount=`netstat -apn | grep 7913 | wc -l`
if [ $icount = 0 ] ;then
	/etc/init.d/passwall restart
	exit 0
fi

#kcptun
if [ $use_kcp -gt 0 ] ;then
	icount=`ps -w | grep kcptun_client | grep $kcp_port |grep -v grep | wc -l`
	if [ $icount = 0 ] ;then
		/etc/init.d/passwall restart
		exit 0
	fi
fi

#haproxy
if [ $use_haproxy -gt 0 ] ;then
	icount=`ps -w | grep haproxy | grep $haproxy_port | grep -v grep | wc -l`
	if [ $icount = 0 ] ;then
		/etc/init.d/passwall restart
		exit 0
	fi
fi
