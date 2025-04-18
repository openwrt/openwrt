include $(TOPDIR)/rules.mk

PKG_DISTNAME:=u-boot
PKG_VERSION:=2025.01
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_DISTNAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
	https://ftp.denx.de/pub/u-boot \
	https://mirror.cyberbits.eu/u-boot \
	ftp://ftp.denx.de/pub/u-boot
PKG_HASH:=cdef7d507c93f1bbd9f015ea9bc21fa074268481405501945abc6f854d5b686f
PKG_SOURCE_SUBDIR:=$(PKG_DISTNAME)-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_DISTNAME)-$(PKG_VERSION)

PKG_BUILD_DEPENDS:=fstools

PKG_LICENSE:=GPL-2.0 GPL-2.0+
PKG_LICENSE_FILES:=Licenses/README

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/dumpimage
	SECTION:=utils
	CATEGORY:=Utilities
	SUBMENU:=Boot Loaders
	TITLE:=dumpimage lists and extracts data from U-Boot images.
	URL:=http://www.denx.de/wiki/U-Boot
endef

define Package/dumpimage/description
	dumpimage lists and extracts data from U-Boot images.
	If -l is specified, dumpimage lists the components in
	image.Otherwise, dumpimage extracts the component at
	position to outfile.
endef

define Package/fit-check-sign
	SECTION:=utils
	CATEGORY:=Utilities
	SUBMENU:=Boot Loaders
	TITLE:=verify uImage.FIT
	URL:=http://www.denx.de/wiki/U-Boot
endef

define Package/fit-check-sign/description
	fit_check_sign validates uImage.FIT hashes and signatures
endef

define Package/uboot-envtools
	SECTION:=utils
	CATEGORY:=Utilities
	SUBMENU:=Boot Loaders
	PKG_FLAGS+=nonshared
	TITLE:=read/modify U-Boot bootloader environment
	URL:=http://www.denx.de/wiki/U-Boot
endef

define Package/uboot-envtools/description
	This package includes tools to read and modify U-Boot
	bootloader environment.
endef

define Package/uboot-envtools/conffiles
	/etc/config/ubootenv
	/etc/fw_env.config
	/etc/fw_sys.config
endef

define Build/Configure
	$(call Build/Compile/Default,tools-only_defconfig)
	$(SED) 's/CONFIG_TOOLS_LIBCRYPTO=y/# CONFIG_TOOLS_LIBCRYPTO is not set/' $(PKG_BUILD_DIR)/.config
	$(SED) 's/CONFIG_TOOLS_KWBIMAGE=y/# CONFIG_TOOLS_KWBIMAGE is not set/' $(PKG_BUILD_DIR)/.config
endef

MAKE_FLAGS += \
	ARCH="sandbox" \
	TARGET_CFLAGS="$(TARGET_CFLAGS)" \
	TARGET_LDFLAGS="$(TARGET_LDFLAGS)" \
	NO_PYTHON=1 \
	V=$(if $(findstring c,$(OPENWRT_VERBOSE)),1,)

define Build/Compile

ifneq ($(CONFIG_PACKAGE_uboot-envtools),)
	$(call Build/Compile/Default,envtools)
endif
ifneq ($(CONFIG_PACKAGE_dumpimage)$(CONFIG_PACKAGE_fit-check-sign),)
	$(call Build/Compile/Default,cross_tools)
endif

endef

define Package/dumpimage/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tools/dumpimage $(1)/usr/bin
endef

define Package/fit-check-sign/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tools/fit_check_sign $(1)/usr/bin
endef

define Package/uboot-envtools/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tools/env/fw_printenv $(1)/usr/sbin
	$(LN) fw_printenv $(1)/usr/sbin/fw_setenv
	$(INSTALL_BIN) ./uboot-envtools/files/fw_printsys $(1)/usr/sbin
	$(INSTALL_BIN) ./uboot-envtools/files/fw_setsys $(1)/usr/sbin
	$(INSTALL_BIN) ./uboot-envtools/files/fw_loadenv $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc/board.d
	$(INSTALL_DATA) ./uboot-envtools/files/fw_defaults $(1)/etc/board.d/05_fw_defaults
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_DATA) ./uboot-envtools/files/uboot-envtools.sh $(1)/lib
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(if $(wildcard ./uboot-envtools/files/$(BOARD)_$(SUBTARGET)), \
		$(INSTALL_DATA) ./uboot-envtools/files/$(BOARD)_$(SUBTARGET) \
		$(1)/etc/uci-defaults/30_uboot-envtools, \
		$(if $(wildcard ./uboot-envtools/files/$(BOARD)), \
			$(INSTALL_DATA) ./uboot-envtools/files/$(BOARD) \
			$(1)/etc/uci-defaults/30_uboot-envtools \
		) \
	)
endef

$(eval $(call BuildPackage,dumpimage))
$(eval $(call BuildPackage,fit-check-sign))
$(eval $(call BuildPackage,uboot-envtools))
