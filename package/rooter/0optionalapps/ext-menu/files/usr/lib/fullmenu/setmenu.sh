#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Menu Change" "$@"
}

full=$1
source /etc/openwrt_release
twone=$(echo "$DISTRIB_RELEASE" | grep "21.02")

if [ $full = "0" ]; then
	fv="1"
	if [ ! -z "$twone" ]; then # 21.02
		cp /usr/lib/fullmenu/full21/luci-mod-status.json /usr/share/luci/menu.d
		cp /usr/lib/fullmenu/full21/luci-mod-system.json /usr/share/luci/menu.d
		cp /usr/lib/fullmenu/full21/luci-app-opkg.json /usr/share/luci/menu.d
	else # 19.07.6
		cp /usr/lib/fullmenu/full19/luci-mod-status.json /usr/share/luci/menu.d
		cp /usr/lib/fullmenu/full19/luci-mod-system.json /usr/share/luci/menu.d
	fi
else
	fv="0"
	if [ ! -z "$twone" ]; then # 21.02
		cp /usr/lib/fullmenu/limited21/luci-mod-status.json /usr/share/luci/menu.d
		cp /usr/lib/fullmenu/limited21/luci-mod-system.json /usr/share/luci/menu.d
		rm -f /usr/share/luci/menu.d/luci-app-opkg.json
	else # 19.07.6
		cp /usr/lib/fullmenu/limited19/luci-mod-status.json /usr/share/luci/menu.d
		cp /usr/lib/fullmenu/limited19/luci-mod-system.json /usr/share/luci/menu.d
	fi
fi
uci set custom.menu.full=$fv
uci commit custom