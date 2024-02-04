#!/bin/sh

stretch() {
	#zhenduiluanshezhiDNSderen
	local dnsmasq_server=$(uci -q get dhcp.@dnsmasq[0].server)
	local dnsmasq_noresolv=$(uci -q get dhcp.@dnsmasq[0].noresolv)
	local _flag
	for server in $dnsmasq_server; do
		[ -z "$(echo $server | grep '\/')" ] && _flag=1
	done
	[ -z "$_flag" ] && [ "$dnsmasq_noresolv" = "1" ] && {
		uci -q delete dhcp.@dnsmasq[0].noresolv
		uci -q set dhcp.@dnsmasq[0].resolvfile="$RESOLVFILE"
		uci commit dhcp
	}
}

backup_servers() {
	DNSMASQ_DNS=$(uci show dhcp | grep "@dnsmasq" | grep ".server=" | awk -F '=' '{print $2}' | sed "s/'//g" | tr ' ' ',')
	if [ -n "${DNSMASQ_DNS}" ]; then
		uci -q set $CONFIG.@global[0].dnsmasq_servers="${DNSMASQ_DNS}"
		uci commit $CONFIG
	fi
}

restore_servers() {
	OLD_SERVER=$(uci -q get $CONFIG.@global[0].dnsmasq_servers | tr "," " ")
	for server in $OLD_SERVER; do
		uci -q del_list dhcp.@dnsmasq[0].server=$server
		uci -q add_list dhcp.@dnsmasq[0].server=$server
	done
	uci commit dhcp
	uci -q delete $CONFIG.@global[0].dnsmasq_servers
	uci commit $CONFIG
}

logic_restart() {
	local no_log
	eval_set_val $@
	_LOG_FILE=$LOG_FILE
	[ -n "$no_log" ] && LOG_FILE="/dev/null"
	if [ -f "$TMP_PATH/default_DNS" ]; then
		backup_servers
		#sed -i "/list server/d" /etc/config/dhcp >/dev/null 2>&1
		for server in $(uci -q get dhcp.@dnsmasq[0].server); do
			[ -n "$(echo $server | grep '\/')" ] || uci -q del_list dhcp.@dnsmasq[0].server="$server" 
		done
		/etc/init.d/dnsmasq restart >/dev/null 2>&1
		restore_servers
	else
		/etc/init.d/dnsmasq restart >/dev/null 2>&1
	fi
	echolog "重启 dnsmasq 服务"
	LOG_FILE=${_LOG_FILE}
}

restart() {
	local no_log
	eval_set_val $@
	_LOG_FILE=$LOG_FILE
	[ -n "$no_log" ] && LOG_FILE="/dev/null"
	/etc/init.d/dnsmasq restart >/dev/null 2>&1
	echolog "重启 dnsmasq 服务"
	LOG_FILE=${_LOG_FILE}
}

gen_items() {
	local dnss settype setnames outf ipsetoutf
	eval_set_val $@
	
	awk -v dnss="${dnss}" -v settype="${settype}" -v setnames="${setnames}" -v outf="${outf}" -v ipsetoutf="${ipsetoutf}" '
		BEGIN {
			if(outf == "") outf="/dev/stdout";
			if(ipsetoutf == "") ipsetoutf=outf;
			split(dnss, dns, ","); setdns=length(dns)>0; setlist=length(setnames)>0;
			if(setdns) for(i in dns) if(length(dns[i])==0) delete dns[i];
			fail=1;
		}
		! /^$/&&!/^#/ {
			fail=0
			if(setdns) for(i in dns) printf("server=/.%s/%s\n", $0, dns[i]) >>outf;
			if(setlist) printf("%s=/.%s/%s\n", settype, $0, setnames) >>ipsetoutf;
		}
		END {fflush(outf); close(outf); fflush(ipsetoutf); close(ipsetoutf); exit(fail);}
	'
}

add() {
	local TMP_DNSMASQ_PATH DNSMASQ_CONF_FILE DEFAULT_DNS LOCAL_DNS TUN_DNS NFTFLAG NO_LOGIC_LOG
	eval_set_val $@
	_LOG_FILE=$LOG_FILE
	[ -n "$NO_LOGIC_LOG" ] && LOG_FILE="/dev/null"
	mkdir -p "${TMP_DNSMASQ_PATH}" "${DNSMASQ_PATH}" "/tmp/dnsmasq.d"
	
	local set_type="ipset"
	[ "${NFTFLAG}" = "1" ] && {
		set_type="nftset"
		local setflag_4="4#inet#fw4#"
		local setflag_6="6#inet#fw4#"
	}
	
	#始终用国内DNS解析节点域名
	servers=$(uci show "${CONFIG}" | grep ".address=" | cut -d "'" -f 2)
	hosts_foreach "servers" host_from_url | grep '[a-zA-Z]$' | sort -u | gen_items settype="${set_type}" setnames="${setflag_4}passwall2_vpslist,${setflag_6}passwall2_vpslist6" dnss="${LOCAL_DNS:-${DEFAULT_DNS}}" outf="${TMP_DNSMASQ_PATH}/10-vpslist_host.conf" ipsetoutf="${TMP_DNSMASQ_PATH}/ipset.conf"
	echolog "  - [$?]节点列表中的域名(vpslist)：${DEFAULT_DNS:-默认}"
	
	echo "conf-dir=${TMP_DNSMASQ_PATH}" > $DNSMASQ_CONF_FILE
	[ -n "${TUN_DNS}" ] && {
		echo "${DEFAULT_DNS}" > $TMP_PATH/default_DNS
		cat <<-EOF >> $DNSMASQ_CONF_FILE
			server=${TUN_DNS}
			all-servers
			no-poll
			no-resolv
		EOF
		echolog "  - [$?]默认：${TUN_DNS}"
	}
	LOG_FILE=${_LOG_FILE}
}

del() {
	rm -rf /tmp/dnsmasq.d/dnsmasq-$CONFIG.conf
	rm -rf $DNSMASQ_PATH/dnsmasq-$CONFIG.conf
	rm -rf $TMP_DNSMASQ_PATH
}

arg1=$1
shift
case $arg1 in
stretch)
	stretch $@
	;;
add)
	add $@
	;;
del)
	del $@
	;;
restart)
	restart $@
	;;
logic_restart)
	logic_restart $@
	;;
*) ;;
esac
