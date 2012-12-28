#!/bin/sh
ip6tables -N ipv6-site-border
ip6tables -A forwarding_rule -s fc00::/7 -j ipv6-site-border
ip6tables -A forwarding_rule -d fc00::/7 -j ipv6-site-border

mkdir -p /var/etc/ipv6-firewall.d
for i in /var/etc/ipv6-firewall.d/*; do
	[ -f "$i" ] && . "$i"
done
