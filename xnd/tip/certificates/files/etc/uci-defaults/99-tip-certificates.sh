#!/bin.sh

uci add system certificates
uci set system.@certificates[-1].key=/etc/ucentral/key.pem
uci set system.@certificates[-1].cert=/etc/ucentral/cert.pem
uci set system.@certificates[-1].ca=/etc/ucentral/cas.pem
