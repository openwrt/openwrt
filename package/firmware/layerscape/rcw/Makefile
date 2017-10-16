#
# Copyright (C) 2016 Jiang Yutang <jiangyutang1978@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

PKG_NAME:=rcw
PKG_VERSION:=2017.09
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/qoriq-open-source/rcw.git
PKG_SOURCE_VERSION:=1a6236a34166219de324a105971eb9de05cde76e
PKG_MIRROR_HASH:=6da17ad6b4d148bd991a85bf601f845a41231d6f0bf20153ba1a7a387727aef0

PKG_BUILD_DIR=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=LICENSE

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define rcw/Default
	TITLE:=
	CONFIG:=
endef

define rcw/ls1043ardb
	TITLE=RCW binary $(PKG_NAME)-$(PKG_VERSION) for NXP ls1043ardb 64b/32b Dev Board
	CONFIG=ls1043ardb/RR_FQPP_1455/rcw_1600.bin
endef

define rcw/ls1046ardb
	TITLE=RCW binary $(PKG_NAME)-$(PKG_VERSION) for NXP ls1046ardb 64b/32b Dev Board
	CONFIG=ls1046ardb/RR_FFSSPPPH_1133_5559/rcw_1800_qspiboot_swap.bin
endef

define rcw/ls1012ardb
	TITLE=RCW binary $(PKG_NAME)-$(PKG_VERSION) for NXP ls1012ardb 64b/32b Dev Board
	CONFIG=ls1012ardb/R_SPNH_3508/PBL_0x35_0x08_800_250_1000_default.bin
endef

define rcw/ls1088ardb
	TITLE=RCW binary $(PKG_NAME)-$(PKG_VERSION) for NXP ls1088ardb 64b/32b Dev Board
	CONFIG=ls1088ardb/FCQQQQQQQQ_PPP_H_0x1d_0x0d/PBL_QSPI_1600_700_2100_0x1d_0x0d.bin
endef

define rcw/ls2088ardb
	TITLE=RCW binary $(PKG_NAME)-$(PKG_VERSION) for NXP ls2088ardb 64b/32b Dev Board
	CONFIG=ls2088ardb/FFFFFFFF_PP_HH_0x2a_0x41/PBL_0x2a_0x41_1800_700_1866_1600.bin
endef

RCWS := \
	ls1043ardb \
	ls1046ardb \
	ls1012ardb \
	ls1088ardb \
	ls2088ardb

define Package/rcw/template
define Package/rcw-layerscape-$(1)
	SECTION:=firmware
	CATEGORY:=Firmware
	DEPENDS:= @TARGET_layerscape
	TITLE:=$(2)
	URL:=http://git.freescale.com/git/cgit.cgi/ppc/sdk/rcw.git
	VARIANT:=$(1)
	MAINTAINER:=Jiang Yutang <jiangyutang1978@gmail.com>
endef
endef

define BuildRCWPackage
	$(eval $(rcw/Default))
	$(eval $(rcw/$(1)))
	$(call Package/rcw/template,$(1),$(TITLE))
endef

ifdef BUILD_VARIANT
$(eval $(call rcw/$(BUILD_VARIANT)))
RCW_CONFIG:=$(if $(CONFIG),$(CONFIG),$(BUILD_VARIANT))
endif

define Build/Configure
endef

define Package/rcw/install/default
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/$(RCW_CONFIG) $(STAGING_DIR_IMAGE)/$(1)-rcw.bin
endef

define Package/rcw/install/template
define Package/rcw-layerscape-$(1)/install
	$(call Package/rcw/install/default,$(2))
endef
endef

$(foreach r,$(RCWS), \
	$(eval $(call Package/rcw/install/template,$(r),$(r))) \
)

$(foreach r,$(RCWS), \
	$(eval $(call BuildRCWPackage,$(r))) \
	$(eval $(call BuildPackage,rcw-layerscape-$(r))) \
)
