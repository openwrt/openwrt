#!/bin/sh
# Copyright (C) 2018-2020 L-WRT Team
# Copyright (C) 2021-2025 xiaorouji
# Copyright (C) 2026 Openwrt-Passwall Organization

. /lib/functions.sh
. /lib/functions/service.sh
. /usr/share/libubox/jshn.sh

. /usr/share/passwall/utils.sh
GLOBAL_ACL_PATH=${TMP_ACL_PATH}/default
LUA_UTIL_PATH=/usr/lib/lua/luci/passwall
UTIL_SINGBOX=$LUA_UTIL_PATH/util_sing-box.lua
UTIL_SS=$LUA_UTIL_PATH/util_shadowsocks.lua
UTIL_XRAY=$LUA_UTIL_PATH/util_xray.lua
UTIL_NAIVE=$LUA_UTIL_PATH/util_naiveproxy.lua
UTIL_HYSTERIA2=$LUA_UTIL_PATH/util_hysteria2.lua
SINGBOX_BIN=$(first_type $(config_t_get global_app sing_box_file) sing-box)
XRAY_BIN=$(first_type $(config_t_get global_app xray_file) xray)

check_run_environment() {
	local prefer_nft=$(config_t_get global_forwarding prefer_nft 1)
	local dnsmasq_info=$(dnsmasq -v 2>/dev/null)
	local dnsmasq_ver=$(echo "$dnsmasq_info" | sed -n '1s/.*version \([0-9.]*\).*/\1/p')
	# local dnsmasq_opts=$(echo "$dnsmasq_info" | grep -i "Compile time options")
	local dnsmasq_ipset=0; echo "$dnsmasq_info" | grep -qw "ipset" && dnsmasq_ipset=1
	local dnsmasq_nftset=0; echo "$dnsmasq_info" | grep -qw "nftset" && dnsmasq_nftset=1
	local has_ipt=0; { command -v iptables-legacy || command -v iptables; } >/dev/null && has_ipt=1
	local has_ipset=$(command -v ipset >/dev/null && echo 1 || echo 0)
	local has_fw4=$(command -v fw4 >/dev/null && echo 1 || echo 0)
	if [ "$prefer_nft" = "1" ]; then
		if [ "$dnsmasq_nftset" -eq 1 ] && [ "$has_fw4" -eq 1 ]; then
			USE_TABLES="nftables"
		elif [ "$has_ipset" -eq 1 ] && [ "$has_ipt" -eq 1 ] && [ "$dnsmasq_ipset" -eq 1 ]; then
			echolog "警告：nftables (fw4) 应用环境不完整，切换至 iptables。(has_fw4:$has_fw4/dnsmasq_nftset:$dnsmasq_nftset)"
			USE_TABLES="iptables"
		fi
	else
		if [ "$has_ipset" -eq 1 ] && [ "$has_ipt" -eq 1 ] && [ "$dnsmasq_ipset" -eq 1 ]; then
			USE_TABLES="iptables"
		elif [ "$dnsmasq_nftset" -eq 1 ] && [ "$has_fw4" -eq 1 ]; then
			echolog "警告：iptables (fw3) 应用环境不完整，切换至 nftables。(has_ipt:$has_ipt/has_ipset:$has_ipset/dnsmasq_ipset:$dnsmasq_ipset)"
			USE_TABLES="nftables"
		fi
	fi

	if [ -n "$USE_TABLES" ]; then
		local dep_list
		local file_path="/usr/lib/opkg/info"
		local file_ext=".control"
		[ -d "/lib/apk/packages" ] && { file_path="/lib/apk/packages"; file_ext=".list"; }

		if [ "$USE_TABLES" = "iptables" ]; then
			dep_list="iptables-mod-tproxy iptables-mod-socket iptables-mod-iprange iptables-mod-conntrack-extra kmod-ipt-nat"
		else
			dep_list="kmod-nft-socket kmod-nft-tproxy kmod-nft-nat kmod-nf-reject kmod-nf-reject6"
			nftflag=1
			local v_num=$(echo "$dnsmasq_ver" | tr -cd '0-9')
			if [ "${v_num:-0}" -lt 292 ]; then
				echolog "提示：Dnsmasq ($dnsmasq_ver) 低于 2.92，建议升级以增强稳定性。"
			fi
		fi
		local pkg
		for pkg in $dep_list; do
			if [ ! -s "${file_path}/${pkg}${file_ext}" ]; then
				echolog "警告：${USE_TABLES} 透明代理缺失基础依赖 ${pkg}！"
			fi
		done
	else
		echolog "警告：不满足任何透明代理系统环境。(has_fw4:$has_fw4/has_ipt:$has_ipt/has_ipset:$has_ipset/dnsmasq_nftset:$dnsmasq_nftset/dnsmasq_ipset:$dnsmasq_ipset)"
	fi
}

run_ipt2socks() {
	local flag proto tcp_tproxy local_port socks_address socks_port socks_username socks_password log_file
	local _extra_param=""
	eval_set_val "$@"
	[ -n "$log_file" ] || log_file="/dev/null"
	socks_address=$(get_host_ip "ipv4" ${socks_address})
	[ -n "$socks_username" ] && [ -n "$socks_password" ] && _extra_param="${_extra_param} -a $socks_username -k $socks_password"
	[ -n "$tcp_tproxy" ] || _extra_param="${_extra_param} -R"
	case "$proto" in
	UDP)
		flag="${flag}_UDP"
		_extra_param="${_extra_param} -U"
	;;
	TCP)
		flag="${flag}_TCP"
		_extra_param="${_extra_param} -T"
	;;
	*)
		flag="${flag}_TCP_UDP"
	;;
	esac
	_extra_param="${_extra_param} -o 60 -n 65535 -v"
	ln_run "$(first_type ipt2socks)" "ipt2socks_${flag}" $log_file -l $local_port -b 0.0.0.0 -s $socks_address -p $socks_port ${_extra_param}
}

run_singbox() {
	local flag type node tcp_redir_port tcp_proxy_way udp_redir_port socks_address socks_port socks_username socks_password http_address http_port http_username http_password
	local dns_listen_port direct_dns_query_strategy direct_dns_port direct_dns_udp_server direct_dns_tcp_server remote_dns_protocol remote_dns_udp_server remote_dns_tcp_server remote_dns_doh remote_dns_client_ip remote_fakedns remote_dns_query_strategy dns_cache dns_socks_address dns_socks_port
	local loglevel log_file config_file server_host server_port no_run use_proxy_list use_gfw_list chn_list
	eval_set_val "$@"
	[ -z "$type" ] && {
		type=$(echo $(config_n_get $node type) | tr 'A-Z' 'a-z')
		[ "$type" != "sing-box" ] && [ -n "$SINGBOX_BIN" ] && type="sing-box"
	}
	[ -z "$type" ] && return 1
	[ -n "$log_file" ] || local log_file="/dev/null"
	json_init
	if [ "$log_file" = "/dev/null" ]; then
		json_add_string "log" "0"
	else
		json_add_string "log" "1"
		json_add_string "logfile" "${log_file}"
	fi
	[ -z "$loglevel" ] && local loglevel=$(config_t_get global loglevel "warn")
	json_add_string "loglevel" "$loglevel"

	[ -n "$flag" ] && json_add_string "flag" "$flag"
	[ -n "$node" ] && json_add_string "node" "$node"
	[ -n "$use_proxy_list" ] && json_add_string "use_proxy_list" "$use_proxy_list"
	[ -n "$use_gfw_list" ] && json_add_string "use_gfw_list" "$use_gfw_list"
	[ -n "$chn_list" ] && json_add_string "chn_list" "$chn_list"
	[ -n "$server_host" ] && json_add_string "server_host" "$server_host"
	[ -n "$server_port" ] && json_add_string "server_port" "$server_port"
	[ -n "$tcp_redir_port" ] && json_add_string "tcp_redir_port" "$tcp_redir_port"
	[ -n "$tcp_proxy_way" ] && json_add_string "tcp_proxy_way" "$tcp_proxy_way"
	[ -n "$udp_redir_port" ] && json_add_string "udp_redir_port" "$udp_redir_port"
	[ -n "$socks_address" ] && json_add_string "local_socks_address" "$socks_address"
	[ -n "$socks_port" ] && json_add_string "local_socks_port" "$socks_port"
	[ -n "$socks_username" ] && [ -n "$socks_password" ] && {
		json_add_string "local_socks_username" "$socks_username"
		json_add_string "local_socks_password" "$socks_password"
	}
	[ -n "$http_address" ] && json_add_string "local_http_address" "$http_address"
	[ -n "$http_port" ] && json_add_string "local_http_port" "$http_port"
	[ -n "$http_username" ] && [ -n "$http_password" ] && {
		json_add_string "local_http_username" "$http_username"
		json_add_string "local_http_password" "$http_password"
	}
	[ -n "$dns_socks_address" ] && [ -n "$dns_socks_port" ] && {
		json_add_string "dns_socks_address" "${dns_socks_address}"
		json_add_string "dns_socks_port" "${dns_socks_port}"
	}
	[ -n "$dns_listen_port" ] && json_add_string "dns_listen_port" "${dns_listen_port}"
	[ -n "$dns_cache" ] && json_add_string "dns_cache" "${dns_cache}"

	if [ -n "$direct_dns_udp_server" ]; then
		direct_dns_port=$(echo ${direct_dns_udp_server} | awk -F '#' '{print $2}')
		json_add_string "direct_dns_udp_server" "$(echo ${direct_dns_udp_server} | awk -F '#' '{print $1}')"
	elif [ -n "$direct_dns_tcp_server" ]; then
		direct_dns_port=$(echo ${direct_dns_tcp_server} | awk -F '#' '{print $2}')
		json_add_string "direct_dns_tcp_server" "$(echo ${direct_dns_tcp_server} | awk -F '#' '{print $1}')"
	else
		local local_dns=$(echo -n $(echo "${LOCAL_DNS}" | sed "s/,/\n/g" | head -n1) | tr " " ",")
		json_add_string "direct_dns_udp_server" "$(echo ${local_dns} | awk -F '#' '{print $1}')"
		direct_dns_port=$(echo ${local_dns} | awk -F '#' '{print $2}')
	fi
	json_add_string "direct_dns_port" "${direct_dns_port:-53}"
	direct_dns_query_strategy=${direct_dns_query_strategy:-UseIP}
	json_add_string "direct_dns_query_strategy" "${direct_dns_query_strategy}"

	[ -n "$remote_dns_query_strategy" ] && json_add_string "remote_dns_query_strategy" "${remote_dns_query_strategy}"
	case "$remote_dns_protocol" in
		udp|tcp)
			local _proto="$remote_dns_protocol"
			local _dns=$(get_first_dns remote_dns_${_proto}_server 53)
			local _dns_address=$(echo ${_dns} | awk -F '#' '{print $1}')
			local _dns_port=$(echo ${_dns} | awk -F '#' '{print $2}')
			json_add_string "remote_dns_${_proto}_server" "${_dns_address}"
			json_add_string "remote_dns_${_proto}_port" "${_dns_port}"
		;;
		doh|http3)
			json_add_string "remote_dns_doh" "${remote_dns_doh}"
			[ "$remote_dns_protocol" = "http3" ] && json_add_string "remote_dns_http3" "1"
		;;
	esac

	[ -n "$remote_dns_client_ip" ] && json_add_string "remote_dns_client_ip" "${remote_dns_client_ip}"
	[ "$remote_fakedns" = "1" ] && json_add_string "remote_dns_fake" "1"
	[ -n "$no_run" ] && json_add_string "no_run" "1"
	local _json_arg="$(json_dump)"
	lua $UTIL_SINGBOX gen_config "${_json_arg}" > $config_file
	[ -n "$no_run" ] && return

	local test_log_file=$log_file
	[ "$test_log_file" = "/dev/null" ] && test_log_file="${TMP_PATH}/${config_file##*/}_test.log"
	$SINGBOX_BIN check -c "$config_file" > $test_log_file 2>&1; local status=$?
	if [ "${status}" = 0 ]; then
		ln_run "$SINGBOX_BIN" "sing-box" "${log_file}" run -c "$config_file"
	else
		echolog "Sing-box 配置文件 $config_file 校验有误，进程启动失败，错误信息："
		cat ${test_log_file} >> ${LOG_FILE}
	fi
	[ "$test_log_file" != "$log_file" ] && rm -f "${test_log_file}"
}

run_xray() {
	local flag type node tcp_redir_port tcp_proxy_way udp_redir_port socks_address socks_port socks_username socks_password http_address http_port http_username http_password
	local dns_listen_port direct_dns_query_strategy direct_dns_port direct_dns_udp_server direct_dns_tcp_server remote_dns_protocol remote_dns_udp_server remote_dns_tcp_server remote_dns_doh remote_dns_client_ip remote_fakedns remote_dns_query_strategy dns_cache dns_socks_address dns_socks_port
	local loglevel log_file config_file server_host server_port no_run use_proxy_list use_gfw_list chn_list
	eval_set_val "$@"
	[ -z "$type" ] && {
		type=$(echo $(config_n_get $node type) | tr 'A-Z' 'a-z')
		[ "$type" != "xray" ] && [ -n "$XRAY_BIN" ] && type="xray"
	}
	[ -z "$type" ] && return 1
	json_init
	[ -n "$log_file" ] || local log_file="/dev/null"
	[ -z "$loglevel" ] && local loglevel=$(config_t_get global loglevel "warning")
	[ -n "$flag" ] && json_add_string "flag" "$flag"
	[ -n "$node" ] && json_add_string "node" "$node"
	[ -n "$use_proxy_list" ] && json_add_string "use_proxy_list" "$use_proxy_list"
	[ -n "$use_gfw_list" ] && json_add_string "use_gfw_list" "$use_gfw_list"
	[ -n "$chn_list" ] && json_add_string "chn_list" "$chn_list"
	[ -n "$server_host" ] && json_add_string "server_host" "$server_host"
	[ -n "$server_port" ] && json_add_string "server_port" "$server_port"
	[ -n "$tcp_redir_port" ] && json_add_string "tcp_redir_port" "$tcp_redir_port"
	[ -n "$tcp_proxy_way" ] && json_add_string "tcp_proxy_way" "$tcp_proxy_way"
	[ -n "$udp_redir_port" ] && json_add_string "udp_redir_port" "$udp_redir_port"
	[ -n "$socks_address" ] && json_add_string "local_socks_address" "$socks_address"
	[ -n "$socks_port" ] && json_add_string "local_socks_port" "$socks_port"
	[ -n "$socks_username" ] && [ -n "$socks_password" ] && {
		json_add_string "local_socks_username" "$socks_username"
		json_add_string "local_socks_password" "$socks_password"
	}
	[ -n "$http_address" ] && json_add_string "local_http_address" "$http_address"
	[ -n "$http_port" ] && json_add_string "local_http_port" "$http_port"
	[ -n "$http_username" ] && [ -n "$http_password" ] && {
		json_add_string "local_http_username" "$http_username"
		json_add_string "local_http_password" "$http_password"
	}
	[ -n "$dns_socks_address" ] && [ -n "$dns_socks_port" ] && {
		json_add_string "dns_socks_address" "${dns_socks_address}"
		json_add_string "dns_socks_port" "${dns_socks_port}"
	}
	[ -n "$dns_listen_port" ] && json_add_string "dns_listen_port" "${dns_listen_port}"
	
	if [ -n "$direct_dns_udp_server" ]; then
		direct_dns_port=$(echo ${direct_dns_udp_server} | awk -F '#' '{print $2}')
		json_add_string "direct_dns_udp_server" "$(echo ${direct_dns_udp_server} | awk -F '#' '{print $1}')"
	elif [ -n "$direct_dns_tcp_server" ]; then
		direct_dns_port=$(echo ${direct_dns_tcp_server} | awk -F '#' '{print $2}')
		json_add_string "direct_dns_tcp_server" "$(echo ${direct_dns_tcp_server} | awk -F '#' '{print $1}')"
	else
		local local_dns=$(echo -n $(echo "${LOCAL_DNS}" | sed "s/,/\n/g" | head -n1) | tr " " ",")
		json_add_string "direct_dns_udp_server" "$(echo ${local_dns} | awk -F '#' '{print $1}')"
		direct_dns_port=$(echo ${local_dns} | awk -F '#' '{print $2}')
	fi
	json_add_string "direct_dns_port" "${direct_dns_port:-53}"

	direct_dns_query_strategy=${direct_dns_query_strategy:-UseIP}
	json_add_string "direct_dns_query_strategy" "${direct_dns_query_strategy}"
	[ -n "$remote_dns_query_strategy" ] && json_add_string "remote_dns_query_strategy" "${remote_dns_query_strategy}"
	[ -n "$remote_dns_client_ip" ] && json_add_string "remote_dns_client_ip" "${remote_dns_client_ip}"
	[ "$remote_fakedns" = "1" ] && json_add_string "remote_dns_fake" "1"
	[ -n "$dns_cache" ] && json_add_string "dns_cache" "${dns_cache}"

	case "$remote_dns_protocol" in
		udp|tcp)
			local _proto="$remote_dns_protocol"
			local _dns=$(get_first_dns remote_dns_${_proto}_server 53)
			local _dns_address=$(echo ${_dns} | awk -F '#' '{print $1}')
			local _dns_port=$(echo ${_dns} | awk -F '#' '{print $2}')
			json_add_string "remote_dns_${_proto}_server" "${_dns_address}"
			json_add_string "remote_dns_${_proto}_port" "${_dns_port}"
		;;
		doh)
			json_add_string "remote_dns_doh" "${remote_dns_doh}"
		;;
	esac

	json_add_string "loglevel" "$loglevel"
	[ -n "$no_run" ] && json_add_string "no_run" "1"
	local _json_arg="$(json_dump)"
	lua $UTIL_XRAY gen_config "${_json_arg}" > $config_file
	[ -n "$no_run" ] && return

	local test_log_file=$log_file
	[ "$test_log_file" = "/dev/null" ] && test_log_file="${TMP_PATH}/${config_file##*/}_test.log"
	$XRAY_BIN run -test -c "$config_file" > $test_log_file; local status=$?
	if [ "${status}" = 0 ]; then
		ln_run "$XRAY_BIN" "xray" "${log_file}" run -c "$config_file"
	else
		echolog "Xray 配置文件 $config_file 校验有误，进程启动失败，错误信息："
		cat ${test_log_file} >> ${LOG_FILE}
	fi
	[ "$test_log_file" != "$log_file" ] && rm -f "${test_log_file}"
}

