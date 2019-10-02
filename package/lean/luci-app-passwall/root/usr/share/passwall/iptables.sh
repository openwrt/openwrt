#!/bin/sh

IPSET_LANIPLIST="laniplist"
IPSET_VPSIPLIST="vpsiplist"
IPSET_ROUTER="router"	
IPSET_GFW="gfwlist"
IPSET_CHN="chnroute"
IPSET_BLACKLIST="blacklist"
IPSET_WHITELIST="whitelist"

iptables_nat="iptables -t nat"
iptables_mangle="iptables -t mangle"
ip6tables_nat="ip6tables -t nat"

factor(){
	if [ -z "$1" ] || [ -z "$2" ]; then
		echo ""
	else
		echo "$2 $1"
	fi
}

get_jump_mode(){
	case "$1" in
		disable)
			echo "j"
		;;
		*)
			echo "g"
		;;
	esac
}

get_ip_mark(){
	if [ -z "$1" ]; then
		echo ""
	else
		echo $1 | awk -F "." '{printf ("0x%02X", $1)} {printf ("%02X", $2)} {printf ("%02X", $3)} {printf ("%02X", $4)}'
	fi
}

get_action_chain() {
	case "$1" in
		disable)
			echo "RETURN"
		;;
		global)
			echo "SS_GLO"
		;;
		gfwlist)
			echo "SS_GFW"
		;;
		chnroute)
			echo "SS_CHN"
		;;
		gamemode)
			echo "SS_GAME"
		;;
		returnhome)
			echo "SS_HOME"
		;;
	esac
}

get_action_chain_name() {
	case "$1" in
		disable)
			echo "不代理"
		;;
		global)
			echo "全局"
		;;
		gfwlist)
			echo "GFW"
		;;
		chnroute)
			echo "大陆白名单"
		;;
		gamemode)
			echo "游戏"
		;;
		returnhome)
			echo "回国"
		;;
	esac
}

gen_laniplist() {
	cat <<-EOF
		0.0.0.0/8
		10.0.0.0/8
		100.64.0.0/10
		127.0.0.0/8
		169.254.0.0/16
		172.16.0.0/12
		192.168.0.0/16
		224.0.0.0/4
		240.0.0.0/4
EOF
}

