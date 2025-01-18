#!/bin/bash
#
# Copyright (c) 2019-2020 P3TERX <https://p3terx.com>
#
# This is free software, licensed under the MIT License.
# See /LICENSE for more information.
#
# https://github.com/P3TERX/Actions-OpenWrt
# File name: diy-part2.sh
# Description: OpenWrt DIY script part 2 (After Update feeds)
#

# Modify default IP
sed -i 's/192.168.1.1/192.168.1.1/g' package/base-files/files/bin/config_generate
sed -i '/CONFIG_PACKAGE_glib2=y/d' .config
#echo 'CONFIG_FEED_atinout=y' >>.config
#echo 'CONFIG_PACKAGE_luci-app-atinout-mod=y' >>.config
rm -rf feeds/packages/net/mosdns
rm -rf feeds/packages/net/msd_lite
rm -rf feeds/packages/net/smartdns
rm -rf feeds/luci/themes/luci-theme-argon
rm -rf feeds/luci/applications/luci-app-mosdns
rm -rf feeds/luci/applications/luci-app-netdata
rm -rf feeds/packages/net/quectel-cm
rm -rf feeds/packages/quectel-cm
rm -rf feeds/packages/kernel/fibocom-qmi-wwan/
rm -rf feeds/packages/kernel/quectel-qmi-wwan/
rm -rf feeds/packages/lang/golang
git clone https://github.com/kenzok8/golang feeds/packages/lang/golang
./scripts/feeds update -a; ./scripts/feeds install -a