run_dns2socks() {
	local flag socks socks_address socks_port socks_username socks_password listen_address listen_port dns cache log_file
	local _extra_param=""
	eval_set_val "$@"
	[ -n "$flag" ] && flag="_${flag}"
	[ -n "$log_file" ] || log_file="/dev/null"
	dns=$(get_first_dns dns 53 | sed 's/#/:/g')
	[ -n "$socks" ] && {
		socks="${socks//#/:}"
		socks_address=$(echo $socks | awk -F ':' '{print $1}')
		socks_port=$(echo $socks | awk -F ':' '{print $2}')
	}
	[ -n "$socks_username" ] && [ -n "$socks_password" ] && _extra_param="${_extra_param} /u $socks_username /p $socks_password"
	[ -z "$cache" ] && cache=1
	[ "$cache" = "0" ] && _extra_param="${_extra_param} /d"
	ln_run "$(first_type dns2socks)" "dns2socks${flag}" $log_file ${_extra_param} "${socks_address}:${socks_port}" "${dns}" "${listen_address}:${listen_port}"
}

run_chinadns_ng() {
	local _flag _listen_port _dns_local _dns_trust _no_ipv6_trust _use_direct_list _use_proxy_list _gfwlist _chnlist _default_mode _default_tag _no_logic_log _tcp_node _filter_https _log
	local _extra_param=""
	eval_set_val "$@"

	local _CONF_FILE=$TMP_ACL_PATH/$_flag/chinadns_ng.conf
	local _LOG_FILE="/dev/null"
	[ "${_log}" = "1" ] && _LOG_FILE=$TMP_ACL_PATH/$_flag/chinadns_ng.log

	_extra_param="-FLAG ${_flag} -TCP_NODE ${_tcp_node} -LISTEN_PORT ${_listen_port} -DNS_LOCAL ${_dns_local} -DNS_TRUST ${_dns_trust}"
	_extra_param="${_extra_param} -USE_DIRECT_LIST ${_use_direct_list} -USE_PROXY_LIST ${_use_proxy_list} -USE_BLOCK_LIST ${_use_block_list}"
	_extra_param="${_extra_param} -GFWLIST ${_gfwlist} -CHNLIST ${_chnlist} -NO_IPV6_TRUST ${_no_ipv6_trust} -DEFAULT_MODE ${_default_mode}"
	_extra_param="${_extra_param} -DEFAULT_TAG ${_default_tag} -NFTFLAG ${nftflag} -NO_LOGIC_LOG ${_no_logic_log}"
	_extra_param="${_extra_param} -FILTER_HTTPS ${_filter_https} -LOG_FILE ${_LOG_FILE}"

	lua $APP_PATH/helper_chinadns_add.lua ${_extra_param} > ${_CONF_FILE}
	ln_run "$(first_type chinadns-ng)" chinadns-ng "${_LOG_FILE}" -C ${_CONF_FILE}
}

run_socks() {
	local flag node bind socks_port config_file http_port http_config_file relay_port log_file no_run
	eval_set_val "$@"
	[ -n "$config_file" ] && [ -z "$(echo ${config_file} | grep $TMP_PATH)" ] && config_file=$TMP_PATH/$config_file
	[ -n "$http_port" ] || http_port=0
	[ -n "$http_config_file" ] && [ -z "$(echo ${http_config_file} | grep $TMP_PATH)" ] && http_config_file=$TMP_PATH/$http_config_file
	if [ -n "$log_file" ] && [ -z "$(echo ${log_file} | grep $TMP_PATH)" ]; then
		log_file=$TMP_PATH/$log_file
	else
		log_file="/dev/null"
	fi

	local node2socks_port=0
	local type remarks server_host server_port
	if [ "$(config_get_type $node)" = "socks" ]; then
		node2socks_port=$(config_n_get $node port 0)
	fi
	if [ "$node2socks_port" = "0" ]; then
		type=$(echo $(config_n_get $node type) | tr 'A-Z' 'a-z')
		remarks=$(config_n_get $node remarks)
		server_host=$(config_n_get $node address)
		server_port=$(config_n_get $node port)
	else
		type="socks"
		server_host="127.0.0.1"
		server_port=$node2socks_port
		remarks="Socks 配置($server_port 端口)"
	fi

	[ -n "$relay_port" ] && {
		server_host="127.0.0.1"
		server_port=$relay_port
	}
	local error_msg tmp

	if [ -n "$server_host" ] && [ -n "$server_port" ]; then
		check_host $server_host
		[ $? != 0 ] && {
			echolog "  - Socks节点：[$remarks]${server_host} 是非法的服务器地址，无法启动！"
			return 1
		}
		tmp="${server_host}:${server_port}"
	else
		error_msg="某种原因，此 Socks 服务的相关配置已失联，启动中止！"
	fi

	if [ "$type" = "sing-box" ] || [ "$type" = "xray" ]; then
		local protocol=$(config_n_get $node protocol)
		if [ "$protocol" = "_balancing" ] || [ "$protocol" = "_shunt" ] || [ "$protocol" = "_iface" ] || [ "$protocol" = "_urltest" ]; then
			unset error_msg
		fi
	fi

	if [ -n "${error_msg}" ] && { [ -n "$(config_n_get $node hysteria_hop)" ] || [ -n "$(config_n_get $node hysteria2_hop)" ] || [ "$(config_n_get $node hysteria2_realms)" = "1" ]; }; then
		unset error_msg
	fi

	[ -n "${error_msg}" ] && {
		[ "$bind" != "127.0.0.1" ] && echolog "  - Socks节点：[$remarks]${tmp}，启动中止 ${bind}:${socks_port} ${error_msg}"
		return 1
	}
	[ "$bind" != "127.0.0.1" ] && echolog "  - Socks节点：[$remarks]${tmp}，启动 ${bind}:${socks_port}"

	json_init
	json_add_string "node" "${node}"
	json_add_string "server_host" "${server_host}"
	json_add_string "server_port" "${server_port}"
	case "$type" in
	socks)
		local _socks_address _socks_port _socks_username _socks_password
		if [ "$node2socks_port" = "0" ]; then
			_socks_address=$(config_n_get $node address)
			_socks_port=$(config_n_get $node port)
			_socks_username=$(config_n_get $node username)
			_socks_password=$(config_n_get $node password)
		else
			_socks_address="127.0.0.1"
			_socks_port=$node2socks_port
		fi
		[ "$http_port" != "0" ] && {
			http_flag=1
			config_file="${config_file%%.*}+http${config_file#${config_file%%.*}}"
			json_add_string "local_http_address" "$bind"
			json_add_string "local_http_port" "$http_port"
		}
		json_add_null "server_host"
		json_add_null "server_port"
		json_add_string "local_socks_address" "$bind"
		json_add_string "local_socks_port" "$socks_port"
		json_add_string "server_proto" "socks"
		json_add_string "server_address" "${_socks_address}"
		json_add_string "server_port" "${_socks_port}"
		json_add_string "server_username" "${_socks_username}"
		json_add_string "server_password" "${_socks_password}"
		if [ -n "${SINGBOX_BIN}" ]; then
			type="sing-box"
			local bin="${SINGBOX_BIN}"
			local util="${UTIL_SINGBOX}"
		elif [ -n "${XRAY_BIN}" ]; then
			type="xray"
			local bin="${XRAY_BIN}"
			local util="${UTIL_XRAY}"
		fi
		[ -n "${bin}" ] && [ -n "${util}" ] && {
			lua ${util} gen_proto_config "$(json_dump)" > $config_file
			[ -n "$no_run" ] || ln_run "$bin" $type $log_file run -c "$config_file"
		}
		unset bin util
	;;
	sing-box)
		[ "$http_port" != "0" ] && {
			http_flag=1
			config_file="${config_file%%.*}+http${config_file#${config_file%%.*}}"
			local _args="http_address=$bind http_port=$http_port"
		}
		[ -n "$relay_port" ] && _args="${_args} server_host=$server_host server_port=$server_port"
		[ -n "$no_run" ] && _args="${_args} no_run=1"
		run_singbox flag=$flag node=$node socks_address=$bind socks_port=$socks_port config_file=$config_file log_file=$log_file ${_args}
	;;
	xray)
		[ "$http_port" != "0" ] && {
			http_flag=1
			config_file="${config_file%%.*}+http${config_file#${config_file%%.*}}"
			local _args="http_address=$bind http_port=$http_port"
		}
		[ -n "$relay_port" ] && _args="${_args} server_host=$server_host server_port=$server_port"
		[ -n "$no_run" ] && _args="${_args} no_run=1"
		run_xray flag=$flag node=$node socks_address=$bind socks_port=$socks_port config_file=$config_file log_file=$log_file ${_args}
	;;
	naiveproxy)
		json_add_string "run_type" "socks"
		json_add_string "local_addr" "$bind"
		json_add_string "local_port" "$socks_port"
		lua $UTIL_NAIVE gen_config "$(json_dump)" > $config_file
		[ -n "$no_run" ] || ln_run "$(first_type naive)" naive $log_file "$config_file"
	;;
	ssr)
		json_add_string "local_addr" "$bind"
		json_add_string "local_port" "$socks_port"
		lua $UTIL_SS gen_config "$(json_dump)" > $config_file
		[ -n "$no_run" ] || ln_run "$(first_type ssr-local)" "ssr-local" $log_file -c "$config_file" -v -u
	;;
	ss-rust)
		[ "$http_port" != "0" ] && {
			http_flag=1
			config_file="${config_file%%.*}+http${config_file#${config_file%%.*}}"
			json_add_string "local_http_address" "$bind"
			json_add_string "local_http_port" "$http_port"
		}
		[ -n "$no_run" ] || {
			local plugin_sh="${config_file%.json}_plugin.sh"
			json_add_string "plugin_sh" "$plugin_sh"
		}
		json_add_string "local_socks_address" "$bind"
		json_add_string "local_socks_port" "$socks_port"
		lua $UTIL_SS gen_config "$(json_dump)" > $config_file
		[ -n "$no_run" ] || ln_run "$(first_type sslocal)" "sslocal" $log_file -c "$config_file" -v
	;;
	hysteria2)
		[ "$http_port" != "0" ] && {
			http_flag=1
			config_file="${config_file%%.*}+http${config_file#${config_file%%.*}}"
			json_add_string "local_http_address" "$bind"
			json_add_string "local_http_port" "$http_port"
		}
		json_add_string "local_socks_address" "$bind"
		json_add_string "local_socks_port" "$socks_port"
		lua $UTIL_HYSTERIA2 gen_config "$(json_dump)" > $config_file
		[ -n "$no_run" ] || ln_run "$(first_type $(config_t_get global_app hysteria_file))" "hysteria" $log_file -c "$config_file" client
	;;
	esac

	set_cache_var "node_${node}_socks_port" "${socks_port}"

	# http to socks
	[ -z "$http_flag" ] && [ "$http_port" != "0" ] && [ -n "$http_config_file" ] && [ "$type" != "sing-box" ] && [ "$type" != "xray" ] && [ "$type" != "socks" ] && {
		json_init
		json_add_string "local_http_address" "$bind"
		json_add_string "local_http_port" "$http_port"
		json_add_string "server_proto" "socks"
		json_add_string "server_address" "127.0.0.1"
		json_add_string "server_port" "$socks_port"
		json_add_string "server_username" "$_username"
		json_add_string "server_password" "$_password"
		if [ -n "${SINGBOX_BIN}" ]; then
			type="sing-box"
			local bin="${SINGBOX_BIN}"
			local util="${UTIL_SINGBOX}"
		elif [ -n "${XRAY_BIN}" ]; then
			type="xray"
			local bin="${XRAY_BIN}"
			local util="${UTIL_XRAY}"
		fi
		[ -n "${bin}" ] && [ -n "${util}" ] && {
			lua ${util} gen_proto_config "$(json_dump)" > $http_config_file
			[ -n "$no_run" ] || ln_run "$bin" $type /dev/null run -c "$http_config_file"
		}
		unset bin util
	}
	unset http_flag

	[ -z "$no_run" ] && [ "${server_host}" != "127.0.0.1" ] && [ "$type" != "sing-box" ] && [ "$type" != "xray" ] && echo "${node}" >> $TMP_PATH/direct_node_list
}

