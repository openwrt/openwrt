#!/bin/bash

setup_docker_firewall() {
	sleep 2
	iptables -N DOCKER-MAN
	iptables -I DOCKER-USER -j DOCKER-MAN
	iptables -A DOCKER-MAN -i br-lan -o docker0 -j RETURN
	iptables -A DOCKER-MAN -m conntrack --ctstate NEW,INVALID -o docker0 -j DROP
	iptables -A DOCKER-MAN -m conntrack --ctstate ESTABLISHED,RELATED -o docker0 -j RETURN
	iptables -A DOCKER-MAN -j RETURN
}

check_docker_status() {
	iptables -L DOCKER-MAN -n >/dev/null 2>&1; ret=$?
	if [ $ret -eq 0 ]; then
		t_status="docker-fw: loaded"
	else
		t_status="docker-fw: inactive"
	fi
	return $ret
}

# ---------------------------------------------------------

case $1 in
start)
	check_docker_status && { echo "$t_status"; exit 0; }
	setup_docker_firewall
	;;
status)
	check_docker_status;	 echo "$t_status"
	;;
*)
	echo "$0 start|stop|status"
	;;
esac


