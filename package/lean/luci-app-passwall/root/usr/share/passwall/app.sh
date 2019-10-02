#!/bin/sh
# Copyright (C) 2018-2019 Lienol <lawlienol@gmail.com>

. $IPKG_INSTROOT/lib/functions.sh
. $IPKG_INSTROOT/lib/functions/service.sh

CONFIG=passwall
CONFIG_PATH=/var/etc/$CONFIG
RUN_PID_PATH=$CONFIG_PATH/pid
HAPROXY_FILE=$CONFIG_PATH/haproxy.cfg
CONFIG_TCP_FILE=$CONFIG_PATH/TCP.json
CONFIG_UDP_FILE=$CONFIG_PATH/UDP.json
CONFIG_SOCKS5_FILE=$CONFIG_PATH/SOCKS5.json
LOCK_FILE=$CONFIG_PATH/$CONFIG.lock
LOG_FILE=/var/log/$CONFIG.log
APP_PATH=/usr/share/$CONFIG
APP_PATH_RULE=$APP_PATH/rule
APP_PATH_DNSMASQ=$APP_PATH/dnsmasq.d
TMP_DNSMASQ_PATH=/var/etc/dnsmasq-passwall.d
DNSMASQ_PATH=/etc/dnsmasq.d
lanip=$(uci get network.lan.ipaddr)

get_date(){
	echo "$(date "+%Y-%m-%d %H:%M:%S")"
}

echolog()
{
	echo -e "$(get_date): $1" >> $LOG_FILE
}

find_bin(){
	bin_name=$1
	result=`find /usr/*bin -iname "$bin_name" -type f`
	if [ -z "$result" ]; then
		echo ""
		echolog "找不到$bin_name主程序，无法启动！"
	else
		echo "$result"
	fi
}

config_n_get() {
	local ret=$(uci get $CONFIG.$1.$2 2>/dev/null)
	echo ${ret:=$3}
}

config_t_get() {
	local index=0
	[ -n "$4" ] && index=$4
	local ret=$(uci get $CONFIG.@$1[$index].$2 2>/dev/null)
	echo ${ret:=$3}
}

get_host_ip() {
	local network_type host isip
	network_type=$1
	host=$2
	isip=""
	ip=$host
	if [ "$network_type" == "ipv6" ]; then
		isip=`echo $host | grep -E "([[a-f0-9]{1,4}(:[a-f0-9]{1,4}){7}|[a-f0-9]{1,4}(:[a-f0-9]{1,4}){0,7}::[a-f0-9]{0,4}(:[a-f0-9]{1,4}){0,7}])"`
		if [ -n "$isip" ];then
			isip=`echo $host | cut -d '[' -f2 | cut -d ']' -f1`
		else
			isip=`echo $host | grep -E "([a-f0-9]{1,4}(:[a-f0-9]{1,4}){7}|[a-f0-9]{1,4}(:[a-f0-9]{1,4}){0,7}::[a-f0-9]{0,4}(:[a-f0-9]{1,4}){0,7})"`
		fi
	else
		isip=`echo $host|grep -E "([0-9]{1,3}[\.]){3}[0-9]{1,3}"`
	fi
	if [ -z "$isip" ];then
		vpsrip=""
		if [ "$use_ipv6" == "1" ];then
			vpsrip=`resolveip -6 -t 2 $host|awk 'NR==1{print}'`
			[ -z "$vpsrip" ] && vpsrip=`dig @208.67.222.222 $host AAAA 2>/dev/null |grep 'IN'|awk -F ' ' '{print $5}'|grep -E "([a-f0-9]{1,4}(:[a-f0-9]{1,4}){7}|[a-f0-9]{1,4}(:[a-f0-9]{1,4}){0,7}::[a-f0-9]{0,4}(:[a-f0-9]{1,4}){0,7})"|head -n1`
		else
			vpsrip=`resolveip -4 -t 2 $host|awk 'NR==1{print}'`
			[ -z "$vpsrip" ] && vpsrip=`dig @208.67.222.222 $host 2>/dev/null |grep 'IN'|awk -F ' ' '{print $5}'|grep -E "([0-9]{1,3}[\.]){3}[0-9]{1,3}"|head -n1`
		fi
		ip=$vpsrip
	fi
	echo $ip
}

check_port_exists(){
	port=$1
	protocol=$2
	result=
	if [ "$protocol" = "tcp" ];then
		result=`netstat -tlpn | grep "\<$port\>"`
	elif [ "$protocol" = "udp" ];then
		result=`netstat -ulpn | grep "\<$port\>"`
	fi
	if [ -n "$result" ];then
		echo 1
	else
		echo 0
	fi
}

get_not_exists_port_after(){
	port=$1
	protocol=$2
	result=$(check_port_exists $port $protocol)
	if [ "$result" = 1 ];then
		temp=
		if [ "$port" -lt 65535 ];then
			temp=`expr $port + 1`
		elif [ "$port" -gt 1 ];then
			temp=`expr $port - 1`
		fi
		get_not_exists_port_after $temp $protocol
	else
		echo $port
	fi
}

SOCKS5_PROXY_SERVER=$(config_t_get global socks5_proxy_server nil)
TCP_REDIR_SERVER=$(config_t_get global tcp_redir_server nil)
UDP_REDIR_SERVER=$(config_t_get global udp_redir_server nil)
[ "$UDP_REDIR_SERVER" == "default" ] && UDP_REDIR_SERVER=$TCP_REDIR_SERVER

TCP_REDIR_SERVER2=
TCP_REDIR_SERVER3=
UDP_REDIR_SERVER2=
UDP_REDIR_SERVER3=
TCP_REDIR_SERVER_NUM=$(config_t_get global_other tcp_redir_server_num 1)
UDP_REDIR_SERVER_NUM=$(config_t_get global_other udp_redir_server_num 1)

if [ "$TCP_REDIR_SERVER_NUM" -ge 2 ] ;then
	for i in $(seq 2 $TCP_REDIR_SERVER_NUM)
	do
		eval TCP_REDIR_SERVER$i=$(config_t_get global tcp_redir_server$i nil)
	done
fi

if [ "$UDP_REDIR_SERVER_NUM" -ge 2 ] ;then
	for i in $(seq 2 $UDP_REDIR_SERVER_NUM)
	do
		eval UDP_REDIR_SERVER$i=$(config_t_get global udp_redir_server$i nil)
	done
fi