run_redir() {
	local node proto bind local_port config_file log_file
	eval_set_val "$@"
	local tcp_node_socks_flag tcp_node_http_flag
	[ -n "$config_file" ] && [ -z "$(echo ${config_file} | grep $TMP_PATH)" ] && config_file=${GLOBAL_ACL_PATH}/${config_file}
	if [ -n "$log_file" ] && [ -z "$(echo ${log_file} | grep $TMP_PATH)" ]; then
		log_file=${GLOBAL_ACL_PATH}/${log_file}
	else
		log_file="/dev/null"
	fi
	local proto=$(echo $proto | tr 'A-Z' 'a-z')
	local PROTO=$(echo $proto | tr 'a-z' 'A-Z')

	local node2socks_port=0
	local type remarks server_host port
	if [ "$(config_get_type $node)" = "socks" ]; then
		node2socks_port=$(config_n_get $node port 0)
	fi
	if [ "$node2socks_port" = "0" ]; then
		type=$(echo $(config_n_get $node type) | tr 'A-Z' 'a-z')
		remarks=$(config_n_get $node remarks)
		server_host=$(config_n_get $node address)
		port=$(config_n_get $node port)
	else
		if [ "${DNS_MODE}" = "xray" ]; then
			type="xray"
		elif [ "${DNS_MODE}" = "sing-box" ]; then
			type="sing-box"
		else
			type="socks"
		fi
		server_host="127.0.0.1"
		port=$node2socks_port
		remarks="Socks 配置($port 端口)"
	fi

	local enable_log=$(config_t_get global log_${proto} 1)
	[ "$enable_log" != "1" ] && log_file="/dev/null"
	[ -n "$server_host" ] && [ -n "$port" ] && {
		check_host $server_host
		[ $? != 0 ] && {
			echolog "${PROTO}节点：[$remarks]${server_host} 是非法的服务器地址，无法启动！"
			return 1
		}
	}
	[ "$bind" != "127.0.0.1" ] && echolog "${PROTO}节点：[$remarks]，监听端口：$local_port"

	json_init
	json_add_string "node" "${node}"
	case "$PROTO" in
	UDP)
		case "$type" in
		socks)
			local _socks_address _socks_port _socks_username _socks_password
			if [ "$node2socks_port" = "0" ]; then
				_socks_address=$(config_n_get $node address)
				_socks_port=$(config_n_get $node port)
				_socks_username=$(config_n_get $node username)
				_socks_password=$(config_n_get $node password)
			else
				_socks_address="127.0.0.1"
				_socks_port=$node2socks_port
			fi
			run_ipt2socks flag=default proto=UDP local_port=${local_port} socks_address=${_socks_address} socks_port=${_socks_port} socks_username=${_socks_username} socks_password=${_socks_password} log_file=${log_file}
		;;
		sing-box)
			local protocol=$(config_n_get $node protocol)
			[ "$protocol" = "_shunt" ] && {
				local geoip_path="${V2RAY_LOCATION_ASSET%*/}/geoip.dat"
				local geosite_path="${V2RAY_LOCATION_ASSET%*/}/geosite.dat"
				if [ ! -s "$geoip_path" ] || [ ! -s "$geosite_path" ]; then
					echolog "* 缺少Geo规则文件，UDP Sing-Box分流节点无法正常使用！"
				fi
			}
			run_singbox flag=UDP node=$node udp_redir_port=$local_port config_file=$config_file log_file=$log_file
		;;
		xray)
			local protocol=$(config_n_get $node protocol)
			[ "$protocol" = "_shunt" ] && {
				local geoip_path="${V2RAY_LOCATION_ASSET%*/}/geoip.dat"
				local geosite_path="${V2RAY_LOCATION_ASSET%*/}/geosite.dat"
				if [ ! -s "$geoip_path" ] || [ ! -s "$geosite_path" ]; then
					echolog "* 缺少Geo规则文件，UDP Xray分流节点无法正常使用！"
				fi
			}
			run_xray flag=UDP node=$node udp_redir_port=$local_port config_file=$config_file log_file=$log_file
		;;
		naiveproxy)
			echolog "Naiveproxy不支持UDP转发！"
		;;
		ssr)
			json_add_string "local_addr" "0.0.0.0"
			json_add_string "local_port" "$local_port"
			lua $UTIL_SS gen_config "$(json_dump)" > $config_file
			ln_run "$(first_type ssr-redir)" "ssr-redir" $log_file -c "$config_file" -v -U
		;;
		ss-rust)
			local plugin_sh="${config_file%.json}_plugin.sh"
			json_add_string "plugin_sh" "$plugin_sh"
			json_add_string "local_udp_redir_port" "$local_port"
			lua $UTIL_SS gen_config "$(json_dump)" > $config_file
			ln_run "$(first_type sslocal)" "sslocal" $log_file -c "$config_file" -v
		;;
		hysteria2)
			json_add_string "local_udp_redir_port" "$local_port"
			lua $UTIL_HYSTERIA2 gen_config "$(json_dump)" > $config_file
			ln_run "$(first_type $(config_t_get global_app hysteria_file))" "hysteria" $log_file -c "$config_file" client
		;;
		esac
	;;
	TCP)
		[ "$TCP_UDP" = "1" ] && echolog "UDP节点：与TCP节点相同"
		tcp_node_socks=1
		tcp_node_socks_bind_local=$(config_t_get global tcp_node_socks_bind_local 1)
		tcp_node_socks_bind="127.0.0.1"
		[ "${tcp_node_socks_bind_local}" != "1" ] && tcp_node_socks_bind="0.0.0.0"
		tcp_node_socks_port=$(get_new_port $(config_t_get global tcp_node_socks_port 1070))
		tcp_node_http_port=$(config_t_get global tcp_node_http_port 0)
		[ "$tcp_node_http_port" != "0" ] && tcp_node_http=1
		if [ $PROXY_IPV6 = "1" ]; then
			echolog "开启实验性IPv6透明代理(TProxy)，请确认您的节点及类型支持IPv6！"
		fi

		if [ "${TCP_PROXY_WAY}" = "redirect" ]; then
			can_ipt=$(echo "$REDIRECT_LIST" | grep "$type")
		elif [ "${TCP_PROXY_WAY}" = "tproxy" ]; then
			can_ipt=$(echo "$TPROXY_LIST" | grep "$type")
		fi
		[ -z "$can_ipt" ] && type="socks"

		case "$type" in
		socks)
			_socks_flag=1
			if [ "$node2socks_port" = "0" ]; then
				_socks_address=$(config_n_get $node address)
				_socks_port=$(config_n_get $node port)
				_socks_username=$(config_n_get $node username)
				_socks_password=$(config_n_get $node password)
			else
				_socks_address="127.0.0.1"
				_socks_port=$node2socks_port
			fi
			[ -z "$can_ipt" ] && {
				local _config_file=$config_file
				_config_file="TCP_SOCKS_${node}.json"
				local _port=$(get_new_port 3001)
				run_socks flag="TCP" node=$node bind=127.0.0.1 socks_port=${_port} config_file=${_config_file}
				_socks_address=127.0.0.1
				_socks_port=${_port}
				unset _socks_username
				unset _socks_password
			}
		;;
		sing-box)
			local _flag="TCP"
			local _args=""
			[ "$tcp_node_socks" = "1" ] && {
				tcp_node_socks_flag=1
				_args="${_args} socks_address=${tcp_node_socks_bind} socks_port=${tcp_node_socks_port}"
				config_file="${config_file//TCP/TCP_SOCKS}"
			}
			[ "$tcp_node_http" = "1" ] && {
				tcp_node_http_flag=1
				_args="${_args} http_port=${tcp_node_http_port}"
				config_file="${config_file//TCP/TCP_HTTP}"
			}
			[ "$TCP_UDP" = "1" ] && {
				UDP_REDIR_PORT=$local_port
				unset UDP_NODE
				_flag="TCP_UDP"
				_args="${_args} udp_redir_port=${UDP_REDIR_PORT}"
				config_file="${config_file//TCP/TCP_UDP}"
			}

			local protocol=$(config_n_get $node protocol)
			local v2ray_dns_mode=$(config_t_get global v2ray_dns_mode tcp)
			[ "${DNS_MODE}" != "sing-box" ] && [ "$protocol" = "_shunt" ] && {
				DNS_MODE="sing-box"
				[ "$v2ray_dns_mode" = "tcp+doh" ] && v2ray_dns_mode="tcp"
			}

			[ "$protocol" = "_shunt" ] && {
				local geoip_path="${V2RAY_LOCATION_ASSET%*/}/geoip.dat"
				local geosite_path="${V2RAY_LOCATION_ASSET%*/}/geosite.dat"
				if [ ! -s "$geoip_path" ] || [ ! -s "$geosite_path" ]; then
					echolog "* 缺少Geo规则文件，TCP Sing-Box分流节点无法正常使用！"
				fi
				[ "$(config_n_get $node fakedns)" = "1" ] && {
					USE_FAKEDNS=1
					GLOBAL_SHUNT_NODE_FAKEDNS=1
				}
			}

			[ "${DNS_MODE}" = "sing-box" ] && {
				NO_PLUGIN_DNS=1
				config_file="${config_file//.json/_DNS.json}"
				_args="${_args} remote_dns_query_strategy=${REMOTE_DNS_QUERY_STRATEGY}"
				DNSMASQ_FILTER_PROXY_IPV6=0
				local _remote_dns_client_ip=$(config_t_get global remote_dns_client_ip)
				[ -n "${_remote_dns_client_ip}" ] && _args="${_args} remote_dns_client_ip=${_remote_dns_client_ip}"
				[ "${DNS_CACHE}" = "0" ] && _args="${_args} dns_cache=0"
				resolve_dns_port=${NEXT_DNS_LISTEN_PORT}
				_args="${_args} dns_listen_port=${resolve_dns_port}"

				case "$(config_t_get global direct_dns_mode "auto")" in
					udp)
						_args="${_args} direct_dns_udp_server=$(normalize_dns "$(config_t_get global direct_dns 223.5.5.5:53)")"
					;;
					tcp)
						_args="${_args} direct_dns_tcp_server=$(normalize_dns "$(config_t_get global direct_dns 223.5.5.5:53)")"
					;;
				esac

				_args="${_args} remote_dns_protocol=${v2ray_dns_mode}"
				case "$v2ray_dns_mode" in
					udp|tcp)
						local _proto="$v2ray_dns_mode"
						_args="${_args} remote_dns_${_proto}_server=${REMOTE_DNS}"
						resolve_dns_log="Sing-Box DNS(127.0.0.1#${resolve_dns_port}) -> ${_proto}://${REMOTE_DNS}"
					;;
					doh|http3)
						local remote_dns_doh=$(config_t_get global remote_dns_doh "https://1.1.1.1/dns-query")
						_args="${_args} remote_dns_doh=${remote_dns_doh}"
						resolve_dns_log="Sing-Box DNS(127.0.0.1#${resolve_dns_port}) -> ${remote_dns_doh}"
					;;
				esac
				local remote_fakedns=$(config_t_get global remote_fakedns 0)
				[ "${remote_fakedns}" = "1" ] && {
					REMOTE_FAKEDNS=1
					_args="${_args} remote_fakedns=1"
					resolve_dns_log="${resolve_dns_log} + FakeDNS"
					USE_FAKEDNS=1
				}
				NEXT_DNS_LISTEN_PORT=$(expr $NEXT_DNS_LISTEN_PORT + 1)
			}
			_args="${_args} use_proxy_list=$USE_PROXY_LIST use_gfw_list=$USE_GFW_LIST chn_list=$CHN_LIST"
			run_singbox flag=$_flag node=$node tcp_redir_port=$local_port tcp_proxy_way=$TCP_PROXY_WAY config_file=$config_file log_file=$log_file ${_args}
		;;
		xray)
			local _flag="TCP"
			local _args=""
			[ "$tcp_node_socks" = "1" ] && {
				tcp_node_socks_flag=1
				_args="${_args} socks_address=${tcp_node_socks_bind} socks_port=${tcp_node_socks_port}"
				config_file="${config_file//TCP/TCP_SOCKS}"
			}
			[ "$tcp_node_http" = "1" ] && {
				tcp_node_http_flag=1
				_args="${_args} http_port=${tcp_node_http_port}"
				config_file="${config_file//TCP/TCP_HTTP}"
			}
			[ "$TCP_UDP" = "1" ] && {
				UDP_REDIR_PORT=$local_port
				unset UDP_NODE
				_flag="TCP_UDP"
				_args="${_args} udp_redir_port=${UDP_REDIR_PORT}"
				config_file="${config_file//TCP/TCP_UDP}"
			}

			local protocol=$(config_n_get $node protocol)
			local v2ray_dns_mode=$(config_t_get global v2ray_dns_mode tcp)
			[ "${DNS_MODE}" != "xray" ] && [ "$protocol" = "_shunt" ] && {
				DNS_MODE="xray"
				[ "$v2ray_dns_mode" = "http3" ] && v2ray_dns_mode="tcp"
			}
			#兼容旧模式，择机移除
			[ "$v2ray_dns_mode" = "tcp+doh" ] && v2ray_dns_mode="tcp"

			[ "$protocol" = "_shunt" ] && {
				local geoip_path="${V2RAY_LOCATION_ASSET%*/}/geoip.dat"
				local geosite_path="${V2RAY_LOCATION_ASSET%*/}/geosite.dat"
				if [ ! -s "$geoip_path" ] || [ ! -s "$geosite_path" ]; then
					echolog "* 缺少Geo规则文件，TCP Xray分流节点无法正常使用！"
				fi
				[ "$(config_n_get $node fakedns)" = "1" ] && {
					USE_FAKEDNS=1
					GLOBAL_SHUNT_NODE_FAKEDNS=1
				}
			}

			[ "${DNS_MODE}" = "xray" ] && {
				NO_PLUGIN_DNS=1
				config_file="${config_file//.json/_DNS.json}"
				_args="${_args} remote_dns_query_strategy=${REMOTE_DNS_QUERY_STRATEGY}"
				DNSMASQ_FILTER_PROXY_IPV6=0
				local _remote_dns_client_ip=$(config_t_get global remote_dns_client_ip)
				[ -n "${_remote_dns_client_ip}" ] && _args="${_args} remote_dns_client_ip=${_remote_dns_client_ip}"
				[ "${DNS_CACHE}" = "0" ] && _args="${_args} dns_cache=0"
				resolve_dns_port=${NEXT_DNS_LISTEN_PORT}
				_args="${_args} dns_listen_port=${resolve_dns_port}"

				case "$(config_t_get global direct_dns_mode "auto")" in
					udp)
						_args="${_args} direct_dns_udp_server=$(normalize_dns "$(config_t_get global direct_dns 223.5.5.5:53)")"
					;;
					tcp)
						_args="${_args} direct_dns_tcp_server=$(normalize_dns "$(config_t_get global direct_dns 223.5.5.5:53)")"
					;;
				esac

				_args="${_args} remote_dns_protocol=${v2ray_dns_mode}"
				case "$v2ray_dns_mode" in
					udp|tcp)
						local _proto="$v2ray_dns_mode"
						_args="${_args} remote_dns_${_proto}_server=${REMOTE_DNS}"
						resolve_dns_log="Xray DNS(127.0.0.1#${resolve_dns_port}) -> ${_proto}://${REMOTE_DNS}"
					;;
					doh)
						local remote_dns_doh=$(config_t_get global remote_dns_doh "https://1.1.1.1/dns-query")
						_args="${_args} remote_dns_doh=${remote_dns_doh}"
						resolve_dns_log="Xray DNS(127.0.0.1#${resolve_dns_port}) -> ${remote_dns_doh}"
					;;
				esac
				local remote_fakedns=$(config_t_get global remote_fakedns 0)
				[ "${remote_fakedns}" = "1" ] && {
					REMOTE_FAKEDNS=1
					_args="${_args} remote_fakedns=1"
					resolve_dns_log="${resolve_dns_log} + FakeDNS"
					USE_FAKEDNS=1
				}
				NEXT_DNS_LISTEN_PORT=$(expr $NEXT_DNS_LISTEN_PORT + 1)
			}
			_args="${_args} use_proxy_list=$USE_PROXY_LIST use_gfw_list=$USE_GFW_LIST chn_list=$CHN_LIST"
			run_xray flag=$_flag node=$node tcp_redir_port=$local_port tcp_proxy_way=$TCP_PROXY_WAY config_file=$config_file log_file=$log_file ${_args}
		;;
		naiveproxy)
			json_add_string "run_type" "redir"
			json_add_string "local_addr" "0.0.0.0"
			json_add_string "local_port" "$local_port"
			lua $UTIL_NAIVE gen_config "$(json_dump)" > $config_file
			ln_run "$(first_type naive)" naive $log_file "$config_file"
		;;
		ssr)
			[ "${TCP_PROXY_WAY}" = "tproxy" ] && json_add_string "tcp_tproxy" "true"
			local _extra_param
			[ "$TCP_UDP" = "1" ] && {
				config_file="${config_file//TCP/TCP_UDP}"
				UDP_REDIR_PORT=$TCP_REDIR_PORT
				unset UDP_NODE
				_extra_param="-u"
			}
			json_add_string "local_addr" "0.0.0.0"
			json_add_string "local_port" "$local_port"
			lua $UTIL_SS gen_config "$(json_dump)" > $config_file
			ln_run "$(first_type ssr-redir)" "ssr-redir" $log_file -c "$config_file" -v ${_extra_param}
		;;
		ss-rust)
			json_add_string "local_tcp_redir_port" "$local_port"
			[ "${TCP_PROXY_WAY}" = "tproxy" ] && json_add_string "tcp_tproxy" "true"
			[ "$tcp_node_socks" = "1" ] && {
				tcp_node_socks_flag=1
				config_file="${config_file//TCP/TCP_SOCKS}"
				json_add_string "local_socks_address" "${tcp_node_socks_bind}"
				json_add_string "local_socks_port" "${tcp_node_socks_port}"
			}
			[ "$tcp_node_http" = "1" ] && {
				tcp_node_http_flag=1
				config_file="${config_file//TCP/TCP_HTTP}"
				json_add_string "local_http_port" "${tcp_node_http_port}"
			}
			[ "$TCP_UDP" = "1" ] && {
				config_file="${config_file//TCP/TCP_UDP}"
				UDP_REDIR_PORT=$TCP_REDIR_PORT
				unset UDP_NODE
				json_add_string "local_udp_redir_port" "$local_port"
			}
			local plugin_sh="${config_file%.json}_plugin.sh"
			json_add_string "plugin_sh" "$plugin_sh"
			lua $UTIL_SS gen_config "$(json_dump)" > $config_file
			ln_run "$(first_type sslocal)" "sslocal" $log_file -c "$config_file" -v
		;;
		hysteria2)
			json_add_string "local_tcp_redir_port" "$local_port"
			[ "$tcp_node_socks" = "1" ] && {
				tcp_node_socks_flag=1
				config_file="${config_file//TCP/TCP_SOCKS}"
				json_add_string "local_socks_address" "${tcp_node_socks_bind}"
				json_add_string "local_socks_port" "${tcp_node_socks_port}"
			}
			[ "$tcp_node_http" = "1" ] && {
				tcp_node_http_flag=1
				config_file="${config_file//TCP/TCP_HTTP}"
				json_add_string "local_http_port" "${tcp_node_http_port}"
			}
			[ "$TCP_UDP" = "1" ] && {
				config_file="${config_file//TCP/TCP_UDP}"
				UDP_REDIR_PORT=$TCP_REDIR_PORT
				unset UDP_NODE
				json_add_string "local_udp_redir_port" "$local_port"
			}
			json_add_string "tcp_proxy_way" "${TCP_PROXY_WAY}"
			lua $UTIL_HYSTERIA2 gen_config "$(json_dump)" > $config_file
			ln_run "$(first_type $(config_t_get global_app hysteria_file))" "hysteria" $log_file -c "$config_file" client
		;;
		esac
		if [ -n "${_socks_flag}" ]; then
			local _flag="TCP"
			[ "$TCP_UDP" = "1" ] && {
				_flag="TCP_UDP"
				UDP_REDIR_PORT=$TCP_REDIR_PORT
				unset UDP_NODE
			}
			local _socks_tproxy=""
			[ "${TCP_PROXY_WAY}" = "tproxy" ] && _socks_tproxy="1"
			run_ipt2socks flag=default proto=${_flag} tcp_tproxy=${_socks_tproxy} local_port=${local_port} socks_address=${_socks_address} socks_port=${_socks_port} socks_username=${_socks_username} socks_password=${_socks_password} log_file=${log_file}
		fi

		[ -z "$tcp_node_socks_flag" ] && {
			[ "$tcp_node_socks" = "1" ] && {
				local config_file="SOCKS_TCP.json"
				local log_file="SOCKS_TCP.log"
				local http_port=0
				local http_config_file="HTTP2SOCKS_TCP.json"
				[ "$tcp_node_http" = "1" ] && [ -z "$tcp_node_http_flag" ] && {
					http_port=$tcp_node_http_port
				}
				run_socks flag=TCP node=$node bind=$tcp_node_socks_bind socks_port=$tcp_node_socks_port config_file=$config_file http_port=$http_port http_config_file=$http_config_file
			}
		}

		[ "$tcp_node_socks" = "1" ] && {
			TCP_SOCKS_server="127.0.0.1:$tcp_node_socks_port"
			set_cache_var "GLOBAL_TCP_SOCKS_server" "${TCP_SOCKS_server}"
		}
	;;
	esac
	unset tcp_node_socks_flag tcp_node_http_flag
	[ "$type" != "sing-box" ] && [ "$type" != "xray" ] && echo "${node}" >> $TMP_PATH/direct_node_list
	[ -n "${redir_port}" ] && set_cache_var "node_${node}_${PROTO}_redir_port" "${local_port}"
}

