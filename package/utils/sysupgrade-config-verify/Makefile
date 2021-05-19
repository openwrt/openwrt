# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=sysupgrade-config-verify
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/sysupgrade-config-verify
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Extra base-files to assist in factory OEM flash
endef

define Package/sysupgrade-config-verify/description
	A set of extra base-files
	that prevents config restore during factory.bin flash
	by verifying each sysupgrade.bin flash.
	This relies on the revision string and "sysupgrade" string
	being in the image metadata
	and for the image metadata to be saved
	into the config tarball during sysupgrade via keep.d
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)

define Package/sysupgrade-config-verify/install
	$(INSTALL_DIR) $(1)/lib/preinit
	$(INSTALL_DATA) ./files/75_config_restore_verify $(1)/lib/preinit
endef

$(eval $(call BuildPackage,sysupgrade-config-verify))