TCP_REDIR_SERVER_IP=""
UDP_REDIR_SERVER_IP=""
SOCKS5_PROXY_SERVER_IP=""
TCP_REDIR_SERVER_IPV6=""
UDP_REDIR_SERVER_IPV6=""
SOCKS5_PROXY_SERVER_IPV6=""
TCP_REDIR_SERVER_PORT=""
UDP_REDIR_SERVER_PORT=""
SOCKS5_PROXY_SERVER_PORT=""
TCP_REDIR_SERVER_TYPE=""
UDP_REDIR_SERVER_TYPE=""
SOCKS5_PROXY_SERVER_TYPE=""

BROOK_SOCKS5_CMD=""
BROOK_TCP_CMD=""
BROOK_UDP_CMD=""
AUTO_SWITCH_ENABLE=$(config_t_get auto_switch enable 0)
TCP_REDIR_PORTS=$(config_t_get global_forwarding tcp_redir_ports '80,443')
UDP_REDIR_PORTS=$(config_t_get global_forwarding udp_redir_ports '1:65535')
KCPTUN_REDIR_PORT=$(config_t_get global_proxy kcptun_port 11183)
PROXY_MODE=$(config_t_get global proxy_mode gfwlist)

load_config() {
	[ "$TCP_REDIR_SERVER" == "nil" -a "$UDP_REDIR_SERVER" == "nil" -a "$SOCKS5_PROXY_SERVER" == "nil" ] && {
		echolog "没有选择服务器！" 
		return 1
	}
	DNS_MODE=$(config_t_get global dns_mode ChinaDNS)
	UP_CHINADNS_MODE=$(config_t_get global up_chinadns_mode OpenDNS_1)
	process=1
	if [ "$(config_t_get global_forwarding process 0)" = "0" ] ;then
		process=$(cat /proc/cpuinfo | grep 'processor' | wc -l)
	else
		process=$(config_t_get global_forwarding process)
	fi
	LOCALHOST_PROXY_MODE=$(config_t_get global localhost_proxy_mode default)
	DNS_FORWARD=$(config_t_get global_dns dns_forward 208.67.222.222:443)
	DNS_FORWARD_IP=$(echo "$DNS_FORWARD" | awk -F':' '{print $1}')
	DNS_FORWARD_PORT=$(echo "$DNS_FORWARD" | awk -F':' '{print $2}')
	DNS1=$(config_t_get global_dns dns_1)
	DNS2=$(config_t_get global_dns dns_2)
	TCP_REDIR_PORT=$(config_t_get global_proxy tcp_redir_port 1041)
	UDP_REDIR_PORT=$(config_t_get global_proxy udp_redir_port 1042)
	TCP_REDIR_PORT2=
	UDP_REDIR_PORT2=
	TCP_REDIR_PORT3=
	UDP_REDIR_PORT3=
	SOCKS5_PROXY_PORT=$(config_t_get global_proxy socks5_proxy_port 1043)
	PROXY_IPV6=$(config_t_get global_proxy proxy_ipv6 0)
	mkdir -p /var/etc $CONFIG_PATH $RUN_PID_PATH
	config_load $CONFIG
	[ "$TCP_REDIR_SERVER" != "nil" ] && {
		TCP_REDIR_SERVER_TYPE=`echo $(config_get $TCP_REDIR_SERVER server_type) | tr 'A-Z' 'a-z'`
		gen_config_file $TCP_REDIR_SERVER $TCP_REDIR_PORT TCP $CONFIG_TCP_FILE
		echo "$TCP_REDIR_SERVER" > $CONFIG_PATH/tcp_server_id
	}
	[ "$UDP_REDIR_SERVER" != "nil" ] && {
		UDP_REDIR_SERVER_TYPE=`echo $(config_get $UDP_REDIR_SERVER server_type) | tr 'A-Z' 'a-z'`
		gen_config_file $UDP_REDIR_SERVER $UDP_REDIR_PORT UDP $CONFIG_UDP_FILE
		echo "$UDP_REDIR_SERVER" > $CONFIG_PATH/udp_server_id
	}
	[ "$SOCKS5_PROXY_SERVER" != "nil" ] && {
		SOCKS5_PROXY_SERVER_TYPE=`echo $(config_get $SOCKS5_PROXY_SERVER server_type) | tr 'A-Z' 'a-z'`
		gen_config_file $SOCKS5_PROXY_SERVER $SOCKS5_PROXY_PORT Socks5 $CONFIG_SOCKS5_FILE
		echo "$SOCKS5_PROXY_SERVER" > $CONFIG_PATH/socks5_server_id
	}

	return 0
}