start_redir() {
	local proto=${1}
	eval node=\$${proto}_NODE
	if [ -n "$node" ]; then
		TYPE=$(echo $(config_n_get $node type) | tr 'A-Z' 'a-z')
		local config_file="${proto}.json"
		local log_file="${proto}.log"
		eval current_port=\$${proto}_REDIR_PORT
		local port=$(echo $(get_new_port $current_port $proto))
		eval ${proto}_REDIR_PORT=$port
		run_redir node=$node proto=${proto} bind=0.0.0.0 local_port=$port config_file=$config_file log_file=$log_file
		set_cache_var "ACL_GLOBAL_${proto}_node" "${node}"
		set_cache_var "ACL_GLOBAL_${proto}_redir_port" "${port}"
		[ "$TCP_UDP" = "1" ] && {
			set_cache_var "ACL_GLOBAL_UDP_node" "${node}"
			set_cache_var "ACL_GLOBAL_UDP_redir_port" "${port}"
		}
	else
		[ "${proto}" = "UDP" ] && [ "$TCP_UDP" = "1" ] && return
		echolog "${proto}节点没有选择或为空，不代理${proto}。"
	fi
}

start_socks() {
	[ "$SOCKS_ENABLED" = "1" ] && {
		local ids=$(uci show $CONFIG | grep "=socks" | awk -F '.' '{print $2}' | awk -F '=' '{print $1}')
		[ -n "$ids" ] && {
			echolog "分析 Socks 服务的节点配置..."
			for id in $ids; do
				local enabled=$(config_n_get $id enabled 0)
				[ "$enabled" = "0" ] && continue
				local node=$(config_n_get $id node)
				[ -z "$node" ] && continue
				local bind_local=$(config_n_get $id bind_local 0)
				local bind="0.0.0.0"
				[ "$bind_local" = "1" ] && bind="127.0.0.1"
				local port=$(config_n_get $id port)
				local config_file="${id}.json"
				local log_file="${id}.log"
				local log=$(config_n_get $id log 1)
				[ "$log" = "0" ] && log_file=""
				local http_port=$(config_n_get $id http_port 0)
				local http_config_file="${flag}_http.json"
				local enable_autoswitch=$(config_n_get $id enable_autoswitch 0)
				local no_rec=0
				[ "$enable_autoswitch" = "1" ] && no_rec=1
				NO_REC_PROCESS=$no_rec $APP_PATH/app.sh run_socks flag=$id node=$node bind=$bind socks_port=$port config_file=$config_file http_port=$http_port http_config_file=$http_config_file log_file=$log_file
				set_cache_var "${id}" "$node"
				#自动切换逻辑
				[ "$enable_autoswitch" = "1" ] && { $APP_PATH/socks_auto_switch.sh ${id} > /dev/null 2>&1 & }
			done
		}
	}
}

socks_node_switch() {
	local flag new_node
	eval_set_val "$@"
	[ -n "$flag" ] && [ -n "$new_node" ] && {
		local suffix pf filename
		# 结束 SS 插件进程
		for suffix in "" "+http"; do
			pf="$TMP_PATH/${flag}${suffix}_plugin.pid"
			[ -s "$pf" ] && kill -9 "$(head -n1 "$pf")" >/dev/null 2>&1
		done

		busybox pgrep -af "$TMP_BIN_PATH" | awk -v P1="${flag}" 'BEGIN{IGNORECASE=1}$0~P1 && !/acl\/|acl_/{print $1}' | xargs kill -9 >/dev/null 2>&1
		for suffix in "" "+http" "_http"; do
			rm -rf "$TMP_PATH/${flag}${suffix}"*
		done

		for filename in $(ls ${TMP_SCRIPT_FUNC_PATH}); do
			grep -q "${flag}" "${TMP_SCRIPT_FUNC_PATH}/${filename}" && \
				rm -f "${TMP_SCRIPT_FUNC_PATH}/${filename}" "${TMP_PATH}/script_rstats/${filename}.count"
		done
		local bind_local=$(config_n_get $flag bind_local 0)
		local bind="0.0.0.0"
		[ "$bind_local" = "1" ] && bind="127.0.0.1"
		local port=$(config_n_get $flag port)
		local config_file="${flag}.json"
		local log_file="${flag}.log"
		local log=$(config_n_get $flag log 1)
		[ "$log" = "0" ] && log_file=""
		local http_port=$(config_n_get $flag http_port 0)
		local http_config_file="${flag}_http.json"
		LOG_FILE="/dev/null"
		run_socks flag=$flag node=$new_node bind=$bind socks_port=$port config_file=$config_file http_port=$http_port http_config_file=$http_config_file log_file=$log_file
		set_cache_var "${flag}" "$new_node"
		local USE_TABLES=$(get_cache_var "USE_TABLES")
		[ -n "$USE_TABLES" ] && source $APP_PATH/${USE_TABLES}.sh filter_direct_node_list
	}
}

clean_crontab() {
	[ -f "${LOCK_PATH}/${CONFIG}_cron.lock" ] && return
	touch /etc/crontabs/root
	#sed -i "/${CONFIG}/d" /etc/crontabs/root >/dev/null 2>&1
	sed -i "/$(echo "/etc/init.d/${CONFIG}" | sed 's#\/#\\\/#g')/d" /etc/crontabs/root >/dev/null 2>&1
	sed -i "/$(echo "lua ${APP_PATH}/rule_update.lua log" | sed 's#\/#\\\/#g')/d" /etc/crontabs/root >/dev/null 2>&1
	sed -i "/$(echo "lua ${APP_PATH}/subscribe.lua start" | sed 's#\/#\\\/#g')/d" /etc/crontabs/root >/dev/null 2>&1

	busybox pgrep -af "${CONFIG}/" | awk '/tasks\.sh/{print $1}' | xargs kill -9 >/dev/null 2>&1
	rm -f ${LOCK_PATH}/${CONFIG}_tasks.lock
}

