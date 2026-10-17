#!/bin/bash
# Build the Doodle Labs Smart Radio open firmware against any OpenWrt release.
#
#   ./build.sh                        # default: openwrt-24.10 (stable)
#   BRANCH=openwrt-25.12 ./build.sh   # latest stable (25.12.0 released)
#   BRANCH=main ./build.sh            # OpenWrt development trunk
#
# GV is compiled in by default; runtime gate is radio.main.gv_email in UCI.
set -euo pipefail

BRANCH="${BRANCH:-openwrt-24.10}"
PORT="$(cd "$(dirname "$0")" && pwd)"
OW="${OW:-$PORT/../openwrt-${BRANCH#openwrt-}}"

# OpenWRT refuses to build (and silently makes broken packages) unless umask
# yields group/other read. The host login umask here is 027 and each shell
# resets to it, so pin it for the whole build.
umask 0022

echo "### branch=$BRANCH  tree=$OW ###"
[ -d "$OW/.git" ] || git clone --depth 1 -b "$BRANCH" https://github.com/openwrt/openwrt.git "$OW"
cd "$OW"

# --- integrate port ---
cp -v "$PORT/target/linux/ath79/dts/qca9533_doodlelabs_smartradio.dts" target/linux/ath79/dts/
grep -q doodlelabs_smartradio target/linux/ath79/image/generic.mk \
  || cat "$PORT/target/linux/ath79/image/generic-doodlelabs.mk" >> target/linux/ath79/image/generic.mk

N=target/linux/ath79/generic/base-files/etc/board.d/02_network
grep -q "doodlelabs,smartradio)" "$N" || python3 - "$N" <<'PY'
import sys; p=sys.argv[1]; s=open(p).read()
case='\tdoodlelabs,smartradio)\n\t\tucidef_set_interface_lan "eth0 eth1"\n\t\t;;\n'
m='case "$board" in\n'; i=s.index(m)+len(m); open(p,'w').write(s[:i]+case+s[i:])
PY

mkdir -p files && cp -rv "$PORT/files/." files/

# --- custom packages ---
cp -rv "$PORT/package/wfb-ng"            package/
cp -rv "$PORT/package/mavp2p"            package/
cp -rv "$PORT/package/wireless-regdb-gv" package/
cp -rv "$PORT/package/luci-app-smartradio" package/

# --- GV regdb integration into wireless-regdb build ---
WRD=package/firmware/wireless-regdb/Makefile
if ! grep -q SMARTRADIO_REGDB_GV_UNLOCK "$WRD"; then
	# Append gv.db.txt to regulatory.db when unlock symbol is set
	sed -i '/define Build\/Prepare/a \\tifeq ($(CONFIG_SMARTRADIO_REGDB_GV_UNLOCK),y)\n\t\tcat $(TOPDIR)/package/wireless-regdb-gv/gv.db.txt >> $(PKG_BUILD_DIR)/db.txt\n\tendif' "$WRD"
fi
# Wire the Kconfig symbol
grep -q SMARTRADIO_REGDB_GV_UNLOCK package/wireless-regdb-gv/Config.in 2>/dev/null && {
	grep -q wireless-regdb-gv target/linux/ath79/Kconfig 2>/dev/null \
	  || echo 'source "package/wireless-regdb-gv/Config.in"' >> package/Config.in
} || true

# --- feeds + config ---
./scripts/feeds update -a
./scripts/feeds install \
	kmod-batman-adv batctl-default alfred \
	wpad-mesh-openssl \
	luci luci-base luci-mod-network luci-mod-status luci-mod-system \
	luci-app-firewall luci-theme-bootstrap uhttpd uhttpd-mod-ubus rpcd rpcd-mod-rpcsys \
	mtd-utils \
	socat ser2net \
	gpsd gpsd-clients \
	igmpproxy \
	kmod-can kmod-can-raw kmod-can-bcm kmod-can-gw \
	kmod-can-usb-8dev kmod-can-usb-ems kmod-can-usb-peak kmod-can-usb-kvaser kmod-can-slcan \
	canutils socketcand cannelloni \
	kmod-usb2 kmod-usb-ehci \
	kmod-usb-serial kmod-usb-serial-ftdi kmod-usb-serial-cp210x kmod-usb-serial-ch341 \
	kmod-usb-net kmod-usb-net-smsc95xx kmod-usb-net-rndis \
	kmod-sched-cake kmod-sched-mqprio \
	golang libsodium \
	iperf3 tcpdump picocom

cat > .config <<EOF
CONFIG_TARGET_ath79=y
CONFIG_TARGET_ath79_generic=y
CONFIG_TARGET_ath79_generic_DEVICE_doodlelabs_smartradio=y
CONFIG_TARGET_ROOTFS_INITRAMFS=y
CONFIG_TARGET_ROOTFS_SQUASHFS=y
CONFIG_SMARTRADIO_REGDB_GV_UNLOCK=y
CONFIG_PACKAGE_mtd-utils=y
CONFIG_PACKAGE_socat=y
CONFIG_PACKAGE_gpsd=y
CONFIG_PACKAGE_kmod-can=y
CONFIG_PACKAGE_kmod-can-raw=y
CONFIG_PACKAGE_canutils=y
CONFIG_PACKAGE_socketcand=y
CONFIG_PACKAGE_kmod-usb-serial-ftdi=y
CONFIG_PACKAGE_kmod-usb-net-smsc95xx=y
CONFIG_PACKAGE_iperf3=y
CONFIG_PACKAGE_luci=y
# 24.10 LuCI is ucode-based; luci-lua-runtime breaks theme rendering
# CONFIG_PACKAGE_luci-lua-runtime is not set
# CONFIG_PACKAGE_luci-lib-nixio is not set
CONFIG_PACKAGE_uhttpd=y
CONFIG_PACKAGE_rpcd=y
CONFIG_PACKAGE_luci-app-batman-adv=y
CONFIG_PACKAGE_luci-app-smartradio=y
CONFIG_PACKAGE_wfb-ng=y
CONFIG_PACKAGE_mavp2p=y
CONFIG_PACKAGE_ucode-mod-nl80211=y
CONFIG_PACKAGE_ucode-mod-socket=y
CONFIG_PACKAGE_ucode-mod-uloop=y
EOF
make defconfig >/dev/null

grep -q "DEVICE_doodlelabs_smartradio=y" .config || { echo "device not selected"; exit 1; }
# GV is now compiled in; runtime gate is operator email in radio.main.gv_email

echo "### building (umask $(umask), -j$(nproc)) ###"
make -j"$(nproc)"
echo "### images: $OW/bin/targets/ath79/generic/ ###"
ls -la bin/targets/ath79/generic/*doodlelabs_smartradio* 2>/dev/null || true