gen_ss_ssr_config_file() {
	local server_type local_port kcptun server configfile
	server_type=$1
	local_port=$2
	kcptun=$3
	server=$4
	configfile=$5
	local server_port encrypt_method
	server_port=$(config_get $server server_port)
	encrypt_method=$(config_get $server ss_encrypt_method)
	[ "$server_type" == "ssr" ] && encrypt_method=$(config_get $server ssr_encrypt_method)
	[ "$kcptun" == "1" ] && {
		server_ip=127.0.0.1
		server_host=127.0.0.1
		server_port=$KCPTUN_REDIR_PORT
	}
	cat <<-EOF >$configfile
	{
		"server": "$server_host",
		"_comment": "$server_ip",
		"server_port": $server_port,
		"local_address": "0.0.0.0",
		"local_port": $local_port,
		"password": "$(config_get $server password)",
		"timeout": $(config_get $server timeout),
		"method": "$encrypt_method",
		"fast_open": $(config_get $server fast_open),
		"reuse_port": true,
	EOF
	[ "$1" == "ssr" ] && {
		cat <<-EOF >>$configfile
		"protocol": "$(config_get $server protocol)",
		"protocol_param": "$(config_get $server protocol_param)",
		"obfs": "$(config_get $server obfs)",
		"obfs_param": "$(config_get $server obfs_param)"
		EOF
	}
	echo -e "}" >> $configfile
}

gen_config_file() {
	local server local_port redir_type config_file_path server_host server_ip server_port server_type use_ipv6 network_type
	server=$1
	local_port=$2
	redir_type=$3
	config_file_path=$4
	server_host=$(config_get $server server)
	use_ipv6=$(config_get $server use_ipv6)
	network_type="ipv4"
	[ "$use_ipv6" == "1" ] && network_type="ipv6"
	server_ip=$(get_host_ip $network_type $server_host)
	server_port=$(config_get $server server_port)
	server_type=`echo $(config_get $server server_type) | tr 'A-Z' 'a-z'`
	echolog "$redir_type服务器IP地址:$server_ip"
	
	if [ "$redir_type" == "Socks5" ]; then
		if [ "$network_type" == "ipv6" ];then
			SOCKS5_PROXY_SERVER_IPV6=$server_ip
		else
			SOCKS5_PROXY_SERVER_IP=$server_ip
		fi
		SOCKS5_PROXY_SERVER_PORT=$server_port
		if [ "$server_type" == "ss" -o "$server_type" == "ssr" ]; then
			gen_ss_ssr_config_file $server_type $local_port 0 $server $config_file_path
		fi
		if [ "$server_type" == "v2ray" ]; then
			lua /usr/lib/lua/luci/model/cbi/passwall/api/gen_v2ray_client_config_file.lua $server nil nil $local_port > $config_file_path
		fi
		if [ "$server_type" == "brook" ]; then
			BROOK_SOCKS5_CMD="client -l 0.0.0.0:$local_port -i 0.0.0.0 -s $server_ip:$server_port -p $(config_get $server password)"
		fi
	fi
	
	if [ "$redir_type" == "UDP" ]; then
		if [ "$network_type" == "ipv6" ];then
			UDP_REDIR_SERVER_IPV6=$server_ip
		else
			UDP_REDIR_SERVER_IP=$server_ip
		fi
		UDP_REDIR_SERVER_PORT=$server_port
		if [ "$server_type" == "ss" -o "$server_type" == "ssr" ]; then
			gen_ss_ssr_config_file $server_type $local_port 0 $server $config_file_path
		fi
		if [ "$server_type" == "v2ray" ]; then
			lua /usr/lib/lua/luci/model/cbi/passwall/api/gen_v2ray_client_config_file.lua $server udp $local_port nil > $config_file_path
		fi
		if [ "$server_type" == "brook" ]; then
			BROOK_UDP_CMD="tproxy -l 0.0.0.0:$local_port -s $server_ip:$server_port -p $(config_get $server password)"
		fi
	fi
	
	if [ "$redir_type" == "TCP" ]; then
		if [ "$network_type" == "ipv6" ];then
			TCP_REDIR_SERVER_IPV6=$server_ip
		else
			TCP_REDIR_SERVER_IP=$server_ip
		fi
		TCP_REDIR_SERVER_PORT=$server_port
		if [ "$server_type" == "v2ray" ]; then
			lua /usr/lib/lua/luci/model/cbi/passwall/api/gen_v2ray_client_config_file.lua $server tcp $local_port nil > $config_file_path
		else
			local kcptun_use kcptun_server_host kcptun_port kcptun_config
			kcptun_use=$(config_get $server use_kcp)
			kcptun_server_host=$(config_get $server kcp_server)
			kcptun_port=$(config_get $server kcp_port)
			kcptun_config=$(config_get $server kcp_opts)
			kcptun_path=""
			lbenabled=$(config_t_get global_haproxy balancing_enable 0)
			if [ "$kcptun_use" == "1" ] && ([ -z "$kcptun_port" ] || [ -z "$kcptun_config" ]); then
				echolog "【检测到启用KCP，但未配置KCP参数】，跳过~"
			fi
			if [ "$kcptun_use" == "1" -a -n "$kcptun_port" -a -n "$kcptun_config" -a "$lbenabled" == "1" ];then
				echolog "【检测到启用KCP，但KCP与负载均衡二者不能同时开启】，跳过~"
			fi
			
			if [ "$kcptun_use" == "1" ];then
				if [ -f "$(config_t_get global_kcptun kcptun_client_file)" ];then
					kcptun_path=$(config_t_get global_kcptun kcptun_client_file)
				else
					temp=$(find_bin kcptun_client)
					[ -n "$temp" ] && kcptun_path=$temp
				fi
			fi
			
			if [ "$kcptun_use" == "1" -a -z "$kcptun_path" ] && ([ -n "$kcptun_port" ] || [ -n "$kcptun_config" ]);then
				echolog "【检测到启用KCP，但未安装KCP主程序，请自行到自动更新下载KCP】，跳过~"
			fi
			
			if [ "$kcptun_use" == "1" -a -n "$kcptun_port" -a -n "$kcptun_config" -a "$lbenabled" == "0" -a -n "$kcptun_path" ];then
				if [ -z "$kcptun_server_host" ]; then
					start_kcptun "$kcptun_path" $server_ip $kcptun_port "$kcptun_config"
				else
					kcptun_use_ipv6=$(config_get $server kcp_use_ipv6)
					network_type="ipv4"
					[ "$kcptun_use_ipv6" == "1" ] && network_type="ipv6"
					kcptun_server_ip=$(get_host_ip $network_type $kcptun_server_host)
					echolog "KCP服务器IP地址:$kcptun_server_ip"
					TCP_REDIR_SERVER_IP=$kcptun_server_ip
					start_kcptun "$kcptun_path" $kcptun_server_ip $kcptun_port "$kcptun_config"
				fi
				echolog "运行Kcptun..." 
				if [ "$server_type" == "ss" -o "$server_type" == "ssr" ]; then
					gen_ss_ssr_config_file $server_type $local_port 1 $server $config_file_path
				fi
				if [ "$server_type" == "brook" ]; then
					BROOK_TCP_CMD="tproxy -l 0.0.0.0:$local_port -s 127.0.0.1:$KCPTUN_REDIR_PORT -p $(config_get $server password)"
				fi
			else
				if [ "$server_type" == "ss" -o "$server_type" == "ssr" ]; then
					gen_ss_ssr_config_file $server_type $local_port 0 $server $config_file_path
				fi
				if [ "$server_type" == "brook" ]; then
					BROOK_TCP_CMD="tproxy -l 0.0.0.0:$local_port -s $server_ip:$server_port -p $(config_get $server password)"
				fi
			fi
		fi
	fi
	return 0
}

start_kcptun() {
	kcptun_bin=$1
	if [ -z "$kcptun_bin" ]; then
		echolog "找不到Kcptun客户端主程序，无法启用！！！" 
	else
		$kcptun_bin --log $CONFIG_PATH/kcptun -l 0.0.0.0:$KCPTUN_REDIR_PORT -r $2:$3 $4 >/dev/null 2>&1 &
	fi
}

start_tcp_redir_other() {
	if [ "$TCP_REDIR_SERVER_NUM" -ge 2 ] ;then
		for i in $(seq 2 $TCP_REDIR_SERVER_NUM)
		do
			eval temp_server=\$TCP_REDIR_SERVER$i
			[ "$temp_server" != "nil" ] && {
				TYPE=`echo $(config_get $temp_server server_type) | tr 'A-Z' 'a-z'`
				local config_file=$CONFIG_PATH/TCP$i.json
				local port_temp=`expr $TCP_REDIR_PORT + 1 `
				local port=`echo $(get_not_exists_port_after $port_temp tcp)`
				eval TCP_REDIR_PORT$i=$port
				gen_config_file $temp_server $port TCP $config_file
				if [ "$TYPE" == "v2ray" ]; then
					v2ray_path=$(config_t_get global_v2ray v2ray_client_file)
					if [ -f "${v2ray_path}/v2ray" ];then
						${v2ray_path}/v2ray -config=$config_file > /dev/null &
					else
						v2ray_bin=$(find_bin V2ray)
						[ -n "$v2ray_bin" ] && $v2ray_bin -config=$config_file > /dev/null &
					fi
				elif [ "$TYPE" == "brook" ]; then
					brook_bin=$(find_bin Brook)
					[ -n "$brook_bin" ] && $brook_bin $BROOK_TCP_CMD &>/dev/null &
				else
					ss_bin=$(find_bin "$TYPE"-redir)
					[ -n "$ss_bin" ] && {
						for k in $(seq 1 $process)
						do
							$ss_bin -c $config_file -f $RUN_PID_PATH/tcp_${TYPE}_$k_$i > /dev/null 2>&1 &
						done
					}
				fi
			}
		done
	fi
}

start_udp_redir_other() {
	if [ "$UDP_REDIR_SERVER_NUM" -ge 2 ] ;then
		for i in $(seq 2 $UDP_REDIR_SERVER_NUM)
		do
			eval temp_server=\$UDP_REDIR_SERVER$i
			[ "$temp_server" != "nil" ] && {
				TYPE=`echo $(config_get $temp_server server_type) | tr 'A-Z' 'a-z'`
				local config_file=$CONFIG_PATH/UDP$i.json
				local port_temp=`expr $TCP_REDIR_PORT + 1 `
				local port=`echo $(get_not_exists_port_after $port_temp udp)`
				eval UDP_REDIR_PORT$i=$port
				gen_config_file $temp_server $port UDP $config_file
				if [ "$TYPE" == "v2ray" ]; then
					v2ray_path=$(config_t_get global_v2ray v2ray_client_file)
					if [ -f "${v2ray_path}/v2ray" ];then
						${v2ray_path}/v2ray -config=$config_file > /dev/null &
					else
						v2ray_bin=$(find_bin V2ray)
						[ -n "$v2ray_bin" ] && $v2ray_bin -config=$config_file > /dev/null &
					fi
				elif [ "$TYPE" == "brook" ]; then
					brook_bin=$(find_bin brook)
					[ -n "$brook_bin" ] && $brook_bin $BROOK_UDP_CMD &>/dev/null &
				else
					ss_bin=$(find_bin "$TYPE"-redir)
					[ -n "$ss_bin" ] && {
						$ss_bin -c $config_file -f $RUN_PID_PATH/udp_${TYPE}_1_$i -U > /dev/null 2>&1 &
					}
				fi
			}
		done
	fi
}

start_tcp_redir() {
	if [ "$TCP_REDIR_SERVER" != "nil" ];then
		if [ "$TCP_REDIR_SERVER_TYPE" == "v2ray" ]; then
			v2ray_path=$(config_t_get global_v2ray v2ray_client_file)
			if [ -f "${v2ray_path}/v2ray" ];then
				${v2ray_path}/v2ray -config=$CONFIG_TCP_FILE > /dev/null &
			else
				v2ray_bin=$(find_bin V2ray)
				[ -n "$v2ray_bin" ] && $v2ray_bin -config=$CONFIG_TCP_FILE > /dev/null &
			fi
		elif [ "$TCP_REDIR_SERVER_TYPE" == "brook" ]; then
			brook_bin=$(find_bin Brook)
			[ -n "$brook_bin" ] && $brook_bin $BROOK_TCP_CMD &>/dev/null &
		else
			ss_bin=$(find_bin "$TCP_REDIR_SERVER_TYPE"-redir)
			[ -n "$ss_bin" ] && {
				for i in $(seq 1 $process)
				do
					$ss_bin -c $CONFIG_TCP_FILE -f $RUN_PID_PATH/tcp_${TCP_REDIR_SERVER_TYPE}_$i > /dev/null 2>&1 &
				done
			}
		fi
	fi
}

start_udp_redir() {
	if [ "$UDP_REDIR_SERVER" != "nil" ];then
		if [ "$UDP_REDIR_SERVER_TYPE" == "v2ray" ]; then
			v2ray_path=$(config_t_get global_v2ray v2ray_client_file)
			if [ -f "${v2ray_path}/v2ray" ];then
				${v2ray_path}/v2ray -config=$CONFIG_UDP_FILE > /dev/null &
			else
				v2ray_bin=$(find_bin V2ray)
				[ -n "$v2ray_bin" ] && $v2ray_bin -config=$CONFIG_UDP_FILE > /dev/null &
			fi
		elif [ "$UDP_REDIR_SERVER_TYPE" == "brook" ]; then
			brook_bin=$(find_bin brook)
			[ -n "$brook_bin" ] && $brook_bin $BROOK_UDP_CMD &>/dev/null &
		else
			ss_bin=$(find_bin "$UDP_REDIR_SERVER_TYPE"-redir)
			[ -n "$ss_bin" ] && {
				$ss_bin -c $CONFIG_UDP_FILE -f $RUN_PID_PATH/udp_${UDP_REDIR_SERVER_TYPE}_1 -U > /dev/null 2>&1 &
			}
		fi
	fi
}

start_socks5_proxy() {
	if [ "$SOCKS5_PROXY_SERVER" != "nil" ];then
		if [ "$SOCKS5_PROXY_SERVER_TYPE" == "v2ray" ]; then
			v2ray_path=$(config_t_get global_v2ray v2ray_client_file)
			if [ -f "${v2ray_path}/v2ray" ];then
				${v2ray_path}/v2ray -config=$CONFIG_SOCKS5_FILE > /dev/null &
			else
				v2ray_bin=$(find_bin V2ray)
				[ -n "$v2ray_bin" ] && $v2ray_bin -config=$CONFIG_SOCKS5_FILE > /dev/null &
			fi
		elif [ "$SOCKS5_PROXY_SERVER_TYPE" == "brook" ]; then
			brook_bin=$(find_bin brook)
			[ -n "$brook_bin" ] && $brook_bin $BROOK_SOCKS5_CMD &>/dev/null &
		else
			ss_bin=$(find_bin "$SOCKS5_PROXY_SERVER_TYPE"-local)
			[ -n "$ss_bin" ] && $ss_bin -c $CONFIG_SOCKS5_FILE -b 0.0.0.0 > /dev/null 2>&1 &
		fi
	fi
}

clean_log() {
	logsnum=$(cat $LOG_FILE 2>/dev/null | wc -l)
	if [ "$logsnum" -gt 300 ];then
		rm -f $LOG_FILE >/dev/null 2>&1 &
		echolog "日志文件过长，清空处理！" 
	fi
}

set_cru() {
	autoupdate=$(config_t_get global_rules auto_update)
	weekupdate=$(config_t_get global_rules week_update)
	dayupdate=$(config_t_get global_rules time_update)
	autoupdatesubscribe=$(config_t_get global_subscribe auto_update_subscribe)
	weekupdatesubscribe=$(config_t_get global_subscribe week_update_subscribe)
	dayupdatesubscribe=$(config_t_get global_subscribe time_update_subscribe)
	if [ "$autoupdate" = "1" ];then
		if [ "$weekupdate" = "7" ];then
			echo "0 $dayupdate * * * $APP_PATH/rule_update.sh" >> /etc/crontabs/root
			echolog "设置自动更新规则在每天 $dayupdate 点。" 
		else
			echo "0 $dayupdate * * $weekupdate $APP_PATH/rule_update.sh" >> /etc/crontabs/root
			echolog "设置自动更新规则在星期 $weekupdate 的 $dayupdate 点。" 
		fi
	else
		sed -i '/rule_update.sh/d' /etc/crontabs/root >/dev/null 2>&1 &
	fi

	if [ "$autoupdatesubscribe" = "1" ];then
		if [ "$weekupdatesubscribe" = "7" ];then
			echo "0 $dayupdatesubscribe * * * $APP_PATH/subscription.sh" >> /etc/crontabs/root
			echolog "设置服务器订阅自动更新规则在每天 $dayupdatesubscribe 点。" 
		else
			echo "0 $dayupdatesubscribe * * $weekupdate $APP_PATH/subscription.sh" >> /etc/crontabs/root
			echolog "设置服务器订阅自动更新规则在星期 $weekupdate 的 $dayupdatesubscribe 点。" 
		fi
	else
		sed -i '/subscription.sh/d' /etc/crontabs/root >/dev/null 2>&1 &
	fi
}

start_crontab() {
	sed -i '/$CONFIG/d' /etc/crontabs/root >/dev/null 2>&1 &
	start_daemon=$(config_t_get global_delay start_daemon)
	if [ "$start_daemon" = "1" ];then
		echo "*/2 * * * * nohup $APP_PATH/monitor.sh > /dev/null 2>&1" >> /etc/crontabs/root
		echolog "已启动守护进程。" 
	fi
	
	auto_on=$(config_t_get global_delay auto_on)
	if [ "$auto_on" = "1" ];then
		time_off=$(config_t_get global_delay time_off)
		time_on=$(config_t_get global_delay time_on)
		time_restart=$(config_t_get global_delay time_restart)
		[ -z "$time_off" -o "$time_off" != "nil" ] && {
			echo "0 $time_off * * * /etc/init.d/$CONFIG stop" >> /etc/crontabs/root
			echolog "设置自动关闭在每天 $time_off 点。" 
		}
		[ -z "$time_on" -o "$time_on" != "nil" ] && {
			echo "0 $time_on * * * /etc/init.d/$CONFIG start" >> /etc/crontabs/root
			echolog "设置自动开启在每天 $time_on 点。" 
		}
		[ -z "$time_restart" -o "$time_restart" != "nil" ] && {
			echo "0 $time_restart * * * /etc/init.d/$CONFIG restart" >> /etc/crontabs/root
			echolog "设置自动重启在每天 $time_restart 点。" 
		}
	fi
	
	[ "$AUTO_SWITCH_ENABLE" = "1" ] && {
		testing_time=$(config_t_get auto_switch testing_time)
		[ -n "$testing_time" ] && {
			echo "*/$testing_time * * * * nohup $APP_PATH/test.sh > /dev/null 2>&1" >> /etc/crontabs/root
			echolog "设置每$testing_time分钟执行检测脚本。"
		}
	}
	/etc/init.d/cron restart
}

stop_crontab() {
	sed -i "/$CONFIG/d" /etc/crontabs/root >/dev/null 2>&1 &
	ps | grep "$APP_PATH/test.sh" | grep -v "grep" | awk '{print $1}' | xargs kill -9 >/dev/null 2>&1 &
	rm -f /var/lock/passwall_test.lock >/dev/null 2>&1 &
	/etc/init.d/cron restart
	echolog "清除定时执行命令。" 
}

start_dns() {
	case "$DNS_MODE" in
		dns2socks)
			dns2socks_bin=$(find_bin dns2socks)
			sslocal_bin=$(find_bin "$TCP_REDIR_SERVER_TYPE"-local)
			[ -n "$dns2socks_bin" -a -n "$sslocal_bin" ] && {
				nohup $sslocal_bin -c $CONFIG_TCP_FILE -l 3080 -f $RUN_PID_PATH/$TCP_REDIR_SERVER_TYPE-local.pid >/dev/null 2>&1 &
				nohup $dns2socks_bin 127.0.0.1:3080 $DNS_FORWARD 127.0.0.1:7913 >/dev/null 2>&1 &
				echolog "运行DNS转发模式：dns2socks+$TCP_REDIR_SERVER_TYPE-local..." 
			}
		;;
		Pcap_DNSProxy)
			Pcap_DNSProxy_bin=$(find_bin Pcap_DNSProxy)
			[ -n "$Pcap_DNSProxy_bin" ] && {
				nohup $Pcap_DNSProxy_bin -c /etc/pcap-dnsproxy >/dev/null 2>&1 &
				echolog "运行DNS转发模式：Pcap_DNSProxy..."
			}
		;;
		pdnsd)
			pdnsd_bin=$(find_bin pdnsd)
			[ -n "$pdnsd_bin" ] && {
				gen_pdnsd_config
				nohup $pdnsd_bin --daemon -c $CACHEDIR/pdnsd.conf -p $RUN_PID_PATH/pdnsd.pid -d >/dev/null 2>&1 &
				echolog "运行DNS转发模式：Pdnsd..." 
			}
		;;
		local_7913)
			echolog "运行DNS转发模式：使用本机7913端口DNS服务解析域名..." 
		;;
		chinadns)
			chinadns_bin=$(find_bin ChinaDNS)
			[ -n "$chinadns_bin" ] && {
				other=1
				echolog "运行DNS转发模式：ChinaDNS..." 
				dns1=$(config_t_get global_dns dns_1)
				[ "$dns1" = "dnsbyisp" ] && dns1=`cat /tmp/resolv.conf.auto 2>/dev/null | grep -E -o "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" |sort -u |grep -v 0.0.0.0 |grep -v 127.0.0.1|sed -n '2P'`
				case "$UP_CHINADNS_MODE" in
					OpenDNS_1)
						other=0
						nohup $chinadns_bin -p 7913 -c $APP_PATH_RULE/chnroute -m -d -s $dns1,208.67.222.222:443,208.67.222.222:5353 >/dev/null 2>&1 &
						echolog "运行ChinaDNS上游转发模式：$dns1,208.67.222.222..." 
					;;
					OpenDNS_2)
						other=0
						nohup $chinadns_bin -p 7913 -c $APP_PATH_RULE/chnroute -m -d -s $dns1,208.67.220.220:443,208.67.220.220:5353 >/dev/null 2>&1 &
						echolog "运行ChinaDNS上游转发模式：$dns1,208.67.220.220..." 
					;;
					custom)
						other=0
						UP_CHINADNS_CUSTOM=$(config_t_get global up_chinadns_custom '114.114.114.114,208.67.222.222:5353')
						nohup $chinadns_bin -p 7913 -c $APP_PATH_RULE/chnroute -m -d -s $UP_CHINADNS_CUSTOM >/dev/null 2>&1 &
						echolog "运行ChinaDNS上游转发模式：$UP_CHINADNS_CUSTOM..." 
					;;
					dnsproxy)
						dnsproxy_bin=$(find_bin dnsproxy)
						[ -n "$dnsproxy_bin" ] && {
							nohup $dnsproxy_bin -d -T -p 7913 -R $DNS_FORWARD_IP -P $DNS_FORWARD_PORT >/dev/null 2>&1 &
							echolog "运行ChinaDNS上游转发模式：dnsproxy..." 
						}
					;;
					dns-forwarder)
						dnsforwarder_bin=$(find_bin dns-forwarder)
						[ -n "$dnsforwarder_bin" ] && {
							nohup $dnsforwarder_bin -p 7913 -s $DNS_FORWARD >/dev/null 2>&1 &
							echolog "运行ChinaDNS上游转发模式：dns-forwarder..." 
						}
					;;
				esac
				if [ "$other" = "1" ];then
					nohup $chinadns_bin -p 7923 -c $APP_PATH_RULE/chnroute -m -d -s $dns1,127.0.0.1:7913 >/dev/null 2>&1 &
				fi
			}
		;;
	esac
	echolog "若不正常，请尝试其他模式！" 
}