start_crontab() {
	local update_loop

	if [ "$ENABLED_DEFAULT_ACL" = "1" ] || [ "$ENABLED_ACLS" = "1" ]; then
		local start_daemon=$(config_t_get global_delay start_daemon 0)
		[ "$start_daemon" = "1" ] && { $APP_PATH/monitor.sh > /dev/null 2>&1 & }
	fi

	if [ -f "${LOCK_PATH}/${CONFIG}_cron.lock" ]; then
		rm -f "${LOCK_PATH}/${CONFIG}_cron.lock"
		echolog "当前为计划任务自动运行，不重新配置定时任务。"
		return
	fi

	clean_crontab

	if [ "$ENABLED" != "1" ]; then
		/etc/init.d/cron restart
		return
	fi

	build_time() {
		local w="$1"
		local t="$2"
		local h m
		if echo "$t" | grep -q ':'; then
			h="${t%%:*}"
			m="${t##*:}"
		else
			h="$t"
			m=0
		fi
		h=$(printf '%d' "$h")
		m=$(printf '%d' "$m")
		local expr="$m $h * * $w"
		[ "$w" = "7" ] && expr="$m $h * * *"
		echo "$expr"
	}

	add_service_cron() {
		local week="$1"
		local time="$2"
		local action="$3"
		local logmsg="$4"
		[ -z "$week" ] && return
		if [ "$week" = "8" ]; then
			update_loop=1
		else
			local svr_t=$(build_time "$week" "$time")
			echo "$svr_t /etc/init.d/$CONFIG $action > /dev/null 2>&1 &" >>/etc/crontabs/root
		fi
		echolog "$logmsg"
	}

	# ===== stop/start/restart =====
	add_service_cron "$(config_t_get global_delay stop_week_mode)" "$(config_t_get global_delay stop_time_mode)" "stop" "配置定时任务：自动关闭服务。"

	add_service_cron "$(config_t_get global_delay start_week_mode)" "$(config_t_get global_delay start_time_mode)" "start" "配置定时任务：自动开启服务。"

	add_service_cron "$(config_t_get global_delay restart_week_mode)" "$(config_t_get global_delay restart_time_mode)" "restart" "配置定时任务：自动重启服务。"

	# ===== rule update =====
	local rules_update_week_mode=$(config_t_get global_rules update_week_mode)
	local rules_update_time_mode=$(config_t_get global_rules update_time_mode)
	if [ -n "$rules_update_week_mode" ]; then
		if [ "$rules_update_week_mode" = "8" ]; then
			update_loop=1
		else
			local rule_t=$(build_time "$rules_update_week_mode" "$rules_update_time_mode")
			echo "$rule_t lua $APP_PATH/rule_update.lua log all cron > /dev/null 2>&1 &" >>/etc/crontabs/root
		fi
		echolog "配置定时任务：自动更新规则。"
	fi

	# ===== subscribe =====
	local TMP_SUB_PATH=$TMP_PATH/sub_crontabs
	mkdir -p "$TMP_SUB_PATH"
	local item remark sub_update_week_mode sub_update_time_mode
	for item in $(uci show ${CONFIG} | grep "=subscribe_list" | cut -d '.' -sf 2 | cut -d '=' -sf 1); do
		sub_update_week_mode=$(config_n_get $item update_week_mode)
		if [ -n "$sub_update_week_mode" ]; then
			remark=$(config_n_get "$item" remark)
			sub_update_time_mode=$(config_n_get $item update_time_mode)
			echo "$item" >> "$TMP_SUB_PATH/${sub_update_week_mode}_${sub_update_time_mode}"
			echolog "配置定时任务：自动更新【$remark】订阅。"
		fi
	done
	if [ -d "$TMP_SUB_PATH" ]; then
		local name cfgids
		for name in $(ls "$TMP_SUB_PATH"); do
			sub_update_week_mode=${name%_*}
			sub_update_time_mode=${name#*_}
			cfgids=$(tr '\n' ',' < "$TMP_SUB_PATH/$name" | sed 's/,$//')
			if [ "$sub_update_week_mode" = "8" ]; then
				update_loop=1
			else
				local sub_t=$(build_time "$sub_update_week_mode" "$sub_update_time_mode")
				echo "$sub_t lua $APP_PATH/subscribe.lua start $cfgids cron > /dev/null 2>&1 &" >>/etc/crontabs/root
			fi
		done
		rm -rf "$TMP_SUB_PATH"
	fi

	# ===== loop =====
	if [ "$ENABLED_DEFAULT_ACL" = "1" ] || [ "$ENABLED_ACLS" = "1" ]; then
		if [ "$update_loop" = "1" ]; then
			$APP_PATH/tasks.sh > /dev/null 2>&1 &
			echolog "自动更新：启动循环更新进程。"
		fi
	else
		echolog "运行于非代理模式，仅允许服务启停的定时任务。"
	fi

	/etc/init.d/cron restart
}

stop_crontab() {
	[ -f "${LOCK_PATH}/${CONFIG}_cron.lock" ] && return
	clean_crontab
	/etc/init.d/cron restart
	#echolog "清除定时执行命令。"
}

start_dns() {
	echolog "DNS域名解析："

	local china_ng_local_dns=$(IFS=','; set -- $LOCAL_DNS; [ "${1%%[#:]*}" = "127.0.0.1" ] && echo "$1" || ([ -n "$2" ] && echo "$*" || echo "$1"))
	local sing_box_local_dns=
	local direct_dns_mode=$(config_t_get global direct_dns_mode "auto")

	#获取访问控制节点所使用的DNS分流模式
	local ACL_RULE_DNSMASQ=0
	for acl_section in $(uci show ${CONFIG} | grep "=acl_rule" | cut -d '.' -sf 2 | cut -d '=' -sf 1); do
		if [ "$(config_n_get $acl_section enabled)" = "1" ] && \
		   [ "$(config_n_get $acl_section dns_shunt)" = "dnsmasq" ]; then
			ACL_RULE_DNSMASQ=1
			break
		fi
	done

	case "$direct_dns_mode" in
		udp)
			LOCAL_DNS=$(normalize_dns "$(config_t_get global direct_dns 223.5.5.5:53)")
			china_ng_local_dns=${LOCAL_DNS}
			sing_box_local_dns="direct_dns_udp_server=${LOCAL_DNS}"
		;;
		tcp)	
			local DIRECT_DNS=$(normalize_dns "$(config_t_get global direct_dns 223.5.5.5:53)")
			china_ng_local_dns="tcp://${DIRECT_DNS}"
			sing_box_local_dns="direct_dns_tcp_server=${DIRECT_DNS}"

			#当全局（包括访问控制节点）开启chinadns-ng时，不启动新进程。
			[ "$DNS_SHUNT" != "chinadns-ng" ] || [ "$ACL_RULE_DNSMASQ" = "1" ] && {
				LOCAL_DNS="127.0.0.1#${NEXT_DNS_LISTEN_PORT}"
				local china_ng_c_dns="tcp://$(get_first_dns DIRECT_DNS 53)"
				ln_run "$(first_type chinadns-ng)" chinadns-ng "/dev/null" -b :: -l ${NEXT_DNS_LISTEN_PORT} -c ${china_ng_c_dns} -d chn
				echolog "  - ChinaDNS-NG(${LOCAL_DNS}) -> ${china_ng_c_dns}"
				echolog "  * 请确保上游直连 DNS 支持 TCP 查询。"
				NEXT_DNS_LISTEN_PORT=$(expr $NEXT_DNS_LISTEN_PORT + 1)
			}
		;;
		auto)
			#Automatic logic is already done by default
			:
		;;
	esac

	# 追加直连DNS到iptables/nftables
	[ "$(config_t_get global_haproxy balancing_enable 0)" != "1" ] && IPT_APPEND_DNS=
	add_default_port() {
		[ -z "$1" ] && echo "" || echo "$1" | awk -F',' '{for(i=1;i<=NF;i++){if($i !~ /#/) $i=$i"#53";} print $0;}' OFS=','
	}
	LOCAL_DNS=$(add_default_port "$LOCAL_DNS")
	IPT_APPEND_DNS=$(add_default_port "${IPT_APPEND_DNS:-$LOCAL_DNS}")
	echo "$IPT_APPEND_DNS" | grep -q -E "(^|,)$LOCAL_DNS(,|$)" || IPT_APPEND_DNS="${IPT_APPEND_DNS:+$IPT_APPEND_DNS,}$LOCAL_DNS"
	[ -n "$DIRECT_DNS" ] && {
		DIRECT_DNS=$(add_default_port "$DIRECT_DNS")
		echo "$IPT_APPEND_DNS" | grep -q -E "(^|,)$DIRECT_DNS(,|$)" || IPT_APPEND_DNS="${IPT_APPEND_DNS:+$IPT_APPEND_DNS,}$DIRECT_DNS"
	}
	# 排除127.0.0.1的条目
	IPT_APPEND_DNS=$(echo "$IPT_APPEND_DNS" | awk -F',' '{for(i=1;i<=NF;i++) if($i !~ /^127\.0\.0\.1/) printf (i>1?",":"") $i; print ""}' | sed 's/^,\|,$//g')

	TUN_DNS="127.0.0.1#${NEXT_DNS_LISTEN_PORT}"
	[ -n "${NO_PLUGIN_DNS}" ] && TUN_DNS="127.0.0.1#${resolve_dns_port}"

	case "$DNS_MODE" in
	dns2socks)
		local dns2socks_socks_server=$(echo $(config_t_get global socks_server 127.0.0.1:1080) | sed "s/#/:/g")
		local dns2socks_forward=$(get_first_dns REMOTE_DNS 53 | sed 's/#/:/g')
		run_dns2socks socks=$dns2socks_socks_server listen_address=127.0.0.1 listen_port=${NEXT_DNS_LISTEN_PORT} dns=$dns2socks_forward cache=$DNS_CACHE
		echolog "  - dns2socks(${TUN_DNS})，${dns2socks_socks_server} -> tcp://${dns2socks_forward}"
	;;
	sing-box)
		[ -z "${NO_PLUGIN_DNS}" ] && {
			local config_file=$TMP_PATH/DNS.json
			local log_file=$TMP_PATH/DNS.log
			local log_file=/dev/null
			local _args="type=$DNS_MODE config_file=$config_file log_file=$log_file"
			[ "${DNS_CACHE}" = "0" ] && _args="${_args} dns_cache=0"
			_args="${_args} direct_dns_query_strategy=${DIRECT_DNS_QUERY_STRATEGY}"
			_args="${_args} remote_dns_query_strategy=${REMOTE_DNS_QUERY_STRATEGY}"
			DNSMASQ_FILTER_PROXY_IPV6=0
			local remote_fakedns=$(config_t_get global remote_fakedns 0)
			[ "${remote_fakedns}" = "1" ] && {
				REMOTE_FAKEDNS=1
				_args="${_args} remote_fakedns=1"
				USE_FAKEDNS=1
			}
			local _remote_dns_client_ip=$(config_t_get global remote_dns_client_ip)
			[ -n "${_remote_dns_client_ip}" ] && _args="${_args} remote_dns_client_ip=${_remote_dns_client_ip}"
			TCP_PROXY_DNS=1
			local v2ray_dns_mode=$(config_t_get global v2ray_dns_mode tcp)
			_args="${_args} dns_listen_port=${NEXT_DNS_LISTEN_PORT}"
			_args="${_args} remote_dns_protocol=${v2ray_dns_mode}"
			case "$v2ray_dns_mode" in
				udp|tcp)
					local _proto="$v2ray_dns_mode"
					_args="${_args} remote_dns_${_proto}_server=${REMOTE_DNS}"
					echolog "  - Sing-Box DNS(${TUN_DNS}) -> ${_proto}://${REMOTE_DNS}"
				;;
				doh|http3)
					local remote_dns_doh=$(config_t_get global remote_dns_doh "https://1.1.1.1/dns-query")
					_args="${_args} remote_dns_doh=${remote_dns_doh}"
					echolog "  - Sing-Box DNS(${TUN_DNS}) -> ${remote_dns_doh}"
				;;
			esac
			_args="${_args} dns_socks_address=127.0.0.1 dns_socks_port=${tcp_node_socks_port}"
			[ -n "${sing_box_local_dns}" ] && _args="${_args} ${sing_box_local_dns}"
			run_singbox ${_args}
		}
	;;
	xray)
		[ -z "${NO_PLUGIN_DNS}" ] && {
			local config_file=$TMP_PATH/DNS.json
			local log_file=$TMP_PATH/DNS.log
			local log_file=/dev/null
			local _args="type=$DNS_MODE config_file=$config_file log_file=$log_file"
			[ "${DNS_CACHE}" = "0" ] && _args="${_args} dns_cache=0"
			_args="${_args} direct_dns_query_strategy=${DIRECT_DNS_QUERY_STRATEGY}"
			_args="${_args} remote_dns_query_strategy=${REMOTE_DNS_QUERY_STRATEGY}"
			DNSMASQ_FILTER_PROXY_IPV6=0
			local remote_fakedns=$(config_t_get global remote_fakedns 0)
			[ "${remote_fakedns}" = "1" ] && {
				REMOTE_FAKEDNS=1
				_args="${_args} remote_fakedns=1"
				USE_FAKEDNS=1
			}
			local _remote_dns_client_ip=$(config_t_get global remote_dns_client_ip)
			[ -n "${_remote_dns_client_ip}" ] && _args="${_args} remote_dns_client_ip=${_remote_dns_client_ip}"
			TCP_PROXY_DNS=1
			local v2ray_dns_mode=$(config_t_get global v2ray_dns_mode tcp)
			#兼容旧模式，择机移除
			[ "$v2ray_dns_mode" = "tcp+doh" ] && v2ray_dns_mode="tcp"
			_args="${_args} dns_listen_port=${NEXT_DNS_LISTEN_PORT}"
			_args="${_args} remote_dns_protocol=${v2ray_dns_mode}"
			case "$v2ray_dns_mode" in
				udp|tcp)
					local _proto="$v2ray_dns_mode"
					_args="${_args} remote_dns_${_proto}_server=${REMOTE_DNS}"
					echolog "  - Xray DNS(${TUN_DNS}) -> ${_proto}://${REMOTE_DNS}"
				;;
				doh)
					local remote_dns_doh=$(config_t_get global remote_dns_doh "https://1.1.1.1/dns-query")
					_args="${_args} remote_dns_doh=${remote_dns_doh}"
					echolog "  - Xray DNS(${TUN_DNS}) -> ${remote_dns_doh}"
				;;
			esac
			_args="${_args} dns_socks_address=127.0.0.1 dns_socks_port=${tcp_node_socks_port}"
			run_xray ${_args}
		}
	;;
	udp)
		UDP_PROXY_DNS=1
		local china_ng_listen_port=${NEXT_DNS_LISTEN_PORT}
		local china_ng_trust_dns="udp://$(get_first_dns REMOTE_DNS 53)"
		if [ "$DNS_SHUNT" != "chinadns-ng" ] && [ "$FILTER_PROXY_IPV6" = "1" ]; then
			DNSMASQ_FILTER_PROXY_IPV6=0
			local no_ipv6_trust="-N"
			ln_run "$(first_type chinadns-ng)" chinadns-ng "/dev/null" -b :: -l ${china_ng_listen_port} -t ${china_ng_trust_dns} -d gfw ${no_ipv6_trust}
			echolog "  - ChinaDNS-NG(${TUN_DNS}) -> ${china_ng_trust_dns}"
		else
			TUN_DNS=${REMOTE_DNS}
			echolog "  - udp://${TUN_DNS}"
		fi
	;;
	tcp)
		TCP_PROXY_DNS=1
		local china_ng_listen_port=${NEXT_DNS_LISTEN_PORT}
		local china_ng_trust_dns="tcp://$(get_first_dns REMOTE_DNS 53)"
		[ "$DNS_SHUNT" != "chinadns-ng" ] && {
			[ "$FILTER_PROXY_IPV6" = "1" ] && DNSMASQ_FILTER_PROXY_IPV6=0 && local no_ipv6_trust="-N"
			ln_run "$(first_type chinadns-ng)" chinadns-ng "/dev/null" -b :: -l ${china_ng_listen_port} -t ${china_ng_trust_dns} -d gfw ${no_ipv6_trust}
			echolog "  - ChinaDNS-NG(${TUN_DNS}) -> ${china_ng_trust_dns}"
		}
	;;
	esac

	[ -n "${resolve_dns_log}" ] && echolog "  - ${resolve_dns_log}"

	[ -n "${TCP_PROXY_DNS}" ] && echolog "  * 请确认上游 DNS 支持 TCP/DoH 查询，如非直连地址，确保 TCP 代理打开，并且已经正确转发！"
	[ -n "${UDP_PROXY_DNS}" ] && echolog "  * 请确认上游 DNS 支持 UDP 查询并已使用 UDP 节点，如上游 DNS 非直连地址，确保 UDP 代理打开，并且已经正确转发！"

	local china_ng_listen=0
	[ "${DNS_SHUNT}" = "smartdns" ] && {
		if command -v smartdns > /dev/null 2>&1; then
			rm -rf $TMP_PATH2/dnsmasq_default*
			local group_domestic=$(config_t_get global group_domestic)
			local smartdns_remote_dns=$(config_t_get global smartdns_remote_dns)
			if [ -n "${smartdns_remote_dns}" ] && [ "${smartdns_remote_dns}" != "nil" ]; then
				smartdns_remote_dns=$(echo ${smartdns_remote_dns} | tr -s ' ' '|')
			else
				smartdns_remote_dns="tcp://1.1.1.1"
			fi

			echolog "  - 域名解析：使用SmartDNS，请确保配置正常。"
			china_ng_listen="127.0.0.1#${SMARTDNS_LISTEN_PORT}"
			echolog "  - SmartDNS(127.0.0.1#${SMARTDNS_LOCAL_PORT}) -> 国内分组(${group_domestic:-null})，SmartDNS(${china_ng_listen}) -> Dnsmasq"
			china_ng_listen="${china_ng_listen},::1#${SMARTDNS_LISTEN_PORT}"

			local subnet_ip=$(config_t_get global remote_dns_client_ip)
			lua $APP_PATH/helper_smartdns_add.lua -FLAG "default" -SMARTDNS_CONF "/tmp/etc/smartdns/$CONFIG.conf" \
				-LISTEN_PORT ${SMARTDNS_LISTEN_PORT} -LOCAL_PORT ${SMARTDNS_LOCAL_PORT} \
				-LOCAL_GROUP ${group_domestic:-null} -REMOTE_GROUP "passwall_proxy" -REMOTE_PROXY_SERVER ${TCP_SOCKS_server} -USE_DEFAULT_DNS "${USE_DEFAULT_DNS:-direct}" \
				-REMOTE_DNS ${smartdns_remote_dns} -DNS_MODE ${DNS_MODE:-socks} -TUN_DNS ${TUN_DNS} \
				-USE_DIRECT_LIST "${USE_DIRECT_LIST}" -USE_PROXY_LIST "${USE_PROXY_LIST}" -USE_BLOCK_LIST "${USE_BLOCK_LIST}" -USE_GFW_LIST "${USE_GFW_LIST}" -CHN_LIST "${CHN_LIST}" \
				-TCP_NODE ${TCP_NODE} -DEFAULT_PROXY_MODE "${TCP_PROXY_MODE}" -NO_PROXY_IPV6 ${FILTER_PROXY_IPV6:-0} -NFTFLAG ${nftflag:-0} \
				-SUBNET ${subnet_ip:-0} -NO_LOGIC_LOG ${NO_LOGIC_LOG:-0}
			source $APP_PATH/helper_smartdns.sh restart

			USE_DEFAULT_DNS="chinadns_ng"
		else
			DNS_SHUNT="dnsmasq"
			echolog "  * 未安装SmartDNS，默认使用Dnsmasq进行域名解析！"
		fi
	}

	[ "${DNS_SHUNT}" = "chinadns-ng" ] && [ -n "$(first_type chinadns-ng)" ] && {
		chinadns_ng_min=2024.04.13
		chinadns_ng_now=$($(first_type chinadns-ng) -V | grep -i "ChinaDNS-NG " | awk '{print $2}')
		if [ $(check_ver "$chinadns_ng_now" "$chinadns_ng_min") = 1 ]; then
			echolog "  * 注意：当前 ChinaDNS-NG 版本为[ $chinadns_ng_now ]，请更新到[ $chinadns_ng_min ]或以上版本，否则 DNS 有可能无法正常工作！"
		fi

		[ "$FILTER_PROXY_IPV6" = "1" ] && DNSMASQ_FILTER_PROXY_IPV6=0
		[ -z "${china_ng_listen_port}" ] && local china_ng_listen_port=$(expr $NEXT_DNS_LISTEN_PORT + 1)
		china_ng_listen="127.0.0.1#${china_ng_listen_port}"
		[ -z "${china_ng_trust_dns}" ] && local china_ng_trust_dns=${TUN_DNS}

		echolog "  - ChinaDNS-NG(${china_ng_listen})：直连DNS：${china_ng_local_dns}，可信DNS：${china_ng_trust_dns}"

		china_ng_listen="${china_ng_listen},::1#${china_ng_listen_port}"

		run_chinadns_ng \
			_flag="default" \
			_listen_port=${china_ng_listen_port} \
			_dns_local=${china_ng_local_dns} \
			_dns_trust=${china_ng_trust_dns} \
			_no_ipv6_trust=${FILTER_PROXY_IPV6} \
			_use_direct_list=${USE_DIRECT_LIST} \
			_use_proxy_list=${USE_PROXY_LIST} \
			_use_block_list=${USE_BLOCK_LIST} \
			_gfwlist=${USE_GFW_LIST} \
			_chnlist=${CHN_LIST} \
			_default_mode=${TCP_PROXY_MODE} \
			_default_tag=$(config_t_get global chinadns_ng_default_tag smart) \
			_no_logic_log=0 \
			_tcp_node=${TCP_NODE} \
			_filter_https=$(config_t_get global force_https_soa 0) \
			_log=$(config_t_get global log_chinadns_ng 0)

		USE_DEFAULT_DNS="chinadns_ng"
	}

	[ "$USE_DEFAULT_DNS" = "remote" ] && {
		dnsmasq_version=$(dnsmasq -v | grep -i "Dnsmasq version " | awk '{print $3}')
		[ "$(check_ver "$dnsmasq_version" "2.87")" = "1" ] && echolog "Dnsmasq版本低于2.87，有可能无法正常使用！！！"
	}

	local DNSMASQ_TUN_DNS=$(get_first_dns TUN_DNS 53)
	local RUN_NEW_DNSMASQ=1
	RUN_NEW_DNSMASQ=${DNS_REDIRECT}
	if [ "${RUN_NEW_DNSMASQ}" = "0" ]; then
		#The old logic will be removed in the future.
		#Run a copy dnsmasq instance, DNS hijack that don't need a proxy devices.
		[ "1" = "0" ] && {
			DIRECT_DNSMASQ_PORT=$(get_new_port 11400)
			DIRECT_DNSMASQ_CONF=${GLOBAL_ACL_PATH}/direct_dnsmasq.conf
			DIRECT_DNSMASQ_CONF_PATH=${GLOBAL_ACL_PATH}/direct_dnsmasq.d
			mkdir -p ${DIRECT_DNSMASQ_CONF_PATH}
			lua $APP_PATH/helper_dnsmasq.lua copy_instance -LISTEN_PORT ${DIRECT_DNSMASQ_PORT} -DNSMASQ_CONF ${DIRECT_DNSMASQ_CONF} -TMP_DNSMASQ_PATH ${DIRECT_DNSMASQ_CONF_PATH}
			ln_run "$(first_type dnsmasq)" "dnsmasq_direct" "/dev/null" -C ${DIRECT_DNSMASQ_CONF} -x ${GLOBAL_ACL_PATH}/direct_dnsmasq.pid
			echo "${DIRECT_DNSMASQ_PORT}" > ${GLOBAL_ACL_PATH}/direct_dnsmasq_port
		}
		
		#Rewrite the default DNS service configuration
		#Modify the default dnsmasq service
		lua $APP_PATH/helper_dnsmasq.lua stretch
		lua $APP_PATH/helper_dnsmasq.lua add_rule -FLAG "default" -TMP_DNSMASQ_PATH ${GLOBAL_DNSMASQ_CONF_PATH} -DNSMASQ_CONF_FILE ${GLOBAL_DNSMASQ_CONF} \
			-DEFAULT_DNS ${DEFAULT_DNS} -LOCAL_DNS ${LOCAL_DNS} -TUN_DNS ${DNSMASQ_TUN_DNS} \
			-USE_DEFAULT_DNS "${USE_DEFAULT_DNS:-direct}" -CHINADNS_DNS ${china_ng_listen:-0} \
			-USE_DIRECT_LIST "${USE_DIRECT_LIST}" -USE_PROXY_LIST "${USE_PROXY_LIST}" -USE_BLOCK_LIST "${USE_BLOCK_LIST}" -USE_GFW_LIST "${USE_GFW_LIST}" -CHN_LIST "${CHN_LIST}" \
			-TCP_NODE ${TCP_NODE} -DEFAULT_PROXY_MODE ${TCP_PROXY_MODE} -NO_PROXY_IPV6 ${DNSMASQ_FILTER_PROXY_IPV6:-0} -NFTFLAG ${nftflag:-0} \
			-NO_LOGIC_LOG ${NO_LOGIC_LOG:-0}
		uci -q add_list dhcp.@dnsmasq[0].addnmount=${GLOBAL_DNSMASQ_CONF_PATH}
		uci -q commit dhcp
		lua $APP_PATH/helper_dnsmasq.lua logic_restart -LOG 1
	else
		#Run a copy dnsmasq instance, DNS hijack for that need proxy devices.
		GLOBAL_DNSMASQ_PORT=$(get_new_port 11400)
		GLOBAL_DNSMASQ_CONF=${GLOBAL_ACL_PATH}/dnsmasq.conf
		GLOBAL_DNSMASQ_CONF_PATH=${GLOBAL_ACL_PATH}/dnsmasq.d
		lua $APP_PATH/helper_dnsmasq.lua add_rule -FLAG "default" -TMP_DNSMASQ_PATH ${GLOBAL_DNSMASQ_CONF_PATH} -DNSMASQ_CONF_FILE ${GLOBAL_DNSMASQ_CONF} \
			-LISTEN_PORT ${GLOBAL_DNSMASQ_PORT} -DEFAULT_DNS ${DEFAULT_DNS} -LOCAL_DNS ${LOCAL_DNS} -TUN_DNS ${DNSMASQ_TUN_DNS} \
			-USE_DEFAULT_DNS "${USE_DEFAULT_DNS:-direct}" -CHINADNS_DNS ${china_ng_listen:-0} \
			-USE_DIRECT_LIST "${USE_DIRECT_LIST}" -USE_PROXY_LIST "${USE_PROXY_LIST}" -USE_BLOCK_LIST "${USE_BLOCK_LIST}" -USE_GFW_LIST "${USE_GFW_LIST}" -CHN_LIST "${CHN_LIST}" \
			-TCP_NODE ${TCP_NODE} -DEFAULT_PROXY_MODE ${TCP_PROXY_MODE} -NO_PROXY_IPV6 ${DNSMASQ_FILTER_PROXY_IPV6:-0} -NFTFLAG ${nftflag:-0} \
			-NO_LOGIC_LOG ${NO_LOGIC_LOG:-0}
		ln_run "$(first_type dnsmasq)" "dnsmasq_default" "/dev/null" -C ${GLOBAL_DNSMASQ_CONF} -x ${GLOBAL_ACL_PATH}/dnsmasq.pid
		set_cache_var "ACL_default_dns_port" "${GLOBAL_DNSMASQ_PORT}"
		DNS_REDIRECT_PORT=${GLOBAL_DNSMASQ_PORT}
		#dhcp.leases to hosts
		$APP_PATH/lease2hosts.sh > /dev/null 2>&1 &
	fi
}

