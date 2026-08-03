# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2007-2026 OpenWrt.org

# Ethernet or WAN support, pick one

Package/ixp4xx-microcode-ethernet = $(call Package/firmware-default,IXP4xx ethernet firmware,@TARGET_ixp4xx,LICENSE.ixp4xx,,nonshared)
define Package/ixp4xx-microcode-ethernet/install
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ixp4xx/NPE-A \
		$(PKG_BUILD_DIR)/ixp4xx/NPE-B \
		$(PKG_BUILD_DIR)/ixp4xx/NPE-C \
		$(1)/lib/firmware/
endef
$(eval $(call BuildPackage,ixp4xx-microcode-ethernet))

Package/ixp4xx-microcode-wan = $(call Package/firmware-default,IXP4xx WAN firmware,@TARGET_ixp4xx,LICENSE.ixp4xx,,nonshared)
define Package/ixp4xx-microcode-wan/install
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ixp4xx/NPE-A-HSS \
		$(1)/lib/firmware/NPE-A
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ixp4xx/NPE-B \
		$(PKG_BUILD_DIR)/ixp4xx/NPE-C \
		$(1)/lib/firmware/
endef
$(eval $(call BuildPackage,ixp4xx-microcode-wan))
