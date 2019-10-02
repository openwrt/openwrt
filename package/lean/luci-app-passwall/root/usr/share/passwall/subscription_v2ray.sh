#!/bin/sh

. /usr/share/libubox/jshn.sh

CONFIG=passwall
LOCK_FILE=/var/lock/${CONFIG}_subscription_v2ray.lock
Date=$(date "+%Y-%m-%d %H:%M:%S")
LOG_FILE=/var/log/$CONFIG.log

config_t_get() {
	local index=0
	[ -n "$3" ] && index=$3
	local ret=$(uci get $CONFIG.@$1[$index].$2 2>/dev/null)
	#echo ${ret:=$3}
	echo $ret
}

config_t_set() {
	index=$3
	uci set $CONFIG.@$1[$index].$2=$4
}

decode_url_link(){
	link=$1
	num=$2
	len=$((${#link}-$num))
	mod4=$(($len%4))
	if [ "$mod4" -gt 0 ]; then
		var="===="
		newlink=${link}${var:$mod4}
		echo -n "$newlink" | sed 's/-/+/g; s/_/\//g' | /usr/bin/base64 -d -i 2> /dev/null
	else
		echo -n "$link" | sed 's/-/+/g; s/_/\//g' | /usr/bin/base64 -d -i 2> /dev/null
	fi
}

get_remote_config(){
	json_load "$1"
	json_get_var v v
	json_get_var ps ps
	json_get_var server add
	json_get_var server_port port
	json_get_var id id
	json_get_var aid aid
	json_get_var net net
	json_get_var type type
	json_get_var transport net
	json_get_var tls tls
	json_get_var ws_host host
	json_get_var ws_path path
	
	if [ "$tls" == "1" ]; then
		tls="tls"
	else
		tls="none"
	fi
	
	remarks="订阅_$ps"
	group='AutoSuB_V2ray'
	
	##把全部服务器节点写入文件 /usr/share/$CONFIG/serverconfig_v2ray/all_onlineservers
	if [ ! -f "/usr/share/$CONFIG/serverconfig_v2ray/all_onlineservers" ]; then
		echo $server > /usr/share/$CONFIG/serverconfig_v2ray/all_onlineservers
	else
		echo $server >> /usr/share/$CONFIG/serverconfig_v2ray/all_onlineservers
	fi
	
}

get_server_index(){
	[ -f "/etc/config/$CONFIG" ] && {
        v2ray_index=$(uci show $CONFIG | grep -c "=servers")
	}	
}

get_local_servers(){
	[ -f "/etc/config/$CONFIG" ] && [ "`uci show $CONFIG | grep -c 'AutoSuB_V2ray'`" -gt 0 ] && {
		get_server_index
		for i in `seq $v2ray_index -1 1`
		do
			[ "$(uci show $CONFIG.@servers[$(($i-1))]|grep -c "AutoSuB_V2ray")" -eq 1 ] && {
				if [ ! -f "/usr/share/$CONFIG/serverconfig_v2ray/all_localservers" ]; then
					echo $(config_t_get servers server $(($i-1))) > /usr/share/$CONFIG/serverconfig_v2ray/all_localservers
				else
					echo $(config_t_get servers server $(($i-1))) >> /usr/share/$CONFIG/serverconfig_v2ray/all_localservers
				fi
			}
		done
	}
}

add_servers(){
	get_server_index
	uci add $CONFIG servers >/dev/null
	if [ -z "$1" ];then
		config_t_set servers remarks $v2ray_index $remarks
		config_t_set servers group $v2ray_index $group
	else
		config_t_set servers remarks $v2ray_index $remarks
	fi
	config_t_set servers server_type $v2ray_index 'V2ray'
	config_t_set servers v2ray_protocol $v2ray_index 'vmess'
	config_t_set servers server $v2ray_index $server
	config_t_set servers use_ipv6 $v2ray_index 0
	config_t_set servers server_port $v2ray_index $server_port
	config_t_set servers v2ray_security $v2ray_index 'auto'
	config_t_set servers v2ray_VMess_id $v2ray_index $id
	config_t_set servers v2ray_VMess_alterId $v2ray_index $aid
	config_t_set servers v2ray_VMess_level $v2ray_index $v
	config_t_set servers v2ray_transport $v2ray_index $net
	config_t_set servers v2ray_stream_security $v2ray_index $tls
	config_t_set servers v2ray_tcp_guise $v2ray_index $type
	config_t_set servers v2ray_ws_host $v2ray_index $ws_host
	config_t_set servers v2ray_ws_path $v2ray_index $ws_path
	uci commit $CONFIG
}

update_config(){
	isadded_server=$(uci show $CONFIG | grep -c "server='$server'")
	if [ "$isadded_server" -eq 0 ]; then
		add_servers
		let addnum+=1
	else
		index=$(uci show $CONFIG | grep -w "server='$server'" | cut -d '[' -f2|cut -d ']' -f1)
		local_server_port=$(config_t_get servers server_port $index)
		local_vmess_id=$(config_t_get servers v2ray_VMess_id $index)
	fi

}

del_config(){
	# 删除订阅服务器已经不存在的节点
	for localserver in $(cat /usr/share/$CONFIG/serverconfig_v2ray/all_localservers)
	do
		[ "`cat /usr/share/$CONFIG/serverconfig_v2ray/all_onlineservers |grep -c "$localserver"`" -eq 0 ] && {
			for localindex in $(uci show $CONFIG|grep -w "$localserver" |grep -w "server=" |cut -d '[' -f2|cut -d ']' -f1)
			do
				uci delete $CONFIG.@servers[$localindex]
				uci commit $CONFIG
				let delnum+=1 #删除该节点
			done
		}
	done
}

del_all_config(){
	get_server_index
	[ "`uci show $CONFIG | grep -c 'AutoSuB_V2ray'`" -eq 0 ] && exit 0
	current_tcp_redir_server=$(config_t_get global tcp_redir_server)
	is_sub_server=`uci -q get $CONFIG.$current_tcp_redir_server.group`
	for i in `seq $v2ray_index -1 1`
	do
		[ "$(uci show $CONFIG.@servers[$(($i-1))] | grep -c 'AutoSuB_V2ray')" -eq 1 ] && uci delete $CONFIG.@servers[$(($i-1))] && uci commit $CONFIG
	done
	[ -n "$is_sub_server" ] && {
		config_t_set global tcp_redir_server 0 'nil'
		uci commit $CONFIG && /etc/init.d/$CONFIG stop
	}
}

add() {
	V2RAY_LINKS=$(cat /tmp/v2ray_links.conf 2>/dev/null)
	[ -n "$V2RAY_LINKS" ] && {
		[ -f "$LOCK_FILE" ] && return 3
		touch "$LOCK_FILE"
		mkdir -p /usr/share/$CONFIG/serverconfig_v2ray
		rm -f /usr/share/$CONFIG/serverconfig_v2ray/*
		for v2ray_link in $V2RAY_LINKS
		do
			if [ -n "`echo -n "$v2ray_link" | grep 'vmess://'`" ]; then
				new_v2ray_link=`echo -n "$v2ray_link" | sed 's/vmess:\/\///g'`
				decode_v2ray_link=$(decode_url_link $new_v2ray_link 1)
				get_remote_config "$decode_v2ray_link"
				is_added=$(uci show $CONFIG | grep -v "AutoSuB_V2ray" | grep -c "server='$server'")
				[ "$is_added" -gt 0 ] && continue
				add_servers 1
			fi
		done
		[ -f "/usr/share/$CONFIG/serverconfig_v2ray/all_onlineservers" ] && rm -f /usr/share/$CONFIG/serverconfig_v2ray/all_onlineservers
	}
	rm -f /tmp/v2ray_links.conf
	rm -f "$LOCK_FILE"
	exit 0
}

start() {
	#防止并发开启服务
	[ -f "$LOCK_FILE" ] && return 3
	touch "$LOCK_FILE"
	addnum=0
	updatenum=0
	delnum=0
	baseurl_v2ray=$(uci get $CONFIG.@global_subscribe[0].baseurl_v2ray)  ##V2ray订阅地址
	[ -z "$baseurl_v2ray" ] && echo "$Date: V2ray订阅地址为空，跳过！" >> $LOG_FILE && rm -f "$LOCK_FILE" && exit 0
	
	echo "$Date: 开始订阅V2ray..." >> $LOG_FILE
	[ ! -d "/usr/share/$CONFIG/onlineurl_v2ray" ] && mkdir -p /usr/share/$CONFIG/onlineurl_v2ray
	[ ! -d "/usr/share/$CONFIG/serverconfig_v2ray" ] && mkdir -p /usr/share/$CONFIG/serverconfig_v2ray
	rm -f /usr/share/$CONFIG/onlineurl_v2ray/*
	
	/usr/bin/wget --no-check-certificate --timeout=8 -t 2 $baseurl_v2ray -P /usr/share/$CONFIG/onlineurl_v2ray
	[ ! -d "/usr/share/$CONFIG/onlineurl_v2ray" ] || [ "$(ls /usr/share/$CONFIG/onlineurl_v2ray |wc -l)" -eq 0 ] && echo "$Date: 订阅链接下载失败，请重试！" >> $LOG_FILE && rm -f "$LOCK_FILE" && exit 0
	rm -f /usr/share/$CONFIG/serverconfig_v2ray/*
	get_local_servers
	for file in /usr/share/$CONFIG/onlineurl_v2ray/*
	do
		[ -z "$(du -sh $file 2> /dev/null)" ] && echo "$Date: 订阅链接下载 $file 失败，请重试！" >> $LOG_FILE && continue
		maxnum=$(cat "$file" | /usr/bin/base64 -d 2> /dev/null| grep "MAX=" |awk -F"=" '{print $2}')
		if [ -n "$maxnum" ]; then
			urllinks=$(cat "$file" | /usr/bin/base64 -d 2> /dev/null| sed '/MAX=/d' | shuf -n${maxnum} | sed 's/vmess:\/\///g')
		else
			urllinks=$(cat "$file" | /usr/bin/base64 -d 2> /dev/null| sed 's/vmess:\/\///g')
		fi
		[ -z "$urllinks" ] && continue
		for link in $urllinks
		do
			decode_link=$(decode_url_link $link 1)
			get_remote_config "$decode_link"
			update_config
		done
	done
	[ -f "/usr/share/$CONFIG/serverconfig_v2ray/all_localservers" ] && del_config
	echo "$Date: 本次更新，V2ray新增服务器节点 $addnum 个，修改 $updatenum 个，删除 $delnum 个。" >> $LOG_FILE
	rm -f "$LOCK_FILE"
	exit 0
}

stop() {
	[ "`uci show $CONFIG | grep -c 'AutoSuB_V2ray'`" -gt 0 ] && {
		echo "$Date: 在线订阅V2ray节点已全部删除" >> $LOG_FILE
		del_all_config
	}
	rm -rf /usr/share/$CONFIG/onlineurl_v2ray
	rm -rf /usr/share/$CONFIG/serverconfig_v2ray
	rm -f "$LOCK_FILE"
	exit 0
}

case $1 in
stop)
	stop
	;;
add)
	add
	;;
*)
	start
	;;
esac