start_haproxy() {
	[ "$(config_t_get global_haproxy balancing_enable 0)" != "1" ] && return
	local haproxy_ver=$($(first_type haproxy) -v 2>/dev/null | awk 'NR==1 {print $3}' | cut -d'-' -f1)
	if [ "$(check_ver "$haproxy_ver" "3.0.0")" = "1" ]; then
		echolog "* 注意：haproxy($haproxy_ver) 程序版本低，HAPROXY 负载均衡启动失败，请更新到 3.0 以上版本。"
		return
	fi
	local haproxy_path=$TMP_PATH/haproxy
	local haproxy_conf="config.cfg"
	lua $APP_PATH/haproxy.lua -path ${haproxy_path} -conf ${haproxy_conf} -dns ${LOCAL_DNS}
	ln_run "$(first_type haproxy)" haproxy "/dev/null" -f "${haproxy_path}/${haproxy_conf}"
}

acl_app() {
	local items=$(uci show ${CONFIG} | grep "=acl_rule" | cut -d '.' -sf 2 | cut -d '=' -sf 1)
	if [ -z "$items" ]; then
		ENABLED_ACLS=0
		set_cache_var ENABLED_ACLS $ENABLED_ACLS
		return
	else
		local has_enabled
		local sid
		local socks_port redir_port dns_port dnsmasq_port chinadns_port
		local msg msg2
		socks_port=11100
		redir_port=11200
		dns_port=11300
		dnsmasq_port=${GLOBAL_DNSMASQ_PORT:-11400}
		chinadns_port=11500
		for sid in $items; do
			[ "$(config_n_get $sid enabled)" = "1" ] || continue
			has_enabled=1
			eval $(uci -q show "${CONFIG}.${sid}" | cut -d'.' -sf 3-)

			log=${log:-0}
			loglevel=${loglevel:-warn}

			if [ -n "${sources}" ]; then
				for s in $sources; do
					local s2
					is_iprange=$(lua_api "iprange(\"${s}\")")
					if [ "${is_iprange}" = "true" ]; then
						s2="iprange:${s}"
					elif [ -n "$(echo ${s} | grep '^ipset:')" ]; then
						s2="ipset:${s}"
					else
						_ip_or_mac=$(lua_api "ip_or_mac(\"${s}\")")
						if [ "${_ip_or_mac}" = "ip" ]; then
							s2="ip:${s}"
						elif [ "${_ip_or_mac}" = "mac" ]; then
							s2="mac:${s}"
						fi
					fi
					[ -n "${s2}" ] && source_list="${source_list}\n${s2}"
					unset s2
				done
			else
				source_list="any"
			fi

			local acl_path=${TMP_ACL_PATH}/$sid
			mkdir -p ${acl_path}
			[ -n "${source_list}" ] && printf "%b\n" "${source_list}" | sed '/^$/d' > ${acl_path}/source_list

			use_global_config=${use_global_config:-0}
			[ "${use_global_config}" = "1" ] && {
				tcp_node="default"
				udp_node="default"
			}
			tcp_no_redir_ports=${tcp_no_redir_ports:-${TCP_NO_REDIR_PORTS}}
			udp_no_redir_ports=${udp_no_redir_ports:-${UDP_NO_REDIR_PORTS}}
			if has_1_65535 "$tcp_no_redir_ports" && has_1_65535 "$udp_no_redir_ports"; then
				unset use_global_config
				unset tcp_node
				unset udp_node
			else
				use_direct_list=${use_direct_list:-1}
				use_proxy_list=${use_proxy_list:-1}
				use_block_list=${use_block_list:-1}
				use_gfw_list=${use_gfw_list:-1}
				chn_list=${chn_list:-direct}
				tcp_proxy_mode=${tcp_proxy_mode:-proxy}
				udp_proxy_mode=${udp_proxy_mode:-proxy}
				filter_proxy_ipv6=${filter_proxy_ipv6:-0}
				dnsmasq_filter_proxy_ipv6=${filter_proxy_ipv6}
				dns_shunt=${dns_shunt:-dnsmasq}
				dns_mode=${dns_mode:-dns2socks}
				remote_dns=$(normalize_dns "${remote_dns:-1.1.1.1}")
				use_default_dns=${use_default_dns:-direct}
			fi

			[ -n "$tcp_node" ] && {
				local GLOBAL_TCP_NODE=$(get_cache_var "ACL_GLOBAL_TCP_node")
				[ -n "${GLOBAL_TCP_NODE}" ] && GLOBAL_TCP_redir_port=$(get_cache_var "ACL_GLOBAL_TCP_redir_port")
				if [ "$tcp_node" = "default" ]; then
					if [ -n "${GLOBAL_TCP_NODE}" ]; then
						set_cache_var "ACL_${sid}_tcp_node" "${GLOBAL_TCP_NODE}"
						set_cache_var "ACL_${sid}_tcp_redir_port" "${GLOBAL_TCP_redir_port}"
						set_cache_var "ACL_${sid}_dns_port" "${GLOBAL_DNSMASQ_PORT}"
						set_cache_var "ACL_${sid}_tcp_default" "1"
						[ "$GLOBAL_SHUNT_NODE_FAKEDNS" = "1" ] && use_fakedns=1
					else
						echolog "  - 全局节点未启用，跳过【${remarks}】"
					fi
				else
					[ "$(config_get_type $tcp_node)" = "nodes" ] || [ "$(config_get_type $tcp_node)" = "socks" ] && {
						if [ -n "${GLOBAL_TCP_NODE}" ] && [ "$tcp_node" = "${GLOBAL_TCP_NODE}" ]; then
							set_cache_var "ACL_${sid}_tcp_node" "${GLOBAL_TCP_NODE}"
							set_cache_var "ACL_${sid}_tcp_redir_port" "${GLOBAL_TCP_redir_port}"
							set_cache_var "ACL_${sid}_dns_port" "${GLOBAL_DNSMASQ_PORT}"
							set_cache_var "ACL_${sid}_tcp_default" "1"
							[ "$GLOBAL_SHUNT_NODE_FAKEDNS" = "1" ] && use_fakedns=1
						else
							local type protocol
							if [ "$(config_get_type $tcp_node)" = "socks" ]; then
								if [ "${dns_mode}" = "xray" ]; then
									type="xray"
								elif [ "${dns_mode}" = "sing-box" ]; then
									type="sing-box"
								fi
							else
								type=$(echo $(config_n_get $tcp_node type) | tr 'A-Z' 'a-z')
								protocol=$(config_n_get $tcp_node protocol)
							fi
							#兼容旧模式，择机移除
							[ "$v2ray_dns_mode" = "tcp+doh" ] && v2ray_dns_mode="tcp"
							([ "$type" = "sing-box" ] || [ "$type" = "xray" ]) && [ "$protocol" = "_shunt" ] && [ "$type" != "$dns_mode" ] && {
								dns_mode=$type
								[ "$type" = "xray" ] && [ "$v2ray_dns_mode" = "http3" ] && v2ray_dns_mode="tcp"
							}
							dns_cache_key="${dns_mode}_${remote_dns}_${v2ray_dns_mode:-none}_${remote_dns_client_ip:-0}_${remote_fakedns:-0}"
							([ "$v2ray_dns_mode" = "doh" ] || [ "$v2ray_dns_mode" = "http3" ]) && {
								dns_cache_key="${dns_mode}_${remote_dns_doh:-https://1.1.1.1/dns-query}_${v2ray_dns_mode:-doh}_${remote_dns_client_ip:-0}_${remote_fakedns:-0}"
							}

							if [ "$remote_fakedns" = "1" ] || ([ "$protocol" = "_shunt" ] && [ "$(config_n_get $tcp_node fakedns)" = "1" ]); then
								use_fakedns=1
							fi

							run_dns() {
								local _dns_port
								[ -n $1 ] && _dns_port=$1
								[ -z ${_dns_port} ] && {
									dns_port=$(get_new_port $(expr $dns_port + 1))
									_dns_port=$dns_port
									if [ "$dns_mode" = "dns2socks" ]; then
										run_dns2socks flag=acl_${sid} socks_address=127.0.0.1 socks_port=$socks_port listen_address=0.0.0.0 listen_port=${_dns_port} dns=$remote_dns cache=1
									elif [ "$dns_mode" = "sing-box" ] || [ "$dns_mode" = "xray" ]; then
										config_file=$TMP_ACL_PATH/${tcp_node}_SOCKS_${socks_port}_DNS.json
										remote_dns_doh=${remote_dns_doh:-https://1.1.1.1/dns-query}
										local type=${dns_mode}
										[ "${dns_mode}" = "sing-box" ] && type="singbox"
										dnsmasq_filter_proxy_ipv6=0
										remote_dns_query_strategy="UseIP"
										[ "$filter_proxy_ipv6" = "1" ] && remote_dns_query_strategy="UseIPv4"
										run_${type} flag=acl_${sid} type=$dns_mode dns_socks_address=127.0.0.1 dns_socks_port=$socks_port dns_listen_port=${_dns_port} remote_dns_protocol=${v2ray_dns_mode} remote_dns_udp_server=${remote_dns} remote_dns_tcp_server=${remote_dns} remote_dns_doh="${remote_dns_doh}" remote_dns_query_strategy=${remote_dns_query_strategy} remote_dns_client_ip=${remote_dns_client_ip} config_file=$config_file
									fi
									set_cache_var "node_${tcp_node}_$(echo -n "${dns_cache_key}" | md5sum | cut -d " " -f1)" "${_dns_port}"
								}

								[ "$dns_shunt" = "chinadns-ng" ] && [ -n "$(first_type chinadns-ng)" ] && {
									chinadns_ng_min=2024.04.13
									chinadns_ng_now=$($(first_type chinadns-ng) -V | grep -i "ChinaDNS-NG " | awk '{print $2}')
									if [ $(check_ver "$chinadns_ng_now" "$chinadns_ng_min") = 1 ]; then
										echolog "  * 注意：当前 ChinaDNS-NG 版本为[ $chinadns_ng_now ]，请更新到[ $chinadns_ng_min ]或以上版本，否则 DNS 有可能无法正常工作！"
									fi

									[ "$filter_proxy_ipv6" = "1" ] && dnsmasq_filter_proxy_ipv6=0
									chinadns_port=$(expr $chinadns_port + 1)
									_china_ng_listen="127.0.0.1#${chinadns_port},::1#${chinadns_port}"

									_chinadns_local_dns=$(IFS=','; set -- $LOCAL_DNS; [ "${1%%[#:]*}" = "127.0.0.1" ] && echo "$1" || ([ -n "$2" ] && echo "$1,$2" || echo "$1"))
									_direct_dns_mode=$(config_t_get global direct_dns_mode "auto")
									case "${_direct_dns_mode}" in
										udp)
											_chinadns_local_dns=$(normalize_dns "$(config_t_get global direct_dns 223.5.5.5:53)")
										;;
										tcp)
											_chinadns_local_dns="tcp://$(normalize_dns "$(config_t_get global direct_dns 223.5.5.5:53)")"
										;;
									esac

									run_chinadns_ng \
										_flag="$sid" \
										_listen_port=${chinadns_port} \
										_dns_local=${_chinadns_local_dns} \
										_dns_trust=127.0.0.1#${_dns_port} \
										_no_ipv6_trust=${filter_proxy_ipv6} \
										_use_direct_list=${use_direct_list} \
										_use_proxy_list=${use_proxy_list} \
										_use_block_list=${use_block_list} \
										_gfwlist=${use_gfw_list} \
										_chnlist=${chn_list} \
										_default_mode=${tcp_proxy_mode} \
										_default_tag=${chinadns_ng_default_tag:-smart} \
										_no_logic_log=1 \
										_tcp_node=${tcp_node} \
										_filter_https=${force_https_soa:-0} \
										_log=${log}

									use_default_dns="chinadns_ng"
								}

								dnsmasq_port=$(get_new_port $(expr $dnsmasq_port + 1))
								local dnsmasq_conf=${acl_path}/dnsmasq.conf
								local dnsmasq_conf_path=${acl_path}/dnsmasq.d
								lua $APP_PATH/helper_dnsmasq.lua add_rule -FLAG ${sid} -TMP_DNSMASQ_PATH ${dnsmasq_conf_path} -DNSMASQ_CONF_FILE ${dnsmasq_conf} \
									-LISTEN_PORT ${dnsmasq_port} -DEFAULT_DNS ${DEFAULT_DNS} -LOCAL_DNS $LOCAL_DNS \
									-USE_DIRECT_LIST "${use_direct_list}" -USE_PROXY_LIST "${use_proxy_list}" -USE_BLOCK_LIST "${use_block_list}" -USE_GFW_LIST "${use_gfw_list}" -CHN_LIST "${chn_list}" \
									-TUN_DNS "127.0.0.1#${_dns_port}" -USE_DEFAULT_DNS "${use_default_dns:-direct}" -CHINADNS_DNS ${_china_ng_listen:-0} \
									-TCP_NODE $tcp_node -DEFAULT_PROXY_MODE ${tcp_proxy_mode} -NO_PROXY_IPV6 ${dnsmasq_filter_proxy_ipv6:-0} -NFTFLAG ${nftflag:-0} \
									-NO_LOGIC_LOG 1
								ln_run "$(first_type dnsmasq)" "dnsmasq_${sid}" "/dev/null" -C ${dnsmasq_conf} -x ${acl_path}/dnsmasq.pid
								set_cache_var "ACL_${sid}_dns_port" "${dnsmasq_port}"
								set_cache_var "node_${tcp_node}_$(echo -n "${tcp_proxy_mode}_${dns_cache_key}" | md5sum | cut -d " " -f1)" "${dnsmasq_port}"
								#dhcp.leases to hosts
								$APP_PATH/lease2hosts.sh > /dev/null 2>&1 &
							}
							_redir_port=$(get_cache_var "node_${tcp_node}_redir_port")
							_socks_port=$(get_cache_var "node_${tcp_node}_socks_port")
							if [ -n "${_socks_port}" ] && [ -n "${_redir_port}" ]; then
								socks_port=${_socks_port}
								tcp_port=${_redir_port}
								_dnsmasq_port=$(get_cache_var "node_${tcp_node}_$(echo -n "${tcp_proxy_mode}_${dns_cache_key}" | md5sum | cut -d " " -f1)")
								if [ -z "${_dnsmasq_port}" ]; then
									_dns_port=$(get_cache_var "node_${tcp_node}_$(echo -n "${dns_cache_key}" | md5sum | cut -d " " -f1)")
									run_dns ${_dns_port}
								else
									[ -n "${_dnsmasq_port}" ] && set_cache_var "ACL_${sid}_dns_port" "${_dnsmasq_port}"
								fi
							else
								socks_port=$(get_new_port $(expr $socks_port + 1))
								set_cache_var "node_${tcp_node}_socks_port" "${socks_port}"
								redir_port=$(get_new_port $(expr $redir_port + 1))
								set_cache_var "node_${tcp_node}_redir_port" "${redir_port}"
								tcp_port=$redir_port
								local log_file="/dev/null"
								[ "${log}" = "1" ] && log_file="${TMP_ACL_PATH}/${sid}/TCP.log"

								if [ "${type}" = "sing-box" ] || [ "${type}" = "xray" ]; then
									config_file="acl/${tcp_node}_TCP_${redir_port}.json"
									_extra_param="socks_address=127.0.0.1 socks_port=$socks_port"
									[ "${type}" = "${dns_mode}" ] && {
										dns_port=$(get_new_port $(expr $dns_port + 1))
										_dns_port=$dns_port
										config_file="${config_file//TCP_/DNS_${_dns_port}_TCP_}"
										dnsmasq_filter_proxy_ipv6=0
										remote_dns_query_strategy="UseIP"
										[ "$filter_proxy_ipv6" = "1" ] && remote_dns_query_strategy="UseIPv4"
										remote_dns_doh=${remote_dns_doh:-https://1.1.1.1/dns-query}
										_extra_param="${_extra_param} dns_listen_port=${_dns_port} remote_dns_protocol=${v2ray_dns_mode} remote_dns_udp_server=${remote_dns} remote_dns_tcp_server=${remote_dns}"
										_extra_param="${_extra_param} remote_dns_doh=${remote_dns_doh} remote_dns_query_strategy=${remote_dns_query_strategy} remote_fakedns=${remote_fakedns:-0} remote_dns_client_ip=${remote_dns_client_ip}"
									}
									_extra_param="${_extra_param} tcp_proxy_way=$TCP_PROXY_WAY"
									[ -n "$udp_node" ] && ([ "$udp_node" = "tcp" ] || [ "$udp_node" = "$tcp_node" ]) && {
										config_file="${config_file//TCP_/TCP_UDP_}"
										_extra_param="${_extra_param} udp_redir_port=$redir_port"
									}
									config_file="$TMP_PATH/$config_file"
									[ "${type}" = "sing-box" ] && type="singbox"
									_extra_param="${_extra_param} use_proxy_list=$use_proxy_list use_gfw_list=$use_gfw_list chn_list=$chn_list"
									run_${type} flag=$tcp_node node=$tcp_node tcp_redir_port=$redir_port ${_extra_param} config_file=$config_file log_file=$log_file loglevel=$loglevel
								else
									config_file="acl/${tcp_node}_SOCKS_${socks_port}.json"
									run_socks flag=$tcp_node node=$tcp_node bind=127.0.0.1 socks_port=$socks_port config_file=$config_file log_file=$log_file
									# log_file=$TMP_ACL_PATH/ipt2socks_${tcp_node}_${redir_port}.log
									log_file="/dev/null"
									run_ipt2socks flag=acl_${tcp_node} tcp_tproxy=${is_tproxy} local_port=$redir_port socks_address=127.0.0.1 socks_port=$socks_port log_file=$log_file
								fi
								run_dns ${_dns_port}
							fi
							set_cache_var "ACL_${sid}_tcp_node" "${tcp_node}"
							set_cache_var "ACL_${sid}_tcp_redir_port" "${tcp_port}"
						fi
					}
				fi
				[ "${use_fakedns}" = "1" ] && set_cache_var "ACL_${sid}_fakedns" "1"
			}
			[ -n "$udp_node" ] && {
				[ -n "$tcp_node" ] && {
					local protocol=$(config_n_get $tcp_node protocol)
					[ "$protocol" = "_shunt" ] && [ "$udp_node" != "default" ] && {
						udp_node="tcp"
					}
				}
				if [ "$udp_node" = "default" ]; then
					local GLOBAL_UDP_NODE=$(get_cache_var "ACL_GLOBAL_UDP_node")
					[ -n "${GLOBAL_UDP_NODE}" ] && GLOBAL_UDP_redir_port=$(get_cache_var "ACL_GLOBAL_UDP_redir_port")
					if [ -n "${GLOBAL_UDP_NODE}" ]; then
						set_cache_var "ACL_${sid}_udp_node" "${GLOBAL_UDP_NODE}"
						set_cache_var "ACL_${sid}_udp_redir_port" "${GLOBAL_UDP_redir_port}"
						set_cache_var "ACL_${sid}_udp_default" "1"
					else
						echolog "  - 全局节点未启用，跳过【${remarks}】"
					fi
				elif [ "$udp_node" = "tcp" ] || [ "$udp_node" = "$tcp_node" ]; then
					udp_node=$(get_cache_var "ACL_${sid}_tcp_node")
					udp_port=$(get_cache_var "ACL_${sid}_tcp_redir_port")
					set_cache_var "ACL_${sid}_udp_node" "${udp_node}"
					set_cache_var "ACL_${sid}_udp_redir_port" "${udp_port}"
				else
					[ "$(config_get_type $udp_node)" = "nodes" ] || [ "$(config_get_type $udp_node)" = "socks" ] && {
						if [ -n "${GLOBAL_UDP_NODE}" ] && [ "$udp_node" = "${GLOBAL_UDP_NODE}" ]; then
							set_cache_var "ACL_${sid}_udp_node" "${GLOBAL_UDP_NODE}"
							set_cache_var "ACL_${sid}_udp_redir_port" "${GLOBAL_UDP_redir_port}"
							set_cache_var "ACL_${sid}_udp_default" "1"
						else
							_redir_port=$(get_cache_var "node_${udp_node}_redir_port")
							_socks_port=$(get_cache_var "node_${udp_node}_socks_port")
							if [ -n "${_socks_port}" ] && [ -n "${_redir_port}" ]; then
								socks_port=${_socks_port}
								udp_port=${_redir_port}
							else
								socks_port=$(get_new_port $(expr $socks_port + 1))
								set_cache_var "node_${udp_node}_socks_port" "${socks_port}"
								redir_port=$(get_new_port $(expr $redir_port + 1))
								set_cache_var "node_${udp_node}_redir_port" "${redir_port}"
								udp_port=$redir_port
								local log_file="/dev/null"
								[ "${log}" = "1" ] && log_file="${TMP_ACL_PATH}/${sid}/UDP.log"

								local type
								if [ "$(config_get_type $udp_node)" = "socks" ]; then
									if [ "${dns_mode}" = "xray" ]; then
										type="xray"
									elif [ "${dns_mode}" = "sing-box" ]; then
										type="sing-box"
									fi
								else
									type=$(echo $(config_n_get $udp_node type) | tr 'A-Z' 'a-z')
								fi
								if [ -n "${type}" ] && ([ "${type}" = "sing-box" ] || [ "${type}" = "xray" ]); then
									config_file="acl/${udp_node}_UDP_${redir_port}.json"
									config_file="$TMP_PATH/$config_file"
									[ "${type}" = "sing-box" ] && type="singbox"
									run_${type} flag=$udp_node node=$udp_node udp_redir_port=$redir_port config_file=$config_file log_file=$log_file loglevel=$loglevel
								else
									config_file="acl/${udp_node}_SOCKS_${socks_port}.json"
									run_socks flag=$udp_node node=$udp_node bind=127.0.0.1 socks_port=$socks_port config_file=$config_file log_file=$log_file
									# log_file=$TMP_ACL_PATH/ipt2socks_${udp_node}_${redir_port}.log
									log_file="/dev/null"
									run_ipt2socks flag=acl_${udp_node} local_port=$redir_port socks_address=127.0.0.1 socks_port=$socks_port log_file=$log_file
								fi
							fi
							set_cache_var "ACL_${sid}_udp_node" "${udp_node}"
							set_cache_var "ACL_${sid}_udp_redir_port" "${udp_port}"
						fi
					}
				fi
			}
			unset enabled sid remarks sources interface tcp_no_redir_ports udp_no_redir_ports use_global_config tcp_node udp_node use_direct_list use_proxy_list use_block_list use_gfw_list chn_list tcp_proxy_mode udp_proxy_mode filter_proxy_ipv6 dns_mode remote_dns v2ray_dns_mode remote_dns_doh remote_dns_client_ip
			unset _ip _mac _iprange _ipset _ip_or_mac source_list tcp_port udp_port config_file _extra_param dns_cache_key log loglevel
			unset _china_ng_listen _chinadns_local_dns _direct_dns_mode chinadns_ng_default_tag dnsmasq_filter_proxy_ipv6 remote_fakedns force_https_soa use_fakedns
		done
		unset socks_port redir_port dns_port dnsmasq_port chinadns_port
		[ -n "${has_enabled}" ] || {
			ENABLED_ACLS=0
			set_cache_var ENABLED_ACLS $ENABLED_ACLS
		}
	fi
}

