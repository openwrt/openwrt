#=================================================
# Description: Build OpenWrt using GitHub Actions
# Author: sirpdboy
# https://github.com/sirpdboy/openwrt
#!/bin/bash
clear
rm -rf ./package/new
##beardropper
git clone https://github.com/NateLol/luci-app-beardropper package/new/luci-app-beardropper
sed -i 's/"luci.fs"/"luci.sys".net/g' package/new/luci-app-beardropper/luasrc/model/cbi/beardropper/setting.lua
sed -i '/firewall/d' package/new/luci-app-beardropper/root/etc/uci-defaults/luci-beardropper
mv package/new/luci-app-beardropper/po/zh_Hans   package/new/luci-app-beardropper/po/zh-cn
#argon主题
rm -rf ./package/lean/luci-theme-argon
rm -rf ./package/lean/luci-theme-rosy
rm -rf ./package/lean/luci-theme-opentomcat

##SSRP
svn co https://github.com/fw876/helloworld/trunk/luci-app-ssr-plus package/new/luci-app-ssr-plus
##simple-obfs
#git clone -b master --single-branch https://github.com/aa65535/openwrt-simple-obfs package/new/simple-obfs
##SeverChan
git clone -b master --single-branch https://github.com/tty228/luci-app-serverchan package/new/luci-app-serverchan
##SmartDNS
svn co https://github.com/kenzok8/openwrt-packages/trunk/smartdns package/new/smartdns 
svn co https://github.com/kenzok8/openwrt-packages/trunk/luci-app-smartdns package/new/luci-app-smartdns
##上网APP过滤
git clone -b master --single-branch https://github.com/destan19/OpenAppFilter package/new/OpenAppFilter
##AdGuardHome
svn co https://github.com/kenzok8/openwrt-packages/trunk/AdGuardHome package/new/AdGuardHome
svn co https://github.com/kenzok8/openwrt-packages/trunk/luci-app-adguardhome package/new/luci-app-adguardhome
##OpenClash
svn co https://github.com/vernesong/OpenClash/branches/master/luci-app-openclash package/new/luci-app-openclash
##clash
git clone -b master --single-branch https://github.com/frainzy1477/luci-app-clash package/new/luci-app-clash
#svn co https://github.com/kenzok8/openwrt-packages/trunk/luci-app-clash package/new/luci-app-clash
sed -i 's/), 5)/), 48)/g' package/new/luci-app-clash/luasrc/controller/clash.lua
##passwall
svn co https://github.com/kenzok8/openwrt-packages/trunk/luci-app-passwall package/new/luci-app-passwall
##luci-app-vssr
svn co https://github.com/jerrykuku/luci-app-vssr/trunk/ package/new/luci-app-vssr
#授予权限
chmod -R 755 ./
exit 0
