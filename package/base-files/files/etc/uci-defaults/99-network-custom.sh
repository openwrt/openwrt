#!/bin/sh

. /lib/functions.sh
. /etc/openwrt_release

wifi_password="TPTlam011205@!"
ten_wifi="TPT Lam"
hostname="DOANDUY"

uci set dropbear.@dropbear[0].PasswordAuth='on'
uci set dropbear.@dropbear[0].RootPasswordAuth='on'
hmod -R u=rwX,go= /etc/dropbear
uci commit dropbear
/etc/init.d/dropbear restart

sed -i -re 's/^(option check_signature.*)/#\1/g' /etc/opkg.conf

uci set network.lan.ipaddr="192.168.1.1"
uci commit network
/etc/init.d/network restart

uci set 'network.lan.ipv6=0'
uci set 'network.wan.ipv6=0'
uci set 'dhcp.lan.dhcpv6=disabled'
/etc/init.d/odhcpd disable
/etc/init.d/odhcpd stop
uci commit

uci -q delete dhcp.lan.dhcpv6
uci -q delete dhcp.lan.ra
uci commit dhcp
/etc/init.d/odhcpd restart

uci set network.lan.delegate="0"
uci commit network
/etc/init.d/network restart

uci set base_config.@status[0].SSID='TPT Lam'
uci set base_config.@status[0].SSID_PASSWD=${wifi_password}
uci set base_config.@status[0].country="CN"
uci commit base_config

uci commit wireless

# uci set wireless.radio0.disabled="0"
# uci set wireless.radio1.disabled="0"
# uci set wireless.wifinet0.disabled="0"
# uci set wireless.wifinet1.disabled="0"
# uci commit wireless
# uci commit 
# uci commit base_config
wifi reload
/sbin/wifi reload

uci delete system.ntp.server
uci add_list system.ntp.server='0.vn.pool.ntp.org'
uci add_list system.ntp.server='2.asia.pool.ntp.org'
uci add_list system.ntp.server='1.asia.pool.ntp.org'
uci add_list system.ntp.server='125.235.4.198'
uci add_list system.ntp.server='115.73.220.183'
uci add_list system.ntp.server='222.255.146.26'

uci set system.@system[0]=system 
uci set system.@system[0].hostname="${hostname}"
uci set system.@system[0].zonename='Asia/Ho_Chi_Minh'
uci set system.@system[0].timezone='<+07>-7'
uci commit system


cat << EOI >> /etc/firewall.include

nft add rule inet fw4 mangle_prerouting iifname wwan0 ip ttl set 65
nft add rule inet fw4 mangle_postrouting oifname wwan0 ip ttl set 64

nft add rule inet fw4 mangle_prerouting iifname eth1 ip ttl set 65
nft add rule inet fw4 mangle_postrouting oifname eth1 ip ttl set 64

nft add rule inet fw4 mangle_prerouting iifname eth2 ip ttl set 65
nft add rule inet fw4 mangle_postrouting oifname eth2 ip ttl set 64

nft add rule inet fw4 mangle_prerouting iifname eth3 ip ttl set 65
nft add rule inet fw4 mangle_postrouting oifname eth3 ip ttl set 64

nft add rule inet fw4 mangle_prerouting iifname eth4 ip ttl set 65
nft add rule inet fw4 mangle_postrouting oifname eth4 ip ttl set 64

nft add rule inet fw4 mangle_prerouting iifname eth5 ip ttl set 65
nft add rule inet fw4 mangle_postrouting oifname eth5 ip ttl set 64

nft add rule inet fw4 mangle_prerouting iifname wwan1 ip ttl set 65
nft add rule inet fw4 mangle_postrouting oifname wwan1 ip ttl set 64

nft add rule inet fw4 mangle_prerouting iifname usb0 ip ttl set 65
nft add rule inet fw4 mangle_postrouting oifname usb0 ip ttl set 65

nft add rule inet fw4 mangle_prerouting iifname usb1 ip ttl set 65
nft add rule inet fw4 mangle_postrouting oifname usb1 ip ttl set 65

nft add rule inet fw4 mangle_prerouting iifname usb2 ip ttl set 65
nft add rule inet fw4 mangle_postrouting oifname usb2 ip ttl set 65

nft add rule inet fw4 mangle_prerouting iifname usb82 ip ttl set 65
nft add rule inet fw4 mangle_postrouting oifname usb82 ip ttl set 65


EOI

uci add firewall include
uci set firewall.@include[0].path='/etc/firewall.include'
uci set firewall.@include[0].fw4_compatible='1'
uci commit firewall
service firewall restart

cat << EOI >> /etc/init.d/disable_interface.sh


EOI

chmod 755 /etc/init.d/disable_interface.sh

sed -i '/net.ipv4.ip_default_ttl/d' /etc/sysctl.d/50-local.conf
echo "net.ipv4.ip_default_ttl=64" >> /etc/sysctl.conf
sysctl -p


# /etc/init.d/network restart

uci set network.wwan.apn='m3-world' uci set network.wwan.apn='m3-world' 

uci set network.wwan.apn='v-internet' uci set network.wwan.apn='v-internet' 

uci set network.wwan.apn='m9-wintel' uci set network.wwan.apn='m9-wintel' 

exit 0