start() {
	busybox pgrep -f /tmp/etc/passwall/bin > /dev/null 2>&1 && {
		logger -t PSW-RESTART "Upgrade or overload residue is detected, and the subprocess is being called to perform complete cleaning..."
		(stop)
		sleep 2
	}
	mkdir -p /tmp/etc /tmp/log $TMP_PATH $TMP_BIN_PATH $TMP_SCRIPT_FUNC_PATH $TMP_ROUTE_PATH $TMP_ACL_PATH $TMP_PATH2
	get_config
	export V2RAY_LOCATION_ASSET=$(config_t_get global_rules v2ray_location_asset "/usr/share/v2ray/")
	export XRAY_LOCATION_ASSET=$V2RAY_LOCATION_ASSET
	export ENABLE_DEPRECATED_GEOSITE=true
	export ENABLE_DEPRECATED_GEOIP=true
	export SS_SYSTEM_DNS_RESOLVER_FORCE_BUILTIN=1
	ulimit -n 65535
	start_haproxy
	start_socks
	nftflag=0
	USE_TABLES=""
	check_run_environment
	if [ "$ENABLED_DEFAULT_ACL" = 1 ] || [ "$ENABLED_ACLS" = 1 ]; then
		[ "$(uci -q get dhcp.@dnsmasq[0].dns_redirect)" = "1" ] && {
			uci -q set ${CONFIG}.@global[0].dnsmasq_dns_redirect='1'
			uci -q commit ${CONFIG}
			uci -q set dhcp.@dnsmasq[0].dns_redirect='0'
			uci -q commit dhcp
			lua $APP_PATH/helper_dnsmasq.lua restart -LOG 0
		}
	fi
	[ "$ENABLED_DEFAULT_ACL" = 1 ] && {
		mkdir -p ${GLOBAL_ACL_PATH}
		start_redir TCP
		start_redir UDP
		start_dns
	}
	[ -n "$USE_TABLES" ] && source $APP_PATH/${USE_TABLES}.sh start
	set_cache_var "USE_TABLES" "$USE_TABLES"
	if [ "$ENABLED_DEFAULT_ACL" = 1 ] || [ "$ENABLED_ACLS" = 1 ]; then
		bridge_nf_ipt=$(sysctl -e -n net.bridge.bridge-nf-call-iptables)
		set_cache_var "bak_bridge_nf_ipt" "$bridge_nf_ipt"
		sysctl -w net.bridge.bridge-nf-call-iptables=0 >/dev/null 2>&1
		[ "$PROXY_IPV6" = "1" ] && {
			bridge_nf_ip6t=$(sysctl -e -n net.bridge.bridge-nf-call-ip6tables)
			set_cache_var "bak_bridge_nf_ip6t" "$bridge_nf_ip6t"
			sysctl -w net.bridge.bridge-nf-call-ip6tables=0 >/dev/null 2>&1
		}
	fi
	
	start_crontab
	echolog "运行完成！\n"

	[ "$ENABLED" = 1 ] && [ "$1" = "boot" ] && {
		local cfgids item
		for item in $(uci show ${CONFIG} | grep "=subscribe_list" | cut -d '.' -sf 2 | cut -d '=' -sf 1); do
			if [ "$(config_n_get "$item" boot_update 0)" = "1" ]; then
				cfgids="${cfgids:+$cfgids,}$item"
			fi
		done
		[ -n "$cfgids" ] && {
			sleep 5
			lua $APP_PATH/subscribe.lua start $cfgids cron > /dev/null 2>&1 &
		}
	}
}

