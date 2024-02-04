#!/bin/sh

CONFIG=passwall2
LOG_FILE=/tmp/log/$CONFIG.log

echolog() {
	local d="$(date "+%Y-%m-%d %H:%M:%S")"
	#echo -e "$d: $1"
	echo -e "$d: $1" >> $LOG_FILE
}

config_n_get() {
	local ret=$(uci -q get "${CONFIG}.${1}.${2}" 2>/dev/null)
	echo "${ret:=$3}"
}

config_t_get() {
	local index=0
	[ -n "$4" ] && index=$4
	local ret=$(uci -q get $CONFIG.@$1[$index].$2 2>/dev/null)
	echo ${ret:=$3}
}

test_url() {
	local url=$1
	local try=1
	[ -n "$2" ] && try=$2
	local timeout=2
	[ -n "$3" ] && timeout=$3
	local extra_params=$4
	curl --help all | grep "\-\-retry-all-errors" > /dev/null
	[ $? == 0 ] && extra_params="--retry-all-errors ${extra_params}"
	status=$(/usr/bin/curl -I -o /dev/null -skL $extra_params --connect-timeout ${timeout} --retry ${try} -w %{http_code} "$url")
	case "$status" in
		204|\
		200)
			status=200
		;;
	esac
	echo $status
}

test_proxy() {
	result=0
	status=$(test_url "https://www.google.com/generate_204" ${retry_num} ${connect_timeout})
	if [ "$status" = "200" ]; then
		result=0
	else
		status2=$(test_url "https://www.baidu.com" ${retry_num} ${connect_timeout})
		if [ "$status2" = "200" ]; then
			result=1
		else
			result=2
			ping -c 3 -W 1 223.5.5.5 > /dev/null 2>&1
			[ $? -eq 0 ] && {
				result=1
			}
		fi
	fi
	echo $result
}

url_test_node() {
	result=0
	local node_id=$1
	local _type=$(echo $(config_n_get ${node_id} type nil) | tr 'A-Z' 'a-z')
	[ "${_type}" != "nil" ] && {
		local _tmp_port=$(/usr/share/${CONFIG}/app.sh get_new_port 61080 tcp,udp)
		/usr/share/${CONFIG}/app.sh run_socks flag="url_test_${node_id}" node=${node_id} bind=127.0.0.1 socks_port=${_tmp_port} config_file=url_test_${node_id}.json
		local curlx="socks5h://127.0.0.1:${_tmp_port}"
		sleep 1s
		result=$(curl --connect-timeout 3 -o /dev/null -I -skL -w "%{http_code}:%{time_starttransfer}" -x $curlx "https://www.google.com/generate_204")
		pgrep -af "url_test_${node_id}" | awk '! /test\.sh/{print $1}' | xargs kill -9 >/dev/null 2>&1
		rm -rf "/tmp/etc/${CONFIG}/url_test_${node_id}.json"
	}
	echo $result
}

test_node() {
	local node_id=$1
	local _type=$(echo $(config_n_get ${node_id} type nil) | tr 'A-Z' 'a-z')
	[ "${_type}" != "nil" ] && {
		local _tmp_port=$(/usr/share/${CONFIG}/app.sh get_new_port 61080 tcp,udp)
		/usr/share/${CONFIG}/app.sh run_socks flag="test_node_${node_id}" node=${node_id} bind=127.0.0.1 socks_port=${_tmp_port} config_file=test_node_${node_id}.json
		local curlx="socks5h://127.0.0.1:${_tmp_port}"
		sleep 1s
		_proxy_status=$(test_url "https://www.google.com/generate_204" ${retry_num} ${connect_timeout} "-x $curlx")
		pgrep -af "test_node_${node_id}" | awk '! /test\.sh/{print $1}' | xargs kill -9 >/dev/null 2>&1
		rm -rf "/tmp/etc/${CONFIG}/test_node_${node_id}.json"
		if [ "${_proxy_status}" -eq 200 ]; then
			return 0
		fi
	}
	return 1
}

arg1=$1
shift
case $arg1 in
test_url)
	test_url $@
	;;
url_test_node)
	url_test_node $@
	;;
esac