load_acl(){
	local enabled
	local aclremarks
	local ipaddr
	local macaddr
	local proxy_mode
	local tcp_redir_server
	local udp_redir_server
	local tcp_redir_ports
	local udp_redir_ports
	config_get enabled $1 enabled
	config_get aclremarks $1 aclremarks
	config_get ipaddr $1 ipaddr
	config_get macaddr $1 macaddr
	config_get proxy_mode $1 proxy_mode
	config_get tcp_redir_server $1 tcp_redir_server
	config_get udp_redir_server $1 udp_redir_server
	config_get tcp_redir_ports $1 tcp_redir_ports
	config_get udp_redir_ports $1 udp_redir_ports
	[ -z "$proxy_mode" -o "$proxy_mode" = "default" ] && proxy_mode=$PROXY_MODE
	[ -z "$tcp_redir_ports" -o "$tcp_redir_ports" = "default" ] && tcp_redir_ports=$TCP_REDIR_PORTS
	[ -z "$udp_redir_ports" -o "$udp_redir_ports" = "default" ] && udp_redir_ports=$UDP_REDIR_PORTS
	local ip_mark=`get_ip_mark $ipaddr`								 
	[ "$enabled" == "1" -a -n "$proxy_mode" ] && {
		if [ -n "$ipaddr" ] || [ -n "$macaddr" ]; then
			if [ -n "$ipaddr" -a -n "$macaddr" ]; then
				echolog "访问控制：IP：$ipaddr，MAC：$macaddr，代理模式：$(get_action_chain_name $proxy_mode)" 
			else
				[ -n "$ipaddr" ] && echolog "访问控制：IP：$ipaddr，代理模式：$(get_action_chain_name $proxy_mode)" 
				[ -n "$macaddr" ] && echolog "访问控制：MAC：$macaddr，代理模式：$(get_action_chain_name $proxy_mode)" 
			fi
			[ "$tcp_redir_server" != "nil" ] && {
				$iptables_mangle -A SS_ACL $(factor $ipaddr "-s") -p tcp -m set --match-set $IPSET_BLACKLIST dst -m comment --comment "$aclremarks"  -j TTL --ttl-set 14$tcp_redir_server
				$iptables_mangle -A SS_ACL $(factor $ipaddr "-s") -p tcp $(factor $macaddr "-m mac --mac-source") $(factor $tcp_redir_ports "-m multiport --dport") -m comment --comment "$aclremarks" -$(get_jump_mode $proxy_mode) $(get_action_chain $proxy_mode)$tcp_redir_server
			}
			[ "$udp_redir_server" != "nil" ] && {
				$iptables_mangle -A SS_ACL $(factor $ipaddr "-s") -p udp -m set --match-set $IPSET_BLACKLIST dst -m comment --comment "$aclremarks"  -j TPROXY --on-port 104$udp_redir_server --tproxy-mark 0x1/0x1
				$iptables_mangle -A SS_ACL $(factor $ipaddr "-s") -p udp $(factor $macaddr "-m mac --mac-source") $(factor $udp_redir_ports "-m multiport --dport") -m comment --comment "$aclremarks" -$(get_jump_mode $proxy_mode) $(get_action_chain $proxy_mode)$udp_redir_server
			}
			[ -z "$ipaddr" ] && {
				lower_macaddr=`echo $macaddr | tr '[A-Z]' '[a-z]'`
				ipaddr=`ip neigh show | grep -E "([0-9]{1,3}[\.]){3}[0-9]{1,3}" | grep $lower_macaddr | awk '{print $1}'`
				[ -z "$ipaddr" ] && {
					dhcp_index=`uci show dhcp | grep $lower_macaddr |awk -F'.' '{print $2}'`
					ipaddr=`uci -q get dhcp.$dhcp_index.ip`
				}
				[ -z "$ipaddr" ] && ipaddr=`cat /tmp/dhcp.leases | grep -E "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" |grep $lower_macaddr |awk '{print $3}'`
			}
		fi
	}
}

filter_vpsip(){
	local server_host server_ip use_ipv6 network_type
	server_host=$(config_get $1 server)
	use_ipv6=$(config_get $1 use_ipv6)
	network_type="ipv4"
	[ "$use_ipv6" == "1" ] && network_type="ipv6"
	server_ip=$(get_host_ip $network_type $server_host)
	
	[ -n "$server_ip" -a "$server_ip" != "$TCP_REDIR_SERVER_IP" ] && {
		[ "$network_type" == "ipv4" ] && ipset add $IPSET_VPSIPLIST $server_ip >/dev/null 2>&1 &
	}
}

dns_hijack(){
	dnshijack=$(config_t_get global_dns dns_53)
	if [ "$dnshijack" = "1" -o "$1" = "force" ];then
		chromecast_nu=`$iptables_nat -L SS -v -n --line-numbers|grep "dpt:53"|awk '{print $1}'`
		is_right_lanip=`$iptables_nat -L SS -v -n --line-numbers|grep "dpt:53" |grep "$lanip"`
		if [ -z "$chromecast_nu" ]; then
			echolog "添加接管局域网DNS解析规则..." 
			$iptables_nat -I SS -i br-lan -p udp --dport 53 -j DNAT --to $lanip 2>/dev/null
		else
			if [ -z "$is_right_lanip" ]; then
				echolog "添加接管局域网DNS解析规则..." 
				$iptables_nat -D SS $chromecast_nu >/dev/null 2>&1 &
				$iptables_nat -I SS -i br-lan -p udp --dport 53 -j DNAT --to $lanip 2>/dev/null
			else
				echolog " DNS劫持规则已经添加，跳过~" >>$LOG_FILE
			fi
		fi
	fi
}

