#!/bin/sh

IPT=/usr/sbin/iptables

for T in filter nat mangle ; do
  $IPT -t $T -F
  $IPT -t $T -X
done

$IPT -t filter -A INPUT -m state --state INVALID -j DROP
$IPT -t filter -A INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT 
$IPT -t filter -A INPUT -p icmp -j ACCEPT 
$IPT -t filter -A INPUT -i vlan1 -p tcp -j REJECT --reject-with tcp-reset 
$IPT -t filter -A INPUT -i vlan1 -j REJECT --reject-with icmp-port-unreachable 
$IPT -t filter -A FORWARD -m state --state INVALID -j DROP 
$IPT -t filter -A FORWARD -m state --state RELATED,ESTABLISHED -j ACCEPT 
$IPT -t filter -A FORWARD -i vlan1 -m state --state NEW,INVALID -j DROP

$IPT -t nat -A POSTROUTING -o vlan1 -j MASQUERADE

echo "1"   >/proc/sys/net/ipv4/ip_forward
echo "1"   >/proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
echo "1"   >/proc/sys/net/ipv4/icmp_ignore_bogus_error_responses
echo "30"  >/proc/sys/net/ipv4/tcp_fin_timeout
echo "120" >/proc/sys/net/ipv4/tcp_keepalive_time
echo "0"   >/proc/sys/net/ipv4/tcp_timestamps
