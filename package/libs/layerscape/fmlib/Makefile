#
# Copyright 2017 NXP
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fmlib
PKG_VERSION:=2017.12
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/qoriq-open-source/fmlib.git
PKG_SOURCE_VERSION:=95f34b1fe585ba0eced11433c88a593a16baf281
PKG_MIRROR_HASH:=595f55cb083e3c0ceabc7e8742b6401853da47707a402922e3cd05b2c30de234

PKG_BUILD_PARALLEL:=0

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define Package/fmlib
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Layerscape Frame Manager User Space Library
  DEPENDS:=@TARGET_layerscape
endef

define Package/fmlib/description
  The Frame Manager library provides an API on top of the
  Frame Manager driver ioctl calls, that provides a user
  space application with a simple way to configure driver
  parameters and PCD (parse - classify - distribute) rules.
endef

MAKE_FLAGS += \
	all install-libfm-arm \
	KERNEL_SRC="$(LINUX_DIR)" \
	PREFIX="$(PKG_INSTALL_DIR)"

define Build/InstallDev
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

define Package/fmlib/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/* $(1)/lib/
endef

$(eval $(call BuildPackage,fmlib))
