#!/bin/sh
uci batch << EOF
set wireless.radio0='wifi-device'
set wireless.radio0.country='IN'
set wireless.radio0.type='mac80211'
set wireless.radio0.path='platform/soc/fe300000.mmcnr/mmc_host/mmc1/mmc1:0001/mmc1:0001:1'
set wireless.radio0.htmode='HT20'
set wireless.radio0.channel='7'
set wireless.radio0.hwmode='11g'
set wireless.radio0.legacy_rates='0'
set wireless.@wifinet[0]='wifi-iface'
set wireless.@wifinet[0].device='radio0'
set wireless.@wifinet[0].mode='ap'
set wireless.@wifinet[0].disabled='0'
set wireless.@wifinet[0].ssid='raspberry_console'
set wireless.@wifinet[0].encryption='psk2+ccmp'
set wireless.@wifinet[0].key='RandomKey94'
set wireless.@wifinet[0].network='lan'
commit
EOF
uci commit wireless
exit 0
