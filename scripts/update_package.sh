#=================================================
# Description: Build OpenWrt using GitHub Actions
# Author: sirpdboy
# https://github.com/sirpdboy/openwrt
#!/bin/bash
clear
rm -rf ./package/new
git clone https://github.com/NateLol/luci-app-beardropper package/new/luci-app-beardropper
sed -i 's/"luci.fs"/"luci.sys".net/g' package/new/luci-app-beardropper/luasrc/model/cbi/beardropper/setting.lua
sed -i '/firewall/d' package/new/luci-app-beardropper/root/etc/uci-defaults/luci-beardropper
mv package/new/luci-app-beardropper/po/zh_Hans   package/new/luci-app-beardropper/po/zh-cn
rm -rf ./package/lean/luci-theme-argon
rm -rf ./package/lean/luci-theme-opentomcat
svn co https://github.com/fw876/helloworld/trunk/luci-app-ssr-plus package/new/luci-app-ssr-plus
git clone -b master --single-branch https://github.com/tty228/luci-app-serverchan package/new/luci-app-serverchan
svn co https://github.com/pymumu/smartdns/trunk/package/openwrt package/new/smartdns/smartdns
svn co https://github.com/project-openwrt/openwrt/branches/18.06-kernel5.4/package/ntlf9t/luci-app-smartdns package/new/smartdns/luci-app-smartdns
git clone -b master --single-branch https://github.com/destan19/OpenAppFilter package/new/OpenAppFilter
svn co https://github.com/kenzok8/openwrt-packages/trunk/adguardhome package/new/AdGuardHome
svn co https://github.com/kenzok8/openwrt-packages/trunk/luci-app-adguardhome package/new/luci-app-adguardhome
svn co https://github.com/vernesong/OpenClash/branches/master/luci-app-openclash package/new/luci-app-openclash
git clone -b master --single-branch https://github.com/frainzy1477/luci-app-clash package/new/luci-app-clash
sed -i 's/), 5)/), 48)/g' package/new/luci-app-clash/luasrc/controller/clash.lua
svn co https://github.com/Lienol/openwrt-package/trunk/lienol/luci-app-passwall package/new/luci-app-passwall
svn co https://github.com/jerrykuku/luci-app-vssr/trunk/  package/new/luci-app-vssr
svn co https://github.com/jerrykuku/luci-app-jd-dailybonus/trunk/ package/new/luci-app-jd-dailybonus
chmod -R 755 ./
exit 0