add_firewall_rule() {
	echolog "开始加载防火墙规则..." 
	echolog "默认代理模式：$(get_action_chain_name $PROXY_MODE)" 
	ipset -! create $IPSET_LANIPLIST nethash && ipset flush $IPSET_LANIPLIST
	ipset -! create $IPSET_VPSIPLIST nethash && ipset flush $IPSET_VPSIPLIST
	ipset -! create $IPSET_ROUTER nethash && ipset flush $IPSET_ROUTER
	ipset -! create $IPSET_GFW nethash && ipset flush $IPSET_GFW
	ipset -! create $IPSET_CHN nethash && ipset flush $IPSET_CHN
	ipset -! create $IPSET_BLACKLIST nethash && ipset flush $IPSET_BLACKLIST
	ipset -! create $IPSET_WHITELIST nethash && ipset flush $IPSET_WHITELIST
	
	sed -e "s/^/add $IPSET_CHN &/g" $APP_PATH_RULE/chnroute | awk '{print $0} END{print "COMMIT"}' | ipset -R
	sed -e "s/^/add $IPSET_BLACKLIST &/g" $APP_PATH_RULE/blacklist_ip | awk '{print $0} END{print "COMMIT"}' | ipset -R
	sed -e "s/^/add $IPSET_WHITELIST &/g" $APP_PATH_RULE/whitelist_ip | awk '{print $0} END{print "COMMIT"}' | ipset -R
	
	ipset -! -R <<-EOF || return 1
			$(gen_laniplist | sed -e "s/^/add $IPSET_LANIPLIST /")
EOF
	
	ISP_DNS=`cat /tmp/resolv.conf.auto 2>/dev/null | grep -E -o "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" | sort -u |grep -v 0.0.0.0 |grep -v 127.0.0.1`
	[ -n "$ISP_DNS" ] && {
		for ispip in $ISP_DNS
		do
			ipset -! add $IPSET_WHITELIST $ispip >/dev/null 2>&1 &
		done
	}
		
	# 忽略特殊IP段
	lan_ip=`ifconfig br-lan | grep "inet addr" | awk '{print $2}' | awk -F : '{print $2}'` #路由器lan IP
	lan_ipv4=`ip address show br-lan | grep -w "inet" |awk '{print $2}'`  #当前LAN IPv4段
	[ -n "$lan_ipv4" ] && ipset add $IPSET_LANIPLIST $lan_ipv4 >/dev/null 2>&1 &
	
	#  过滤所有节点IP
	config_foreach filter_vpsip "servers"
	
	$iptables_mangle -N SS
	$iptables_mangle -A SS -m set --match-set $IPSET_LANIPLIST dst -j RETURN
	$iptables_mangle -A SS -m set --match-set $IPSET_VPSIPLIST dst -j RETURN
	$iptables_mangle -A SS -m set --match-set $IPSET_WHITELIST dst -j RETURN
	$iptables_mangle -N SS_ACL
	
	if [[ "$TCP_REDIR_SERVER_NUM" -ge 1 ]] || [[ "$UDP_REDIR_SERVER_NUM" -ge 1 ]] ;then
		local max_num=1
		[ "$TCP_REDIR_SERVER_NUM" -ge "$UDP_REDIR_SERVER_NUM" ] && max_num=$TCP_REDIR_SERVER_NUM
		if [ "$max_num" -ge 1 ] ;then
			for i in $(seq 1 $max_num)
			do
				$iptables_mangle -N SS_GLO$i
				$iptables_mangle -N SS_GFW$i
				$iptables_mangle -N SS_CHN$i
				$iptables_mangle -N SS_HOME$i
				$iptables_mangle -N SS_GAME$i
				
				ip rule add fwmark 1 lookup 100
				ip route add local 0.0.0.0/0 dev lo table 100
			done
		fi
	fi
	
	if [ "$TCP_REDIR_SERVER_NUM" -ge 1 ] ;then
		for i in $(seq 1 $TCP_REDIR_SERVER_NUM)
		do
			local k=$i
			local local_port=104$k
			local ttl=14$k
			eval temp_server=\$TCP_REDIR_SERVER$k
			eval local_port=\$TCP_REDIR_PORT$k
			[ "$k" == 1 ] && {
				local_port=$TCP_REDIR_PORT
				temp_server=$TCP_REDIR_SERVER
			}
			# 生成TCP转发规则
			if [ "$temp_server" != "nil" ];then
				local server_host=$(config_get $temp_server server)
				local TCP_REDIR_SERVER_PORT=$(config_get $temp_server server_port)
				local TCP_REDIR_SERVER_IP=$(get_host_ip "ipv4" $server_host)
				[ -n "$SOCKS5_PROXY_SERVER_IP" -a -n "$SOCKS5_PROXY_SERVER_PORT" ] && $iptables_mangle -A SS -p tcp -d $SOCKS5_PROXY_SERVER_IP -m multiport --dports $SOCKS5_PROXY_SERVER_PORT -j RETURN
				[ -n "$TCP_REDIR_SERVER_IP" -a -n "$TCP_REDIR_SERVER_PORT" ] && $iptables_mangle -A SS -p tcp -d $TCP_REDIR_SERVER_IP -m multiport --dports $TCP_REDIR_SERVER_PORT -j RETURN
				if [ "$TCP_REDIR_SERVER_TYPE" == "brook" ]; then
					$iptables_mangle -A PREROUTING -p tcp -m socket -j MARK --set-mark 1
					$iptables_mangle -A PREROUTING -p tcp -j SS$k
					
					$iptables_mangle -A SS$k -p tcp -m set --match-set $IPSET_BLACKLIST dst -j TPROXY --on-port $local_port --tproxy-mark 0x1/0x1
					# 全局模式
					$iptables_mangle -A SS_GLO$k -p tcp -j TPROXY --tproxy-mark 0x1/0x1 --on-port $local_port
					
					# GFWLIST模式
					$iptables_mangle -A SS_GFW$k -p tcp -m set --match-set $IPSET_GFW dst -j TPROXY --on-port $local_port --tproxy-mark 0x1/0x1
					$iptables_mangle -A SS_GFW$k -p tcp -m set --match-set $IPSET_ROUTER dst -j TPROXY --on-port $local_port --tproxy-mark 0x1/0x1
					
					# 大陆白名单模式
					$iptables_mangle -A SS_CHN$k -p tcp -m set --match-set $IPSET_CHN dst -j RETURN
					$iptables_mangle -A SS_CHN$k -p tcp -j TPROXY --on-port $local_port --tproxy-mark 0x1/0x1
					
					# 回国模式
					$iptables_mangle -A SS_HOME$k -p tcp -m set --match-set $IPSET_CHN dst -j TPROXY --on-port $local_port --tproxy-mark 0x1/0x1
					
					# 游戏模式
					$iptables_mangle -A SS_GAME$k -p tcp -m set --match-set $IPSET_CHN dst -j RETURN
					
					# 用于本机流量转发，默认只走router
					$iptables_mangle -A SS$k -s $lan_ip -p tcp -m set --match-set $IPSET_ROUTER dst -j TPROXY --on-port $local_port --tproxy-mark 0x1/0x1
					$iptables_mangle -A OUTPUT -p tcp -m multiport --dport $TCP_REDIR_PORTS -m set --match-set $IPSET_ROUTER dst -j MARK --set-mark 1
				else
					#$iptables_mangle -I SS_ACL 1 -p tcp -m set --match-set $IPSET_BLACKLIST dst -j TTL --ttl-set $ttl
					# 全局模式
					$iptables_mangle -A SS_GLO$k -p tcp -j TTL --ttl-set $ttl
					
					# GFWLIST模式
					$iptables_mangle -A SS_GFW$k -p tcp -m set --match-set $IPSET_GFW dst -j TTL --ttl-set $ttl
					$iptables_mangle -A SS_GFW$k -p tcp -m set --match-set $IPSET_ROUTER dst -j TTL --ttl-set $ttl
					
					# 大陆白名单模式
					$iptables_mangle -A SS_CHN$k -p tcp -m set --match-set $IPSET_CHN dst -j RETURN
					#$iptables_mangle -A SS_CHN$k -p tcp -m geoip ! --destination-country CN -j TTL --ttl-set $ttl
					$iptables_mangle -A SS_CHN$k -p tcp -j TTL --ttl-set $ttl
					
					# 回国模式
					#$iptables_mangle -A SS_HOME$k -p tcp -m geoip --destination-country CN -j TTL --ttl-set $ttl
					$iptables_mangle -A SS_HOME$k -p tcp -m set --match-set $IPSET_CHN dst -j TTL --ttl-set $ttl
					
					# 游戏模式
					$iptables_mangle -A SS_GAME$k -p tcp -m set --match-set $IPSET_CHN dst -j RETURN
					
					[ "$k" == 1 ] && {
						$iptables_nat -N SS
						
						is_add_prerouting=0
						
						KP_INDEX=`$iptables_nat -L PREROUTING|tail -n +3|sed -n -e '/^KOOLPROXY/='`
						if [ -n "$KP_INDEX" ]; then
							let KP_INDEX+=1
							#确保添加到KOOLPROXY规则之后
							$iptables_nat -I PREROUTING $KP_INDEX -j SS
							is_add_prerouting=1
						fi
						
						ADBYBY_INDEX=`$iptables_nat -L PREROUTING|tail -n +3|sed -n -e '/^ADBYBY/='`
						if [ -n "$ADBYBY_INDEX" ]; then
							let ADBYBY_INDEX+=1
							#确保添加到ADBYBY规则之后
							$iptables_nat -I PREROUTING $ADBYBY_INDEX -j SS
							is_add_prerouting=1
						fi
						
						if [ "$is_add_prerouting" == 0 ]; then
							#如果去广告没有运行，确保添加到prerouting_rule规则之后
							PR_INDEX=`$iptables_nat -L PREROUTING|tail -n +3|sed -n -e '/^prerouting_rule/='`
							if [ -z "$PR_INDEX" ]; then
								PR_INDEX=1
							else
								let PR_INDEX+=1
							fi
							$iptables_nat -I PREROUTING $PR_INDEX -j SS
						fi
						# 用于本机流量转发，默认只走router
						#$iptables_nat -I OUTPUT -j SS
						$iptables_nat -A OUTPUT -m set --match-set $IPSET_LANIPLIST dst -j RETURN
						$iptables_nat -A OUTPUT -m set --match-set $IPSET_VPSIPLIST dst -j RETURN
						$iptables_nat -A OUTPUT -m set --match-set $IPSET_WHITELIST dst -j RETURN
						$iptables_nat -A OUTPUT -p tcp -m multiport --dport $TCP_REDIR_PORTS -m set --match-set $IPSET_ROUTER dst -j REDIRECT --to-ports $TCP_REDIR_PORT
						$iptables_nat -A OUTPUT -p tcp -m multiport --dport $TCP_REDIR_PORTS -m set --match-set $IPSET_BLACKLIST dst -j REDIRECT --to-ports $TCP_REDIR_PORT
						
						[ "$LOCALHOST_PROXY_MODE" == "global" ] && $iptables_nat -A OUTPUT -p tcp -m multiport --dport $TCP_REDIR_PORTS -j REDIRECT --to-ports $TCP_REDIR_PORT
						[ "$LOCALHOST_PROXY_MODE" == "gfwlist" ] && $iptables_nat -A OUTPUT -p tcp -m multiport --dport $TCP_REDIR_PORTS -m set --match-set $IPSET_GFW dst -j REDIRECT --to-ports $TCP_REDIR_PORT
						[ "$LOCALHOST_PROXY_MODE" == "chnroute" ] && {
							$iptables_nat -A OUTPUT -p tcp -m multiport --dport $TCP_REDIR_PORTS -m set --match-set $IPSET_CHN dst -j RETURN
							$iptables_nat -A OUTPUT -p tcp -m multiport --dport $TCP_REDIR_PORTS -j REDIRECT --to-ports $TCP_REDIR_PORT
						}
					}
					# 重定所有流量到透明代理端口
					$iptables_nat -A SS -p tcp -m ttl --ttl-eq $ttl -j REDIRECT --to $local_port
					echolog "IPv4 防火墙TCP转发规则加载完成！" 
				fi
			fi
		done
	else
		echolog "主服务器未选择，无法转发TCP！" 
	fi
		
	if [ "$UDP_REDIR_SERVER_NUM" -ge 1 ] ;then
		for i in $(seq 1 $UDP_REDIR_SERVER_NUM)
		do
			local k=$i
			local local_port=104$k
			local ttl=14$k
			eval temp_server=\$UDP_REDIR_SERVER$k
			eval local_port=\$UDP_REDIR_PORT$k
			[ "$k" == 1 ] && {
				local_port=$UDP_REDIR_PORT
				temp_server=$UDP_REDIR_SERVER
			}
			#  生成UDP转发规则
			if [ "$temp_server" != "nil" ];then
				local server_host=$(config_get $temp_server server)
				local UDP_REDIR_SERVER_PORT=$(config_get $temp_server server_port)
				local UDP_REDIR_SERVER_IP=$(get_host_ip "ipv4" $server_host)
				[ -n "$UDP_REDIR_SERVER_IP" -a -n "$UDP_REDIR_SERVER_PORT" ] && $iptables_mangle -A SS -p udp -d $UDP_REDIR_SERVER_IP -m multiport --dports $UDP_REDIR_SERVER_PORT -j RETURN
				if [ "$UDP_REDIR_SERVER_TYPE" == "brook" ]; then
					$iptables_mangle -A PREROUTING -p udp -m socket -j MARK --set-mark 1
					$iptables_mangle -A PREROUTING -p udp -j SS
				fi
				#$iptables_mangle -I SS_ACL$k 1 -p udp -m set --match-set $IPSET_BLACKLIST dst -j TPROXY --on-port $local_port --tproxy-mark 0x1/0x1
				#  全局模式
				$iptables_mangle -A SS_GLO$k -p udp -j TPROXY --on-port $local_port --tproxy-mark 0x1/0x1
				
				#  GFWLIST模式
				$iptables_mangle -A SS_GFW$k -p udp -m set --match-set $IPSET_GFW dst -j TPROXY --on-port $local_port --tproxy-mark 0x1/0x1
				$iptables_mangle -A SS_GFW$k -p udp -m set --match-set $IPSET_ROUTER dst -j TPROXY --on-port $local_port --tproxy-mark 0x1/0x1
				
				#  大陆白名单模式
				$iptables_mangle -A SS_CHN$k -p udp -m set --match-set $IPSET_CHN dst -j RETURN
				$iptables_mangle -A SS_CHN$k -p udp -j TPROXY --on-port $local_port --tproxy-mark 0x1/0x1
				
				#  回国模式
				$iptables_mangle -A SS_HOME$k -p udp -m set --match-set $IPSET_CHN dst -j TPROXY --on-port $local_port --tproxy-mark 0x1/0x1
				
				#  游戏模式
				$iptables_mangle -A SS_GAME$k -p udp -m set --match-set $IPSET_CHN dst -j RETURN
				$iptables_mangle -A SS_GAME$k -p udp -j TPROXY --on-port $local_port --tproxy-mark 0x1/0x1
				
				echolog "IPv4 防火墙UDP转发规则加载完成！"
			fi
		done
	else
		echolog "UDP服务器未选择，无法转发UDP！" 
	fi
	
	$iptables_mangle -A PREROUTING -j SS
	$iptables_mangle -A SS -j SS_ACL
		
	local max_num=1
	[ "$TCP_REDIR_SERVER_NUM" -ge "$UDP_REDIR_SERVER_NUM" ] && max_num=$TCP_REDIR_SERVER_NUM
	if [ "$max_num" -ge 1 ] ;then
		for i in $(seq 1 $max_num)
		do
			local k=$i
			#  加载ACLS
			[ "$k" == 1 ] && config_foreach load_acl "acl_rule"
				
			#  加载默认代理模式
			if [ "$PROXY_MODE" == "disable" ];then
				[ "$TCP_REDIR_SERVER" != "nil" ] && $iptables_mangle -A SS_ACL -p tcp -m comment --comment "Default" -j $(get_action_chain $PROXY_MODE)
				[ "$UDP_REDIR_SERVER" != "nil" ] && $iptables_mangle -A SS_ACL -p udp -m comment --comment "Default" -j $(get_action_chain $PROXY_MODE)
			else
				[ "$PROXY_MODE" == "gfwlist" ] && dns_hijack "force"
				[ "$TCP_REDIR_SERVER" != "nil" ] && $iptables_mangle -A SS_ACL -p tcp -m multiport --dport $TCP_REDIR_PORTS -m comment --comment "Default" -j $(get_action_chain $PROXY_MODE)$k
				[ "$UDP_REDIR_SERVER" != "nil" ] && $iptables_mangle -A SS_ACL -p udp -m multiport --dport $UDP_REDIR_PORTS -m comment --comment "Default" -j $(get_action_chain $PROXY_MODE)$k
			fi
			
			if [ "$PROXY_IPV6" == "1" ];then
				lan_ipv6=`ip address show br-lan | grep -w "inet6" |awk '{print $2}'`  #当前LAN IPv6段
				$ip6tables_nat -N SS
				$ip6tables_nat -N SS_ACL
				$ip6tables_nat -A PREROUTING -j SS
				[ -n "$lan_ipv6" ] && {
					for ip in $lan_ipv6
					do
						$ip6tables_nat -A SS -d $ip -j RETURN
					done
				}
				[ "$use_ipv6" == "1" -a -n "$server_ip" ] && $ip6tables_nat -A SS -d $server_ip -j RETURN
				$ip6tables_nat -N SS_GLO$k
				$ip6tables_nat -N SS_GFW$k
				$ip6tables_nat -N SS_CHN$k
				$ip6tables_nat -N SS_HOME$k
				$ip6tables_nat -A SS_GLO$k -p tcp -j REDIRECT --to $TCP_REDIR_PORT
				$ip6tables_nat -A SS -j SS_GLO$k
				#$ip6tables_nat -I OUTPUT -p tcp -j SS
				echolog "IPv6防火墙规则加载完成！" 
			fi
		done
	fi
}

