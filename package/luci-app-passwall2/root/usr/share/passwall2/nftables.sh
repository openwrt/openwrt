#!/bin/bash

DIR="$(cd "$(dirname "$0")" && pwd)"
MY_PATH=$DIR/nftables.sh
NFTSET_LANLIST="passwall2_lanlist"
NFTSET_VPSLIST="passwall2_vpslist"

NFTSET_LANLIST6="passwall2_lanlist6"
NFTSET_VPSLIST6="passwall2_vpslist6"

FORCE_INDEX=0

. /lib/functions/network.sh

FWI=$(uci -q get firewall.passwall2.path 2>/dev/null)
FAKE_IP="198.18.0.0/16"
FAKE_IP_6="fc00::/18"

factor() {
	if [ -z "$1" ] || [ -z "$2" ]; then
		echo ""
	elif [ "$1" == "1:65535" ]; then
		echo ""
	# acl mac address
	elif [ -n "$(echo $1 | grep -E '([A-Fa-f0-9]{2}:){5}[A-Fa-f0-9]{2}')" ]; then
		echo "$2 {$1}"
	else
		echo "$2 {$(echo $1 | sed 's/:/-/g')}"
	fi
}

insert_rule_before() {
	[ $# -ge 4 ] || {
		return 1
	}
	local table_name="${1}"; shift
	local chain_name="${1}"; shift
	local keyword="${1}"; shift
	local rule="${1}"; shift
	local default_index="${1}"; shift
	default_index=${default_index:-0}
	local _index=$(nft -a list chain $table_name $chain_name 2>/dev/null | grep "$keyword" | awk -F '# handle ' '{print$2}' | head -n 1 | awk '{print $1}')
	if [ -z "${_index}" ] && [ "${default_index}" = "0" ]; then
		nft "add rule $table_name $chain_name $rule"
	else
		if [ -z "${_index}" ]; then
			_index=${default_index}
		fi
		nft "insert rule $table_name $chain_name position $_index $rule"
	fi
}

insert_rule_after() {
	[ $# -ge 4 ] || {
		return 1
	}
	local table_name="${1}"; shift
	local chain_name="${1}"; shift
	local keyword="${1}"; shift
	local rule="${1}"; shift
	local default_index="${1}"; shift
	default_index=${default_index:-0}
	local _index=$(nft -a list chain $table_name $chain_name 2>/dev/null | grep "$keyword" | awk -F '# handle ' '{print$2}' | head -n 1 | awk '{print $1}')
	if [ -z "${_index}" ] && [ "${default_index}" = "0" ]; then
		nft "add rule $table_name $chain_name $rule"
	else
		if [ -n "${_index}" ]; then
			_index=$((_index + 1))
		else
			_index=${default_index}
		fi
		nft "insert rule $table_name $chain_name position $_index $rule"
	fi
}

RULE_LAST_INDEX() {
	[ $# -ge 3 ] || {
		echolog "索引列举方式不正确（nftables），终止执行！"
		return 1
	}
	local table_name="${1}"; shift
	local chain_name="${1}"; shift
	local keyword="${1}"; shift
	local default="${1:-0}"; shift
	local _index=$(nft -a list chain $table_name $chain_name 2>/dev/null | grep "$keyword" | awk -F '# handle ' '{print$2}' | head -n 1 | awk '{print $1}')
	echo "${_index:-${default}}"
}

REDIRECT() {
	local s="counter redirect"
	[ -n "$1" ] && {
		local s="$s to :$1"
		[ "$2" == "MARK" ] && s="counter meta mark set $1"
		[ "$2" == "TPROXY" ] && {
			s="counter meta mark 1 tproxy to :$1"
		}
		[ "$2" == "TPROXY4" ] && {
			s="counter meta mark 1 tproxy ip to :$1"
		}
		[ "$2" == "TPROXY6" ] && {
			s="counter meta mark 1 tproxy ip6 to :$1"
		}

	}
	echo $s
}

destroy_nftset() {
	for i in "$@"; do
		nft flush set inet fw4 $i 2>/dev/null
		nft delete set inet fw4 $i 2>/dev/null
	done
}

insert_nftset() {
	local nftset_name="${1}"; shift
	local timeout_argument="${1}"; shift
	local defalut_timeout_argument="3650d"
	local nftset_elements

	[ -n "${1}" ] && {
		if [ "$timeout_argument" == "0" ]; then
			nftset_elements=$(echo -e $@ | sed "s/\s/ timeout $defalut_timeout_argument, /g" | sed "s/$/ timeout $defalut_timeout_argument/")
		else
			nftset_elements=$(echo -e $@ | sed "s/\s/ timeout $timeout_argument, /g" | sed "s/$/ timeout $timeout_argument/")
		fi
		mkdir -p $TMP_PATH2/nftset
		cat > "$TMP_PATH2/nftset/$nftset_name" <<-EOF
			define $nftset_name = {$nftset_elements}	
			add element inet fw4 $nftset_name \$$nftset_name
		EOF
		nft -f "$TMP_PATH2/nftset/$nftset_name"
		rm -rf "$TMP_PATH2/nftset"
	}
}

gen_nftset() {
	local nftset_name="${1}"; shift
	local ip_type="${1}"; shift
	#  0 - don't set defalut timeout
	local timeout_argument_set="${1}"; shift
	#  0 - don't let element timeout(3650 days)
	local timeout_argument_element="${1}"; shift

	nft "list set inet fw4 $nftset_name" &>/dev/null
	if [ $? -ne 0 ]; then
		if [ "$timeout_argument_set" == "0" ]; then
			nft "add set inet fw4 $nftset_name { type $ip_type; flags interval, timeout; auto-merge; }"
		else
			nft "add set inet fw4 $nftset_name { type $ip_type; flags interval, timeout; timeout $timeout_argument_set; gc-interval $timeout_argument_set; auto-merge; }"
		fi
	fi
	[ -n "${1}" ] && insert_nftset $nftset_name $timeout_argument_element $@
}

get_action_chain_name() {
	echo "全局代理"
}

