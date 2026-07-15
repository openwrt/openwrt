# SPDX-License-Identifier: Apache-2.0

include $(TOPDIR)/rules.mk

PKG_VERSION:=0.4.0
PKG_RELEASE:=6

LUCI_TITLE:=LuCI support for the OMCI agent RPC service
LUCI_PKGARCH:=all
PKG_MAINTAINER:=Matheus Sampaio Queiroga <srherobrine20@gmail.com>
PKG_LICENSE:=Apache-2.0
LUCI_DEPENDS:=+luci-base +omci-agent @TARGET_airoha

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage - OpenWrt buildroot signature
