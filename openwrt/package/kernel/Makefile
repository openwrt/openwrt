# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kernel
PKG_VERSION:=$(LINUX_VERSION)-$(BOARD)
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/packages

include $(INCLUDE_DIR)/package.mk
ifeq ($(DUMP),)
-include $(LINUX_DIR)/.config
include $(INCLUDE_DIR)/netfilter.mk
endif

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile
endef

define KernelPackage/Defaults
FILES:=
KCONFIG:=
AUTOLOAD:=
endef

define ModuleAutoLoad
	export modules=; \
	add_module() { \
		mkdir -p $(2)/etc/modules.d; \
		echo "$$$$$$$$2" > $(2)/etc/modules.d/$$$$$$$$1-$(1); \
		modules="$$$$$$$${modules:+$$$$$$$$modules }$$$$$$$$1-$(1)"; \
	}; \
	$(3) \
	[ -n "$$$$$$$$modules" ] && { \
		mkdir -p $(2)/etc/modules.d; \
		echo "#!/bin/sh" >> $(2)/CONTROL/postinst; \
		echo "[ -z \"\$$$$$$$$IPKG_INSTROOT\" ] || exit" >> $(2)/CONTROL/postinst; \
		echo ". /etc/functions.sh" >> $(2)/CONTROL/postinst; \
		echo "load_modules $$$$$$$$modules" >> $(2)/CONTROL/postinst; \
		chmod 0755 $(2)/CONTROL/postinst; \
	}
endef
 

define KernelPackage
  NAME:=$(1)
  $(eval $(call KernelPackage/Defaults))
  $(eval $(call KernelPackage/$(1)))
  $(eval $(call KernelPackage/$(1)/$(KERNEL)))

  ifneq ($(findstring m,$(KCONFIG)),m)
    CONFIG_PACKAGE_kmod-$(1):=
  endif

  define Package/kmod-$(1)
    TITLE:=$(TITLE)
    SECTION:=kernel
    CATEGORY:=Kernel modules
    DESCRIPTION:=$(DESCRIPTION)
    EXTRA_DEPENDS:='kernel (=$(PKG_VERSION))'
  endef

  define Package/kmod-$(1)/install
	mkdir -p $$(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(FILES) $$(1)/lib/modules/$(LINUX_VERSION)/
	$(call ModuleAutoLoad,$(1),$$(1),$(AUTOLOAD))
  endef
  $$(eval $$(call BuildPackage,kmod-$(1)))
endef

define AutoLoad
add_module $(1) "$(2)";
endef

include ./modules.mk