stop() {
	clean_log
	eval_cache_var
	[ -n "$USE_TABLES" ] && source $APP_PATH/${USE_TABLES}.sh stop
	delete_ip2route
	# 结束 SS 插件进程
	# kill_all xray-plugin v2ray-plugin obfs-local shadow-tls
	local pid_file pid
	find "$TMP_PATH" -type f -name '*_plugin.pid' 2>/dev/null | while read -r pid_file; do
		read -r pid < "$pid_file"
		if [ -n "$pid" ]; then
			kill -9 "$pid" >/dev/null 2>&1
		fi
	done
	busybox pgrep -af "${CONFIG}/monitor\.sh" | xargs -r kill -9 >/dev/null 2>&1
	busybox pgrep -f "sleep.*(6s|9s|58s)" | xargs -r kill -9 >/dev/null 2>&1
	busybox pgrep -af "${CONFIG}/" | awk '! /app\.sh|subscribe\.lua|rule_update\.lua|tasks\.sh|server_app\.lua|ujail/{print $1}' | xargs -r kill -9 >/dev/null 2>&1
	unset V2RAY_LOCATION_ASSET
	unset XRAY_LOCATION_ASSET
	unset SS_SYSTEM_DNS_RESOLVER_FORCE_BUILTIN
	stop_crontab
	source $APP_PATH/helper_smartdns.sh del
	rm -rf $GLOBAL_DNSMASQ_CONF
	rm -rf $GLOBAL_DNSMASQ_CONF_PATH
	[ "1" = "1" ] && {
		#restore logic
		bak_dnsmasq_dns_redirect=$(config_t_get global dnsmasq_dns_redirect)
		[ -n "${bak_dnsmasq_dns_redirect}" ] && {
			uci -q set dhcp.@dnsmasq[0].dns_redirect="${bak_dnsmasq_dns_redirect}"
			uci -q commit dhcp
			uci -q delete ${CONFIG}.@global[0].dnsmasq_dns_redirect
			uci -q commit ${CONFIG}
		}
		if [ -z "${ACL_default_dns_port}" ] || [ -n "${bak_dnsmasq_dns_redirect}" ]; then
			uci -q del_list dhcp.@dnsmasq[0].addnmount="${GLOBAL_DNSMASQ_CONF_PATH}"
			uci -q commit dhcp
			lua $APP_PATH/helper_dnsmasq.lua restart -LOG 0
		fi
		[ -n "${bak_bridge_nf_ipt}" ] && sysctl -w net.bridge.bridge-nf-call-iptables=${bak_bridge_nf_ipt} >/dev/null 2>&1
		[ -n "${bak_bridge_nf_ip6t}" ] && sysctl -w net.bridge.bridge-nf-call-ip6tables=${bak_bridge_nf_ip6t} >/dev/null 2>&1
	}
	rm -rf $TMP_PATH
	rm -f ${LOCK_PATH}/${CONFIG}_socks_auto_switch*
	rm -f ${LOCK_PATH}/${CONFIG}_lease2hosts*
	rm -f ${LOCK_PATH}/${CONFIG}_monitor*
	echolog "清空并关闭相关程序和缓存完成。"
	exit 0
}

get_config() {
	ENABLED_DEFAULT_ACL=0
	TCP_REDIR_PORT=1041
	UDP_REDIR_PORT=1051
	ENABLED=$(config_t_get global enabled 0)
	SOCKS_ENABLED=$(config_t_get global socks_enabled 0)
	TCP_NODE=$(config_t_get global tcp_node)
	UDP_NODE=$(config_t_get global udp_node)
	TCP_UDP=0
	if [ "$UDP_NODE" = "tcp" ]; then
		UDP_NODE=$TCP_NODE
		TCP_UDP=1
	elif [ "$UDP_NODE" = "$TCP_NODE" ]; then
		TCP_UDP=1
	fi
	[ -n "$TCP_NODE" ] && {
		local protocol=$(config_n_get $TCP_NODE protocol)
		[ "$protocol" = "_shunt" ] && [ -n "$UDP_NODE" ] && {
			UDP_NODE=$TCP_NODE
			TCP_UDP=1
		}
	}
	[ "$ENABLED" = 1 ] && {
		local _node
		for _node in "$TCP_NODE" "$UDP_NODE"; do
			if [ -n "$_node" ] && ([ "$(config_get_type $_node)" = "nodes" ] || [ "$(config_get_type $_node)" = "socks" ]); then
				ENABLED_DEFAULT_ACL=1
			fi
		done
	}
	ENABLED_ACLS=$(config_t_get global acl_enable 0)
	set_cache_var ENABLED_DEFAULT_ACL $ENABLED_DEFAULT_ACL
	set_cache_var ENABLED_ACLS $ENABLED_ACLS

	TCP_PROXY_WAY=$(config_t_get global_forwarding tcp_proxy_way redirect)
	PROXY_IPV6=$(config_t_get global_forwarding ipv6_tproxy 0)
	TCP_REDIR_PORTS=$(config_t_get global_forwarding tcp_redir_ports '80,443')
	UDP_REDIR_PORTS=$(config_t_get global_forwarding udp_redir_ports '1:65535')
	TCP_NO_REDIR_PORTS=$(config_t_get global_forwarding tcp_no_redir_ports 'disable')
	UDP_NO_REDIR_PORTS=$(config_t_get global_forwarding udp_no_redir_ports 'disable')
	TCP_PROXY_DROP_PORTS=$(config_t_get global_forwarding tcp_proxy_drop_ports 'disable')
	UDP_PROXY_DROP_PORTS=$(config_t_get global_forwarding udp_proxy_drop_ports '80,443')
	USE_DIRECT_LIST=$(config_t_get global use_direct_list 1)
	USE_PROXY_LIST=$(config_t_get global use_proxy_list 1)
	USE_BLOCK_LIST=$(config_t_get global use_block_list 1)
	USE_GFW_LIST=$(config_t_get global use_gfw_list 1)
	CHN_LIST=$(config_t_get global chn_list direct)
	TCP_PROXY_MODE=$(config_t_get global tcp_proxy_mode proxy)
	UDP_PROXY_MODE=$(config_t_get global udp_proxy_mode proxy)
	[ "${TCP_PROXY_MODE}" != "disable" ] && TCP_PROXY_MODE="proxy"
	[ "${UDP_PROXY_MODE}" != "disable" ] && UDP_PROXY_MODE="proxy"
	LOCALHOST_PROXY=$(config_t_get global localhost_proxy 1)
	[ "${LOCALHOST_PROXY}" = 1 ] && {
		LOCALHOST_TCP_PROXY_MODE=$TCP_PROXY_MODE
		LOCALHOST_UDP_PROXY_MODE=$UDP_PROXY_MODE
	}
	CLIENT_PROXY=$(config_t_get global client_proxy 1)
	DNS_SHUNT=$(config_t_get global dns_shunt dnsmasq)
	[ -z "$(first_type $DNS_SHUNT)" ] && DNS_SHUNT="dnsmasq"
	DNS_MODE=$(config_t_get global dns_mode tcp)
	[ "$DNS_SHUNT" = "smartdns" ] && DNS_MODE=$(config_t_get global smartdns_dns_mode socks)
	REMOTE_DNS=$(normalize_dns "$(config_t_get global remote_dns 1.1.1.1:53)")
	USE_DEFAULT_DNS=$(config_t_get global use_default_dns direct)
	FILTER_PROXY_IPV6=$(config_t_get global filter_proxy_ipv6 0)
	DNS_REDIRECT=$(config_t_get global dns_redirect 1)

	REDIRECT_LIST="socks ss-rust ssr sing-box xray naiveproxy hysteria2"
	TPROXY_LIST="socks ss-rust ssr sing-box xray hysteria2"

	NEXT_DNS_LISTEN_PORT=15353
	TUN_DNS="127.0.0.1#${NEXT_DNS_LISTEN_PORT}"
	DNS_CACHE=0
	DIRECT_DNS_QUERY_STRATEGY="UseIP"
	REMOTE_DNS_QUERY_STRATEGY="UseIP"
	[ "$FILTER_PROXY_IPV6" = "1" ] && REMOTE_DNS_QUERY_STRATEGY="UseIPv4"
	DNSMASQ_FILTER_PROXY_IPV6=${FILTER_PROXY_IPV6}

	IPT_APPEND_DNS=${LOCAL_DNS}

	DNSMASQ_CONF_DIR=/tmp/dnsmasq.d
	DEFAULT_DNSMASQ_CFGID="$(uci -q show "dhcp.@dnsmasq[0]" | awk 'NR==1 {split($0, conf, /[.=]/); print conf[2]}')"
	if [ -f "/tmp/etc/dnsmasq.conf.$DEFAULT_DNSMASQ_CFGID" ]; then
		DNSMASQ_CONF_DIR="$(awk -F '=' '/^conf-dir=/ {print $2}' "/tmp/etc/dnsmasq.conf.$DEFAULT_DNSMASQ_CFGID")"
		if [ -n "$DNSMASQ_CONF_DIR" ]; then
			DNSMASQ_CONF_DIR=${DNSMASQ_CONF_DIR%*/}
		else
			DNSMASQ_CONF_DIR="/tmp/dnsmasq.d"
		fi
	fi
	set_cache_var GLOBAL_DNSMASQ_CONF ${DNSMASQ_CONF_DIR}/dnsmasq-${CONFIG}.conf
	set_cache_var GLOBAL_DNSMASQ_CONF_PATH ${GLOBAL_ACL_PATH}/dnsmasq.d

	SMARTDNS_LOCAL_PORT=0
	SMARTDNS_LISTEN_PORT=0
	[ "${DNS_SHUNT}" = "smartdns" ] && {
		NEXT_DNS_LISTEN_PORT=$(expr $NEXT_DNS_LISTEN_PORT + 1)
		SMARTDNS_LOCAL_PORT=${NEXT_DNS_LISTEN_PORT}
		NEXT_DNS_LISTEN_PORT=$(expr $NEXT_DNS_LISTEN_PORT + 1)
		SMARTDNS_LISTEN_PORT=${NEXT_DNS_LISTEN_PORT}
		NEXT_DNS_LISTEN_PORT=$(expr $NEXT_DNS_LISTEN_PORT + 1)
		LOCAL_DNS="127.0.0.1#${SMARTDNS_LOCAL_PORT}"
		set_cache_var "SMARTDNS_LOCAL_PORT" "${SMARTDNS_LOCAL_PORT}"
	}
}

get_local_dns() {
	RESOLVFILE=/tmp/resolv.conf.d/resolv.conf.auto
	[ -f "${RESOLVFILE}" ] && [ -s "${RESOLVFILE}" ] || RESOLVFILE=/tmp/resolv.conf.auto

	ISP_DNS=$(cat $RESOLVFILE 2>/dev/null | grep -E -o "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" | grep -v -E '^(0\.0\.0\.0|127\.0\.0\.1)$' | awk '!seen[$0]++')
	ISP_DNS6=$(cat $RESOLVFILE 2>/dev/null | grep -E "([A-Fa-f0-9]{1,4}::?){1,7}[A-Fa-f0-9]{1,4}" | awk -F % '{print $1}' | awk -F " " '{print $2}' | grep -v -Fx ::1 | grep -v -Fx :: | awk '!seen[$0]++')

	DEFAULT_DNS=$(uci show dhcp.@dnsmasq[0] | grep "\.server=" | awk -F '=' '{print $2}' | sed "s/'//g" | tr ' ' '\n' | grep -v "\/" | sed ':label;N;s/\n/,/;b label')
	[ -z "${DEFAULT_DNS}" ] && [ "$(echo $ISP_DNS | tr ' ' '\n' | wc -l)" -ge 1 ] && DEFAULT_DNS=$(echo -n $ISP_DNS | tr ' ' '\n' | tr '\n' ',' | sed 's/,$//')
	LOCAL_DNS="${DEFAULT_DNS:-119.29.29.29,223.5.5.5}"
}

get_local_dns

arg1=$1
shift
case $arg1 in
run_socks)
	run_socks "$@"
	;;
socks_node_switch)
	socks_node_switch "$@"
	;;
start)
	start "$@"
	;;
stop)
	stop
	;;
esac
