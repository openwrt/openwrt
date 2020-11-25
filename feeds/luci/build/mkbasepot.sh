#!/bin/sh

[ -d ./build ] || {
	echo "Please execute as ./build/mkbasepot.sh" >&2
	exit 1
}

echo -n "Updating modules/luci-base/po/templates/base.pot ... "

./build/i18n-scan.pl \
	modules/luci-base/ modules/luci-compat/ modules/luci-mod-admin-full/ \
	modules/luci-mod-network modules/luci-mod-status modules/luci-mod-system/ \
	protocols/ themes/ \
> modules/luci-base/po/templates/base.pot

echo "done"