add_dnsmasq() {
	mkdir -p $TMP_DNSMASQ_PATH $DNSMASQ_PATH /var/dnsmasq.d
	local wirteconf dnsconf dnsport isp_dns isp_ip
	dnsport=$(config_t_get global_dns dns_port)
	[ -z "$dnsport" ] && dnsport=0
	if [ "$DNS1" = "dnsbyisp" -o "$DNS2" = "dnsbyisp" ]; then
		cat > /etc/dnsmasq.conf <<EOF
all-servers
no-poll
no-resolv
cache-size=2048
local-ttl=60
neg-ttl=3600
max-cache-ttl=1200
EOF
		echolog "生成Dnsmasq配置文件。" 
		
		if [ "$dnsport" != "0" ]; then
			isp_dns=`cat /tmp/resolv.conf.auto 2>/dev/null | grep -E -o "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" | sort -u | grep -v 0.0.0.0 | grep -v 127.0.0.1`
			failcount=0
			while [ "$failcount" -lt "10" ]
			do
				interface=`ifconfig | grep "$dnsport" | awk '{print $1}'`
				if [ -z "$interface" ];then
					echolog "找不到出口接口：$dnsport，1分钟后再重试" 
					let "failcount++"
					[ "$failcount" -ge 10 ] && exit 0
					sleep 1m
				else
					[ -n "$isp_dns" ] && {
						for isp_ip in $isp_dns
						do
							echo server=$isp_ip >> /etc/dnsmasq.conf
							route add -host ${isp_ip} dev ${dnsport}
							echolog "添加运营商DNS出口路由表：$dnsport" 
						done
					}
					[ "$DNS1" != "dnsbyisp" ] && {
						route add -host ${DNS1} dev ${dnsport}
						echolog "添加DNS1出口路由表：$dnsport" 
						echo server=$DNS1 >> /etc/dnsmasq.conf
					}
					[ "$DNS2" != "dnsbyisp" ] && {
						route add -host ${DNS2} dev ${dnsport}
						echolog "添加DNS2出口路由表：$dnsport" 
						echo server=$DNS2 >> /etc/dnsmasq.conf
					}
					break
				fi
			done
		else
			isp_dns=`cat /tmp/resolv.conf.auto 2>/dev/null | grep -E -o "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" | sort -u | grep -v 0.0.0.0 | grep -v 127.0.0.1`
			[ -n "$isp_dns" ] && {
				for isp_ip in $isp_dns
				do
					echo server=$isp_ip >> /etc/dnsmasq.conf
				done
			}
			[ "$DNS1" != "dnsbyisp" ] && {
				echo server=$DNS1 >> /etc/dnsmasq.conf
			}
			[ "$DNS2" != "dnsbyisp" ] && {
				echo server=$DNS2 >> /etc/dnsmasq.conf
			}
		fi
	else
		wirteconf=$(cat /etc/dnsmasq.conf 2>/dev/null | grep "server=$DNS1")
		dnsconf=$(cat /etc/dnsmasq.conf 2>/dev/null | grep "server=$DNS2")
		if [ "$dnsport" != "0" ]; then
			failcount=0
			while [ "$failcount" -lt "10" ]
			do
				interface=`ifconfig | grep "$dnsport" | awk '{print $1}'`
				if [ -z "$interface" ];then
					echolog "找不到出口接口：$dnsport，1分钟后再重试" 
					let "failcount++"
					[ "$failcount" -ge 10 ] && exit 0
					sleep 1m
				else
					route add -host ${DNS1} dev ${dnsport}
					echolog "添加DNS1出口路由表：$dnsport" 
					route add -host ${DNS2} dev ${dnsport}
					echolog "添加DNS2出口路由表：$dnsport" 
					break
				fi
			done
		fi
		if [ -z "$wirteconf" ] || [ -z "$dnsconf" ];then
			cat > /etc/dnsmasq.conf <<EOF
all-servers
no-poll
no-resolv
server=$DNS1
server=$DNS2
cache-size=2048
local-ttl=60
neg-ttl=3600
max-cache-ttl=1200
EOF
			echolog "生成Dnsmasq配置文件。" 
		fi
	fi
# if [ -n "cat /var/state/network |grep pppoe|awk -F '.' '{print $2}'" ]; then
	# sed -i '/except-interface/d' /etc/dnsmasq.conf >/dev/null 2>&1 &
	# for wanname in $(cat /var/state/network |grep pppoe|awk -F '.' '{print $2}')
	# do
		# echo "except-interface=$(uci get network.$wanname.ifname)" >>/etc/dnsmasq.conf
	# done
# fi

	subscribe_by_ss=$(config_t_get global_subscribe subscribe_by_ss)
	[ -z "$subscribe_by_ss" ] && subscribe_by_ss=0
	[ "$subscribe_by_ss" -eq 1 ] && {
		baseurl=$(config_t_get global_subscribe baseurl)
		[ -n "$baseurl" ] && {
			for url in $baseurl
			do
				if [ -n "`echo -n "$url" |grep "//"`" ]; then
					echo -n "$url" | awk -F'/' '{print $3}' | sed "s/^/server=&\/./g" | sed "s/$/\/127.0.0.1#7913/g" >> $TMP_DNSMASQ_PATH/subscribe.conf
					echo -n "$url" | awk -F'/' '{print $3}' | sed "s/^/ipset=&\/./g" | sed "s/$/\/router/g" >> $TMP_DNSMASQ_PATH/subscribe.conf
				else
					echo -n "$url" | awk -F'/' '{print $1}' | sed "s/^/server=&\/./g" | sed "s/$/\/127.0.0.1#7913/g" >> $TMP_DNSMASQ_PATH/subscribe.conf
					echo -n "$url" | awk -F'/' '{print $1}' | sed "s/^/ipset=&\/./g" | sed "s/$/\/router/g" >> $TMP_DNSMASQ_PATH/subscribe.conf
				fi
			done
		restdns=1
		}
	}

	if [ ! -f "$TMP_DNSMASQ_PATH/gfwlist.conf" ];then
		ln -s $APP_PATH_DNSMASQ/gfwlist.conf $TMP_DNSMASQ_PATH/gfwlist.conf
		restdns=1
	fi
	
	if [ ! -f "$TMP_DNSMASQ_PATH/blacklist_host.conf" ];then
		cat $APP_PATH_RULE/blacklist_host | awk '{print "server=/."$1"/127.0.0.1#7913\nipset=/."$1"/blacklist"}' >> $TMP_DNSMASQ_PATH/blacklist_host.conf
		restdns=1
	fi
	
	if [ ! -f "$TMP_DNSMASQ_PATH/whitelist_host.conf" ];then
		cat $APP_PATH_RULE/whitelist_host | sed "s/^/ipset=&\/./g" | sed "s/$/\/&whitelist/g" | sort | awk '{if ($0!=line) print;line=$0}' >$TMP_DNSMASQ_PATH/whitelist_host.conf
		restdns=1
	fi
	
	if [ ! -f "$TMP_DNSMASQ_PATH/router.conf" ];then
		cat $APP_PATH_RULE/router | awk '{print "server=/."$1"/127.0.0.1#7913\nipset=/."$1"/router"}' >> $TMP_DNSMASQ_PATH/router.conf
		restdns=1
	fi
	
	userconf=$(grep -c "" $APP_PATH_DNSMASQ/user.conf)
	if [ "$userconf" -gt 0  ];then
		ln -s $APP_PATH_DNSMASQ/user.conf $TMP_DNSMASQ_PATH/user.conf
		restdns=1
	fi
	
	backhome=$(config_t_get global proxy_mode gfwlist)
	if [ "$backhome" == "returnhome" ];then
		rm -rf $TMP_DNSMASQ_PATH/gfwlist.conf
		rm -rf $TMP_DNSMASQ_PATH/blacklist_host.conf
		rm -rf $TMP_DNSMASQ_PATH/whitelist_host.conf
		restdns=1
		echolog "生成回国模式Dnsmasq配置文件。" 
	fi
	
	echo "conf-dir=$TMP_DNSMASQ_PATH" > /var/dnsmasq.d/dnsmasq-$CONFIG.conf
	echo "conf-dir=$TMP_DNSMASQ_PATH" > $DNSMASQ_PATH/dnsmasq-$CONFIG.conf
	if [ "$restdns" == 1 ];then
		echolog "重启Dnsmasq。。。" 
		/etc/init.d/dnsmasq restart  2>/dev/null
	fi
}

gen_pdnsd_config() {
	CACHEDIR=/var/pdnsd
	CACHE=$CACHEDIR/pdnsd.cache
	if ! test -f "$CACHE"; then
		mkdir -p `dirname $CACHE`
		touch $CACHE
		chown -R root.nogroup $CACHEDIR
	fi
	cat > $CACHEDIR/pdnsd.conf <<-EOF
	global {
		perm_cache=1024;
		cache_dir="/var/pdnsd";
		run_as="root";
		server_ip = 127.0.0.1;
		server_port=7913;
		status_ctl = on;
		query_method=tcp_only;
		min_ttl=1d;
		max_ttl=1w;
		timeout=10;
		tcp_qtimeout=1;
		par_queries=2;
		neg_domain_pol=on;
		udpbufsize=1024;
		}
	server {
		label = "opendns";
		ip = 208.67.222.222, 208.67.220.220;
		edns_query=on;
		port = 5353;
		timeout = 4;
		interval=60;
		uptest = none;
		purge_cache=off;
		caching=on;
		}
	source {
		ttl=86400;
		owner="localhost.";
		serve_aliases=on;
		file="/etc/hosts";
		}
EOF
}

stop_dnsmasq() {
	if [ "$TCP_REDIR_SERVER" == "nil" ]; then
		rm -rf /var/dnsmasq.d/dnsmasq-$CONFIG.conf
		rm -rf $DNSMASQ_PATH/dnsmasq-$CONFIG.conf
		rm -rf $TMP_DNSMASQ_PATH
		/etc/init.d/dnsmasq restart  2>/dev/null
		echolog "没有选择服务器！" 
	fi
}

start_haproxy(){
	enabled=$(config_t_get global_haproxy balancing_enable 0)
	[ "$enabled" = "1" ] && {
		haproxy_bin=$(find_bin haproxy)
		[ -n "$haproxy_bin" ] && {
			bport=$(config_t_get global_haproxy haproxy_port)
			cat <<-EOF >$HAPROXY_FILE
global
    log         127.0.0.1 local2
    chroot      /usr/bin
    pidfile     $RUN_PID_PATH/haproxy.pid
    maxconn     60000
    stats socket  $RUN_PID_PATH/haproxy.sock
    user        root
    daemon
defaults
    mode                    tcp
    log                     global
    option                  tcplog
    option                  dontlognull
    option http-server-close
    #option forwardfor       except 127.0.0.0/8
    option                  redispatch
    retries                 2
    timeout http-request    10s
    timeout queue           1m
    timeout connect         10s
    timeout client          1m
    timeout server          1m
    timeout http-keep-alive 10s
    timeout check           10s
    maxconn                 3000
listen shadowsocks
    bind 0.0.0.0:$bport
    mode tcp
EOF
			for i in $(seq 0 100)
			do
				bips=$(config_t_get balancing lbss '' $i)
				bports=$(config_t_get balancing lbort '' $i)
				bweight=$(config_t_get balancing lbweight '' $i)
				exports=$(config_t_get balancing export '' $i)
				bbackup=$(config_t_get balancing backup '' $i)
				if [ -z "$bips" ] || [ -z "$bports" ] ; then
					break
				fi
				if [ "$bbackup" = "1" ] ; then
					bbackup=" backup"
					echolog "添加故障转移备服务器$bips" 
				else
					bbackup=""
					echolog "添加负载均衡主服务器$bips" 
				fi
				si=`echo $bips|grep -E "([0-9]{1,3}[\.]){3}[0-9]{1,3}"`
				if [ -z "$si" ];then      
					bips=`resolveip -4 -t 2 $bips|awk 'NR==1{print}'`
					if [ -z "$bips" ];then
						bips=`nslookup $bips localhost | sed '1,4d' | awk '{print $3}' | grep -v :|awk 'NR==1{print}'`
					fi
					echolog "服务器IP为：$bips"
				fi
				echo "    server ss$i $bips:$bports weight $bweight check inter 1500 rise 1 fall 3 $bbackup" >> $HAPROXY_FILE
				if [ "$exports" != "0" ]; then
					failcount=0
					while [ "$failcount" -lt "10" ]
					do
						interface=`ifconfig | grep "$exports" | awk '{print $1}'`
						if [ -z "$interface" ];then
							echolog "找不到出口接口：$exports，1分钟后再重试" 
							let "failcount++"
							[ "$failcount" -ge 10 ] && exit 0
							sleep 1m
						else
							route add -host ${bips} dev ${exports}
							echolog "添加SS出口路由表：$exports" 
							echo "$bips" >> /tmp/balancing_ip
							break
						fi
					done
				fi
			done
			#生成负载均衡控制台
			adminstatus=$(config_t_get global_haproxy admin_enable)
			if [ "$adminstatus" = "1" ];then
				adminport=$(config_t_get global_haproxy admin_port)
				adminuser=$(config_t_get global_haproxy admin_user)
				adminpassword=$(config_t_get global_haproxy admin_password)
			cat <<-EOF >>$HAPROXY_FILE
		listen status
			bind 0.0.0.0:$adminport
			mode http                   
			stats refresh 30s
			stats uri  /  
			stats auth $adminuser:$adminpassword
			#stats hide-version
			stats admin if TRUE
		EOF
			fi
			nohup $haproxy_bin -f $HAPROXY_FILE 2>&1
			echolog "负载均衡服务运行成功！" 
		}
	}
} 

add_vps_port() {
	multiwan=$(config_t_get global_dns wan_port 0)
	if [ "$multiwan" != "0" ]; then
		failcount=0
		while [ "$failcount" -lt "10" ]
		do
			interface=`ifconfig | grep "$multiwan" | awk '{print $1}'`
			if [ -z "$interface" ];then
				echolog "找不到出口接口：$multiwan，1分钟后再重试" 
				let "failcount++"
				[ "$failcount" -ge 10 ] && exit 0
				sleep 1m
			else
				route add -host ${TCP_REDIR_SERVER_IP} dev ${multiwan}
				route add -host ${UDP_REDIR_SERVER_IP} dev ${multiwan}
				echolog "添加SS出口路由表：$multiwan" 
				echo "$TCP_REDIR_SERVER_IP" > $CONFIG_PATH/tcp_ip
				echo "$UDP_REDIR_SERVER_IP" > $CONFIG_PATH/udp_ip
				break
			fi
		done
	fi
}

del_vps_port() {
	tcp_ip=$(cat $CONFIG_PATH/tcp_ip 2> /dev/null)
	udp_ip=$(cat $CONFIG_PATH/udp_ip 2> /dev/null)
	[ -n "$tcp_ip" ] && route del -host ${tcp_ip}
	[ -n "$udp_ip" ] && route del -host ${udp_ip}
}

kill_all() {
	kill -9 $(pidof $@) >/dev/null 2>&1 &
}

boot() {
	local delay=$(config_t_get global_delay start_delay 0)
	if [ "$delay" -gt 0 ]; then
		[ "$TCP_REDIR_SERVER" != "nil" -o "$UDP_REDIR_SERVER" != "nil" ] && {
			echolog "执行启动延时 $delay 秒后再启动!" 
			sleep $delay && start >/dev/null 2>&1 &
		}
	else
		start
	fi
	return 0
}

start() {
	echolog "开始运行脚本！" 
	! load_config && return 1
	add_vps_port
	start_haproxy
	#防止并发开启服务
	[ -f "$LOCK_FILE" ] && return 3
	touch "$LOCK_FILE"
	start_tcp_redir
	start_udp_redir
	start_socks5_proxy
	start_tcp_redir_other
	start_udp_redir_other
	start_dns
	add_dnsmasq
	source $APP_PATH/iptables.sh start
	/etc/init.d/dnsmasq restart >/dev/null 2>&1 &
	start_crontab
	set_cru
	rm -f "$LOCK_FILE"
	echolog "运行完成！" 
	return 0
}

stop() {
	while [ -f "$LOCK_FILE" ]; do
		sleep 1s
	done
	clean_log
	source $APP_PATH/iptables.sh stop
	del_vps_port
	kill_all pdnsd Pcap_DNSProxy brook dns2socks haproxy dns-forwarder chinadns dnsproxy
	ps -w | grep -E "$CONFIG_TCP_FILE|$CONFIG_UDP_FILE|$CONFIG_SOCKS5_FILE" | grep -v "grep" | awk '{print $1}' | xargs kill -9 >/dev/null 2>&1 &
	ps -w | grep -E "$CONFIG_PATH" | grep -v "grep" | awk '{print $1}' | xargs kill -9 >/dev/null 2>&1 &
	ps -w | grep "kcptun_client" | grep "$KCPTUN_REDIR_PORT" | grep -v "grep" | awk '{print $1}' | xargs kill -9 >/dev/null 2>&1 &
	rm -rf /var/pdnsd/pdnsd.cache
	rm -rf $TMP_DNSMASQ_PATH
	rm -rf $CONFIG_PATH
	stop_dnsmasq
	stop_crontab
	echolog "关闭相关服务，清理相关文件和缓存完成。\n"
	sleep 1s
}

case $1 in
stop)
	stop
	;;
start)
	start
	;;
boot)
	boot
	;;
*)
	echo "Usage: $0 (start|stop|restart)"
    ;;
esac