gen_lanlist() {
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

gen_lanlist_6() {
	cat <<-EOF
		::/128
		::1/128
		::ffff:0:0/96
		::ffff:0:0:0/96
		64:ff9b::/96
		100::/64
		2001::/32
		2001:20::/28
		2001:db8::/32
		2002::/16
		fc00::/7
		fe80::/10
		ff00::/8
	EOF
}

get_wan_ip() {
	local NET_IF
	local NET_ADDR
	
	network_flush_cache
	network_find_wan NET_IF
	network_get_ipaddr NET_ADDR "${NET_IF}"
	
	echo $NET_ADDR
}

get_wan6_ip() {
	local NET_IF
	local NET_ADDR
	
	network_flush_cache
	network_find_wan6 NET_IF
	network_get_ipaddr6 NET_ADDR "${NET_IF}"
	
	echo $NET_ADDR
}

load_acl() {
	[ "$ENABLED_ACLS" == 1 ] && {
		acl_app
		echolog "访问控制："
		for sid in $(ls -F ${TMP_ACL_PATH} | grep '/$' | awk -F '/' '{print $1}'); do
			eval $(uci -q show "${CONFIG}.${sid}" | cut -d'.' -sf 3-)

			tcp_no_redir_ports=${tcp_no_redir_ports:-default}
			udp_no_redir_ports=${udp_no_redir_ports:-default}
			tcp_proxy_mode="global"
			udp_proxy_mode="global"
			node=${node:-default}
			[ "$tcp_no_redir_ports" = "default" ] && tcp_no_redir_ports=$TCP_NO_REDIR_PORTS
			[ "$udp_no_redir_ports" = "default" ] && udp_no_redir_ports=$UDP_NO_REDIR_PORTS
			[ "$tcp_redir_ports" = "default" ] && tcp_redir_ports=$TCP_REDIR_PORTS
			[ "$udp_redir_ports" = "default" ] && udp_redir_ports=$UDP_REDIR_PORTS
			[ "$tcp_no_redir_ports" = "1:65535" ] && tcp_proxy_mode="disable"
			[ "$udp_no_redir_ports" = "1:65535" ] && udp_proxy_mode="disable"

			node_remark=$(config_n_get $NODE remarks)
			[ -s "${TMP_ACL_PATH}/${sid}/var_node" ] && node=$(cat ${TMP_ACL_PATH}/${sid}/var_node)
			[ -s "${TMP_ACL_PATH}/${sid}/var_port" ] && redir_port=$(cat ${TMP_ACL_PATH}/${sid}/var_port)
			[ -n "$node" ] && [ "$node" != "default" ] && node_remark=$(config_n_get $node remarks)
			
			for i in $(cat ${TMP_ACL_PATH}/${sid}/rule_list); do
				if [ -n "$(echo ${i} | grep '^iprange:')" ]; then
					_iprange=$(echo ${i} | sed 's#iprange:##g')
					_ipt_source=$(factor ${_iprange} "ip saddr")
					msg="备注【$remarks】，IP range【${_iprange}】，"
				elif [ -n "$(echo ${i} | grep '^ipset:')" ]; then
					_ipset=$(echo ${i} | sed 's#ipset:##g')
					_ipt_source="ip daddr @${_ipset}"
					msg="备注【$remarks】，NFTset【${_ipset}】，"
				elif [ -n "$(echo ${i} | grep '^ip:')" ]; then
					_ip=$(echo ${i} | sed 's#ip:##g')
					_ipt_source=$(factor ${_ip} "ip saddr")
					msg="备注【$remarks】，IP【${_ip}】，"
				elif [ -n "$(echo ${i} | grep '^mac:')" ]; then
					_mac=$(echo ${i} | sed 's#mac:##g')
					_ipt_source=$(factor ${_mac} "ether saddr")
					msg="备注【$remarks】，MAC【${_mac}】，"
				else
					continue
				fi
				
				local nftset_whitelist="passwall2_${sid}_whitelist"
				local nftset_whitelist6="passwall2_${sid}_whitelist6"
				gen_nftset $nftset_whitelist ipv4_addr 0 0
				gen_nftset $nftset_whitelist6 ipv6_addr 0 0

				[ -n "$redir_port" ] && {
					if [ "$tcp_proxy_mode" != "disable" ]; then
						[ -s "${TMP_ACL_PATH}/${sid}/var_redirect_dns_port" ] && nft "add rule inet fw4 PSW2_REDIRECT ip protocol udp ${_ipt_source} udp dport 53 counter redirect to $(cat ${TMP_ACL_PATH}/${sid}/var_redirect_dns_port) comment \"$remarks\""
						msg2="${msg}使用TCP节点[$node_remark] [$(get_action_chain_name $tcp_proxy_mode)]"
						if [ -n "${is_tproxy}" ]; then
							msg2="${msg2}(TPROXY:${redir_port})代理"
						else
							msg2="${msg2}(REDIRECT:${redir_port})代理"
						fi

						[ -z "${is_tproxy}" ] && nft "add rule inet fw4 PSW2_NAT ip protocol tcp ${_ipt_source} ip daddr @$nftset_whitelist counter return comment \"$remarks\""
						[ -n "${is_tproxy}" ] && nft "add rule inet fw4 PSW2_MANGLE ip protocol tcp ${_ipt_source} ip daddr @$nftset_whitelist counter return comment \"$remarks\""

						[ "$accept_icmp" = "1" ] && {
							nft "add rule inet fw4 PSW2_ICMP_REDIRECT ip protocol icmp ${_ipt_source} ip daddr $FAKE_IP $(REDIRECT) comment \"$remarks\""
							nft "add rule inet fw4 PSW2_ICMP_REDIRECT ip protocol icmp ${_ipt_source} $(REDIRECT) comment \"$remarks\""
							nft "add rule inet fw4 PSW2_ICMP_REDIRECT ip protocol icmp ${_ipt_source} return comment \"$remarks\""
						}

						[ "$accept_icmpv6" = "1" ] && [ "$PROXY_IPV6" == "1" ] && {
							nft "add rule inet fw4 PSW2_ICMP_REDIRECT meta l4proto icmpv6 ${_ipt_source} ip6 daddr $FAKE_IP_6 $(REDIRECT) comment \"$remarks\"" 2>/dev/null
							nft "add rule inet fw4 PSW2_ICMP_REDIRECT meta l4proto icmpv6 ${_ipt_source} $(REDIRECT) comment \"$remarks\"" 2>/dev/null
							nft "add rule inet fw4 PSW2_ICMP_REDIRECT meta l4proto icmpv6 ${_ipt_source} return comment \"$remarks\"" 2>/dev/null
						}

						[ "$tcp_no_redir_ports" != "disable" ] && {
							nft "add rule inet fw4 $nft_prerouting_chain ${_ipt_source} ip protocol tcp $(factor $tcp_no_redir_ports "tcp dport") counter return comment \"$remarks\""
							nft "add rule inet fw4 PSW2_MANGLE_V6 comment ${_ipt_source} meta l4proto tcp $(factor $tcp_no_redir_ports "tcp dport") counter return comment \"$remarks\""
							msg2="${msg2}[$?]除${tcp_no_redir_ports}外的"
						}
						msg2="${msg2}所有端口"

						if [ -z "${is_tproxy}" ]; then
							nft "add rule inet fw4 PSW2_NAT ip protocol tcp ${_ipt_source} ip daddr $FAKE_IP $(REDIRECT $redir_port) comment \"$remarks\""
							nft "add rule inet fw4 PSW2_NAT ip protocol tcp ${_ipt_source} $(factor $tcp_redir_ports "tcp dport") $(REDIRECT $redir_port) comment \"$remarks\""
						else
							nft "add rule inet fw4 PSW2_MANGLE ip protocol tcp ${_ipt_source} ip daddr $FAKE_IP counter jump PSW2_RULE comment \"$remarks\""
							nft "add rule inet fw4 PSW2_MANGLE ip protocol tcp ${_ipt_source} $(factor $tcp_redir_ports "tcp dport") counter jump PSW2_RULE comment \"$remarks\""
							nft "add rule inet fw4 PSW2_MANGLE meta nfproto {ipv4} meta l4proto tcp ${_ipt_source} $(REDIRECT $redir_port TPROXY4) comment \"$remarks\""
						fi

						[ "$PROXY_IPV6" == "1" ] && {
							nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto tcp ${_ipt_source} ip6 daddr @$nftset_whitelist6 counter return comment \"$remarks\""
							nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto tcp ${_ipt_source} ip6 daddr $FAKE_IP_6 counter jump PSW2_RULE comment \"$remarks\""
							nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto tcp ${_ipt_source} $(factor $tcp_redir_ports "tcp dport") jump PSW2_RULE comment \"$remarks\"" 2>/dev/null
							nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto tcp ${_ipt_source} $(REDIRECT $redir_port TPROXY) comment \"$remarks\"" 2>/dev/null
						}
					else
						msg2="${msg}不代理TCP"
					fi
					echolog "  - ${msg2}"
				}

				nft "add rule inet fw4 $nft_prerouting_chain ip protocol tcp ${_ipt_source} counter return comment \"$remarks\""
				nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto tcp ${_ipt_source} counter return comment \"$remarks\"" 2>/dev/null

				[ -n "$redir_port" ] && {
					if [ "$udp_proxy_mode" != "disable" ]; then
						msg2="${msg}使用UDP节点[$node_remark] [$(get_action_chain_name $udp_proxy_mode)]"
						msg2="${msg2}(TPROXY:${redir_port})代理"
						[ "$udp_no_redir_ports" != "disable" ] && {
							nft "add rule inet fw4 PSW2_MANGLE meta l4proto udp ${_ipt_source} $(factor $udp_no_redir_ports "udp dport") counter return comment \"$remarks\""
							nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto udp ${_ipt_source} $(factor $udp_no_redir_ports "udp dport") counter return comment \"$remarks\"" 2>/dev/null
							msg2="${msg2}[$?]除${udp_no_redir_ports}外的"
						}
						msg2="${msg2}所有端口"

						nft "add rule inet fw4 PSW2_MANGLE ip protocol udp ${_ipt_source} ip daddr @$nftset_whitelist counter return comment \"$remarks\""
						nft "add rule inet fw4 PSW2_MANGLE ip protocol udp ${_ipt_source} ip daddr $FAKE_IP counter jump PSW2_RULE comment \"$remarks\""
						nft "add rule inet fw4 PSW2_MANGLE ip protocol udp ${_ipt_source} $(factor $udp_redir_ports "udp dport") jump PSW2_RULE comment \"$remarks\""
						nft "add rule inet fw4 PSW2_MANGLE ip protocol udp ${_ipt_source} $(REDIRECT $redir_port TPROXY4) comment \"$remarks\""

						[ "$PROXY_IPV6" == "1" ] && [ "$PROXY_IPV6_UDP" == "1" ] && {
							nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto udp ${_ipt_source} ip6 daddr @$nftset_whitelist6 counter return comment \"$remarks\""
							nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto udp ${_ipt_source} ip6 daddr $FAKE_IP_6 counter jump PSW2_RULE comment \"$remarks\""
							nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto udp ${_ipt_source} $(factor $udp_redir_ports "udp dport") counter jump PSW2_RULE comment \"$remarks\"" 2>/dev/null
							nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto udp ${_ipt_source} $(REDIRECT $redir_port TPROXY) comment \"$remarks\"" 2>/dev/null
						}
					else
						msg2="${msg}不代理UDP"
					fi
					echolog "  - ${msg2}"
				}
				nft "add rule inet fw4 PSW2_MANGLE ip protocol udp ${_ipt_source} counter return comment \"$remarks\""
				nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto udp ${_ipt_source} counter return comment \"$remarks\"" 2>/dev/null
			done
			unset enabled sid remarks sources tcp_proxy_mode udp_proxy_mode tcp_no_redir_ports udp_no_redir_ports tcp_redir_ports udp_redir_ports node
			unset _ip _mac _iprange _ipset _ip_or_mac rule_list redir_port node_remark
			unset msg msg2
		done
	}

	[ "$ENABLED_DEFAULT_ACL" == 1 ] && {
		#  加载默认代理模式
		if [ "$TCP_PROXY_MODE" != "disable" ]; then
			[ "$TCP_NO_REDIR_PORTS" != "disable" ] && {
				nft add rule inet fw4 $nft_prerouting_chain ip protocol tcp $(factor $TCP_NO_REDIR_PORTS "tcp dport") counter return comment \"默认\"
				nft add rule inet fw4 PSW2_MANGLE_V6 meta l4proto tcp $(factor $TCP_NO_REDIR_PORTS "tcp dport") counter return comment \"默认\"
			}
			[ "$NODE" != "nil" ] && {
				msg="TCP默认代理：使用TCP节点[$(config_n_get $NODE remarks)] [$(get_action_chain_name $TCP_PROXY_MODE)]"
				if [ -n "${is_tproxy}" ]; then
					msg="${msg}(TPROXY:${REDIR_PORT})代理"
				else
					msg="${msg}(REDIRECT:${REDIR_PORT})代理"
				fi
				
				[ "$TCP_NO_REDIR_PORTS" != "disable" ] && msg="${msg}除${TCP_NO_REDIR_PORTS}外的"
				msg="${msg}所有端口"

				[ -z "${is_tproxy}" ] && nft "add rule inet fw4 PSW2_NAT ip protocol tcp ip daddr @$nftset_global_whitelist counter return comment \"$remarks\""
				[ -n "${is_tproxy}" ] && nft "add rule inet fw4 PSW2_MANGLE ip protocol tcp ip daddr @$nftset_global_whitelist counter return comment \"$remarks\""

				[ "$accept_icmp" = "1" ] && {
					nft "add rule inet fw4 PSW2_ICMP_REDIRECT ip protocol icmp ip daddr $FAKE_IP $(REDIRECT) comment \"默认\""
					nft "add rule inet fw4 PSW2_ICMP_REDIRECT ip protocol icmp $(REDIRECT) comment \"默认\""
					nft "add rule inet fw4 PSW2_ICMP_REDIRECT ip protocol icmp return comment \"默认\""
				}

				[ "$accept_icmpv6" = "1" ] && [ "$PROXY_IPV6" == "1" ] && {
					nft "add rule inet fw4 PSW2_ICMP_REDIRECT meta l4proto icmpv6 ip6 daddr $FAKE_IP_6 $(REDIRECT) comment \"默认\""
					nft "add rule inet fw4 PSW2_ICMP_REDIRECT meta l4proto icmpv6 $(REDIRECT) comment \"默认\""
					nft "add rule inet fw4 PSW2_ICMP_REDIRECT meta l4proto icmpv6 return comment \"默认\""
				}

				if [ -z "${is_tproxy}" ]; then
					nft "add rule inet fw4 PSW2_NAT ip protocol tcp ip daddr $FAKE_IP $(REDIRECT $REDIR_PORT) comment \"默认\""
					nft "add rule inet fw4 PSW2_NAT ip protocol tcp $(factor $TCP_REDIR_PORTS "tcp dport") $(REDIRECT $REDIR_PORT) comment \"默认\""
				else
					nft "add rule inet fw4 PSW2_MANGLE ip protocol tcp ip daddr $FAKE_IP counter jump PSW2_RULE comment \"默认\""
					nft "add rule inet fw4 PSW2_MANGLE ip protocol tcp $(factor $TCP_REDIR_PORTS "tcp dport") jump PSW2_RULE comment \"默认\""
					nft "add rule inet fw4 PSW2_MANGLE meta l4proto tcp $(REDIRECT $REDIR_PORT TPROXY) comment \"默认\""
				fi

				[ "$PROXY_IPV6" == "1" ] && {
					nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto tcp ip6 daddr @$nftset_global_whitelist6 counter return comment \"$remarks\""
					nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto tcp ip6 daddr $FAKE_IP_6 jump PSW2_RULE comment \"默认\""
					nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto tcp $(factor $TCP_REDIR_PORTS "tcp dport") jump PSW2_RULE comment \"默认\""
					nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto tcp $(REDIRECT $REDIR_PORT TPROXY) comment \"默认\""
				}

				echolog "${msg}"
			}
		fi

		#  加载UDP默认代理模式
		if [ "$UDP_PROXY_MODE" != "disable" ]; then
			[ "$UDP_NO_REDIR_PORTS" != "disable" ] && {
				nft "add inet fw4 PSW2_MANGLE ip protocol udp $(factor $UDP_NO_REDIR_PORTS "udp dport") counter return comment \"默认\""
				nft "add inet fw4 PSW2_MANGLE_V6 counter meta l4proto udp $(factor $UDP_NO_REDIR_PORTS "udp dport") counter return comment \"默认\""
			}

			[ -n "1" ] && {
				msg="UDP默认代理：使用UDP节点[$(config_n_get $NODE remarks)] [$(get_action_chain_name $UDP_PROXY_MODE)](TPROXY:${REDIR_PORT})代理"

				[ "$UDP_NO_REDIR_PORTS" != "disable" ] && msg="${msg}除${UDP_NO_REDIR_PORTS}外的"
				msg="${msg}所有端口"

				nft "add rule inet fw4 PSW2_MANGLE ip protocol udp ip daddr @$nftset_global_whitelist counter return comment \"$remarks\""
				nft "add rule inet fw4 PSW2_MANGLE ip protocol udp ip daddr $FAKE_IP counter jump PSW2_RULE comment \"默认\""
				nft "add rule inet fw4 PSW2_MANGLE ip protocol udp $(factor $UDP_REDIR_PORTS "udp dport") jump PSW2_RULE comment \"默认\""
				nft "add rule inet fw4 PSW2_MANGLE meta l4proto udp $(REDIRECT $REDIR_PORT TPROXY) comment \"默认\""

				[ "$PROXY_IPV6" == "1" ] && [ "$PROXY_IPV6_UDP" == "1" ] && {
					nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto udp ip6 daddr @$nftset_global_whitelist6 counter return comment \"$remarks\""
					nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto udp ip6 daddr $FAKE_IP_6 jump PSW2_RULE comment \"默认\""
					nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto udp $(factor $UDP_REDIR_PORTS "udp dport") jump PSW2_RULE comment \"默认\""
					nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto udp $(REDIRECT $REDIR_PORT TPROXY) comment \"默认\""
				}

				echolog "${msg}"
				udp_flag=1
			}
		fi
	}
}

filter_haproxy() {
	for item in $(uci show $CONFIG | grep ".lbss=" | cut -d "'" -f 2); do
		local ip=$(get_host_ip ipv4 $(echo $item | awk -F ":" '{print $1}') 1)
		[ -n "$ip" ] && insert_nftset $NFTSET_VPSLIST 0 $ip
	done
	echolog "加入负载均衡的节点到nftset[$NFTSET_VPSLIST]直连完成"
}

filter_vps_addr() {
	for server_host in $@; do
		local vps_ip4=$(get_host_ip "ipv4" ${server_host})
		local vps_ip6=$(get_host_ip "ipv6" ${server_host})
		[ -n "$vps_ip4" ] && insert_nftset $NFTSET_VPSLIST 0 $vps_ip4
		[ -n "$vps_ip6" ] && insert_nftset $NFTSET_VPSLIST6 0 $vps_ip6
	done
}

filter_vpsip() {
	insert_nftset $NFTSET_VPSLIST 0 $(uci show $CONFIG | grep ".address=" | cut -d "'" -f 2 | grep -E "([0-9]{1,3}[\.]){3}[0-9]{1,3}" | sed -e "/^$/d")
	insert_nftset $NFTSET_VPSLIST6 0 $(uci show $CONFIG | grep ".address=" | cut -d "'" -f 2 | grep -E "([A-Fa-f0-9]{1,4}::?){1,7}[A-Fa-f0-9]{1,4}" | sed -e "/^$/d")
	echolog "加入所有节点到nftset[$NFTSET_VPSLIST]直连完成"
}

filter_node() {
	local proxy_node=${1}
	local stream=$(echo ${2} | tr 'A-Z' 'a-z')
	local proxy_port=${3}

	filter_rules() {
		local node=${1}
		local stream=${2}
		local _proxy=${3}
		local _port=${4}
		local _is_tproxy msg msg2

		if [ -n "$node" ] && [ "$node" != "nil" ]; then
			local type=$(echo $(config_n_get $node type) | tr 'A-Z' 'a-z')
			local address=$(config_n_get $node address)
			local port=$(config_n_get $node port)
			_is_tproxy=${is_tproxy}
			[ "$stream" == "udp" ] && _is_tproxy="TPROXY"
			if [ -n "${_is_tproxy}" ]; then
				msg="TPROXY"
			else
				msg="REDIRECT"
			fi
		else
			echolog "  - 节点配置不正常，略过"
			return 0
		fi

		local ADD_INDEX=$FORCE_INDEX
		for _ipt in 4 6; do
			[ "$_ipt" == "4" ] && _ip_type=ip && _set_name=$NFTSET_VPSLIST
			[ "$_ipt" == "6" ] && _ip_type=ip6 && _set_name=$NFTSET_VPSLIST6
			nft "list chain inet fw4 $nft_output_chain" 2>/dev/null | grep -q "${address}:${port}"
			if [ $? -ne 0 ]; then
				unset dst_rule
				local dst_rule="jump PSW2_RULE"
				msg2="按规则路由(${msg})"
				[ -n "${is_tproxy}" ] || {
					dst_rule=$(REDIRECT $_port)
					msg2="套娃使用(${msg}:${port} -> ${_port})"
				}
				[ -n "$_proxy" ] && [ "$_proxy" == "1" ] && [ -n "$_port" ] || {
					ADD_INDEX=$(RULE_LAST_INDEX "inet fw4" $nft_output_chain $_set_name $FORCE_INDEX)
					dst_rule="return"
					msg2="直连代理"
				}
				nft "insert rule inet fw4 $nft_output_chain position $ADD_INDEX meta l4proto $stream $_ip_type daddr $address $stream dport $port $dst_rule comment \"${address}:${port}\"" 2>/dev/null
			else
				msg2="已配置过的节点，"
			fi
		done
		msg="[$?]$(echo ${2} | tr 'a-z' 'A-Z')${msg2}使用链${ADD_INDEX}，节点（${type}）：${address}:${port}"
		#echolog "  - ${msg}"
	}

	local proxy_protocol=$(config_n_get $proxy_node protocol)
	local proxy_type=$(echo $(config_n_get $proxy_node type nil) | tr 'A-Z' 'a-z')
	[ "$proxy_type" == "nil" ] && echolog "  - 节点配置不正常，略过！：${proxy_node}" && return 0
	if [ "$proxy_protocol" == "_balancing" ]; then
		#echolog "  - 多节点负载均衡（${proxy_type}）..."
		proxy_node=$(config_n_get $proxy_node balancing_node)
		for _node in $proxy_node; do
			filter_rules "$_node" "$stream"
		done
	elif [ "$proxy_protocol" == "_shunt" ]; then
		#echolog "  - 按请求目的地址分流（${proxy_type}）..."
		local default_node=$(config_n_get $proxy_node default_node _direct)
		local main_node=$(config_n_get $proxy_node main_node nil)
		if [ "$main_node" != "nil" ]; then
			filter_rules $main_node $stream
		else
			if [ "$default_node" != "_direct" ] && [ "$default_node" != "_blackhole" ]; then
				filter_rules $default_node $stream
			fi
		fi
:<<!
		local default_node_address=$(get_host_ip ipv4 $(config_n_get $default_node address) 1)
		local default_node_port=$(config_n_get $default_node port)

		local shunt_ids=$(uci show $CONFIG | grep "=shunt_rules" | awk -F '.' '{print $2}' | awk -F '=' '{print $1}')
		for shunt_id in $shunt_ids; do
			#local shunt_proxy=$(config_n_get $proxy_node "${shunt_id}_proxy" 0)
			local shunt_proxy=0
			local shunt_node=$(config_n_get $proxy_node "${shunt_id}" nil)
			[ "$shunt_node" != "nil" ] && {
				[ "$shunt_proxy" == 1 ] && {
					local shunt_node_address=$(get_host_ip ipv4 $(config_n_get $shunt_node address) 1)
					local shunt_node_port=$(config_n_get $shunt_node port)
					[ "$shunt_node_address" == "$default_node_address" ] && [ "$shunt_node_port" == "$default_node_port" ] && {
						shunt_proxy=0
					}
				}
				filter_rules "$(config_n_get $proxy_node $shunt_id)" "$stream" "$shunt_proxy" "$proxy_port"
			}
		done
!
	else
		#echolog "  - 普通节点（${proxy_type}）..."
		filter_rules "$proxy_node" "$stream"
	fi
}

dns_hijack() {
	nft "add rule inet fw4 dstnat ip protocol udp ip dport 53 redirect to 53"
	echolog "强制转发本机DNS端口 UDP/53 的请求[$?]"
}

add_firewall_rule() {
	echolog "开始加载防火墙规则..."
	gen_nftset $NFTSET_LANLIST ipv4_addr 0 0 $(gen_lanlist)
	gen_nftset $NFTSET_VPSLIST ipv4_addr 0 0

	gen_nftset $NFTSET_LANLIST6 ipv6_addr 0 0 $(gen_lanlist_6)
	gen_nftset $NFTSET_VPSLIST6 ipv6_addr 0 0

	# 忽略特殊IP段
	local lan_ifname lan_ip
	lan_ifname=$(uci -q -p /tmp/state get network.lan.ifname)
	[ -n "$lan_ifname" ] && {
		lan_ip=$(ip address show $lan_ifname | grep -w "inet" | awk '{print $2}')
		lan_ip6=$(ip address show $lan_ifname | grep -w "inet6" | awk '{print $2}')
		#echolog "本机IPv4网段互访直连：${lan_ip}"
		#echolog "本机IPv6网段互访直连：${lan_ip6}"

		[ -n "$lan_ip" ] && insert_nftset $NFTSET_LANLIST 0 $(echo $lan_ip | sed -e "s/ /\n/g")
		[ -n "$lan_ip6" ] && insert_nftset $NFTSET_LANLIST6 0 $(echo $lan_ip6 | sed -e "s/ /\n/g")
	}

	[ -n "$ISP_DNS" ] && {
		#echolog "处理 ISP DNS 例外..."
		for ispip in $ISP_DNS; do
			insert_nftset $NFTSET_LANLIST 0 $ispip >/dev/null 2>&1 &
			#echolog "  - 追加到白名单：${ispip}"
		done
	}

	[ -n "$ISP_DNS6" ] && {
		#echolog "处理 ISP IPv6 DNS 例外..."
		for ispip6 in $ISP_DNS6; do
			insert_nftset $NFTSET_LANLIST6 0 $ispip6 >/dev/null 2>&1 &
			#echolog "  - 追加到白名单：${ispip6}"
		done
	}
	
	local nftset_global_whitelist="passwall2_global_whitelist"
	local nftset_global_whitelist6="passwall2_global_whitelist6"
	gen_nftset $nftset_global_whitelist ipv4_addr 0 0
	gen_nftset $nftset_global_whitelist6 ipv6_addr 0 0

	#  过滤所有节点IP
	filter_vpsip > /dev/null 2>&1 &
	filter_haproxy > /dev/null 2>&1 &
	# Prevent some conditions
	filter_vps_addr $(config_n_get $NODE address) > /dev/null 2>&1 &

	accept_icmp=$(config_t_get global_forwarding accept_icmp 0)
	accept_icmpv6=$(config_t_get global_forwarding accept_icmpv6 0)

	local tcp_proxy_way=$(config_t_get global_forwarding tcp_proxy_way redirect)
	if [ "$tcp_proxy_way" = "redirect" ]; then
		unset is_tproxy
		nft_prerouting_chain="PSW2_NAT"
		nft_output_chain="PSW2_OUTPUT_NAT"
	elif [ "$tcp_proxy_way" = "tproxy" ]; then
		is_tproxy="TPROXY"
		nft_prerouting_chain="PSW2_MANGLE"
		nft_output_chain="PSW2_OUTPUT_MANGLE"
	fi

	nft "add chain inet fw4 nat_output { type nat hook output priority -1; }"

	nft "add chain inet fw4 PSW2_DIVERT"
	nft "flush chain inet fw4 PSW2_DIVERT"
	nft "add rule inet fw4 PSW2_DIVERT meta l4proto tcp socket transparent 1 mark set 1 counter accept"

	nft "add chain inet fw4 PSW2_REDIRECT"
	nft "flush chain inet fw4 PSW2_REDIRECT"
	nft "add rule inet fw4 dstnat jump PSW2_REDIRECT"

	# for ipv4 ipv6 tproxy mark
	nft "add chain inet fw4 PSW2_RULE"
	nft "flush chain inet fw4 PSW2_RULE"
	nft "add rule inet fw4 PSW2_RULE meta mark set ct mark counter"
	nft "add rule inet fw4 PSW2_RULE meta mark 1 counter return"
	nft "add rule inet fw4 PSW2_RULE tcp flags &(fin|syn|rst|ack) == syn meta mark set mark and 0x0 xor 0x1 counter"
	nft "add rule inet fw4 PSW2_RULE meta l4proto udp ct state new meta mark set mark and 0x0 xor 0x1 counter"
	nft "add rule inet fw4 PSW2_RULE ct mark set mark counter"

	#ipv4 tproxy mode and udp
	nft "add chain inet fw4 PSW2_MANGLE"
	nft "flush chain inet fw4 PSW2_MANGLE"
	nft "add rule inet fw4 PSW2_MANGLE ip daddr @$NFTSET_LANLIST counter return"
	nft "add rule inet fw4 PSW2_MANGLE ip daddr @$NFTSET_VPSLIST counter return"

	nft "add chain inet fw4 PSW2_OUTPUT_MANGLE"
	nft "flush chain inet fw4 PSW2_OUTPUT_MANGLE"
	nft "add rule inet fw4 PSW2_OUTPUT_MANGLE ip daddr @$NFTSET_LANLIST counter return"
	nft "add rule inet fw4 PSW2_OUTPUT_MANGLE ip daddr @$NFTSET_VPSLIST counter return"
	nft "add rule inet fw4 PSW2_OUTPUT_MANGLE ip daddr @$nftset_global_whitelist counter return"
	nft "add rule inet fw4 PSW2_OUTPUT_MANGLE meta mark 0xff counter return"

	# jump chains
	nft "add rule inet fw4 mangle_prerouting meta nfproto {ipv4} counter jump PSW2_MANGLE"
	insert_rule_before "inet fw4" "mangle_prerouting" "PSW2_MANGLE" "counter jump PSW2_DIVERT"

	#ipv4 tcp redirect mode
	[ -z "${is_tproxy}" ] && {
		nft "add chain inet fw4 PSW2_NAT"
		nft "flush chain inet fw4 PSW2_NAT"
		nft "add rule inet fw4 PSW2_NAT ip daddr @$NFTSET_LANLIST counter return"
		nft "add rule inet fw4 PSW2_NAT ip daddr @$NFTSET_VPSLIST counter return"
		nft "add rule inet fw4 dstnat ip protocol tcp counter jump PSW2_NAT"

		nft "add chain inet fw4 PSW2_OUTPUT_NAT"
		nft "flush chain inet fw4 PSW2_OUTPUT_NAT"
		nft "add rule inet fw4 PSW2_OUTPUT_NAT ip daddr @$NFTSET_LANLIST counter return"
		nft "add rule inet fw4 PSW2_OUTPUT_NAT ip daddr @$NFTSET_VPSLIST counter return"
		nft "add rule inet fw4 PSW2_OUTPUT_NAT ip daddr @$nftset_global_whitelist counter return"
		nft "add rule inet fw4 PSW2_OUTPUT_NAT meta mark 0xff counter return"
	}

	#icmp ipv6-icmp redirect
	if [ "$accept_icmp" = "1" ]; then
		nft "add chain inet fw4 PSW2_ICMP_REDIRECT"
		nft "flush chain inet fw4 PSW2_ICMP_REDIRECT"
		nft "add rule inet fw4 PSW2_ICMP_REDIRECT ip daddr @$NFTSET_LANLIST counter return"
		nft "add rule inet fw4 PSW2_ICMP_REDIRECT ip daddr @$NFTSET_VPSLIST counter return"

		[ "$accept_icmpv6" = "1" ] && {
			nft "add rule inet fw4 PSW2_ICMP_REDIRECT ip6 daddr @$NFTSET_LANLIST6 counter return"
			nft "add rule inet fw4 PSW2_ICMP_REDIRECT ip6 daddr @$NFTSET_VPSLIST6 counter return"
		}

		nft "add rule inet fw4 dstnat meta l4proto {icmp,icmpv6} counter jump PSW2_ICMP_REDIRECT"
		nft "add rule inet fw4 nat_output meta l4proto {icmp,icmpv6} counter jump PSW2_ICMP_REDIRECT"
	fi

	WAN_IP=$(get_wan_ip)
	if [ -n "${WAN_IP}" ]; then
		[ -n "${is_tproxy}" ] && nft "add rule inet fw4 PSW2_MANGLE ip daddr ${WAN_IP} counter return comment \"WAN_IP_RETURN\"" || nft "add rule inet fw4 PSW2_NAT ip daddr ${WAN_IP} counter return comment \"WAN_IP_RETURN\""
	fi
	unset WAN_IP

	ip rule add fwmark 1 lookup 100
	ip route add local 0.0.0.0/0 dev lo table 100

	#ipv6 tproxy mode and udp
	nft "add chain inet fw4 PSW2_MANGLE_V6"
	nft "flush chain inet fw4 PSW2_MANGLE_V6"
	nft "add rule inet fw4 PSW2_MANGLE_V6 ip6 daddr @$NFTSET_LANLIST6 counter return"
	nft "add rule inet fw4 PSW2_MANGLE_V6 ip6 daddr @$NFTSET_VPSLIST6 counter return"
	nft "add rule inet fw4 PSW2_MANGLE_V6 ip6 daddr @$nftset_global_whitelist6 counter return"

	nft "add chain inet fw4 PSW2_OUTPUT_MANGLE_V6"
	nft "flush chain inet fw4 PSW2_OUTPUT_MANGLE_V6"
	nft "add rule inet fw4 PSW2_OUTPUT_MANGLE_V6 ip6 daddr @$NFTSET_LANLIST6 counter return"
	nft "add rule inet fw4 PSW2_OUTPUT_MANGLE_V6 ip6 daddr @$NFTSET_VPSLIST6 counter return"
	nft "add rule inet fw4 PSW2_OUTPUT_MANGLE_V6 ip6 daddr @$nftset_global_whitelist6 counter return"
	nft "add rule inet fw4 PSW2_OUTPUT_MANGLE_V6 meta mark 0xff counter return"

	# jump chains
	[ "$PROXY_IPV6" == "1" ] && {
		nft "add rule inet fw4 mangle_prerouting meta nfproto {ipv6} counter jump PSW2_MANGLE_V6"
		nft "add rule inet fw4 mangle_output meta nfproto {ipv6} counter jump PSW2_OUTPUT_MANGLE_V6 comment \"PSW2_OUTPUT_MANGLE\""

		WAN6_IP=$(get_wan6_ip)
		[ -n "${WAN6_IP}" ] && nft "add rule inet fw4 PSW2_MANGLE_V6 ip6 daddr ${WAN6_IP} counter return comment \"WAN6_IP_RETURN\""
		unset WAN6_IP

		ip -6 rule add fwmark 1 table 100
		ip -6 route add local ::/0 dev lo table 100
	}
	
	# 过滤Socks节点
	[ "$SOCKS_ENABLED" = "1" ] && {
		local ids=$(uci show $CONFIG | grep "=socks" | awk -F '.' '{print $2}' | awk -F '=' '{print $1}')
		#echolog "分析 Socks 服务所使用节点..."
		local id enabled node port msg num
		for id in $ids; do
			enabled=$(config_n_get $id enabled 0)
			[ "$enabled" == "1" ] || continue
			node=$(config_n_get $id node nil)
			port=$(config_n_get $id port 0)
			msg="Socks 服务 [:${port}]"
			if [ "$node" == "nil" ] || [ "$port" == "0" ]; then
				msg="${msg} 未配置完全，略过"
			else
				filter_node $node TCP > /dev/null 2>&1 &
				filter_node $node UDP > /dev/null 2>&1 &
			fi
			#echolog "  - ${msg}"
		done
	}

	[ "$ENABLED_DEFAULT_ACL" == 1 ] && {
		# 加载路由器自身代理 TCP
		if [ "$NODE" != "nil" ] && [ "$LOCALHOST_PROXY" = "1" ]; then
			echolog "加载路由器自身 TCP 代理..."

			[ "$accept_icmp" = "1" ] && {
				nft "add rule inet fw4 PSW2_ICMP_REDIRECT oif lo ip protocol icmp ip daddr $FAKE_IP counter redirect"
				nft "add rule inet fw4 PSW2_ICMP_REDIRECT oif lo ip protocol icmp counter redirect"
				nft "add rule inet fw4 PSW2_ICMP_REDIRECT oif lo ip protocol icmp counter return"
			}

			[ "$accept_icmpv6" = "1" ] && {
				nft "add rule inet fw4 PSW2_ICMP_REDIRECT oif lo meta l4proto icmpv6 ip6 daddr $FAKE_IP_6 counter redirect"
				nft "add rule inet fw4 PSW2_ICMP_REDIRECT oif lo meta l4proto counter redirect"
				nft "add rule inet fw4 PSW2_ICMP_REDIRECT oif lo meta l4proto icmpv6 counter return"
			}

			[ -n "${is_tproxy}" ] && {
				echolog "  - 启用 TPROXY 模式"
			}

			[ "$TCP_NO_REDIR_PORTS" != "disable" ] && {
				nft "add rule inet fw4 $nft_output_chain ip protocol tcp $(factor $TCP_NO_REDIR_PORTS "tcp dport") counter return"
				nft "add rule inet fw4 PSW2_OUTPUT_MANGLE_V6 meta l4proto tcp $(factor $TCP_NO_REDIR_PORTS "tcp dport") counter return"
				echolog "  - [$?]不代理TCP 端口：$TCP_NO_REDIR_PORTS"
			}

			if [ -z "${is_tproxy}" ]; then
				nft "add rule inet fw4 PSW2_OUTPUT_NAT ip protocol tcp ip daddr $FAKE_IP $(REDIRECT $REDIR_PORT)"
				nft "add rule inet fw4 PSW2_OUTPUT_NAT ip protocol tcp $(factor $TCP_REDIR_PORTS "tcp dport") $(REDIRECT $REDIR_PORT)"
				nft "add rule inet fw4 nat_output ip protocol tcp counter jump PSW2_OUTPUT_NAT"
			else
				nft "add rule inet fw4 PSW2_OUTPUT_MANGLE ip protocol tcp ip daddr $FAKE_IP counter jump PSW2_RULE"
				nft "add rule inet fw4 PSW2_OUTPUT_MANGLE ip protocol tcp $(factor $TCP_REDIR_PORTS "tcp dport") jump PSW2_RULE"
				nft "add rule inet fw4 PSW2_MANGLE meta l4proto tcp iif lo $(REDIRECT $REDIR_PORT TPROXY) comment \"本机\""
				nft "add rule inet fw4 PSW2_MANGLE ip protocol tcp iif lo counter return comment \"本机\""
				nft "add rule inet fw4 mangle_output meta nfproto {ipv4} meta l4proto tcp counter jump PSW2_OUTPUT_MANGLE comment \"PSW2_OUTPUT_MANGLE\""
			fi

			[ "$PROXY_IPV6" == "1" ] && {
				nft "add rule inet fw4 PSW2_OUTPUT_MANGLE_V6 meta l4proto tcp ip6 daddr $FAKE_IP_6 jump PSW2_RULE"
				nft "add rule inet fw4 PSW2_OUTPUT_MANGLE_V6 meta l4proto tcp $(factor $TCP_REDIR_PORTS "tcp dport") jump PSW2_RULE"
				nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto tcp iif lo $(REDIRECT $REDIR_PORT TPROXY) comment \"本机\""
				nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto tcp iif lo counter return comment \"本机\""
			}

			for iface in $(ls ${TMP_IFACE_PATH}); do
				nft "insert rule inet fw4 $nft_output_chain ip protocol tcp oif $iface counter return"
				nft "insert rule inet fw4 PSW2_OUTPUT_MANGLE_V6 ip protocol tcp oif $iface counter return"
			done
		fi
		
		# 处理轮换节点的分流或套娃
		filter_node $NODE TCP > /dev/null 2>&1 &
		filter_node $NODE UDP > /dev/null 2>&1 &

		# 加载路由器自身代理 UDP
		if [ "$NODE" != "nil" ] && [ "$LOCALHOST_PROXY" = "1" ]; then
			echolog "加载路由器自身 UDP 代理..."

			[ "$UDP_NO_REDIR_PORTS" != "disable" ] && {
				nft add rule inet fw4 PSW2_OUTPUT_MANGLE ip protocol udp $(factor $UDP_NO_REDIR_PORTS "udp dport") counter return
				nft add rule inet fw4 PSW2_OUTPUT_MANGLE_V6 meta l4proto udp $(factor $UDP_NO_REDIR_PORTS "udp dport") counter return
				echolog "  - [$?]不代理 UDP 端口：$UDP_NO_REDIR_PORTS"
			}

			nft "add rule inet fw4 PSW2_OUTPUT_MANGLE ip protocol udp ip daddr $FAKE_IP counter jump PSW2_RULE"
			nft "add rule inet fw4 PSW2_OUTPUT_MANGLE ip protocol udp $(factor $UDP_REDIR_PORTS "udp dport") jump PSW2_RULE"
			nft "add rule inet fw4 PSW2_MANGLE meta l4proto udp iif lo $(REDIRECT $REDIR_PORT TPROXY) comment \"本机\""
			nft "add rule inet fw4 PSW2_MANGLE ip protocol udp iif lo counter return comment \"本机\""
			nft "add rule inet fw4 mangle_output meta nfproto {ipv4} meta l4proto udp counter jump PSW2_OUTPUT_MANGLE comment \"PSW2_OUTPUT_MANGLE\""

			if [ "$PROXY_IPV6_UDP" == "1" ]; then
				nft "add rule inet fw4 PSW2_OUTPUT_MANGLE_V6 meta l4proto udp ip6 daddr $FAKE_IP_6 jump PSW2_RULE"
				nft "add rule inet fw4 PSW2_OUTPUT_MANGLE_V6 meta l4proto udp $(factor $UDP_REDIR_PORTS "udp dport") jump PSW2_RULE"
				nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto udp iif lo $(REDIRECT $REDIR_PORT TPROXY) comment \"本机\""
				nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto udp iif lo counter return comment \"本机\""
			fi

			for iface in $(ls ${TMP_IFACE_PATH}); do
				nft "insert rule inet fw4 $nft_output_chain ip protocol udp oif $iface counter return"
				nft "insert rule inet fw4 PSW2_OUTPUT_MANGLE_V6 ip protocol udp oif $iface counter return"
			done
		fi

		nft "add rule inet fw4 mangle_output oif lo counter return comment \"PSW2_OUTPUT_MANGLE\""
		nft "add rule inet fw4 mangle_output meta mark 1 counter return comment \"PSW2_OUTPUT_MANGLE\""

		nft "add rule inet fw4 PSW2_MANGLE ip protocol udp udp dport 53 counter return"
		nft "add rule inet fw4 PSW2_MANGLE_V6 meta l4proto udp udp dport 53 counter return"
	}

	#  加载ACLS
	load_acl

	[ -n "${is_tproxy}" -o -n "${udp_flag}" ] && {
		bridge_nf_ipt=$(sysctl -e -n net.bridge.bridge-nf-call-iptables)
		echo -n $bridge_nf_ipt > $TMP_PATH/bridge_nf_ipt
		sysctl -w net.bridge.bridge-nf-call-iptables=0 >/dev/null 2>&1
		[ "$PROXY_IPV6" == "1" ] && {
			bridge_nf_ip6t=$(sysctl -e -n net.bridge.bridge-nf-call-ip6tables)
			echo -n $bridge_nf_ip6t > $TMP_PATH/bridge_nf_ip6t
			sysctl -w net.bridge.bridge-nf-call-ip6tables=0 >/dev/null 2>&1
		}
	}
	echolog "防火墙规则加载完成！"
}

del_firewall_rule() {
	for nft in "forward" "dstnat" "srcnat" "nat_output" "mangle_prerouting" "mangle_output"; do
        local handles=$(nft -a list chain inet fw4 ${nft} 2>/dev/null | grep -E "PSW2_" | awk -F '# handle ' '{print$2}')
		for handle in $handles; do
			nft delete rule inet fw4 ${nft} handle ${handle} 2>/dev/null
		done
	done

	for handle in $(nft -a list chains | grep -E "chain PSW2_" | grep -v "PSW2_RULE" | awk -F '# handle ' '{print$2}'); do
		nft delete chain inet fw4 handle ${handle} 2>/dev/null
	done

	# Need to be removed at the end, otherwise it will show "Resource busy"
	nft delete chain inet fw4 handle $(nft -a list chains | grep -E "PSW2_RULE" | awk -F '# handle ' '{print$2}') 2>/dev/null

	ip rule del fwmark 1 lookup 100 2>/dev/null
	ip route del local 0.0.0.0/0 dev lo table 100 2>/dev/null

	ip -6 rule del fwmark 1 table 100 2>/dev/null
	ip -6 route del local ::/0 dev lo table 100 2>/dev/null

	destroy_nftset $NFTSET_LANLIST
	destroy_nftset $NFTSET_VPSLIST

	destroy_nftset $NFTSET_LANLIST6
	destroy_nftset $NFTSET_VPSLIST6

	$DIR/app.sh echolog "删除相关防火墙规则完成。"
}

flush_nftset() {
	del_firewall_rule
	for _name in $(nft -a list sets | grep -E "passwall2" | awk -F 'set ' '{print $2}' | awk '{print $1}'); do
		destroy_nftset ${_name}
	done
	rm -rf /tmp/singbox_passwall2_*
	/etc/init.d/passwall2 reload
}

flush_include() {
	echo '#!/bin/sh' >$FWI
}

gen_include() {
	local nft_chain_file=$TMP_PATH/PSW2_RULE.nft
	local nft_set_file=$TMP_PATH/PSW2_SETS.nft
	echo "#!/usr/sbin/nft -f" > $nft_chain_file
	echo "#!/usr/sbin/nft -f" > $nft_set_file
	for chain in $(nft -a list chains | grep -E "chain PSW2_" | awk -F ' ' '{print$2}'); do
		nft list chain inet fw4 ${chain} >> $nft_chain_file
	done
	
	for set_name in $(nft -a list sets | grep -E "set passwall2_" | awk -F ' ' '{print$2}'); do
		nft list set inet fw4 ${set_name} >> $nft_set_file
	done

	local __nft=" "
	__nft=$(cat <<- EOF

		[ -z "\$(nft list sets 2>/dev/null | grep "passwall2_")" ] && nft -f ${nft_set_file}
		[ -z "\$(nft list chain inet fw4 nat_output 2>/dev/null)" ] && nft "add chain inet fw4 nat_output { type nat hook output priority -1; }"
		nft -f ${nft_chain_file}

		nft "add rule inet fw4 dstnat jump PSW2_REDIRECT"

		[ "$accept_icmp" == "1" ] && {
			nft "add rule inet fw4 dstnat meta l4proto {icmp,icmpv6} counter jump PSW2_ICMP_REDIRECT"
			nft "add rule inet fw4 nat_output meta l4proto {icmp,icmpv6} counter jump PSW2_ICMP_REDIRECT"
		}

		[ -z "${is_tproxy}" ] && {
			PR_INDEX=\$(sh ${MY_PATH} RULE_LAST_INDEX "inet fw4" PSW2_NAT WAN_IP_RETURN -1)
			if [ \$PR_INDEX -ge 0 ]; then
				WAN_IP=\$(sh ${MY_PATH} get_wan_ip)
				[ ! -z "\${WAN_IP}" ] && nft "replace rule inet fw4 PSW2_NAT handle \$PR_INDEX ip daddr "\${WAN_IP}" counter return comment \"WAN_IP_RETURN\""
			fi
			nft "add rule inet fw4 dstnat ip protocol tcp counter jump PSW2_NAT"
			nft "add rule inet fw4 nat_output ip protocol tcp counter jump PSW2_OUTPUT_NAT"
		}

		[ -n "${is_tproxy}" ] && {
			PR_INDEX=\$(sh ${MY_PATH} RULE_LAST_INDEX "inet fw4" PSW2_MANGLE WAN_IP_RETURN -1)
			if [ \$PR_INDEX -ge 0 ]; then
				WAN_IP=\$(sh ${MY_PATH} get_wan_ip)
				[ ! -z "\${WAN_IP}" ] && nft "replace rule inet fw4 PSW2_MANGLE handle \$PR_INDEX ip daddr "\${WAN_IP}" counter return comment \"WAN_IP_RETURN\""
			fi
			nft "add rule inet fw4 mangle_prerouting meta nfproto {ipv4} counter jump PSW2_MANGLE"
			nft "add rule inet fw4 mangle_output meta nfproto {ipv4} meta l4proto tcp counter jump PSW2_OUTPUT_MANGLE comment \"PSW2_OUTPUT_MANGLE\""
		}
		\$(sh ${MY_PATH} insert_rule_before "inet fw4" "mangle_prerouting" "PSW2_MANGLE" "counter jump PSW2_DIVERT")

		[ "$UDP_NODE" != "nil" -o "$TCP_UDP" = "1" ] && nft "add rule inet fw4 mangle_output meta nfproto {ipv4} meta l4proto udp counter jump PSW2_OUTPUT_MANGLE comment \"PSW2_OUTPUT_MANGLE\""

		[ "$PROXY_IPV6" == "1" ] && {
			PR_INDEX=\$(sh ${MY_PATH} RULE_LAST_INDEX "inet fw4" PSW2_MANGLE_V6 WAN6_IP_RETURN -1)
			if [ \$PR_INDEX -ge 0 ]; then
				WAN6_IP=\$(sh ${MY_PATH} get_wan6_ip)
				[ ! -z "\${WAN_IP}" ] && nft "replace rule inet fw4 PSW2_MANGLE_V6 handle \$PR_INDEX ip6 daddr "\${WAN6_IP}" counter return comment \"WAN6_IP_RETURN\""
			fi
			nft "add rule inet fw4 mangle_prerouting meta nfproto {ipv6} counter jump PSW2_MANGLE_V6"
			nft "add rule inet fw4 mangle_output meta nfproto {ipv6} counter jump PSW2_OUTPUT_MANGLE_V6 comment \"PSW2_OUTPUT_MANGLE\""
		}

		nft "add rule inet fw4 mangle_output oif lo counter return comment \"PSW2_OUTPUT_MANGLE\""
		nft "add rule inet fw4 mangle_output meta mark 1 counter return comment \"PSW2_OUTPUT_MANGLE\""
	EOF
	)

	cat <<-EOF >> $FWI
	${__nft}
	EOF
	return 0
}

start() {
	[ "$ENABLED_DEFAULT_ACL" == 0 -a "$ENABLED_ACLS" == 0 ] && return
	add_firewall_rule
	gen_include
}

stop() {
	del_firewall_rule
	flush_include
}

arg1=$1
shift
case $arg1 in
RULE_LAST_INDEX)
	RULE_LAST_INDEX "$@"
	;;
insert_rule_before)
	insert_rule_before "$@"
	;;
insert_rule_after)
	insert_rule_after "$@"
	;;
flush_nftset)
	flush_nftset
	;;
get_wan_ip)
	get_wan_ip
	;;
get_wan6_ip)
	get_wan6_ip
	;;
stop)
	stop
	;;
start)
	start
	;;
*) ;;
esac
