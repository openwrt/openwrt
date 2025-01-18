#!/bin/sh
###!/bin/bash
#
# Copyright (c) 2019-2020 P3TERX <https://p3terx.com>
#
# This is free software, licensed under the MIT License.
# See /LICENSE for more information.
#
# https://github.com/P3TERX/Actions-OpenWrt
# File name: diy-part1.sh
# Description: OpenWrt DIY script part 1 (Before Update feeds)
#

# Uncomment a feed source
#sed -i 's/^#\(.*helloworld\)/\1/' feeds.conf.default

# Add a feed source
#echo 'src-git helloworld https://github.com/fw876/helloworld' >>feeds.conf.default
#echo 'src-git passwall https://github.com/xiaorouji/openwrt-passwall' >>feeds.conf.default
 #sed -i '$a src-git kenzo https://github.com/kenzok8/openwrt-packages' feeds.conf.default
 #sed -i '$a src-git small https://github.com/kenzok8/small' feeds.conf.default
 #echo 'src-git 3Ginfo https://github.com/4IceG/luci-app-3ginfo-lite.git' >>feeds.conf.default
 #echo 'src-git Modem https://github.com/4IceG/luci-app-modemband.git' >>feeds.conf.default
 #echo 'src-git SMS https://github.com/4IceG/luci-app-sms-tool.git' >>feeds.conf.default
 #echo 'src-git atinout https://github.com/4IceG/luci-app-atinout-mod.git' >>feeds.conf.default
sed -i '$a src-git modem https://github.com/kiddin9/kwrt-packages.git' feeds.conf.default