del_firewall_rule() {
	echolog "删除所有防火墙规则..."
	ipv4_output_exist=`$iptables_nat -L OUTPUT 2>/dev/null | grep -c -E "SS|$TCP_REDIR_PORTS|$IPSET_LANIPLIST|$IPSET_VPSIPLIST|$IPSET_WHITELIST|$IPSET_ROUTER|$IPSET_BLACKLIST|$IPSET_GFW|$IPSET_CHN"`
	[ -n "$ipv4_output_exist" ] && {
		until [ "$ipv4_output_exist" = 0 ]
		do
			rules=`$iptables_nat -L OUTPUT --line-numbers | grep -E "SS|$TCP_REDIR_PORTS|$IPSET_LANIPLIST|$IPSET_VPSIPLIST|$IPSET_WHITELIST|$IPSET_ROUTER|$IPSET_BLACKLIST|$IPSET_GFW|$IPSET_CHN" | awk '{print $1}'`
			for rule in $rules
			do
				$iptables_nat -D OUTPUT $rule 2> /dev/null
				break
			done
			ipv4_output_exist=`expr $ipv4_output_exist - 1`
		done
	}
	
	ipv6_output_ss_exist=`$ip6tables_nat -L OUTPUT 2>/dev/null | grep -c "SS"`
	[ -n "$ipv6_output_ss_exist" ] && {
		until [ "$ipv6_output_ss_exist" = 0 ]
		do
			rules=`$ip6tables_nat -L OUTPUT --line-numbers | grep "SS" | awk '{print $1}'`
			for rule in $rules
			do
				$ip6tables_nat -D OUTPUT $rule 2> /dev/null
				break
			done
			ipv6_output_ss_exist=`expr $ipv6_output_ss_exist - 1`
		done
	}
		
	$iptables_mangle -D PREROUTING -p tcp -m socket -j MARK --set-mark 1 2>/dev/null
	$iptables_mangle -D PREROUTING -p udp -m socket -j MARK --set-mark 1 2>/dev/null
	$iptables_mangle -D OUTPUT -p tcp -m multiport --dport $TCP_REDIR_PORTS -m set --match-set $IPSET_ROUTER dst -j MARK --set-mark 1 2>/dev/null
	$iptables_mangle -D OUTPUT -p tcp -m multiport --dport $TCP_REDIR_PORTS -m set --match-set $IPSET_GFW dst -j MARK --set-mark 1 2>/dev/null
	$iptables_mangle -D OUTPUT -p tcp -m multiport --dport $TCP_REDIR_PORTS -j MARK --set-mark 1 2>/dev/null
	
	$iptables_nat -D PREROUTING -j SS 2> /dev/null
	$iptables_nat -F SS 2>/dev/null && $iptables_nat -X SS 2>/dev/null
	$iptables_mangle -D PREROUTING -j SS$k 2>/dev/null
	$iptables_mangle -F SS 2>/dev/null && $iptables_mangle -X SS 2>/dev/null
	$iptables_mangle -F SS_ACL 2>/dev/null && $iptables_mangle -X SS_ACL 2>/dev/null
	
	$ip6tables_nat -D PREROUTING -j SS 2>/dev/null
	$ip6tables_nat -F SS 2>/dev/null && $ip6tables_nat -X SS 2>/dev/null
	$ip6tables_nat -F SS_ACL 2>/dev/null && $ip6tables_nat -X SS_ACL 2>/dev/null
		
	local max_num=1
	[ "$TCP_REDIR_SERVER_NUM" -ge "$UDP_REDIR_SERVER_NUM" ] && max_num=$TCP_REDIR_SERVER_NUM
	if [ "$max_num" -ge 1 ] ;then
		for i in $(seq 1 $max_num)
		do
			local k=$i
			$iptables_mangle -F SS_GLO$k 2>/dev/null && $iptables_mangle -X SS_GLO$k 2>/dev/null
			$iptables_mangle -F SS_GFW$k 2>/dev/null && $iptables_mangle -X SS_GFW$k 2>/dev/null
			$iptables_mangle -F SS_CHN$k 2>/dev/null && $iptables_mangle -X SS_CHN$k 2>/dev/null
			$iptables_mangle -F SS_GAME$k 2>/dev/null && $iptables_mangle -X SS_GAME$k 2>/dev/null
			$iptables_mangle -F SS_HOME$k 2>/dev/null && $iptables_mangle -X SS_HOME$k 2>/dev/null
			
			$ip6tables_nat -F SS_GLO$k 2>/dev/null && $ip6tables_nat -X SS_GLO$k 2>/dev/null
			$ip6tables_nat -F SS_GFW$k 2>/dev/null && $ip6tables_nat -X SS_GFW$k 2>/dev/null
			$ip6tables_nat -F SS_CHN$k 2>/dev/null && $ip6tables_nat -X SS_CHN$k 2>/dev/null
			$ip6tables_nat -F SS_HOME$k 2>/dev/null && $ip6tables_nat -X SS_HOME$k 2>/dev/null
			
			ip_rule_exist=`ip rule show | grep "from all fwmark 0x1 lookup 100" | grep -c 100`
			if [ ! -z "$ip_rule_exist" ];then
				until [ "$ip_rule_exist" = 0 ]
				do 
					ip rule del fwmark 1 lookup 100
					ip_rule_exist=`expr $ip_rule_exist - 1`
				done
			fi
			ip route del local 0.0.0.0/0 dev lo table 100 2>/dev/null
		done
	fi
	
	ipset -F $IPSET_ROUTER >/dev/null 2>&1 && ipset -X $IPSET_ROUTER >/dev/null 2>&1 &
	ipset -F $IPSET_GFW >/dev/null 2>&1 && ipset -X $IPSET_GFW >/dev/null 2>&1 &
	#ipset -F $IPSET_CHN >/dev/null 2>&1 && ipset -X $IPSET_CHN >/dev/null 2>&1 &
	ipset -F $IPSET_BLACKLIST >/dev/null 2>&1 && ipset -X $IPSET_BLACKLIST >/dev/null 2>&1 &
	ipset -F $IPSET_WHITELIST >/dev/null 2>&1 && ipset -X $IPSET_WHITELIST >/dev/null 2>&1 &
	ipset -F $IPSET_VPSIPLIST >/dev/null 2>&1 && ipset -X $IPSET_VPSIPLIST >/dev/null 2>&1 &
	ipset -F $IPSET_LANIPLIST >/dev/null 2>&1 && ipset -X $IPSET_LANIPLIST >/dev/null 2>&1 &
}

start(){
	add_firewall_rule
	dns_hijack
}

stop(){
	del_firewall_rule
}

case $1 in
stop)
	stop
	;;
start)
	start
	;;
*)
esac
